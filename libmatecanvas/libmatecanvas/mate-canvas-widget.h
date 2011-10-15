/*
 * Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation
 * All rights reserved.
 *
 * This file is part of the Mate Library.
 *
 * The Mate Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Mate Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Mate Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/*
  @NOTATION@
 */
/* Widget item type for MateCanvas widget
 *
 * MateCanvas is basically a port of the Tk toolkit's most excellent canvas widget.  Tk is
 * copyrighted by the Regents of the University of California, Sun Microsystems, and other parties.
 *
 *
 * Author: Federico Mena <federico@nuclecu.unam.mx>
 */

#ifndef MATE_CANVAS_WIDGET_H
#define MATE_CANVAS_WIDGET_H


#include <libmatecanvas/mate-canvas.h>


#ifdef __cplusplus
extern "C" {
#endif


/* Widget item for canvas.  The widget is positioned with respect to an anchor point.
 * The following object arguments are available:
 *
 * name			type			read/write	description
 * ------------------------------------------------------------------------------------------
 * widget		GtkWidget*		RW		Pointer to the widget
 * x			double			RW		X coordinate of anchor point
 * y			double			RW		Y coordinate of anchor point
 * width		double			RW		Width of widget (see below)
 * height		double			RW		Height of widget (see below)
 * anchor		GtkAnchorType		RW		Anchor side for widget
 * size_pixels		boolean			RW		Specifies whether the widget size
 *								is specified in pixels or canvas units.
 *								If it is in pixels, then the widget will not
 *								be scaled when the canvas zoom factor changes.
 *								Otherwise, it will be scaled.
 */


#define MATE_TYPE_CANVAS_WIDGET            (mate_canvas_widget_get_type ())
#define MATE_CANVAS_WIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MATE_TYPE_CANVAS_WIDGET, MateCanvasWidget))
#define MATE_CANVAS_WIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MATE_TYPE_CANVAS_WIDGET, MateCanvasWidgetClass))
#define MATE_IS_CANVAS_WIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MATE_TYPE_CANVAS_WIDGET))
#define MATE_IS_CANVAS_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MATE_TYPE_CANVAS_WIDGET))
#define MATE_CANVAS_WIDGET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MATE_TYPE_CANVAS_WIDGET, MateCanvasWidgetClass))


typedef struct _MateCanvasWidget MateCanvasWidget;
typedef struct _MateCanvasWidgetClass MateCanvasWidgetClass;

struct _MateCanvasWidget {
	MateCanvasItem item;

	GtkWidget *widget;		/* The child widget */

	double x, y;			/* Position at anchor */
	double width, height;		/* Dimensions of widget */
	GtkAnchorType anchor;		/* Anchor side for widget */

	int cx, cy;			/* Top-left canvas coordinates for widget */
	int cwidth, cheight;		/* Size of widget in pixels */

	guint destroy_id;		/* Signal connection id for destruction of child widget */

	guint size_pixels : 1;		/* Is size specified in (unchanging) pixels or units (get scaled)? */
	guint in_destroy : 1;		/* Is child widget being destroyed? */
};

struct _MateCanvasWidgetClass {
	MateCanvasItemClass parent_class;
};


/* Standard Gtk function */
GType mate_canvas_widget_get_type (void) G_GNUC_CONST;


#ifdef __cplusplus
}
#endif

#endif
