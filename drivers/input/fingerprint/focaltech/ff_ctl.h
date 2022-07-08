/*
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
**/

#ifndef __FF_CTRL_API_H__
#define __FF_CTRL_API_H__

#include <linux/ioctl.h>

/* Device node. */
#define FF_CTL_DEV_NAME "/dev/focaltech_fp"

/* Max driver version buffer length. */
#define FF_DRV_VERSION_LEN 32
#define FF_DRV_IC_INFORMATION_LEN 64

typedef struct {
    unsigned int code;
    int value;
} ff_key_event_t;

typedef struct {
    /* Using asynchronous notification mechanism instead of NETLINK. */
    bool enable_fasync;

    /* Gesture(Key emulation & Navigation) key codes. */
    int32_t keycode_nav_left;
    int32_t keycode_nav_right;
    int32_t keycode_nav_up;
    int32_t keycode_nav_down;
    int32_t keycode_double_click;
    int32_t keycode_click;
    int32_t keycode_long_press;
    int32_t keycode_simulation;

    /* For '/dev/spidevB.C' of REE-Emulation. */
    bool enable_spidev;
    int32_t spidev_bus;
    int32_t spidev_c_s;

    /* For obsolete driver that doesn't support device tree. */
    int32_t gpio_mosi_pin;
    int32_t gpio_miso_pin;
    int32_t gpio_ck_pin;
    int32_t gpio_cs_pin;
    int32_t gpio_rst_pin;
    int32_t gpio_int_pin;
    int32_t gpio_power_pin;
    int32_t gpio_iovcc_pin;

    /* Logging driver to logcat through uevent mechanism. */
    int32_t log_level;
    bool logcat_driver;
} ff_driver_config_t;

typedef struct {
    char ic_name[FF_DRV_IC_INFORMATION_LEN];
    char ca_version[FF_DRV_IC_INFORMATION_LEN];
    char ta_version[FF_DRV_IC_INFORMATION_LEN];
} ic_information_t;

typedef struct {
    short bus;
    unsigned char cs;
} ff_spidev_info_t;

/* Get the string of the command. */
#define FF_CTL_CMD_NAME(cmd) (#cmd)

/* Magic code for IOCTL-subsystem, 'f'(0x66) means '[F]ocalTech'. */
#define FF_IOC_MAGIC 'f'

/* Allocate/Release driver resource (GPIO/SPI etc.). */
#define FF_IOC_INIT_DRIVER      _IO(FF_IOC_MAGIC, 0x00)
#define FF_IOC_FREE_DRIVER      _IO(FF_IOC_MAGIC, 0x01)

/* HW reset the fingerprint module. */
#define FF_IOC_RESET_DEVICE     _IO(FF_IOC_MAGIC, 0x02)

/* Low-level IRQ control. */
#define FF_IOC_ENABLE_IRQ       _IO(FF_IOC_MAGIC, 0x03)
#define FF_IOC_DISABLE_IRQ      _IO(FF_IOC_MAGIC, 0x04)

/* SPI bus clock control, for power-saving purpose. */
#define FF_IOC_ENABLE_SPI_CLK   _IO(FF_IOC_MAGIC, 0x05)
#define FF_IOC_DISABLE_SPI_CLK  _IO(FF_IOC_MAGIC, 0x06)

/* Fingerprint module power control. */
#define FF_IOC_ENABLE_POWER     _IO(FF_IOC_MAGIC, 0x07)
#define FF_IOC_DISABLE_POWER    _IO(FF_IOC_MAGIC, 0x08)

/* Androind system-wide key event, for navigation purpose. */
#define FF_IOC_REPORT_KEY_EVENT _IOW(FF_IOC_MAGIC, 0x09, ff_key_event_t)

/* Sync 'ff_driver_config_t', the driver configuration. */
#define FF_IOC_SYNC_CONFIG     _IOWR(FF_IOC_MAGIC, 0x0a, ff_driver_config_t)

/* Query the driver version string. */
#define FF_IOC_GET_VERSION      _IOR(FF_IOC_MAGIC, 0x0b, const char)

#define FF_IOC_SET_IC_INFORMATION _IOW(FF_IOC_MAGIC, 0x0c, ic_information_t)
#define FF_IOC_GET_IC_INFORMATION _IOR(FF_IOC_MAGIC, 0x0d, ic_information_t)

#define FF_IOC_GET_SPIDEV_INFO  _IOR(FF_IOC_MAGIC, 0x0e, ff_spidev_info_t)

#endif /* __FF_CTRL_API_H__ */
