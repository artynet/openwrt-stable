/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author Federico Vaga <federicov@linino.org>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/moduleparam.h>
#include <linux/jiffies.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/irq.h>


#define SPI_TTY_BUF_LEN 64
#define SPI_TTY_IRQ (IRQF_TRIGGER_HIGH)
#define SPI_TTY_MAX_MESSAGE_LEN 250	/* the 5bytes to 255 are reserved */

#define SPI_DEFAULT_IGNORE_TX 0xFF
#define SPI_DEFAULT_IGNORE_RX 0xFF
#define SPI_IGNORE_CODE_LENGHT_TX 3
#define SPI_IGNORE_CODE_LENGHT_RX 2

static unsigned int dev_count = 0;
static spinlock_t lock;

#define tty_to_avr_dev(_ptr) ((struct spi_tty_dev *) dev_get_drvdata(_ptr->dev))


static int transfer_delay = 25, transfer_len = 1, max_pending_byte = 1024;

module_param(transfer_delay, int, 0444);
module_param(transfer_len, int, 0444);
module_param(max_pending_byte, int, 0444);

/* * * * Data Structure * * * */
enum spi_tty_commands {
	SPI_AVR_CMD_READ = 0x00,
};

#define SPI_TTY_FLAG_BUSY_INTERRUPT (1 << 0)

/*
 * It describe the driver status
 */
struct spi_tty_dev {
	struct spi_device *spi;

	unsigned long flags;

	unsigned int tty_minor;
	struct device *tty_dev;
	struct tty_port port;

	unsigned int gpio_irq;	/* GPIO used as interrupt source */

	spinlock_t lock;
	int active_msg_count;
	int active_bytes_count;
	wait_queue_head_t wait;

	uint8_t tx_ignore;
	uint8_t rx_ignore;

	uint8_t rx_buf_ignore;

	unsigned int count_interrupt_byte;
};

/*
 * It is used to contain the information about a single SPI message between
 * this driver and an AVR program
 */
struct spi_tty_message {
	struct spi_tty_dev *avr;
	struct spi_message	message;
	unsigned int len;	/* total message length */
	uint8_t tx_buf[SPI_TTY_MAX_MESSAGE_LEN];
	uint8_t rx_buf[SPI_TTY_MAX_MESSAGE_LEN];

	struct list_head list;
};

#define SPI_SERIAL_TTY_MINORS 1
static struct tty_driver *spi_serial_tty_driver = NULL;
static struct spi_tty_dev *spi_tty_all_dev[SPI_SERIAL_TTY_MINORS];

static int spi_tty_send_message(struct spi_tty_dev *avr,
				const uint8_t *data, unsigned int count);

/* * * * SYSFS attributes * * * */
static ssize_t spi_tty_delay_show(struct device *dev,
				  struct device_attribute *attr,
				  char *buf)
{
	return sprintf(buf, "%d\n", transfer_delay);
}

static ssize_t spi_tty_delay_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	long val;

	if (strict_strtol(buf, 0, &val))
		return -EINVAL;

	transfer_delay = val;

	return count;
}

static DEVICE_ATTR(transfer_delay, 644, spi_tty_delay_show, spi_tty_delay_store);

static struct attribute *spi_tty_attr[] = {
	&dev_attr_transfer_delay.attr,
	NULL,
};
static const struct attribute_group spi_tty_attr_group = {
	.attrs = spi_tty_attr,
};
static const struct attribute_group *spi_tty_attr_groups[] = {
	&spi_tty_attr_group,
	NULL,
};

/* * * * Access to GPIO Interrupt Configuration Register * * * */
static void __ath79_gpio_set_int_reg(unsigned long offset,
				     unsigned gpio, int value)
{
	void __iomem *base = ath79_gpio_base;
	//unsigned long flags;
	int reg_val;

	//spin_lock_irqsave(&ath79_gpio_lock, flags);

	reg_val = __raw_readl(base + offset);
	if (value)
		reg_val |= (1 << gpio);
	else
		reg_val &= (~(1 << gpio));
	__raw_writel(reg_val, base + offset);

	//spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

/* FIXME on the manual it is enable (on request irq) */
static void __ath79_gpio_set_int_mode(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_MODE, gpio, value);
}

