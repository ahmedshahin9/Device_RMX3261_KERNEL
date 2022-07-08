/*
 * Copyright (C) 2015-2016 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/module.h>
#include "sprd_img.h"
#include "flash_drv.h"

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "FLASH_OCP8132: %d %d %s : " fmt, current->pid, __LINE__, __func__

#define FLASH_DRIVER_NAME "flash-ocp8132"
#define FLASH_GPIO_MAX 2
#define FLASH_MAX_PWM 16
#define FLASH_MAX_LEVEL 5
#define FLASH_MIN_LEVEL 0

enum {
	GPIO_FLASH_TORCH_MODE,	// 138
	GPIO_CHIP_EN,		// 137
};

struct flash_driver_data {
	int gpio_tab[FLASH_GPIO_MAX];
	u32 torch_led_index;
};

static const char *const flash_gpio_names[FLASH_GPIO_MAX] = {
	"flash-torch-en-gpios",	/* 138  for torch/flash mode */
	"flash-en-gpios",	/* 137  for enable ic pin */
};

static int g_high_level = 0;
static int highlight_opened = 0;
spinlock_t flash_lock;

static int sprd_flash_ocp8132_open_pwm(void *drvd, uint8_t idx, int level)
{
	int ret = 0;
	int i = 0;
	int gpio_id = 0;
	unsigned long flags;

	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;
	if (!drv_data)
		return -EFAULT;

	if (level > FLASH_MAX_LEVEL)
		level = FLASH_MAX_LEVEL;

	if (level < FLASH_MIN_LEVEL)
		level = FLASH_MIN_LEVEL;

	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_CHIP_EN];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_ON);
		}
	}

	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			udelay(550);
			spin_lock_irqsave(&flash_lock, flags);
			for (i = 0; i < FLASH_MAX_PWM - level; i++) {
				pr_info("open_pwm:%d\n", i);
				ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
				udelay(2);
				ret = gpio_direction_output(gpio_id, SPRD_FLASH_ON);
				udelay(2);
			}
			spin_unlock_irqrestore(&flash_lock, flags);
		}
	}
	return ret;
}

static int sprd_flash_ocp8132_init(void *drvd)
{
	int ret = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;

	return ret;
}

static int sprd_flash_ocp8132_deinit(void *drvd)
{
	int ret = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;

	return ret;
}

static int sprd_flash_ocp8132_open_torch(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;

	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_ON);
			if (ret) {
				goto exit;
			}
		}
	}

exit:
	return ret;
}

static int sprd_flash_ocp8132_close_torch(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_CHIP_EN];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}

	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}
exit:
	return ret;
}

static int sprd_flash_ocp8132_open_preflash(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_ON);
			if (ret) {
				goto exit;
			}
		}
	}

exit:
	return ret;
}

static int sprd_flash_ocp8132_close_preflash(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_CHIP_EN];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}

	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}
exit:
	return ret;
}

static int sprd_flash_ocp8132_open_highlight(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;
	if (!drv_data)
			return -EFAULT;

	pr_info("highlight_opened:%d\n", highlight_opened);
	if(1 ==  highlight_opened) {
		gpio_id = drv_data->gpio_tab[GPIO_CHIP_EN];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
		udelay(550);
	}
	idx = drv_data->torch_led_index;
	ret = sprd_flash_ocp8132_open_pwm(drv_data, idx, g_high_level);
	if (ret)
		goto exit;
	highlight_opened = 1;
exit:
	return ret;
}

static int sprd_flash_ocp8132_close_highlight(void *drvd, uint8_t idx)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;

	if (!drv_data)
		return -EFAULT;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_CHIP_EN];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}

	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}
	highlight_opened = 0;
exit:
	return ret;
}

static int sprd_flash_ocp8132_cfg_value_torch(void *drvd, uint8_t idx,
					     struct sprd_flash_element *element)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}
exit:
	return ret;
}

static int sprd_flash_ocp8132_cfg_value_preflash(void *drvd, uint8_t idx, struct sprd_flash_element
						*element)
{
	int ret = 0;
	int gpio_id = 0;
	struct flash_driver_data *drv_data = (struct flash_driver_data *)drvd;
	pr_info("torch_led_index:%d, idx:%d\n", drv_data->torch_led_index, idx);
	idx = drv_data->torch_led_index;
	if (SPRD_FLASH_LED0 & idx) {
		gpio_id = drv_data->gpio_tab[GPIO_FLASH_TORCH_MODE];
		if (gpio_is_valid(gpio_id)) {
			ret = gpio_direction_output(gpio_id, SPRD_FLASH_OFF);
			if (ret)
				goto exit;
		}
	}
exit:
	return ret;
}

