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
#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>
#include <hw/i2c.h>
#include <arm/am335x.h>
#include "omapl1xx.h"
#include "hdmi.h"

static int am335x_lcd_tlc59108(omapl1xx_context_t *omapl1xx, uint32_t val);
static int switch_lcd_on(disp_adapter_t *adapter);
static int switch_lcd_off(disp_adapter_t *adapter);

const unsigned short lcd_palette16[] = { 0x4000, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

//#if LCDDBG
static void disp_regs(char *str, omapl1xx_context_t *omapl1xx)
{
	uint32_t r;

	r = *SYSCONFIG;
	fprintf(stderr, "+ %s %d SYSCONFIG %x\n", __FUNCTION__, __LINE__, r);
	r = *LCDDMA_CTRL;
	fprintf(stderr, "+ %s %d LCDDMA_CTRL %x\n", __FUNCTION__, __LINE__, r);
	r = *RASTER_CTRL;
	fprintf(stderr, "+ %s %d RASTER_CTRL %x\n", __FUNCTION__, __LINE__, r);
	r = *IRQSTATUS_RAW;
	fprintf(stderr, "+ %s %d IRQSTATUS_RAW %x\n", __FUNCTION__, __LINE__, r);
	r = *IRQSTATUS;
	fprintf(stderr, "+ %s %d IRQSTATUS %x\n", __FUNCTION__, __LINE__, r);
	r = *IRQENABLE_SET;
	fprintf(stderr, "+ %s %d IRQENABLE_SET %x\n", __FUNCTION__, __LINE__, r);
	r = *IRQENABLE_CLEAR;
	fprintf(stderr, "+ %s %d IRQENABLE_CLEAR %x\n", __FUNCTION__, __LINE__, r);
	r = *IRQEOI_VECTOR;
	fprintf(stderr, "+ %s %d IRQEOI_VECTOR %x\n", __FUNCTION__, __LINE__, r);
	r = *CLKC_ENABLE;
	fprintf(stderr, "+ %s %d CLKC_ENABLE %x\n", __FUNCTION__, __LINE__, r);
	r = *CLKC_RESET;
	fprintf(stderr, "+ %s %d CLKC_RESET %x\n", __FUNCTION__, __LINE__, r);
	r = *LCDDMA_FB0_BASE;
	fprintf(stderr, "+ %s %d LCDDMA_FB0_BASE %x\n", __FUNCTION__, __LINE__, r);
	r = *LCDDMA_FB1_BASE;
	fprintf(stderr, "+ %s %d LCDDMA_FB1_BASE %x\n", __FUNCTION__, __LINE__, r);
}
//#endif

static void
internal_lcd_init(omapl1xx_context_t *omapl1xx, disp_mode_t mode, int stride)
{
	uint32_t	lcdc = 0;
	uint32_t	lcdt0;
	uint32_t	lcdt1;
	uint32_t	lcdt2;
	uint32_t	tft24=0;
	uint32_t	ppl;
	uint32_t	pplmsb;

	unsigned	physical_palette = (unsigned)disp_phys_addr((void *)lcd_palette16);

	*CLKC_RESET = 0xF;
	delay(100);
	*CLKC_RESET = 0;
	delay(100);

	switch (mode) {
		case 16:
			ppl = stride/2;
			tft24 = 0;
			break;
		case 24:
			ppl = stride/3;
			tft24 = 1;
			break;
		case 32:
			ppl = stride/4;
			tft24 = 3;
			break;
		default:
			fprintf(stderr, "+ %s %d unknown mode %d\n", __FUNCTION__, __LINE__, mode);
			ppl = stride/2;
			tft24 = 0;
			break;
	}

	pplmsb  = ((ppl&(~0x3FF))?1:0);
	ppl    &= 0x3FF;

	if (omapl1xx->type) {
		lcdc |= RASTER_CTRL_TFT;
	}

	lcdt0 = ((omapl1xx->hbp-1) << 24) | ((omapl1xx->hfp-1) << 16) | ((omapl1xx->hsw-1) << 10) | (((ppl / 16) -1) << 4) | (pplmsb<<3);
	lcdt1 = (omapl1xx->vbp << 24) | (omapl1xx->vfp << 16) | ((omapl1xx->vsw-1) << 10) | (omapl1xx->lpp-1);
	lcdt2 = (omapl1xx->sync_ctrl << 25) | (omapl1xx->sync_edge << 24) | (omapl1xx->bias << 23) |
			(omapl1xx->ipc << 22) | (omapl1xx->ihs << 21) | (omapl1xx->ivs << 20) | (omapl1xx->acbi << 16) |
			(omapl1xx->acb << 8);

	*LCD_CTRL        = omapl1xx->pcd << CLKDIV_SHIFT | MODESEL_RASTER;
	*RASTER_TIMING_0 = lcdt0;
	*RASTER_TIMING_1 = lcdt1;
	*RASTER_TIMING_2 = lcdt2;
	*RASTER_SUBPANEL = 0;
	*LCDDMA_CTRL     = LCDDMA_BURST16;

	/* load palette */
	*RASTER_CTRL = lcdc | 0x100000 | (0xFF << 12) | (tft24<<25);

	*LCDDMA_FB0_BASE = physical_palette;
	*LCDDMA_FB0_CEILING = physical_palette + 15;

	*RASTER_CTRL |= 1;

	while (!(*IRQSTATUS_RAW & 0x40))
		continue;

	*RASTER_CTRL &= ~1;

	*RASTER_CTRL &= ~0x300000;
	*RASTER_CTRL |= (0x200000 | (tft24<<25));
	*LCDDMA_CTRL  = LCDDMA_DOUBLE_BUFFER | LCDDMA_BURST16 | LCDDMA_TRESH8;

	*IRQEOI_VECTOR = 0; //
}

static int
omapl1xx_mode_init(disp_adapter_t *adapter, char *optstring)
{
	omapl1xx_context_t	*omapl1xx;
	uint32_t			 lcdcfg;

	if (adapter->shmem == NULL) {
		if ((omapl1xx = calloc(1, sizeof (*omapl1xx))) == NULL)
		{
			fprintf(stderr, "+ %s %d: out of memory\n", __FUNCTION__, __LINE__);
			return -1;
		}

		omapl1xx->free_context = 1;
	} 
	else
	{
		omapl1xx = adapter->shmem;
		memset(omapl1xx, 0, sizeof (*omapl1xx));
	}

	adapter->ms_ctx = omapl1xx;
	adapter->vsync_counter = &omapl1xx->vsync_counter;

	omapl1xx->adapter = adapter;
	omapl1xx->err = 0;

	/* Register with the display utility lib */
	if (disp_register_adapter(adapter) == -1)
	{
		fprintf(stderr, "+ %s %d: unable to register with display lib\n", __FUNCTION__, __LINE__);
		if (omapl1xx->free_context)
		{
			free(omapl1xx);
		}
		return -1;
	}

	get_config_data(omapl1xx, optstring);

	omapl1xx->lregptr = disp_mmap_device_memory(OMAPL1xx_LCD_BASE,
		OMAPL1xx_LCD_SIZE, PROT_READ | PROT_WRITE | PROT_NOCACHE, 0);

	if (omapl1xx->lregptr == NULL)
	{
		fprintf(stderr, "+ %s %d: mapping failed\n", __FUNCTION__, __LINE__);
		goto fail;
	}

	*IRQSTATUS    |= 0x3ff;

	// Clear reset
	*CLKC_RESET    = 0;

	// Clear interrupts
	*IRQEOI_VECTOR = 0;

	// Switch on the clocks
	*CLKC_ENABLE   = CLKC_EN_DMA_CLK_EN | CLKC_EN_LIDD_CLK_EN | CLKC_EN_CORE_CLK_EN;

	if (isr_setup(omapl1xx) == -1)
	{
		fprintf(stderr, "+ %s %d: isr setup failed\n", __FUNCTION__, __LINE__);
		goto fail;
	}

	lcdcfg = AM335XEVM_UPDN;
	if (omapl1xx->lr   ) lcdcfg |=  AM335XEVM_SHLR;
	if (omapl1xx->tb   ) lcdcfg &= ~AM335XEVM_UPDN;
	if (omapl1xx->mode3) lcdcfg |=  AM335XEVM_MODE3;
	if (omapl1xx->dith ) lcdcfg |=  AM335XEVM_DITH;
	am335x_lcd_tlc59108(omapl1xx, lcdcfg);

	switch (omapl1xx->display)
	{
	case AM335X_LCD:
		switch_lcd_on(adapter);
		break;
	case AM335X_HDMI:
#ifdef AM335X_HDMI_DEBUG
		fprintf(stderr, ">>>HDMI<<<\n");
#endif
		if (init_hdmi(adapter, omapl1xx) != 0)
			fprintf(stderr, "HDMI: Initialisation Error!\n");
		break;
	default:
		break;
	}

	return 1;

fail:
	if (omapl1xx->lregptr) {
		munmap_device_memory(omapl1xx->lregptr, OMAPL1xx_LCD_SIZE);
	}

	if (omapl1xx->free_context) {
		free(omapl1xx);
	}

	disp_unregister_adapter(adapter);

	return -1;
}

static void
omapl1xx_mode_fini(disp_adapter_t *adapter)
{
	omapl1xx_context_t	*omapl1xx = adapter->ms_ctx;

	switch_lcd_off(adapter);

	isr_cleanup(omapl1xx);

	*CLKC_RESET = 0xF;
	delay(100);
	*CLKC_RESET = 0;
	delay(100);

	if (omapl1xx) {
		if (omapl1xx->lregptr) {
			munmap_device_memory(omapl1xx->lregptr, OMAPL1xx_LCD_SIZE);
		}
		if (omapl1xx->free_context) {
			free(omapl1xx);
		}
		disp_unregister_adapter(adapter);
	}
}

static int
omapl1xx_get_modeinfo(disp_adapter_t *adapter,
    int dispno, disp_mode_t mode, disp_mode_info_t *info)
{
	omapl1xx_context_t	*omapl1xx = adapter->ms_ctx;

	memset(info, 0, sizeof(*info));
	info->size = sizeof(*info);
	info->mode = mode;

	switch (mode) {
		case 16:
			info->pixel_format = DISP_SURFACE_FORMAT_RGB565;
			break;
		case 24:
			info->pixel_format = DISP_SURFACE_FORMAT_RGB888;
			break;
		case 32:
			info->pixel_format = DISP_SURFACE_FORMAT_ARGB8888;
			break;
		default:
			return -1;
	}

	info->xres = omapl1xx->lcd_xres;
	info->yres = omapl1xx->lcd_yres;

	info->u.fixed.refresh[0] = omapl1xx->refresh;
	info->u.fixed.refresh[1] = 0;

	return 0;
}

static int
omapl1xx_set_mode(disp_adapter_t *adapter, int dispno, disp_mode_t mode,
    disp_crtc_settings_t *settings, disp_surface_t *surf, unsigned flags)
{
	int stride, bpp;

	omapl1xx_context_t	*omapl1xx = adapter->ms_ctx;

	switch (mode) {
		case 16:
			bpp = 2;
			omapl1xx->pixel_format = DISP_LAYER_FORMAT_RGB565;
			break;
		case 24:
			bpp = 3;
			omapl1xx->pixel_format = DISP_LAYER_FORMAT_RGB888;
			break;
		case 32:
			bpp = 4;
			omapl1xx->pixel_format = DISP_LAYER_FORMAT_ARGB8888;
			break;
		default:
			return -1;
	}

	stride = omapl1xx->lcd_xres * bpp;

	if (stride & 31) {
		stride = (stride & ~31) + 32;
	}

	*RASTER_CTRL &= ~1;

	internal_lcd_init(omapl1xx, mode, stride);

	return 0;
}

static int
omapl1xx_get_modelist(disp_adapter_t *adapter, int dispno, unsigned short *list,
    int index, int size)
{
	static unsigned modes[] = { 16, 24, 32 };
	int	i = index;
	int j = 0;

	if (dispno > 0 || size-- < 2) // keep a spot for DISP_MODE_LISTEND by decrementing size
		return -1;

	while (j < size && i < sizeof(modes) / sizeof(modes[0]))
		list[j++] = modes[i++];

	list[i] = DISP_MODE_LISTEND;

	return 0;
}

extern int vints;

static int
omapl1xx_wait_vsync(disp_adapter_t *adapter, int dispno)
{
	iov_t					iov;
	struct _pulse			pulse;
	uint64_t				halfsecond = 500*1000*1000;
	omapl1xx_context_t		*omapl1xx = adapter->ms_ctx;

	if (adapter->callback) {
		adapter->callback(adapter->callback_handle,
			DISP_CALLBACK_UNLOCK, NULL);
	}

	/* Disable interrupts */
	*IRQENABLE_CLEAR |= (EOF0_RAW_SET | EOF1_RAW_SET);
	/* Acknowledge interrupts (defensively) */
	*IRQSTATUS       |= (EOF0_RAW_SET | EOF1_RAW_SET);
	/* Set the interrupt(s) we're interested in */
	atomic_add(&omapl1xx->want_lcd_sync, 1);

	*IRQENABLE_SET   |= (EOF0_RAW_SET | EOF1_RAW_SET | FUF_RAW_SET);

	SETIOV(&iov, &pulse, sizeof (pulse));

	while (1) {
		TimerTimeout(CLOCK_REALTIME,
			_NTO_TIMEOUT_RECEIVE, NULL, &halfsecond, NULL);
		if (MsgReceivev(omapl1xx->lcd_chid, &iov, 1, NULL) == -1) {
			disp_perror(adapter, "MsgReceiveLCD");
			break;
		} else if (pulse.code == OMAPL1xx_LCD_INTERRUPT_PULSE) {
			break;
		}
	}

	atomic_add(&omapl1xx->vsync_counter, 1);

	if (adapter->callback) {
		adapter->callback(adapter->callback_handle,
			DISP_CALLBACK_LOCK, NULL);
	}

	if (omapl1xx->err)
	{
		fprintf(stderr, "==== UNDERFLOW ERROR ====\n");
		omapl1xx->err = 0;
	}

#if LCDDBG
	disp_regs("vsync", omapl1xx);
#endif

	return 0;
}

int
devg_get_modefuncs(disp_adapter_t *adapter, disp_modefuncs_t *funcs, int tabsize)
{
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, init, omapl1xx_mode_init, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, fini, omapl1xx_mode_fini, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, module_info, omapl1xx_module_info, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, get_modeinfo, omapl1xx_get_modeinfo, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, get_modelist, omapl1xx_get_modelist, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, set_mode, omapl1xx_set_mode, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, wait_vsync, omapl1xx_wait_vsync, tabsize);

	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_query, omapl1xx_layer_query, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_enable, omapl1xx_layer_enable, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_disable, omapl1xx_layer_disable, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_surface, omapl1xx_layer_set_surface, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_source_viewport, omapl1xx_layer_set_source_viewport, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_dest_viewport, omapl1xx_layer_set_dest_viewport, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_blending, omapl1xx_layer_set_blending, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_chromakey, omapl1xx_layer_set_chromakey, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_update_begin, omapl1xx_layer_update_begin, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_update_end, omapl1xx_layer_update_end, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_reset, omapl1xx_layer_reset, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_alpha_map, omapl1xx_layer_set_alpha_map, tabsize);

	return 0;
}

