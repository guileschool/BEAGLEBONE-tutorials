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
#include "omapl1xx.h"

static int
omapl1xx_mem_init(disp_adapter_t *adapter, char *optstring)
{
	return 0;
}

static void
omapl1xx_mem_fini(disp_adapter_t *adapter)
{
	return;
}

static int
omapl1xx_mem_reset(disp_adapter_t *adapter, disp_surface_t *surf)
{
	return 0;
}

static disp_surface_t *
omapl1xx_alloc_surface(disp_adapter_t *adapter, int width, int height, unsigned format, unsigned flags, unsigned user_flags)
{
	return NULL;
}

static disp_surface_t *
omapl1xx_alloc_layer_surface(disp_adapter_t *adapter, int dispno[],
    int layer_idx[], int nlayers, unsigned format, int surface_index,
    int width, int height, unsigned sflags, unsigned hint_flags)
{
	return NULL;
}

static int
omapl1xx_free_surface(disp_adapter_t *adapter, disp_surface_t *surf)
{
	return 0;
}

static unsigned long
omapl1xx_mem_avail(disp_adapter_t *adapter, unsigned flags)
{
	return 0;
}

static int
omapl1xx_query_apertures(disp_adapter_t *adapter, disp_aperture_t *ap)
{
	return 0;
}

static int
omapl1xx_query_surface(disp_adapter_t *adapter,
    disp_surface_t *surf, disp_surface_info_t *info)
{
	return 0;
}

/*
 * If a client of the driver wants to allocate memory itself,
 * it must allocate it in accordance with the parameters returned by
 * this function.  Since this memory will not be coming from
 * video memory, we must check the flags accordingly.
 */
static int
omapl1xx_get_alloc_info(disp_adapter_t *adapter,
    int width, int height, unsigned format,
    unsigned flags, unsigned user_flags, disp_alloc_info_t *info)
{
	int			bpp = DISP_BYTES_PER_PIXEL(format);
	int			stride = width * bpp;
	int			tmp = stride / 32;
	unsigned	reject_flags;

	switch (format) {
		case DISP_SURFACE_FORMAT_RGB565:
			reject_flags =
				DISP_SURFACE_2D_READABLE |
				DISP_SURFACE_2D_TARGETABLE | 
				DISP_SURFACE_3D_READABLE |
				DISP_SURFACE_3D_TARGETABLE;
			break;
		case DISP_SURFACE_FORMAT_RGB888:
			reject_flags =
				DISP_SURFACE_2D_READABLE |
				DISP_SURFACE_2D_TARGETABLE |
				DISP_SURFACE_3D_READABLE |
				DISP_SURFACE_3D_TARGETABLE;
			break;
		case DISP_SURFACE_FORMAT_ARGB8888:
			reject_flags =
				DISP_SURFACE_2D_READABLE |
				DISP_SURFACE_2D_TARGETABLE |
				DISP_SURFACE_3D_READABLE |
				DISP_SURFACE_3D_TARGETABLE;
			break;
		default:
			reject_flags =
				DISP_SURFACE_DISPLAYABLE |
				DISP_SURFACE_2D_READABLE |
				DISP_SURFACE_2D_TARGETABLE | 
				DISP_SURFACE_3D_READABLE |
				DISP_SURFACE_3D_TARGETABLE;
			break;
	}

	if (flags & reject_flags) {
		return -1;
	}

	flags &= ~(DISP_SURFACE_CAPS_MASK | reject_flags);

	if (flags & ~(DISP_SURFACE_PAGE_ALIGNED|DISP_SURFACE_PHYS_CONTIG)) {
		return -1;
	}

	if (tmp * 32 != stride) {
		stride = (tmp + 1) * 32;
	}

	info->start_align = 32;
	info->end_align = 1;
	info->min_stride = stride;
	info->max_stride = ~0;
	info->stride_gran = 32;
	info->map_flags = DISP_MAP_PHYS;
	info->prot_flags = DISP_PROT_READ | DISP_PROT_WRITE | DISP_PROT_NOCACHE;
	info->surface_flags = ~reject_flags &
	    (DISP_SURFACE_DRIVER_NOT_OWNER |
	    DISP_SURFACE_CPU_LINEAR_READABLE |
	    DISP_SURFACE_CPU_LINEAR_WRITEABLE |
	    DISP_SURFACE_2D_READABLE | DISP_SURFACE_2D_TARGETABLE | 
	    DISP_SURFACE_3D_READABLE | DISP_SURFACE_3D_TARGETABLE |
	    DISP_SURFACE_PAGE_ALIGNED | DISP_SURFACE_PHYS_CONTIG);

	return 0;
}

static int
omapl1xx_get_alloc_layer_info(disp_adapter_t *adapter, int dispno[], int layer_idx[],
    int nlayers, unsigned format, int surface_index, int width, int height,
    unsigned sflags, unsigned hint_flags, disp_alloc_info_t *info)
{
	unsigned			alloc_format;
	int					stride;
	unsigned			reject_flags;

	if (surface_index > 0) {
		return -1;
	}

	if (format == DISP_LAYER_FORMAT_RGB565)
	{
		alloc_format = DISP_SURFACE_FORMAT_RGB565;
		stride = width * 2;

	} 
	else
	if (format == DISP_LAYER_FORMAT_RGB888)
	{
		alloc_format = DISP_SURFACE_FORMAT_RGB888;
		stride = width * 3;

	}
	else
	if (format == DISP_LAYER_FORMAT_ARGB8888)
	{
		alloc_format = DISP_SURFACE_FORMAT_ARGB8888;
		stride = width * 4;

	}
	else
	{
		return -1;
	}

	reject_flags =
		DISP_SURFACE_2D_READABLE |
		DISP_SURFACE_2D_TARGETABLE |
		DISP_SURFACE_3D_READABLE |
		DISP_SURFACE_3D_TARGETABLE;

	sflags &= DISP_SURFACE_CAPS_MASK | reject_flags;

	info->start_align = 8;
	info->end_align = 1;
	info->min_stride = stride;
	info->max_stride = ~0;
	info->stride_gran = 8;
	info->map_flags = DISP_MAP_PHYS;
	info->prot_flags = DISP_PROT_READ | DISP_PROT_WRITE | DISP_PROT_NOCACHE;
	info->surface_flags = ~reject_flags &
		(DISP_SURFACE_DRIVER_NOT_OWNER |
		DISP_SURFACE_CPU_LINEAR_READABLE |
		DISP_SURFACE_CPU_LINEAR_WRITEABLE |
		DISP_SURFACE_2D_READABLE | DISP_SURFACE_2D_TARGETABLE |
		DISP_SURFACE_3D_READABLE | DISP_SURFACE_3D_TARGETABLE |
		DISP_SURFACE_PAGE_ALIGNED | DISP_SURFACE_PHYS_CONTIG);

	return 0;
}


int
devg_get_memfuncs(disp_adapter_t *adapter, disp_memfuncs_t *funcs, int tabsize)
{
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, init, omapl1xx_mem_init, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, fini, omapl1xx_mem_fini, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, module_info, omapl1xx_module_info, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, reset, omapl1xx_mem_reset, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, alloc_surface, omapl1xx_alloc_surface, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, alloc_layer_surface, omapl1xx_alloc_layer_surface, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, free_surface, omapl1xx_free_surface, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, mem_avail, omapl1xx_mem_avail, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, query_apertures, omapl1xx_query_apertures, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, query_surface, omapl1xx_query_surface, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, get_alloc_info, omapl1xx_get_alloc_info, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, get_alloc_layer_info, omapl1xx_get_alloc_layer_info, tabsize);

	return 0;
}
