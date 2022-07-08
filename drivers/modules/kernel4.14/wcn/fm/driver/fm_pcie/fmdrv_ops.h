/*
 *  FM Drivr for Connectivity chip of Spreadtrum.
 *
 *  FM operations module header.
 *
 *  Copyright (C) 2015 Spreadtrum Company
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 */

#ifndef _FMDRV_OPS_H
#define _FMDRV_OPS_H

extern struct fmdrv_ops *fmdev;
//extern struct wake_lock fm_wakelock;
int  fm_device_init_driver(void);
void fm_device_exit_driver(void);

#endif
