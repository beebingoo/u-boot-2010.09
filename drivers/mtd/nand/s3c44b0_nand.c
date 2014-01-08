/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include <nand.h>
#include <asm/hardware.h>
#include <asm/io.h>

#define S3C44B0_NAND_RB		(0x01)
#define S3C44B0_NAND_CE		(0x02)
#define S3C44B0_NAND_CLE	(0x04)
#define S3C44B0_NAND_ALE	(0x08)	

#define MACRO_NAND_CTL_SETCLE(base) ((*(volatile unsigned *) (base)) |=  0x04)
#define MACRO_NAND_CTL_CLRCLE(base) ((*(volatile unsigned *) (base)) &= ~0x04)

#define MACRO_NAND_CTL_SETALE(base) ((*(volatile unsigned *) (base)) |=  0x08)
#define MACRO_NAND_CTL_CLRALE(base) ((*(volatile unsigned *) (base)) &= ~0x08)

#define MACRO_NAND_ENABLE_CE(base) ((*(volatile unsigned *) (base)) &= ~0x02)
#define MACRO_NAND_DISABLE_CE(base) ((*(volatile unsigned *) (base)) |=  0x02)

static void s3c44b0_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;
	unsigned int base = (unsigned int) &PDATC;

	debugX(1, "hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {
		if ( ctrl & NAND_CLE )
			MACRO_NAND_CTL_SETCLE((unsigned int)base);
		else
			MACRO_NAND_CTL_CLRCLE((unsigned int)base);
		if ( ctrl & NAND_ALE )
			MACRO_NAND_CTL_SETALE((unsigned int)base);
		else
			MACRO_NAND_CTL_CLRALE((unsigned int)base);
		if ( ctrl & NAND_NCE )
			MACRO_NAND_ENABLE_CE((unsigned int)base);
		else
			MACRO_NAND_DISABLE_CE((unsigned int)base);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);

}

static int s3c44b0_dev_ready(struct mtd_info *mtd)
{
	struct nand_chip *this = mtd->priv;

	while (!(PDATC & S3C44B0_NAND_RB));
	debugX(1, "dev_ready\n");

	return 1;
}

int board_nand_init(struct nand_chip *nand)
{
	debugX(1, "board_nand_init()\n");

	/* initialize hardware */

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = nand->IO_ADDR_W = (unsigned *)CONFIG_SYS_NAND_BASE;

	nand->select_chip = NULL;

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c44b0_hwcontrol;

	nand->dev_ready = s3c44b0_dev_ready;

	nand->ecc.mode = NAND_ECC_SOFT;

	nand->chip_delay = 15;

	debugX(1, "end of nand_init\n");

	return 0;
}
