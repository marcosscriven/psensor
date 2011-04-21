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


#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "ui_sensorpref.h"
#include "ui_sensorlist.h"
#include "cfg.h"
#include "ui_color.h"
#include "compat.h"

enum {
	COL_NAME = 0,
	COL_TEMP,
	COL_TEMP_MIN,
	COL_TEMP_MAX,
	COL_COLOR,
	COL_COLOR_STR,
	COL_ENABLED,
	COL_EMPTY,
	COLS_COUNT
};

struct cb_data {
	struct ui_sensorlist *ui_sensorlist;
	struct psensor *sensor;
};

int col_index_to_col(int idx)
{
	if (idx == 5)
		return COL_ENABLED;
	else if (idx > 5)
		return -1;

	return idx;
}

void ui_sensorlist_update(struct ui_sensorlist *list)
{
	GtkTreeIter iter;
	GtkTreeModel *model
	    = gtk_tree_view_get_model(GTK_TREE_VIEW(list->widget));
	gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
	struct psensor **sensor = list->sensors;

	while (valid && *sensor) {
		struct psensor *s = *sensor;

		char *str;

		str = psensor_value_to_string(s->type,
					      s->measures[s->values_max_length -
							  1].value);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_TEMP, str,
				   -1);
		free(str);

		str = psensor_value_to_string(s->type, s->min);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COL_TEMP_MIN, str, -1);
		free(str);

		str = psensor_value_to_string(s->type, s->max);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COL_TEMP_MAX, str, -1);
		free(str);

		valid = gtk_tree_model_iter_next(model, &iter);
		sensor++;
	}
}

/*
 * Returns the sensor corresponding to the x/y position
 * in the table.
 *
 * <null> if none.
 */
struct psensor *ui_sensorlist_get_sensor_at_pos(GtkTreeView * view,
						int x,
						int y, struct psensor **sensors)
{
	GtkTreePath *path;

	gtk_tree_view_get_path_at_pos(view, x, y, &path, NULL, NULL, NULL);

	if (path) {
		gint *i = gtk_tree_path_get_indices(path);
		if (i)
			return *(sensors + *i);
	}
	return NULL;
}

/*
 * Returns the index of the column corresponding
 * to the x position in the table.
 *
 * -1 if none
 */
int ui_sensorlist_get_col_index_at_pos(GtkTreeView *view, int x)
{
	GList *columns = gtk_tree_view_get_columns(view);
	GList *node;
	int colx = 0;
	int coli = 0;

	for (node = columns; node; node = node->next) {
		GtkTreeViewColumn *checkcol = (GtkTreeViewColumn *) node->data;

		if (x >= colx && x < (colx + checkcol->width))
			return coli;
		else
			colx += checkcol->width;

		coli++;
	}

	return -1;
}

void ui_sensorlist_update_sensors_preferences(struct ui_sensorlist *list)
{
	GtkTreeIter iter;
	GtkTreeModel *model
	    = gtk_tree_view_get_model(GTK_TREE_VIEW(list->widget));
	gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
	struct psensor **sensor = list->sensors;

	while (valid && *sensor) {
		GdkColor color;
		gchar *scolor;

		color.red = (*sensor)->color->red;
		color.green = (*sensor)->color->green;
		color.blue = (*sensor)->color->blue;

		scolor = gdk_color_to_string(&color);

		gtk_list_store_set(GTK_LIST_STORE(model),
				   &iter, COL_NAME, (*sensor)->name, -1);

		gtk_list_store_set(GTK_LIST_STORE(model),
				   &iter, COL_COLOR_STR, scolor, -1);

		gtk_list_store_set(GTK_LIST_STORE(model),
				   &iter, COL_ENABLED, (*sensor)->enabled, -1);

		free(scolor);

		valid = gtk_tree_model_iter_next(model, &iter);
		sensor++;
	}
}

static void cb_sensor_settings_activated(GtkWidget *menu_item, gpointer data)
{
	struct cb_data *cb_data = data;
	struct psensor *sensor = cb_data->sensor;
	struct ui_sensorlist *ui_sensorlist = cb_data->ui_sensorlist;
	GtkWidget *diag, *content_area;
	gint result;
	struct ui_sensorpref *w_pref;
	GdkColor color;

	diag = gtk_dialog_new_with_buttons(_("Edit Sensor Preferences"),
					   NULL,
					   GTK_DIALOG_MODAL |
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_STOCK_OK,
					   GTK_RESPONSE_ACCEPT,
					   GTK_STOCK_CANCEL,
					   GTK_RESPONSE_REJECT, NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(diag));

	w_pref = ui_sensorpref_create(ui_sensorlist, sensor);

	gtk_container_add(GTK_CONTAINER(content_area), w_pref->widget);
	gtk_widget_show_all(content_area);

	result = gtk_dialog_run(GTK_DIALOG(diag));

	if (result == GTK_RESPONSE_ACCEPT) {
		const char *newname;

		newname = gtk_entry_get_text(GTK_ENTRY(w_pref->w_name));

		free(sensor->name);
		sensor->name = strdup(newname);
		config_set_sensor_name(sensor->id, newname);

		sensor->enabled
		    = gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(w_pref->w_enabled));
		config_set_sensor_enabled(sensor->id, sensor->enabled);

		sensor->alarm_limit
		    = gtk_spin_button_get_value
		    (GTK_SPIN_BUTTON(w_pref->w_alarm_limit));

		config_set_sensor_alarm_limit(sensor->id, sensor->alarm_limit);

		sensor->alarm_enabled
		    = gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(w_pref->w_alarm_enabled));
		config_set_sensor_alarm_enabled
		    (sensor->id, sensor->alarm_enabled);

		gtk_color_button_get_color(GTK_COLOR_BUTTON(w_pref->w_color),
					   &color);
		color_set(sensor->color, color.red, color.green, color.blue);
		config_set_sensor_color(sensor->id, sensor->color);
		ui_sensorlist_update_sensors_preferences
		    (cb_data->ui_sensorlist);
	}

	gtk_widget_destroy(diag);
	free(w_pref);
}