static void __ath79_gpio_set_int_type(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_TYPE, gpio, value);
}

static void __ath79_gpio_set_int_polarity(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_POLARITY, gpio, value);
}

/* FIXME on the manual it is MASK (on enable/disable irq) */
static void __ath79_gpio_set_int_enable(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_ENABLE, gpio, value);
}

static void __ath79_gpio_set_int_pending(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_PENDING, gpio, value);
}

static int __ath79_gpio_get_int_pending()
{
	return __raw_readl(ath79_gpio_base + AR71XX_GPIO_REG_INT_PENDING);
}

int spi_tty_gpio_to_irq(unsigned gpio)
{
	return ATH79_MISC_IRQ(2); /* MISC GPIO interrupt */
}


#if 0
static void irq_enable(unsigned int gpio)
{
	__ath79_gpio_set_int_mode(gpio, 1);
}
static void irq_disable(unsigned int gpio)
{
	__ath79_gpio_set_int_mode(gpio, 0);
}

/* FIXME maybe no. Not clear from the manual */
static void irq_ack(unsigned int gpio)
{
	__ath79_gpio_set_int_enable(gpio, 0);
}
static void irq_mask(unsigned int gpio)
{
	__ath79_gpio_set_int_enable(gpio, 0);
}

static void irq_unmask(unsigned int gpio,)
{
	__ath79_gpio_set_int_enable(gpio, 1);
}

static int irq_set_type(unsigned int gpio, unsigned long irqflags)
{
	int edge, polarity;

	switch (irqflags & IRQF_TRIGGER_MASK) {
	case IRQF_TRIGGER_HIGH:
		edge = 0;
		polarity = 1;
		break;
	case IRQF_TRIGGER_LOW:
		edge = 0;
		polarity = 0;
		break;
	case IRQF_TRIGGER_RISING:
		edge = 1;
		polarity = 1;
		break;
	case IRQF_TRIGGER_FALLING:
		edge = 1;
		polarity = 0;
		break;
	default:
		return -EINVAL;
	}

	__ath79_gpio_set_int_type(gpio, edge);
	__ath79_gpio_set_int_polarity(gpio, polarity);

	return 0;
}

static struct irq_chip ath79_misc_irq_chip = {
	.name		= "GPIO",
	.irq_enable	= irq_enable,	/* Enable interrupt */
	.irq_disable	= irq_disable,	/* Disable interrupt */
	.irq_ack	= irq_ack,
	.irq_mask	= irq_mask,	/* Disable interrupt */
	.irq_unmask	= irq_unmask,	/* Enable interrupt */
	.irq_set_type	= irq_set_type
};
#endif


/* * * * SPI Communication * * * */

/*
 * The MCU can send bytes on processor transmission. If MCU has
 * less bytes to send than processor, it puts and invalid byte at
 * the end of valid bytes.
 */
static int spi_tty_valid_length(struct spi_tty_message *avr_msg)
{
	struct spi_tty_dev *avr = avr_msg->avr;
	int i, count;

	dev_vdbg(&avr->spi->dev, "%s:%d len: %d\n", __func__, __LINE__,
		avr_msg->len);

	for (i = 1, count = 0; i < avr_msg->len; ++i, ++count) {
		/* All bytes are valid until the first ignore byte */
		if (avr_msg->rx_buf[i] != avr->rx_ignore)
			continue;

		/*
		 * If the loop reaches the end of the buffer, and the
		 * last byte is to ignore, then ignore it without check
		 * for the second byte.
		 *
		 * MCU should NOT put an FF at the end of a buffer
		 */
		if (i == avr_msg->len - 1)
			break;

		/*
		 * If it is a valid byte, skip the code and continue.
		 * Otherwise, is it invalid.
		 */
		if (avr_msg->rx_buf[i + 1] == avr->rx_ignore) {
			if (i == avr_msg->len - 2) {
				count ++;
				break;
			}

			/* Replace escape with next values */
			memmove(&avr_msg->rx_buf[i + 1],
				&avr_msg->rx_buf[i + 2],
				avr_msg->len - i - 2);
			i++;
		} else {
			break;
		}

	}

	dev_dbg(&avr->spi->dev, "%s: number of valid bytes %i\n",
		__func__, count);

	return count;
}