static int switch_lcd_on(disp_adapter_t *adapter)
{
	unsigned	 gpio0_len;
	uintptr_t	 gpio0_regs;
	unsigned	 gpio0_base;

	/* Map GPIO5 registers */
	gpio0_base = AM335X_GPIO0_BASE;
	gpio0_len  = AM335X_GPIO_SIZE;
	gpio0_regs = mmap_device_io(gpio0_len, gpio0_base);
	if (gpio0_regs == (uintptr_t)MAP_FAILED)
	{
		disp_printf(adapter, "LCD: unable to map GPIO0, can't switch LCD on.");
		return -1;
	}
	else
	{
		/* Switch backlight on. This is pin controlled by pin 'conf_ecap0_in_pwm0_out' */
		/* which is GPIO0 7, and has been configured already as such. */
		uint32_t r;

		r  = in32(gpio0_regs+GPIO_OE);
		r &= ~0x80;
		out32(gpio0_regs+GPIO_OE, r);

		r  = in32(gpio0_regs+GPIO_DATAOUT);
		r |=  0x80;
		out32(gpio0_regs+GPIO_DATAOUT, r);

		munmap_device_io(gpio0_regs, gpio0_len);

		return 0;
	}
}

static int switch_lcd_off(disp_adapter_t *adapter)
{
	unsigned	 gpio0_len;
	uintptr_t	 gpio0_regs;
	unsigned	 gpio0_base;

	/* Map GPIO5 registers */
	gpio0_base = AM335X_GPIO0_BASE;
	gpio0_len  = AM335X_GPIO_SIZE;
	gpio0_regs = mmap_device_io(gpio0_len, gpio0_base);
	if (gpio0_regs == (uintptr_t)MAP_FAILED)
	{
		disp_printf(adapter, "LCD: unable to map GPIO0, can't switch LCD on.");
		return -1;
	}
	else
	{
		/* Switch backlight on. This is pin controlled by pin 'conf_ecap0_in_pwm0_out' */
		/* which is GPIO0 7, and has been configured already as such. */
		uint32_t r;

		r  = in32(gpio0_regs+GPIO_OE);
#if LCDDBG
		disp_printf(adapter, "gpio0 oe %x\n", r);
#endif
		r &= ~0x80;
#if LCDDBG
		disp_printf(adapter, "gpio0 oe %x\n", r);
#endif
		out32(gpio0_regs+GPIO_OE, r);

		r  = in32(gpio0_regs+GPIO_DATAOUT);
#if LCDDBG
		disp_printf(adapter, "gpio0 out %x\n", r);
#endif
		r &= ~0x80;
#if LCDDBG
		disp_printf(adapter, "gpio0 out %x\n", r);
#endif
		out32(gpio0_regs+GPIO_DATAOUT, r);

		munmap_device_io(gpio0_regs, gpio0_len);

		return 0;
	}
}

