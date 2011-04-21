/*
    Copyright (C) 2010-2011 wpitchoune@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
*/

#ifndef _PSENSOR_PSENSOR_H_
#define _PSENSOR_PSENSOR_H_

#include "config.h"
#include <sensors/sensors.h>

#include "color.h"
#include "measure.h"

enum psensor_type {
	SENSOR_TYPE_TEMP = 0x0001,
	SENSOR_TYPE_FAN = 0x0002,
	SENSOR_TYPE_REMOTE = 0x0004,

	SENSOR_TYPE_LMSENSOR_TEMP = 0x0100 | SENSOR_TYPE_TEMP,
	SENSOR_TYPE_NVIDIA = 0x0200 | SENSOR_TYPE_TEMP,
	SENSOR_TYPE_HDD_TEMP = 0x0300 | SENSOR_TYPE_TEMP,
	SENSOR_TYPE_LMSENSOR_FAN = 0x0400 | SENSOR_TYPE_FAN
};

struct psensor {
	/* Human readable name of the sensor.  It may not be uniq. */
	char *name;

	/* Uniq id of the sensor */
	char *id;

	/* lm-sensor */
	const sensors_chip_name *iname;
	const sensors_feature *feature;

	/* Maximum length of 'values' */
	int values_max_length;

	/* Last registered measures of the sensor.  Index 0 for the
	   oldest measure.  */
	struct measure *measures;

	/* Color of the sensor used for the graph */
	struct color *color;

	/* Whether the sensor is displayed in the graph */
	int enabled;

	/* see psensor_type */
	unsigned int type;

	/* The maximum detected value of the sensor */
	double max;

	/* The minimum detected value of the sensor */
	double min;

	/*
	   Whether alarm alerts is enabled for this sensor
	 */
	int alarm_enabled;

	/*
	   An alarm is raised if the current sensor value is bigger. 0
	   means no limit
	 */
	double alarm_limit;

	/* Whether the current value is bigger than 'alarm_limit'.  */
	int alarm_raised;

	void (*cb_alarm_raised) (struct psensor *, void *);
	void *cb_alarm_raised_data;

#ifdef HAVE_NVIDIA
	/* Nvidia id for the nvctrl */
	int nvidia_id;
#endif

	char *url;

};

struct psensor *psensor_create(char *id,
			       char *name, unsigned int type,
			       int values_max_length);

void psensor_values_resize(struct psensor *s, int new_size);

void psensor_free(struct psensor *sensor);

void psensor_list_free(struct psensor **sensors);
int psensor_list_size(struct psensor **sensors);

struct psensor *psensor_list_get_by_id(struct psensor **sensors,
				       const char *id);

/*
  Return 1 if there is at least one sensor of a given type, else
  returns 0 */
int psensor_list_contains_type(struct psensor **sensors, unsigned int type);

int is_temp_type(unsigned int type);
int is_fan_type(unsigned int type);

int get_min_temp(struct psensor **sensors);
int get_max_temp(struct psensor **sensors);

int get_min_rpm(struct psensor **sensors);
int get_max_rpm(struct psensor **sensors);

/*
  Converts the value of a sensor to a string.

  parameter 'type' is SENSOR_TYPE_LMSENSOR_TEMP, SENSOR_TYPE_NVIDIA,
  or SENSOR_TYPE_LMSENSOR_FAN
*/
char *psensor_value_to_string(unsigned int type, double value);

struct psensor **get_all_sensors(int values_max_length);

struct psensor **psensor_list_add(struct psensor **sensors,
				  struct psensor *sensor);

void psensor_set_current_value(struct psensor *sensor, double value);
void psensor_set_current_measure(struct psensor *sensor, double value,
				 struct timeval tv);

double psensor_get_current_value(struct psensor *sensor);

struct measure *psensor_get_current_measure(struct psensor *sensor);

/*
  Returns a string representation of a psensor type.
*/
const char *psensor_type_to_str(unsigned int type);

void psensor_list_update_measures(struct psensor **sensors);

#endif