/*
 * This function removes all SPI transfers from a given SPI message and it
 * releases the memory.
 */
static void spi_tty_remove_transfers(struct spi_tty_message *avr_msg)
{
	struct spi_transfer *t, *tmp_t;

	list_for_each_entry_safe(t, tmp_t,&avr_msg->message.transfers, transfer_list) {
		spi_transfer_del(t);
		kfree(t);
	}
}

static void spi_tty_parsing_message(struct tty_struct *tty,
				   struct spi_tty_message *avr_msg)
{
	struct spi_tty_dev *avr = avr_msg->avr;
	int valid_len;

	dev_vdbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);

	/*
	 * In the SPI message length is 1, it means that it was a message
	 * used to fetch AVR TX buffer length
	 */
	if (avr_msg->len <= 1)
		return;

	valid_len = spi_tty_valid_length(avr_msg);

	/*
	 * The incoming message contains valid bytes
	 * Push the filtered buffer in the tty pipeline
	 */
	tty_insert_flip_string(tty, avr_msg->rx_buf + 1, valid_len);
	tty_flip_buffer_push(tty);
	return;
}

static void spi_tty_generate_clock(struct spi_tty_message *avr_msg)
{
	struct spi_tty_dev *avr = avr_msg->avr;

	if (avr_msg->len != 1)
		return;

	if (avr->count_interrupt_byte == 0) {
		dev_err(&avr->spi->dev, "Unexpected single byte transfer\n");
		return;
	}
	/* A special interrupt byte come back */
	if (!avr_msg->tx_buf[0])
		avr->count_interrupt_byte--;

	dev_dbg(&avr->spi->dev,
		"%s: received single byte message (left %i, len %d) \n",
		__func__, avr->count_interrupt_byte, avr_msg->rx_buf[0]);

	if (avr->count_interrupt_byte)
		return;

	/*
	 * All expected special byte received. The last one contains the
	 * clock length to generate. This will be used by the MCU to
	 * transfer byte
	 */
	if (avr_msg->rx_buf[0]) {
		if (avr_msg->rx_buf[0] >= SPI_TTY_MAX_MESSAGE_LEN)
			avr_msg->rx_buf[0] = SPI_TTY_MAX_MESSAGE_LEN - 1;
		spi_tty_send_message(avr, NULL, avr_msg->rx_buf[0]);
	}
}

/*
 * This function is the SPI callback, invoked when message is completed
 */
static void spi_tty_message_complete(void *context)
{
	struct spi_tty_message *avr_msg = context;
	struct spi_tty_dev *avr = avr_msg->avr;
	struct tty_struct *tty = tty_port_tty_get(&avr_msg->avr->port);
	unsigned long flags;
	int i;

	dev_vdbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);

	for (i = 0; i < avr_msg->len; ++i)
		dev_dbg(&avr->spi->dev, "%s: buf[%d] = 0x%x (rx) = 0x%x (tx)\n", __func__, i, avr_msg->rx_buf[i], avr_msg->tx_buf[i]);

	/* Push incoming data to user space throught TTY interface */
	if (tty)
		spi_tty_parsing_message(tty, avr_msg);

	/* If necessary, generate clock */
	spi_tty_generate_clock(avr_msg);

	/* Decrement active messages counter */
	spin_lock_irqsave(&avr->lock, flags);
	avr->active_bytes_count -= avr_msg->len;
	avr->active_msg_count--;
	dev_dbg(&avr->spi->dev, "%s:%d [%d - %d]\n", __func__, __LINE__,
		 avr->active_msg_count, avr->active_bytes_count);
	spin_unlock_irqrestore(&avr->lock, flags);

	if (tty) {
		/* Wake up queue (wait until all transfered) */
		wake_up_interruptible(&avr->wait);
		tty_wakeup(tty);
	}

	tty_kref_put(tty);

	/* Clean up memory */
	spi_tty_remove_transfers(avr_msg);
	kfree(avr_msg);
}