GtkWidget *create_sensor_popup(struct ui_sensorlist *ui_sensorlist,
			       struct psensor *sensor)
{
	GtkWidget *menu;
	GtkWidget *item;
	GtkWidget *separator;
	struct cb_data *data;

	menu = gtk_menu_new();

	item = gtk_menu_item_new_with_label(sensor->name);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);

	item = gtk_menu_item_new_with_label(_("Preferences"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

	data = malloc(sizeof(struct cb_data));
	data->ui_sensorlist = ui_sensorlist;
	data->sensor = sensor;

	g_signal_connect(item,
			 "activate",
			 G_CALLBACK(cb_sensor_settings_activated), data);

	gtk_widget_show_all(menu);

	return menu;
}

int
cb_sensor_line_clicked(GtkWidget *widget,
		       GdkEventButton *event, gpointer data)
{
	struct ui_sensorlist *list = (struct ui_sensorlist *)data;
	GtkTreeView *view = GTK_TREE_VIEW(list->widget);

	struct psensor *sensor = ui_sensorlist_get_sensor_at_pos(view,
								 event->x,
								 event->y,
								 list->sensors);

	if (sensor) {
		int coli = col_index_to_col(ui_sensorlist_get_col_index_at_pos
					    (view, event->x));

		if (coli == COL_COLOR) {
			if (ui_change_color(_("Select foreground color"),
					    sensor->color)) {
				ui_sensorlist_update_sensors_preferences(list);
				config_set_sensor_color(sensor->id,
							sensor->color);
			}
		} else if (coli >= 0 && coli != COL_ENABLED) {
			GtkWidget *menu = create_sensor_popup(list,
							      sensor);

			gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
				       event->button, event->time);

		}

	}
	return FALSE;
}

void
ui_sensorlist_on_toggled(GtkCellRendererToggle *cell,
			 gchar *path_str, gpointer data)
{
	struct ui_sensorlist *list = (struct ui_sensorlist *)data;
	GtkTreeModel *model
	    = gtk_tree_view_get_model(GTK_TREE_VIEW(list->widget));
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
	gboolean fixed;
	gint *i;

	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COL_ENABLED, &fixed, -1);

	fixed ^= 1;

	i = gtk_tree_path_get_indices(path);
	if (i) {
		int n = *i;
		struct psensor **sensor = list->sensors;
		while (n--)
			sensor++;
		(*sensor)->enabled = fixed;
		config_set_sensor_enabled((*sensor)->id, (*sensor)->enabled);
	}

	gtk_list_store_set(GTK_LIST_STORE(model),
			   &iter, COL_ENABLED, fixed, -1);

	gtk_tree_path_free(path);
}

void ui_sensorlist_create_widget(struct ui_sensorlist *ui)
{
	GtkListStore *store;
	GtkCellRenderer *renderer;
	struct psensor **sensor_cur;
	struct psensor **sensors;

	sensors = ui->sensors;

	store = gtk_list_store_new(COLS_COUNT,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_BOOLEAN, G_TYPE_STRING);

	if (ui->widget)
		gtk_widget_destroy(ui->widget);

	ui->widget =
	    GTK_WIDGET(gtk_tree_view_new_with_model(GTK_TREE_MODEL(store)));

	gtk_tree_selection_set_mode(gtk_tree_view_get_selection
				    (GTK_TREE_VIEW(ui->widget)),
				    GTK_SELECTION_NONE);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Sensor"),
						    renderer,
						    "text", COL_NAME, NULL);

	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Current"),
						    renderer,
						    "text", COL_TEMP, NULL);

	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Min"),
						    renderer,
						    "text", COL_TEMP_MIN, NULL);

	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Max"),
						    renderer,
						    "text", COL_TEMP_MAX, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Color"),
						    renderer,
						    "text", COL_COLOR,
						    "background", COL_COLOR_STR,
						    NULL);

	g_signal_connect(ui->widget,
			 "button-press-event",
			 (GCallback) cb_sensor_line_clicked, ui);

	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    _("Enabled"),
						    renderer,
						    "active", COL_ENABLED,
						    NULL);
	g_signal_connect(G_OBJECT(renderer), "toggled",
			 (GCallback) ui_sensorlist_on_toggled, ui);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->widget),
						    -1,
						    "",
						    renderer,
						    "text", COL_EMPTY, NULL);

	sensor_cur = sensors;
	while (*sensor_cur) {
		GtkTreeIter iter;
		GdkColor color;
		gchar *scolor;

		color.red = (*sensor_cur)->color->red;
		color.green = (*sensor_cur)->color->green;
		color.blue = (*sensor_cur)->color->blue;

		scolor = gdk_color_to_string(&color);

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				   COL_NAME, (*sensor_cur)->name,
				   COL_TEMP, _("N/A"),
				   COL_TEMP_MIN, _("N/A"),
				   COL_TEMP_MAX, _("N/A"),
				   COL_COLOR_STR, scolor,
				   COL_ENABLED, (*sensor_cur)->enabled, -1);

		free(scolor);

		sensor_cur++;
	}
}

struct ui_sensorlist *ui_sensorlist_create(struct psensor **sensors)
{
	struct ui_sensorlist *list;

	list = malloc(sizeof(struct ui_sensorlist));
	list->sensors = sensors;
	list->widget = NULL;

	ui_sensorlist_create_widget(list);

	return list;

}
