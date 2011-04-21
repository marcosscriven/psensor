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

#ifndef _PSENSOR_LMSENSOR_H_
#define _PSENSOR_LMSENSOR_H_

#include <sensors/sensors.h>

struct psensor *lmsensor_psensor_create(const sensors_chip_name *chip,
					const sensors_feature *feature,
					int values_max_length);
int lmsensor_init();
void lmsensor_psensor_list_update(struct psensor **sensors);

#endif