/*
 * This function convert a given stream of bytes in an SPI message
 *
 * An SPI message is made of two logic field: length and payload. The first
 * byte is the length of the message (maximum 255 byte); all the other bytes
 * are the payload.
 */
static struct spi_tty_message *spi_tty_message_build(const uint8_t *data,
						     unsigned int len)
{
	struct spi_tty_message *avr_msg;
	struct spi_transfer *t;
	int i, err;

	/* Message length cannot be greater than the maximum message length */
	if (len >= SPI_TTY_MAX_MESSAGE_LEN)
		return ERR_PTR(-EINVAL);

	avr_msg = kmalloc(sizeof(struct spi_tty_message),
			GFP_KERNEL | GFP_ATOMIC);
	if (!avr_msg)
		return ERR_PTR(-ENOMEM);

	spi_message_init(&avr_msg->message);

	/*
	 * The first two payload's bytes tell us if the payload contains
	 * invalid data or not.
	 */
	avr_msg->len = len + 1;
	if (data) {
		/*
		 * Here there is a valid set of bytes. If the first byte is
		 * [0xFF] we must insert its escape sequence [0xFF][0xFF],
		 * otherwise there is nothing to do
		 */
		if (data[0] != 0xFF) {
			memcpy(avr_msg->tx_buf + 1, data, len);
			/* [length] [0x--] [0x--] [0x--] ... [0x--]*/
		} else {
			avr_msg->len++;
			memcpy(avr_msg->tx_buf + 2, data, len);
			avr_msg->tx_buf[1] = 0xFF;
			/* [length] [0xFF] [0xFF] [0x--] ... [0x--]*/
		}

	} else {
		/*
		 * Here there is not a valid set of bytes. We put an invalid
		 * byte sequence in order to invalidate the whole frame.
		 *
		 * It is not necessary to clear the buffer with memset, but it
		 * is useful on debugging
		 */
		memset(avr_msg->tx_buf, 0, avr_msg->len);
		avr_msg->tx_buf[1] = 0xFF;
		/* [length] [0xFF] [0x00] [0x00] ... [0x00] */
	}
	avr_msg->tx_buf[0] = avr_msg->len - 1;

	for (i = 0; i < avr_msg->len ; ++i) {
		t = kzalloc(sizeof(struct spi_transfer),
			GFP_KERNEL | GFP_ATOMIC);
		if (!t) {
			err = -ENOMEM;
			goto err_trans_alloc;
		}

		/*
		 * To allow AVR to complete its buffering we must insert a
		 * delay between each transfer.
		 */
		t->len = transfer_len;
		t->tx_buf = &avr_msg->tx_buf[i];
		t->rx_buf = &avr_msg->rx_buf[i];
		t->delay_usecs = transfer_delay;

		spi_message_add_tail(t, &avr_msg->message);
	}
	avr_msg->message.context = avr_msg;
	avr_msg->message.complete = spi_tty_message_complete;

	return avr_msg;

err_trans_alloc:
	spi_tty_remove_transfers(avr_msg);
	kfree(avr_msg);

	return ERR_PTR(err);
}

/*
 * This function is an helper that build an SPI message and it sends the message
 * immediately on the SPI bus
 */
static int spi_tty_send_message(struct spi_tty_dev *avr,
				const uint8_t *data, unsigned int count)
{
	struct spi_tty_message *avr_msg;
	unsigned long flags;
	int err;

	dev_dbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);

	/* Build SPI message */
	avr_msg = spi_tty_message_build(data, count);
	if (IS_ERR(avr_msg)) {
		dev_err(&avr->spi->dev, "%s: Cannot build message (%ld)\n",
			__func__, PTR_ERR(avr_msg));
		return PTR_ERR(avr_msg);
	}
	avr_msg->avr = avr;

	/* Send SPI message */
	err = spi_async(avr->spi, &avr_msg->message);
	if (err) {
		dev_err(&avr->spi->dev, "%s: Cannot send message (%d)\n",
			__func__, err);
		return err;
	}

	/* Increment active messages counter */
	spin_lock_irqsave(&avr->lock, flags);
	avr->active_bytes_count += avr_msg->len;
	avr->active_msg_count++;
	dev_dbg(&avr->spi->dev, "%s:%d [%d - %d]\n", __func__, __LINE__,
		 avr->active_msg_count, avr->active_bytes_count);
	spin_unlock_irqrestore(&avr->lock, flags);

	return count;
}