#define AM335X_I2C0STR		"/dev/i2c0"
#define AM335X_TLCADDR		0x40

#if LCDDBG
static int lcd_i2c_read(omapl1xx_context_t *omapl1xx, int subaddr)
{
    struct {
        i2c_send_t      hdr;
        uint8_t	        reg[2];
    } msgreg;

    struct {
        i2c_recv_t      hdr;
        uint8_t 		val[256];
    } msgval;
    int rbytes;
    int r;

    msgreg.hdr.slave.addr = omapl1xx->i2c_address;
    msgreg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgreg.hdr.len        = 1;
    msgreg.hdr.stop       = 0;
    msgreg.reg[0]         = subaddr;
    msgreg.reg[1]         = 0;

    r = devctl(omapl1xx->fd, DCMD_I2C_SEND, &msgreg, sizeof(msgreg), NULL);
    if (r)
    {
    	fprintf(stderr, "I2C write failed (DCMD_I2C_SEND), error %d\n", r);
    	return (-1);
    }

    msgval.hdr.slave.addr = omapl1xx->i2c_address;
    msgval.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgval.hdr.len        = 1;
    msgval.hdr.stop       = 1;

    r = devctl(omapl1xx->fd, DCMD_I2C_RECV, &msgval, sizeof(msgval), &rbytes);
    if (r)
    {
    	fprintf(stderr, "I2C read failed (DCMD_I2C_RECV), error %d\n", r);
        return (-1);
    }

#if LCDDBG
    printf("tlc: subaddr %d: %x\n", subaddr, msgval.val[0]);
#endif

    return (rbytes);
}
#endif

