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

#include <locale.h>
#include <libintl.h>
#define _(str) gettext(str)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sensors/sensors.h>
#include <sensors/error.h>

#include "psensor.h"

double
lmsensor_get_value(const sensors_chip_name *name,
		   const sensors_subfeature *sub)
{
	double val;
	int err;

	err = sensors_get_value(name, sub->number, &val);
	if (err) {
		fprintf(stderr,
			_("ERROR: Can't get value of subfeature %s: %s\n"),
			sub->name, sensors_strerror(err));
		val = 0.0;
	}
	return val;
}

double lmsensor_get_temp_input(struct psensor *sensor)
{
	const sensors_chip_name *chip = sensor->iname;
	const sensors_feature *feature = sensor->feature;

	const sensors_subfeature *sf;

	sf = sensors_get_subfeature(chip,
				    feature, SENSORS_SUBFEATURE_TEMP_INPUT);
	if (sf) {
		double val = lmsensor_get_value(chip, sf);
		if (val < 0.0)
			return 0.0;
		else
			return val;
	} else {
		return 0.0;
	}
}

double lmsensor_get_fan_input(struct psensor *sensor)
{
	const sensors_chip_name *chip = sensor->iname;
	const sensors_feature *feature = sensor->feature;

	const sensors_subfeature *sf;

	sf = sensors_get_subfeature(chip,
				    feature, SENSORS_SUBFEATURE_FAN_INPUT);
	if (sf)
		return lmsensor_get_value(chip, sf);
	else
		return 0;
}

void lmsensor_psensor_list_update(struct psensor **sensors)
{
	struct psensor **s_ptr = sensors;

	while (*s_ptr) {
		struct psensor *sensor = *s_ptr;

		if (sensor->type == SENSOR_TYPE_LMSENSOR_TEMP)
			psensor_set_current_value
			    (sensor, lmsensor_get_temp_input(sensor));
		else if (sensor->type == SENSOR_TYPE_LMSENSOR_FAN)
			psensor_set_current_value
			    (sensor, lmsensor_get_fan_input(sensor));

		s_ptr++;
	}
}

struct psensor *
lmsensor_psensor_create(const sensors_chip_name *chip,
			const sensors_feature *feature,
			int values_max_length)
{
	char name[200];
	const sensors_subfeature *sf;
	char *label;
	int type;
	char *id;
	struct psensor *psensor;
	sensors_subfeature_type fault_subfeature;

	if (sensors_snprintf_chip_name(name, 200, chip) < 0)
		return NULL;

	if (feature->type == SENSORS_FEATURE_TEMP) {
		fault_subfeature = SENSORS_SUBFEATURE_TEMP_FAULT;

	} else if (feature->type == SENSORS_FEATURE_FAN) {
		fault_subfeature = SENSORS_SUBFEATURE_FAN_FAULT;

	} else {
		fprintf(stderr,
			_("ERROR: create_sensor, wrong feature type\n"));
		return NULL;
	}

	sf = sensors_get_subfeature(chip, feature, fault_subfeature);
	if (sf && lmsensor_get_value(chip, sf))
		return NULL;

	label = sensors_get_label(chip, feature);
	if (!label)
		return NULL;

	type = 0;
	if (feature->type == SENSORS_FEATURE_TEMP)
		type = SENSOR_TYPE_LMSENSOR_TEMP;
	else if (feature->type == SENSORS_FEATURE_FAN)
		type = SENSOR_TYPE_LMSENSOR_FAN;
	else
		return NULL;

	id = malloc(strlen("lmsensor ") + 1 + strlen(name) + 1 + strlen(label) +
		    1);
	sprintf(id, "lmsensor %s %s", name, label);

	psensor = psensor_create(id, strdup(label), type, values_max_length);

	psensor->iname = chip;
	psensor->feature = feature;

	if (feature->type == SENSORS_FEATURE_TEMP
	    && (lmsensor_get_temp_input(psensor) <= 0.0)) {
		free(psensor);
		return NULL;
	}

	return psensor;
}

int lmsensor_init()
{
	int err = sensors_init(NULL);

	if (err) {
		fprintf(stderr,
			_("ERROR: lm-sensors initialization failure: %s\n"),
			sensors_strerror(err));
		return 0;
	} else {
		return 1;
	}
}
