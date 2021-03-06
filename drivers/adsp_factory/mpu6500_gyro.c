/*
*  Copyright (C) 2012, Samsung Electronics Co. Ltd. All Rights Reserved.
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*/
#include <linux/init.h>
#include <linux/module.h>
#include "adsp.h"
#define VENDOR		"Invensense"
#define CHIP_ID		"MPU6500"

static ssize_t gyro_vendor_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	adsp_unicast(ADSP_FACTORY_MODE_GYRO,NETLINK_MESSAGE_GET_RAW_DATA,0,0);
	return sprintf(buf, "%s\n", VENDOR);
}

static ssize_t gyro_name_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", CHIP_ID);
}

static ssize_t gyro_calibration_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct adsp_data *data = dev_get_drvdata(dev);
	int iCount = 0;
	if( !(data->calib_ready_flag & 1 << ADSP_FACTORY_MODE_GYRO) ){
		adsp_unicast(ADSP_FACTORY_MODE_GYRO,NETLINK_MESSAGE_GET_CALIB_DATA,0,0);
	}
	while( !(data->calib_ready_flag & 1 << ADSP_FACTORY_MODE_GYRO) )
		msleep(20);

	iCount = snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d\n",  data->sensor_calib_data[ADSP_FACTORY_MODE_GYRO].result,
		data->sensor_calib_data[ADSP_FACTORY_MODE_GYRO].x,
		data->sensor_calib_data[ADSP_FACTORY_MODE_GYRO].y,
		data->sensor_calib_data[ADSP_FACTORY_MODE_GYRO].z);

	return iCount;
}

static ssize_t gyro_calibration_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	return size;
}

static ssize_t raw_data_read(struct device *dev,
struct device_attribute *attr, char *buf)
{
	struct adsp_data *data = dev_get_drvdata(dev);
	if( !(data->data_ready_flag & 1 << ADSP_FACTORY_MODE_GYRO) ){
		adsp_unicast(ADSP_FACTORY_MODE_GYRO,NETLINK_MESSAGE_GET_RAW_DATA,0,0);
	}
	while( !(data->data_ready_flag & 1 << ADSP_FACTORY_MODE_GYRO) )
		msleep(20);
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d\n",
		data->sensor_data[ADSP_FACTORY_MODE_GYRO].x,
		data->sensor_data[ADSP_FACTORY_MODE_GYRO].y,
		data->sensor_data[ADSP_FACTORY_MODE_GYRO].z);
}

int gyro_factory_init(struct adsp_data *data)
{
	return 0;
}
int gyro_factory_exit(struct adsp_data *data)
{
	return 0;
}
int gyro_factory_receive_data(struct adsp_data *data)
{
	pr_err("(%s): factory \n", __func__);
	return 0;
}

static struct adsp_fac_ctl adsp_fact_cb = {
	.init_fnc = gyro_factory_init,
	.exit_fnc = gyro_factory_exit,
	.receive_data_fnc = gyro_factory_receive_data
};

static DEVICE_ATTR(name, S_IRUGO, gyro_name_show, NULL);
static DEVICE_ATTR(vendor, S_IRUGO, gyro_vendor_show, NULL);
static DEVICE_ATTR(calibration, S_IRUGO | S_IWUSR | S_IWGRP,
	gyro_calibration_show, gyro_calibration_store);
static DEVICE_ATTR(raw_data, S_IRUGO, raw_data_read, NULL);

static struct device_attribute *gyro_attrs[] = {
	&dev_attr_name,
	&dev_attr_vendor,
	&dev_attr_calibration,
	&dev_attr_raw_data,
	NULL,
};

static int __devinit mpu6500gyro_factory_init(void)
{
	adsp_factory_register("gyro_sensor",ADSP_FACTORY_MODE_GYRO,gyro_attrs,&adsp_fact_cb);
	pr_err("(%s): factory \n", __func__);
	return 0;
}
static void __devexit mpu6500gyro_factory_exit(void)
{
	pr_err("(%s): factory \n", __func__);
}
module_init(mpu6500gyro_factory_init);
module_exit(mpu6500gyro_factory_exit);
