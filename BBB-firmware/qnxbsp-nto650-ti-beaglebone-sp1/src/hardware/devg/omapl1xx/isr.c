/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <atomic.h>

#include "omapl1xx.h"

/* Interrupt handler */
static const struct sigevent *
omapl1xx_lcd_isr(void *arg, int id)
{
	omapl1xx_context_t	*omapl1xx = arg;
	unsigned             status;

	/* Read the status */
	status = *IRQSTATUS;
	/* Acknowledge the interrupt(s) */
	*IRQSTATUS = status;

	/* Any errors? */
	if (status & FUF_RAW_SET)
		omapl1xx->err++;

	/* Disable interrupts */
	*IRQENABLE_CLEAR |= 0x300;

	/* From omapl1xx_wait_vsync? */
	if (omapl1xx->want_lcd_sync)
	{
		omapl1xx->want_lcd_sync = 0;
		*IRQEOI_VECTOR = 0;
		return &omapl1xx->lcd_event;
	}

	/* From omapl1xx_wait_eof? */
	if (omapl1xx->lcd_swap)
	{
		omapl1xx->lcd_swap--;
		omapl1xx->lcd_status = status & 0x300;

		if (omapl1xx->lcd_status & EOF0_RAW_SET) {
			*LCDDMA_FB0_BASE    = omapl1xx->lcd_surf->paddr;
			*LCDDMA_FB0_CEILING = omapl1xx->lcd_surf->paddr + omapl1xx->lcd_size - 1;
			*IRQEOI_VECTOR = 0;
			return &omapl1xx->lcd_event;
		}
		if (omapl1xx->lcd_status & EOF1_RAW_SET) {
			*LCDDMA_FB1_BASE    = omapl1xx->lcd_surf->paddr;
			*LCDDMA_FB1_CEILING = omapl1xx->lcd_surf->paddr + omapl1xx->lcd_size - 1;
			*IRQEOI_VECTOR = 0;
			return &omapl1xx->lcd_event;
		}
	}

	*IRQEOI_VECTOR = 0;
	return NULL;
}

inline int
isr_setup(omapl1xx_context_t *omapl1xx)
{
	int	err;

	if ((omapl1xx->lcd_chid = ChannelCreate(_NTO_CHF_DISCONNECT |
		_NTO_CHF_UNBLOCK)) == -1) {
		return -1;
	}

	if ((omapl1xx->lcd_coid = ConnectAttach(0, 0,
		omapl1xx->lcd_chid, _NTO_SIDE_CHANNEL, 0)) == -1) {
		err = errno;
		goto fail;
	}

	SIGEV_PULSE_INIT(&omapl1xx->lcd_event, omapl1xx->lcd_coid,
		omapl1xx->adapter->pulseprio, OMAPL1xx_LCD_INTERRUPT_PULSE, 0);

	omapl1xx->lcd_iid = InterruptAttach(_NTO_INTR_CLASS_EXTERNAL |
		omapl1xx->irq, omapl1xx_lcd_isr, (void *)omapl1xx, sizeof(*omapl1xx),  _NTO_INTR_FLAGS_END |
		_NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_PROCESS);

	if (omapl1xx->lcd_iid == -1) {
		err = errno;
		goto fail2;
	}

	return 0;

fail2:
	ConnectDetach(omapl1xx->lcd_coid);
fail:
	ChannelDestroy(omapl1xx->lcd_chid);

	errno = err;

	return -1;
}

inline void
isr_cleanup(omapl1xx_context_t *omapl1xx)
{
	ConnectDetach(omapl1xx->lcd_coid);
	ChannelDestroy(omapl1xx->lcd_chid);
	InterruptDetach(omapl1xx->lcd_iid);
}