/* * * * TTY Operations * * * */

/*
 * The kernel invokes this function when a program opens the TTY interface of
 * this driver
 */
static int spi_serial_tty_open(struct tty_struct * tty, struct file * filp)
{
	struct spi_tty_dev *avr = tty_to_avr_dev(tty);

	dev_vdbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);
	/*
	 * Initialize waiting queue. It is initialized here because the queue
	 * it is used only on an opened TTY port
	 */
	init_waitqueue_head(&avr->wait);

	return tty_port_open(&avr->port, tty, filp);
}

/*
 * The kernel invokes this function when a program closes the TTY interface of
 * this driver
 */
static void spi_serial_tty_close(struct tty_struct * tty, struct file * filp)
{
	struct spi_tty_dev *avr = tty_to_avr_dev(tty);

	dev_vdbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);

	tty_ldisc_flush(tty);
	tty_port_close(&avr->port, tty, filp);

	wake_up_interruptible(&avr->port.open_wait);
	wake_up_interruptible(&avr->port.close_wait);
}


/*
 * This function return the number of bytes that this driver can accept. There
 * is not a real limit because se redirect all the traffic to the SPI
 * framework. So, the limit here is indicative.
 */
static int spi_serial_tty_write_room(struct tty_struct *tty)
{
	struct spi_tty_dev *avr = tty_to_avr_dev(tty);
	int size;

	size = max_pending_byte - avr->active_bytes_count;
	if (size >= SPI_TTY_MAX_MESSAGE_LEN)
		size = SPI_TTY_MAX_MESSAGE_LEN - 1;

	dev_vdbg(&avr->spi->dev, "%s:%d room size: %d\n", __func__, __LINE__,
		size);

	return size;
}


/*
 * The kernel invokes this function when a program writes on the TTY interface
 * of this driver
 */
static int spi_serial_tty_write(struct tty_struct * tty,
				const unsigned char *buf, int count)
{
	struct spi_tty_dev *avr = tty_to_avr_dev(tty);
	unsigned int left, len;
	int i;

	if (!buf || !count)
		return 0;

	left = spi_serial_tty_write_room(tty);
	if (left <= 0)
		return 0;

	len = left > count ? count : left;

	dev_vdbg(&avr->spi->dev, "%s:%d %d\n", __func__, __LINE__, count);
	for (i = 0; i < len; ++i) {
		dev_vdbg(&avr->spi->dev, "%s:%d 0x%x\n", __func__, __LINE__, buf[i]);
	}

	return spi_tty_send_message(avr, buf, len);
}


/*
 * It waits until all messages are transmitted. This driver takes track of
 * all pending messages, so we know when the driver sent all messages
 */
static void spi_serial_tty_wait_until_sent(struct tty_struct *tty,
					   int timeout)
{
	struct spi_tty_dev *avr = tty_to_avr_dev(tty);

	dev_vdbg(&avr->spi->dev, "%s:%d %d\n", __func__, __LINE__,
		avr->active_msg_count);

	/* Wait until all messages were sent */
	wait_event_interruptible(avr->wait, avr->active_msg_count == 0);
}

static struct tty_operations spi_serial_ops = {
	.open		= spi_serial_tty_open,
	.close		= spi_serial_tty_close,
	.write		= spi_serial_tty_write,
	.write_room	= spi_serial_tty_write_room,
	.wait_until_sent= spi_serial_tty_wait_until_sent
};

static void spi_serial_port_dtr_rts(struct tty_port *port, int on){
	pr_info("%s:%d\n", __func__, __LINE__);
}

static const struct tty_port_operations spi_serial_port_ops = {
	.dtr_rts = spi_serial_port_dtr_rts,
};
/* * * * Handle AVR Interrupt * * * */

