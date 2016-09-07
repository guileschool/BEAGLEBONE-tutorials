/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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

#include <stddef.h>
#include <devctl.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <pthread.h>
#include <hw/i2c.h>

#include "omapl1xx.h"
#include "hdmi.h"

#define LOG_ERR		stderr

#define HDMI_ADDR	0x39
//#define HDMI_ADDR	0x72

static void hdmi_state(disp_adapter_t *adapter, am335x_hdmi_t *dev);
static int hdmi_thread_init(disp_adapter_t *adapter, am335x_hdmi_t *dev);
static int hdmi_thread_uninit(disp_adapter_t *adapter, am335x_hdmi_t *dev);

static uint8_t vidinfoframe[] =
{
		0xF0, 0x00, 0x68, 0x00, 0x04, 0x00, 0x19,
		0x00, 0xE9, 0x02, 0x04, 0x01, 0x04, 0x06,
};

static int hdmi_write(disp_adapter_t *adapter, am335x_hdmi_t *dev, uint32_t addr, uint32_t reg, uint32_t val)
{
    struct {
        i2c_send_t      hdr;
        uint8_t        	reg;
        uint8_t        	val;
    } msg;

#ifdef AM335X_HDMI_DEBUG
    disp_printf(adapter, "access i2c address %d\n", addr);
#endif
    msg.hdr.slave.addr = addr;
    msg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msg.hdr.len        = 2;
    msg.hdr.stop       = 1;
    msg.reg            = reg;
    msg.val            = val;

    if (devctl (dev->i2c_fd, DCMD_I2C_SEND, &msg, sizeof(msg), NULL))
    {
    	disp_printf(adapter, "HDMI I2C_WRITE failed");
    	return -1;
    }
#ifdef AM335X_HDMI_DEBUG
    disp_printf(adapter, "HDMI: Wrote [0x%02x] = 0x%02x", reg, val);
#endif

    return (EOK);
}

static uint8_t hdmi_read (disp_adapter_t *adapter, am335x_hdmi_t *dev, uint32_t addr, uint32_t reg)
{
    struct {
        i2c_send_t      hdr;
        uint8_t	        reg;
    } msgreg;

    struct {
        i2c_recv_t      hdr;
        uint8_t 		val;
    } msgval;
    int rbytes;

    msgreg.hdr.slave.addr = addr;
    msgreg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgreg.hdr.len        = 1;
    msgreg.hdr.stop       = 0;
    msgreg.reg            = reg;

    if (devctl(dev->i2c_fd, DCMD_I2C_SEND, &msgreg, sizeof(msgreg), NULL))
    {
    	disp_printf(adapter, "HDMI I2C read failed (DCMD_I2C_SEND)");
    	return (-1);
    }

    msgval.hdr.slave.addr = addr;
    msgval.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgval.hdr.len        = 1;
    msgval.hdr.stop       = 1;

    if (devctl(dev->i2c_fd, DCMD_I2C_RECV, &msgval, sizeof(msgval), &rbytes))
    {
    	disp_printf(adapter, "HDMI I2C read failed (DCMD_I2C_RECV)");
        return (-1);
    }

#ifdef AM335X_HDMI_DEBUG
    disp_printf(adapter, "HDMI: Read [0x%02x] = 0x%02x", reg, msgval.val);
#endif

    return (msgval.val);
}