/* write hdmi */
int lcd_i2c_write(omapl1xx_context_t *omapl1xx, uint32_t reg, uint32_t val)
{
	int r;
    struct {
        i2c_send_t      hdr;
        uint8_t         reg;
        uint8_t         val;
    } msg;

    msg.hdr.slave.addr = omapl1xx->i2c_address;
    msg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msg.hdr.len        = 2;
    msg.hdr.stop       = 1;
    msg.reg            = reg;
    msg.val            = val;

    r = devctl(omapl1xx->fd, DCMD_I2C_SEND, &msg, sizeof(msg), NULL);
    if (r)
    {
    	fprintf(stderr, "I2C write failed (DCMD_I2C_SEND), error %d\n", r);
    	return (-1);
    }

    return (EOK);
}

static int am335x_lcd_tlc59108(omapl1xx_context_t *omapl1xx, uint32_t val)
{
	int	r;
#if LCDDBG
	int i;
#endif

	omapl1xx->fd = open(AM335X_I2C0STR, O_RDWR);
	if (omapl1xx->fd == -1)
	{
		fprintf(stderr, "Unable to open %s (errno=%d)\n", AM335X_I2C0STR, errno);
		return -1;
	}

	r = devctl(omapl1xx->fd, DCMD_I2C_LOCK, NULL, 0, NULL);
	if (r)
	{
		fprintf(stderr, "Unable to lock %s (errno=%d)\n", AM335X_I2C0STR, errno);
		close(omapl1xx->fd);
		return -1;
	}

	omapl1xx->i2c_address = AM335X_TLCADDR;
#if LCDDBG
	for (i=0; i<0x14; i++)
	{
		r = lcd_i2c_read(omapl1xx, i);
		if (r<0)
		{
			fprintf(stderr, "Unable to read %s (errno=%d)\n", AM335X_I2C0STR, errno);
			close(omapl1xx->fd);
			return -1;
		}
	}
#endif

	{
		int v1 = 0;
		int v2 = 0;
		if (val & 0x01) v1 |= (0x01)<<0;
		if (val & 0x02) v1 |= (0x01)<<2;
		if (val & 0x04) v1 |= (0x01)<<4;
		if (val & 0x08) v1 |= (0x01)<<6;
		if (val & 0x10) v2 |= (0x01)<<0;
		if (val & 0x20) v2 |= (0x01)<<2;
		if (val & 0x40) v2 |= (0x01)<<4;
		if (val & 0x80) v2 |= (0x01)<<6;
		lcd_i2c_write(omapl1xx, 12, v1);
		lcd_i2c_write(omapl1xx, 13, v2);
		lcd_i2c_write(omapl1xx,  0, 0x01);
#if LCDDBG
		fprintf(stderr, "v1=%x, v2=%x\n", v1, v2);
#endif
	}

#if LCDDBG
	omapl1xx->i2c_address = AM335X_TLCADDR;
	for (i=0; i<0x14; i++)
	{
		r = lcd_i2c_read(omapl1xx, i);
		if (r<0)
		{
			fprintf(stderr, "Unable to read %s (errno=%d)\n", AM335X_I2C0STR, errno);
			close(omapl1xx->fd);
			return -1;
		}
	}
#endif

	r = devctl(omapl1xx->fd, DCMD_I2C_UNLOCK, NULL, 0, NULL);
	if (r)
	{
		fprintf(stderr, "Unable to unlock %s (errno=%d)\n", AM335X_I2C0STR, errno);
		close(omapl1xx->fd);
		return -1;
	}

	close(omapl1xx->fd);

	return 0;
}