static irqreturn_t spi_tty_data_ready_int(int irq, void *arg)
{
	struct spi_tty_dev *avr = arg;
	int irq_status;

	dev_vdbg(&avr->spi->dev, "%s:%d\n", __func__, __LINE__);

	irq_status = __ath79_gpio_get_int_pending();
	dev_dbg(&avr->spi->dev, "%s:%d 0x%x\n", __func__, __LINE__, irq_status);

	if (!(irq_status & (1 << avr->gpio_irq))) {
		dev_dbg(&avr->spi->dev, "%s:%d spurious interrupt\n", __func__, __LINE__);
		return IRQ_NONE;
	}

	if (avr->count_interrupt_byte > 0) {
		dev_dbg(&avr->spi->dev,
			"%s:%d already processing interrupt (%d)\n",
			__func__, __LINE__, avr->count_interrupt_byte);
		return IRQ_HANDLED;
	} else if (avr->count_interrupt_byte != 0)
		dev_warn(&avr->spi->dev,
			"something wrong during interrupt processing (count %d)\n",
			avr->count_interrupt_byte);

	avr->count_interrupt_byte = 2;
	/* Now send a single byte to receive incoming length */
	spi_tty_send_message(avr, NULL, 0);	/* Advise MCU */
	spi_tty_send_message(avr, NULL, 0);	/* MCU use this to send length */

	return IRQ_HANDLED;
}

static int spi_tty_register_interrupt(struct spi_tty_dev *avr,
				      unsigned int gpio_irq)
{
	int err = 0;

	/* Configure Data Ready interrupt */
	avr->gpio_irq = gpio_irq;
	err = gpio_request(avr->gpio_irq, "DRDY");
	if (err) {
		dev_err(&avr->spi->dev, "cannot get GPIO (%i)\n", err);
		return err;
	}
	gpio_direction_input(avr->gpio_irq);
	avr->spi->irq = spi_tty_gpio_to_irq(avr->gpio_irq);
	if (avr->spi->irq < 0)
		goto err_req_irq_gpio;

	/* Low level enable gpio interrupt */
	__ath79_gpio_set_int_mode(avr->gpio_irq, 1);
	__ath79_gpio_set_int_enable(avr->gpio_irq, 1);
	/*
	 * Use interrupt on level to be able to recognize spurious interrupts
	 *     On edge detection, pending status is automatically cleared
	 *     before entering in the interrupt handler, so we read 0x0
	 *     from the pending register.
	 *     FIXME I do not know at the moment why the interrupt
	 *     function is invoked two times.
	 *
	 */
	__ath79_gpio_set_int_type(avr->gpio_irq, 0);
	__ath79_gpio_set_int_polarity(avr->gpio_irq, 1);

	err = request_irq(avr->spi->irq, spi_tty_data_ready_int, SPI_TTY_IRQ,
			  KBUILD_MODNAME, avr);
	if (err)
		goto err_req_irq;

	return 0;

err_req_irq:
	__ath79_gpio_set_int_mode(avr->gpio_irq, 0);
	__ath79_gpio_set_int_enable(avr->gpio_irq, 0);
err_req_irq_gpio:
	gpio_free(avr->gpio_irq);
	return err;
}

static void spi_tty_unregister_interrupt(struct spi_tty_dev *avr)
{
	free_irq(avr->spi->irq, avr);
	__ath79_gpio_set_int_mode(avr->gpio_irq, 0);
	__ath79_gpio_set_int_enable(avr->gpio_irq, 0);
	gpio_free(avr->gpio_irq);
}


