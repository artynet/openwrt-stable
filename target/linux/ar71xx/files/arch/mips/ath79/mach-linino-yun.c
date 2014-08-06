/*
 * Copyright (c) dog hunter AG - Zug - CH
 * General Public License version 2 (GPLv2)
 * Author Federico Vaga <federicov@linino.org>
 */

#define LININO_YUN 1
#define LININO_BOARD_RGB 1

#include "mach-linino.c"

MIPS_MACHINE(ATH79_MACH_LININO_YUN, "linino-yun", "Arduino Yun", ds_setup);
MIPS_MACHINE(ATH79_MACH_LININO_ONE, "linino-one", "Linino One", ds_setup);