int init_hdmi(disp_adapter_t *adapter, omapl1xx_context_t *omapl1xx)
{
	am335x_hdmi_t *dev;
#if 0
	unsigned	 gpio5_len;
	uintptr_t	 gpio5_regs;
	unsigned	 gpio5_base;
#endif
	int			 i;
	int			 ret = -1;

	dev = (am335x_hdmi_t *)malloc(sizeof(am335x_hdmi_t));
	if (!dev)
	{
		disp_printf(adapter, "HDMI: out of memory trying to allocate %d bytes", sizeof(am335x_hdmi_t));
		return -1;
	}

	omapl1xx->hdmi_dev  = dev;

	dev->cable_state = HDMI_CABLE_NOTCONNECTED;		/* Cable not plugged in */

	/* If set to DVI, the driver should work on both DVI and HDMI monitors. */
	/* If set to HDMI, only full HDMI monitors will work. To support the    */
	/* largest number of monitors it is therefore set to 0. Note that this  */
	/*  BSP doesn't support HDMI audio.                                     */
	dev->hdmi_cap    = 0;							/* HDMI: 1, DVI: 0      */

	/* Set I2C device node */
	strncpy(dev->i2c_dev, "/dev/i2c1", MAX_STRLEN);

	/* Connect to the Sii9022A via I2C */
	dev->i2c_fd = open (dev->i2c_dev, O_RDWR);
	if (dev->i2c_fd == -1)
	{
		disp_printf(adapter, "HDMI: Failed to open I2C device: %s", dev->i2c_dev);
		free(dev);
		disp_printf(adapter, "HDMI: device failed to initialise");
		return -1;
	}

#if 0
	/* Map GPIO5 registers */
	gpio5_base = MX53_GPIO5_BASE;
	gpio5_len  = MX53_GPIO_SIZE;
	gpio5_regs = mmap_device_io(gpio5_len, gpio5_base);
	if(gpio5_regs == (uintptr_t)MAP_FAILED)
	{
		disp_printf(adapter, "HDMI: unable to map GPIO5, won't reset HDMI chip.", HDMI_ADDR);
	}
	else
	{
		/* Set DISP0_RESET as GPIO output */
		out32(gpio5_regs + MX53_GPIO_GDIR, in32(gpio5_regs + MX53_GPIO_GDIR) | (1<<0));

		/* Pulse the line to low and back to reset the chip */
		out32(gpio5_regs + MX53_GPIO_DR, in32(gpio5_regs + MX53_GPIO_DR) | (0<<0));
		delay(50);
		out32(gpio5_regs + MX53_GPIO_DR, in32(gpio5_regs + MX53_GPIO_DR) | (1<<0));
		delay(50);

		munmap_device_io(gpio5_regs, gpio5_len);
	}
#endif

	/* Set TPI mode on and exit D3 state */
	if (hdmi_write(adapter, dev, HDMI_ADDR, 0xC7, 0x00) < 0)
	{
		disp_printf(adapter, "HDMI: Could not find Sii9022A device at address 0x%02X", HDMI_ADDR);
		ret = -ENODEV;
		close(dev->i2c_fd);
		free(dev);
		disp_printf(adapter, "HDMI: device failed to initialise");
		return -1;
	}
	delay(50);

	/* HDMI device ID */
	dev->devid[0] = 0;
	dev->devid[1] = 0;
	dev->devid[2] = 0;
	dev->devid[3] = 0;

	/* Ensure TPI mode is on */
	for (i = 10; i > 0; i--)
	{
		dev->devid[0] = hdmi_read(adapter, dev, HDMI_ADDR, 0x1B);
#ifdef AM335X_HDMI_DEBUG
		disp_printf(adapter, "HDMI: read id = 0x%02X", dev->devid[0]);
#endif
		if (dev->devid[0] == 0xb0)
		{
			dev->devid[1] = hdmi_read(adapter, dev, HDMI_ADDR, 0x1C);
#ifdef AM335X_HDMI_DEBUG
			disp_printf(adapter, "HDMI: read id = 0x%02X", dev->devid[1]);
#endif
			dev->devid[2] = hdmi_read(adapter, dev, HDMI_ADDR, 0x1D);
#ifdef AM335X_HDMI_DEBUG
			disp_printf(adapter, "HDMI: read id = 0x%02X", dev->devid[2]);
#endif
			dev->devid[3] = hdmi_read(adapter, dev, HDMI_ADDR, 0x30);
#ifdef AM335X_HDMI_DEBUG
			disp_printf(adapter, "HDMI: read id = 0x%02X", dev->devid[3]);
#endif
			if ((dev->devid[1]==0x00)&&(dev->devid[2]==0x00)&&(dev->devid[3]==0x00))
				disp_printf(adapter, "HDMI: SiI9022 Tx");
			else
			if ((dev->devid[1]==0x02)&&(dev->devid[2]==0x03)&&(dev->devid[3]==0x00))
				disp_printf(adapter, "HDMI: SiI9022A Tx");
			else
			if ((dev->devid[1]==0x00)&&(dev->devid[2]==0x00)&&(dev->devid[3]==0x12))
				disp_printf(adapter, "HDMI: SiI9024 Tx");
			else
			if ((dev->devid[1]==0x02)&&(dev->devid[2]==0x03)&&(dev->devid[3]==0x12))
				disp_printf(adapter, "HDMI: SiI9024A Tx");
			else
			if ((dev->devid[1]==0x20)&&(dev->devid[2]==0x30)&&(dev->devid[3]==0x12))
				disp_printf(adapter, "HDMI: SiI9136-3/9136/9334 Tx");
			else
				disp_printf(adapter, "HDMI: unknown device");
			break;
		}
	}
	if (i == 0)
	{
		disp_printf(adapter, "HDMI: device failed to initialise");
		return -ENODEV;
	}

	return 0;
}

