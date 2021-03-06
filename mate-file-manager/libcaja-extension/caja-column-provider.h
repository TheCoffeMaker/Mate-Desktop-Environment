/*
 *  caja-column-provider.h - Interface for Caja extensions that
 *                               provide column descriptions.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

/* This interface is implemented by Caja extensions that want to
 * add columns to the list view and details to the icon view.
 * Extensions are asked for a list of columns to display.  Each
 * returned column refers to a string attribute which can be filled in
 * by CajaInfoProvider */

#ifndef CAJA_COLUMN_PROVIDER_H
#define CAJA_COLUMN_PROVIDER_H

#include <glib-object.h>
#include "caja-extension-types.h"
#include "caja-column.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAJA_TYPE_COLUMN_PROVIDER           (caja_column_provider_get_type ())
#define CAJA_COLUMN_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), CAJA_TYPE_COLUMN_PROVIDER, CajaColumnProvider))
#define CAJA_IS_COLUMN_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CAJA_TYPE_COLUMN_PROVIDER))
#define CAJA_COLUMN_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CAJA_TYPE_COLUMN_PROVIDER, CajaColumnProviderIface))

    typedef struct _CajaColumnProvider       CajaColumnProvider;
    typedef struct _CajaColumnProviderIface  CajaColumnProviderIface;

    struct _CajaColumnProviderIface
    {
        GTypeInterface g_iface;

        GList *(*get_columns) (CajaColumnProvider *provider);
    };

    /* Interface Functions */
    GType                   caja_column_provider_get_type       (void);
    GList                  *caja_column_provider_get_columns    (CajaColumnProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