/* * * * Driver Initialization * * * */
static int spi_tty_probe(struct spi_device *spi)
{
	struct spi_tty_dev *avr;
	int err = 0;
	unsigned long flags;

	if (dev_count >= SPI_SERIAL_TTY_MINORS)
		return -ENOMEM;

	dev_vdbg(&spi->dev, "%s:%d\n", __func__, __LINE__);

	avr = kzalloc(sizeof(struct spi_tty_dev), GFP_KERNEL);
	if (!avr)
		return -ENOMEM;
	spi_set_drvdata(spi, avr);
	avr->spi = spi;
	spin_lock_init(&avr->lock);
	avr->tx_ignore = SPI_DEFAULT_IGNORE_TX;
	avr->rx_ignore = SPI_DEFAULT_IGNORE_RX;

	err = spi_tty_register_interrupt(avr,
			(unsigned int)spi->dev.platform_data);
	if (err)
		goto err_req_irq;

	/* Initialize port */
	tty_port_init(&avr->port);
	avr->port.ops = &spi_serial_port_ops;

	/* Register new port*/
	avr->tty_minor = dev_count;
	avr->tty_dev = tty_register_device(spi_serial_tty_driver,
					avr->tty_minor, &avr->spi->dev);
	if (IS_ERR(avr->tty_dev)) {
		err = PTR_ERR(avr->tty_dev);
		goto err_req_tty;
	}

	/* add private data to the device */
	dev_set_drvdata(avr->tty_dev, avr);

	spin_lock_irqsave(&lock, flags);
	dev_count++;
	spin_unlock_irqrestore(&lock, flags);

	/* Store new device */
	spi_tty_all_dev[avr->tty_minor] = avr;

	return 0;

err_req_tty:
	spi_tty_unregister_interrupt(avr);
err_req_irq:
	kfree(avr);

	return err;
}

static int spi_tty_remove(struct spi_device *spi)
{
	struct spi_tty_dev *avr;
	unsigned long flags;

	dev_vdbg(&spi->dev, "%s:%d\n", __func__, __LINE__);

	avr = spi_get_drvdata(spi);

	spin_lock_irqsave(&lock, flags);
	if (avr->tty_minor == dev_count - 1)
		dev_count--;
	spin_unlock_irqrestore(&lock, flags);

	/* Remove device */
	spi_tty_all_dev[avr->tty_minor] = NULL;
	tty_unregister_device(spi_serial_tty_driver, avr->tty_minor);
	spi_tty_unregister_interrupt(avr);
	kfree(avr);

	return 0;
}


enum ad788x_devices {
	ID_ATMEGA32U4,
};

static const struct spi_device_id spi_tty_id[] = {
	{"atmega32u4", ID_ATMEGA32U4},
	{}
};

static struct spi_driver spi_tty_driver = {
	.driver = {
		.name	= KBUILD_MODNAME,
		.bus	= &spi_bus_type,
		.owner	= THIS_MODULE,
	},
	.id_table	= spi_tty_id,
	.probe		= spi_tty_probe,
	.remove		= spi_tty_remove,
};

static int spi_serial_init(void)
{
	int err;

	/*
	 * Allocate driver structure and reserve space for a number of
	 * devices
	 */
	spi_serial_tty_driver = alloc_tty_driver(SPI_SERIAL_TTY_MINORS);
	if (!spi_serial_tty_driver)
		return -ENOMEM;

	/*
	 * Configure driver
	 */
	spi_serial_tty_driver->driver_name = "spiserial";
	spi_serial_tty_driver->name = "ttySPI";
	spi_serial_tty_driver->major = 0;
	spi_serial_tty_driver->minor_start = 0;
	spi_serial_tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
	spi_serial_tty_driver->subtype = SERIAL_TYPE_NORMAL;
	spi_serial_tty_driver->flags = TTY_DRIVER_DYNAMIC_DEV;

	tty_set_operations(spi_serial_tty_driver, &spi_serial_ops);
	err = tty_register_driver(spi_serial_tty_driver);
	if (err) {
		pr_err("%s - tty_register_driver failed\n", __func__);
		goto exit_reg_driver;
	}

	return 0;

exit_reg_driver:
	put_tty_driver(spi_serial_tty_driver);
	return err;
}

static int spi_tty_init(void)
{
	int err;

	pr_info("%s: SPI TTY INIT", __func__);
	spin_lock_init(&lock);

	err = spi_serial_init();
	if (err)
		return err;
	return spi_register_driver(&spi_tty_driver);
}

static void spi_tty_exit(void)
{
	int err;
	pr_info("%s: SPI TTY EXIT", __func__);

	err = tty_unregister_driver(spi_serial_tty_driver);
	if (err)
		pr_err("failed to unregister spiserial driver(%d)\n", err);
	put_tty_driver(spi_serial_tty_driver);

	driver_unregister(&spi_tty_driver.driver);
}

module_init(spi_tty_init);
module_exit(spi_tty_exit);
MODULE_AUTHOR("Federico Vaga <federicov@linino.org>");
MODULE_LICENSE("GPL");