int uninit_hdmi(disp_adapter_t *adapter, omapl1xx_context_t *omapl1xx)
{
	int r;

	if (omapl1xx->hdmi_dev != NULL)
	{
#ifdef AM335X_HDMI_DEBUG
		printf("UNINIT\n");
#endif

		hdmi_thread_uninit(adapter, omapl1xx->hdmi_dev);

		r = close(omapl1xx->hdmi_dev->i2c_fd);
		if (r)
		{
			disp_printf(adapter, "HDMI: Failed to close i2c.");
		}

		free(omapl1xx->hdmi_dev);
		omapl1xx->hdmi_dev = NULL;
	}

	return 0;
}

int setup_hdmi(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
	omapl1xx_context_t	*imx_cxt = adapter->ms_ctx;
	uint16_t		data[4];
	uint8_t			*ptr = (uint8_t *)data;
	int				i;

	/* Power up HDMI transmitter */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x1E, 0x00);
	hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x01);

	/* Configure resolution */
	if (imx_cxt->pcd)
		data[0] = 20000/imx_cxt->pcd; // imx_cxt->pix_clk_freq/10000;
	else
		data[0] = 20000;
	data[2] = imx_cxt->lcd_xres; // imx_cxt->xres;
	data[3] = imx_cxt->lcd_yres; // imx_cxt->yres;
	data[1] = imx_cxt->refresh;
#ifdef AM335X_HDMI_DEBUG
	printf("data[0] = %04d (freq in kHz / 10)\n", data[0]);
	printf("data[1] = %04d (refresh in Hz)\n", data[1]);
	printf("data[2] = %04d (xres)\n", data[2]);
	printf("data[3] = %04d (yres)\n", data[3]);
	printf("pixclk  = %d kHz [pcd=%d]\n", data[0]*10, imx_cxt->pcd);
#endif
	for(i=0;i<8;i++)
		hdmi_write(adapter, dev, HDMI_ADDR, i, ptr[i]);

#ifdef AM335X_HDMI_DEBUG
	for(i=0;i<8;i++)
		printf("%d : 0x%02X\n", i, hdmi_read(adapter, dev, HDMI_ADDR, i));
#endif

	/* Configure input bus and pixel repetition */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x08, 0x70);

	/* Configure YC input mode - 20 */
	//hdmi_write(dev, HDMI_ADDR, 0x0B, 0x70);

	/* Configure Sync Methods - 19 */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x60, 0x04);

	/* Configure explicit sync DE generation */

	/* Configure embedded sync extraction */

	/* Configure RGB input */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x09, 0x00);

	/* Configure RGB output */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x0A, 0x00);

	/* Disable interrupts. We use a polling thread. */
	hdmi_write(adapter, dev, HDMI_ADDR, 0x3C, 0x00);

	/* Turn on DVI or HDMI */
	if (dev->hdmi_cap)
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x01);
	else
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x00);

	for (i=0;i<14;i++)
		hdmi_write(adapter, dev, HDMI_ADDR, 0x0C+i, vidinfoframe[i]);

    hdmi_thread_init(adapter, dev);

	return (EOK);
}

