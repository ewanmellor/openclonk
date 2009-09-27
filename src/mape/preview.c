/*
 * mape - C4 Landscape.txt editor
 *
 * Copyright (c) 2005-2009 Armin Burgmeier
 *
 * Portions might be copyrighted by other authors who have contributed
 * to OpenClonk.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * See isc_license.txt for full license and disclaimer.
 *
 * "Clonk" is a registered trademark of Matthes Bender.
 * See clonk_trademark_license.txt for full license.
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include "preview.h"
#include "preferences.h"

MapePreView* mape_pre_view_new(MapeMatTexView* mat_tex,
                               GError** error)
{
	MapePreView* view;
	view = malloc(sizeof(MapePreView) );

	view->mat_tex = mat_tex;

	view->image = gtk_image_new();
	gtk_widget_show(view->image);

	view->event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(view->event_box), view->image);
	gtk_widget_add_events(view->event_box, GDK_BUTTON_PRESS_MASK);
	gtk_widget_show(view->event_box);
	
	view->frame = gtk_frame_new(NULL);/*"Landscape preview");*/
	gtk_frame_set_shadow_type(GTK_FRAME(view->frame), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(view->frame), view->event_box);
	gtk_widget_show(view->frame);
	
	/*gtk_widget_set_size_request(view->image, preferences->map_width, preferences->map_height);*/

	return view;
}

void mape_pre_view_destroy(MapePreView* view)
{
	free(view);
}

void mape_pre_view_update(MapePreView* view,
                          GdkPixbuf* pixbuf)
{
	/* TODO: Unref old pixbuf */
	/* TODO: ref new pixbuf? */
	gtk_image_set_from_pixbuf(GTK_IMAGE(view->image), pixbuf);	
}

void mape_pre_view_apply_preferences(MapePreView* view,
                                      MapePreferences* preferences)
{
	gtk_widget_set_size_request(view->image, preferences->map_width, preferences->map_height);
}

