/*
 *  caja-file-info.h - Information about a file
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
 */

/* CajaFileInfo is an interface to the CajaFile object.  It
 * provides access to the asynchronous data in the CajaFile.
 * Extensions are passed objects of this type for operations. */

#ifndef CAJA_FILE_INFO_H
#define CAJA_FILE_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAJA_TYPE_FILE_INFO           (caja_file_info_get_type ())
#define CAJA_FILE_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), CAJA_TYPE_FILE_INFO, CajaFileInfo))
#define CAJA_IS_FILE_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CAJA_TYPE_FILE_INFO))
#define CAJA_FILE_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CAJA_TYPE_FILE_INFO, CajaFileInfoIface))


#ifndef CAJA_FILE_DEFINED
#define CAJA_FILE_DEFINED
    /* Using CajaFile for the vtable to make implementing this in
     * CajaFile easier */
    typedef struct CajaFile          CajaFile;
#endif

    typedef CajaFile                  CajaFileInfo;
    typedef struct _CajaFileInfoIface CajaFileInfoIface;


    struct _CajaFileInfoIface
    {
        GTypeInterface g_iface;

        gboolean          (*is_gone)              (CajaFileInfo *file);

        char *            (*get_name)             (CajaFileInfo *file);
        char *            (*get_uri)              (CajaFileInfo *file);
        char *            (*get_parent_uri)       (CajaFileInfo *file);
        char *            (*get_uri_scheme)       (CajaFileInfo *file);

        char *            (*get_mime_type)        (CajaFileInfo *file);
        gboolean          (*is_mime_type)         (CajaFileInfo *file,
                const char       *mime_Type);
        gboolean          (*is_directory)         (CajaFileInfo *file);

        void              (*add_emblem)           (CajaFileInfo *file,
                const char       *emblem_name);
        char *            (*get_string_attribute) (CajaFileInfo *file,
                const char       *attribute_name);
        void              (*add_string_attribute) (CajaFileInfo *file,
                const char       *attribute_name,
                const char       *value);
        void              (*invalidate_extension_info) (CajaFileInfo *file);

        char *            (*get_activation_uri)   (CajaFileInfo *file);

        GFileType         (*get_file_type)        (CajaFileInfo *file);
        GFile *           (*get_location)         (CajaFileInfo *file);
        GFile *           (*get_parent_location)  (CajaFileInfo *file);
        CajaFileInfo* (*get_parent_info)      (CajaFileInfo *file);
        GMount *          (*get_mount)            (CajaFileInfo *file);
        gboolean          (*can_write)            (CajaFileInfo *file);

    };

    GList            *caja_file_info_list_copy            (GList            *files);
    void              caja_file_info_list_free            (GList            *files);
    GType             caja_file_info_get_type             (void);

    /* Return true if the file has been deleted */
    gboolean          caja_file_info_is_gone              (CajaFileInfo *file);

    /* Name and Location */
    GFileType         caja_file_info_get_file_type        (CajaFileInfo *file);
    GFile *           caja_file_info_get_location         (CajaFileInfo *file);
    char *            caja_file_info_get_name             (CajaFileInfo *file);
    char *            caja_file_info_get_uri              (CajaFileInfo *file);
    char *            caja_file_info_get_activation_uri   (CajaFileInfo *file);
    GFile *           caja_file_info_get_parent_location  (CajaFileInfo *file);
    char *            caja_file_info_get_parent_uri       (CajaFileInfo *file);
    GMount *          caja_file_info_get_mount            (CajaFileInfo *file);
    char *            caja_file_info_get_uri_scheme       (CajaFileInfo *file);
    /* It's not safe to call this recursively multiple times, as it works
     * only for files already cached by Caja.
     */
    CajaFileInfo* caja_file_info_get_parent_info      (CajaFileInfo *file);

    /* File Type */
    char *            caja_file_info_get_mime_type        (CajaFileInfo *file);
    gboolean          caja_file_info_is_mime_type         (CajaFileInfo *file,
            const char       *mime_type);
    gboolean          caja_file_info_is_directory         (CajaFileInfo *file);
    gboolean          caja_file_info_can_write            (CajaFileInfo *file);


    /* Modifying the CajaFileInfo */
    void              caja_file_info_add_emblem           (CajaFileInfo *file,
            const char       *emblem_name);
    char *            caja_file_info_get_string_attribute (CajaFileInfo *file,
            const char       *attribute_name);
    void              caja_file_info_add_string_attribute (CajaFileInfo *file,
            const char       *attribute_name,
            const char       *value);

    /* Invalidating file info */
    void              caja_file_info_invalidate_extension_info (CajaFileInfo *file);

    CajaFileInfo *caja_file_info_lookup                (GFile *location);
    CajaFileInfo *caja_file_info_create                (GFile *location);
    CajaFileInfo *caja_file_info_lookup_for_uri        (const char *uri);
    CajaFileInfo *caja_file_info_create_for_uri        (const char *uri);

#ifdef __cplusplus
}
#endif

#endif