/*****************************************************************************/
/* hdmi_cable_state()                                                        */
/*****************************************************************************/

static void	hdmi_state_loop(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
	while (1)
	{
		delay(250);
		hdmi_state(adapter, dev);
	}
}

/*****************************************************************************/
/* hdmi_cable_state_handler()                                                */
/*****************************************************************************/

typedef struct hdmi_pointers
{
	am335x_hdmi_t    *dev;
	disp_adapter_t *adapter;
} HDMI_POINTERS;

static void *hdmi_state_handler(void *data)
{
	HDMI_POINTERS *hdmi = (HDMI_POINTERS *)data;
	hdmi_state_loop(hdmi->adapter, hdmi->dev);
	return (NULL);
}

/*****************************************************************************/
/* hdmi_thread_init()                                                        */
/*****************************************************************************/
#define	HDMI_PRIORITY			21

static int hdmi_thread_init(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
	pthread_attr_t			pattr;
	struct sched_param		param;
	HDMI_POINTERS			*hdmi_ptrs = NULL;

	hdmi_ptrs = malloc(sizeof(HDMI_POINTERS));
	if (hdmi_ptrs == NULL)
	{
		disp_printf(adapter, "HDMI: Unable to create thread (errno=%d).", errno);
		return -1;
	}
	hdmi_ptrs->adapter = adapter;
	hdmi_ptrs->dev     = dev;

	pthread_attr_init (&pattr);
	pthread_attr_setschedpolicy (&pattr, SCHED_RR);
	param.sched_priority = HDMI_PRIORITY;
	pthread_attr_setschedparam (&pattr, &param);
	pthread_attr_setinheritsched (&pattr, PTHREAD_EXPLICIT_SCHED);

	if (pthread_create (&dev->tid, &pattr, (void *) hdmi_state_handler, hdmi_ptrs))
	{
		free(hdmi_ptrs);
		disp_printf(adapter, "HDMI: Unable to create thread (errno=%d).", errno);
		return -1;
	}

	return 0;
}

static int hdmi_thread_uninit(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
	if (pthread_cancel(dev->tid))
	{
		disp_printf(adapter, "HDMI: Unable to destroy thread (errno=%d).", errno);
		return -1;
	}

	return 0;
}

static void hdmi_poweron(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
#ifdef AM335X_HDMI_DEBUG
	printf("power ON\n");
#endif
	/* Turn on DVI or HDMI */
	if (dev->hdmi_cap)
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x01);
	else
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x00);
	return;
}

static void hdmi_poweroff(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
#ifdef AM335X_HDMI_DEBUG
	printf("power OFF\n");
#endif
	/* Turn off DVI or HDMI */
	if (dev->hdmi_cap)
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x11);
	else
		hdmi_write(adapter, dev, HDMI_ADDR, 0x1A, 0x10);

	return;
}

static void hdmi_state(disp_adapter_t *adapter, am335x_hdmi_t *dev)
{
	int val;

	val = hdmi_read(adapter, dev, HDMI_ADDR, 0x3D);
	if (val & 0x1)
	{
		/* Cable still present? */
		if (val & 0x4)
		{
			dev->cable_state = HDMI_CABLE_CONNECTED;
			hdmi_poweron(adapter, dev);
		}
		else
		{
			dev->cable_state = HDMI_CABLE_NOTCONNECTED;
			hdmi_poweroff(adapter, dev);
		}
	}
	val = hdmi_read(adapter, dev, HDMI_ADDR, 0x3D);
}
