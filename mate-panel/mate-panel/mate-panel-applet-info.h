/*
 * mate-panel-applet-info.h
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@mate.org>
 * Copyright (C) 2010 Vincent Untz <vuntz@mate.org>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __MATE_PANEL_APPLET_INFO_H__
#define __MATE_PANEL_APPLET_INFO_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _MatePanelAppletInfo MatePanelAppletInfo;

MatePanelAppletInfo *mate_panel_applet_info_new                             (const gchar  *iid,
								    const gchar  *name,
								    const gchar  *comment,
								    const gchar  *icon,
								    const gchar **old_ids);
void             mate_panel_applet_info_free                            (MatePanelAppletInfo *info);

const gchar     *mate_panel_applet_info_get_iid                         (MatePanelAppletInfo *info);
const gchar     *mate_panel_applet_info_get_name                        (MatePanelAppletInfo *info);
const gchar     *mate_panel_applet_info_get_description                 (MatePanelAppletInfo *info);
const gchar     *mate_panel_applet_info_get_icon                        (MatePanelAppletInfo *info);
const gchar * const *mate_panel_applet_info_get_old_ids                 (MatePanelAppletInfo *info);

G_END_DECLS

#endif /* __MATE_PANEL_APPLET_INFO_H__ */