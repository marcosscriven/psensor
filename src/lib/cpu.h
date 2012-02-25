/*
 * Copyright (C) 2010-2012 jeanfi@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#ifndef _PSENSOR_CPU_H_
#define _PSENSOR_CPU_H_

#include "psensor.h"

struct psensor *create_cpu_usage_sensor(int measures_len);

void cpu_usage_sensor_update(struct psensor *);
void cpu_psensor_list_update(struct psensor **);

struct psensor **cpu_psensor_list_add(struct psensor **,
				      int values_max_len);

void cpu_cleanup();

#endif