static int sprd_flash_ocp8132_cfg_value_highlight(void *drvd, uint8_t idx, struct sprd_flash_element
						 *element)
{
	int ret = 0;
	pr_info("element->index:%d\n", element->index);
	g_high_level = element->index;
	return ret;
}

static const struct of_device_id ocp8132_flash_of_match_table[] = {
	{.compatible = "sprd,flash-ocp8132"},
};

static const struct sprd_flash_driver_ops flash_gpio_ops = {
	.open_torch = sprd_flash_ocp8132_open_torch,
	.close_torch = sprd_flash_ocp8132_close_torch,
	.open_preflash = sprd_flash_ocp8132_open_preflash,
	.close_preflash = sprd_flash_ocp8132_close_preflash,
	.open_highlight = sprd_flash_ocp8132_open_highlight,
	.close_highlight = sprd_flash_ocp8132_close_highlight,
	.cfg_value_torch = sprd_flash_ocp8132_cfg_value_torch,
	.cfg_value_preflash = sprd_flash_ocp8132_cfg_value_preflash,
	.cfg_value_highlight = sprd_flash_ocp8132_cfg_value_highlight,
};

static int sprd_flash_ocp8132_probe(struct platform_device *pdev)
{
	int ret = 0;
	u32 gpio_node = 0;
	struct flash_driver_data *drv_data = NULL;
	int gpio[FLASH_GPIO_MAX];
	int j;

	if (IS_ERR_OR_NULL(pdev))
		return -EINVAL;

	if (!pdev->dev.of_node) {
		pr_err("no device node %s", __func__);
		return -ENODEV;
	}
	pr_info("flash-ocp8132 probe\n");

	ret = of_property_read_u32(pdev->dev.of_node, "flash-ic", &gpio_node);
	if (ret) {
		pr_err("no gpio flash\n");
		return -ENODEV;
	}

	drv_data = devm_kzalloc(&pdev->dev, sizeof(*drv_data), GFP_KERNEL);
	if (!drv_data)
		return -ENOMEM;

	pdev->dev.platform_data = (void *)drv_data;

	ret = of_property_read_u32(pdev->dev.of_node,
				   "torch-led-idx", &drv_data->torch_led_index);
	if (ret)
		drv_data->torch_led_index = SPRD_FLASH_LED0;

	for (j = 0; j < FLASH_GPIO_MAX; j++) {
		gpio[j] = of_get_named_gpio(pdev->dev.of_node,
					    flash_gpio_names[j], 0);
		if (gpio_is_valid(gpio[j])) {
			ret = devm_gpio_request(&pdev->dev,
						gpio[j], flash_gpio_names[j]);

			if (ret) {
				pr_err("flash gpio err\n");
				goto exit;
			}

			ret = gpio_direction_output(gpio[j], SPRD_FLASH_OFF);

			if (ret) {
				pr_err("flash gpio output err\n");
				goto exit;
			}
		}
	}

	memcpy((void *)drv_data->gpio_tab, (void *)gpio, sizeof(gpio));

	ret = sprd_flash_ocp8132_init(drv_data);
	if (ret)
		goto exit;
	ret = sprd_flash_register(&flash_gpio_ops, drv_data, SPRD_FLASH_REAR);

exit:
	return ret;
}

static int sprd_flash_ocp8132_remove(struct platform_device *pdev)
{
	int ret = 0;

	ret = sprd_flash_ocp8132_deinit(pdev->dev.platform_data);

	return ret;
}

static struct platform_driver sprd_flash_ocp8132_driver = {
	.probe = sprd_flash_ocp8132_probe,
	.remove = sprd_flash_ocp8132_remove,
	.driver = {
		   .name = FLASH_DRIVER_NAME,
		   .of_match_table = of_match_ptr(ocp8132_flash_of_match_table),
		   },
};

module_platform_driver(sprd_flash_ocp8132_driver);
MODULE_LICENSE("GPL");
