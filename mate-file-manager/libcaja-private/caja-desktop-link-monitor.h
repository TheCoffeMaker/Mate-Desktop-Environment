/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   caja-desktop-link-monitor.h: singleton that manages the desktop links

   Copyright (C) 2003 Red Hat, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef CAJA_DESKTOP_LINK_MONITOR_H
#define CAJA_DESKTOP_LINK_MONITOR_H

#include <gtk/gtk.h>
#include <libcaja-private/caja-desktop-link.h>

#define CAJA_TYPE_DESKTOP_LINK_MONITOR caja_desktop_link_monitor_get_type()
#define CAJA_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), CAJA_TYPE_DESKTOP_LINK_MONITOR, CajaDesktopLinkMonitor))
#define CAJA_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), CAJA_TYPE_DESKTOP_LINK_MONITOR, CajaDesktopLinkMonitorClass))
#define CAJA_IS_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CAJA_TYPE_DESKTOP_LINK_MONITOR))
#define CAJA_IS_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), CAJA_TYPE_DESKTOP_LINK_MONITOR))
#define CAJA_DESKTOP_LINK_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), CAJA_TYPE_DESKTOP_LINK_MONITOR, CajaDesktopLinkMonitorClass))

typedef struct CajaDesktopLinkMonitorDetails CajaDesktopLinkMonitorDetails;

typedef struct
{
    GObject parent_slot;
    CajaDesktopLinkMonitorDetails *details;
} CajaDesktopLinkMonitor;

typedef struct
{
    GObjectClass parent_slot;
} CajaDesktopLinkMonitorClass;

GType   caja_desktop_link_monitor_get_type (void);

CajaDesktopLinkMonitor *   caja_desktop_link_monitor_get (void);
void caja_desktop_link_monitor_delete_link (CajaDesktopLinkMonitor *monitor,
        CajaDesktopLink *link,
        GtkWidget *parent_view);

/* Used by caja-desktop-link.c */
char * caja_desktop_link_monitor_make_filename_unique (CajaDesktopLinkMonitor *monitor,
        const char *filename);

#endif /* CAJA_DESKTOP_LINK_MONITOR_H */
