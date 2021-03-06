/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
   caja-directory-background.c: Helper for the background of a widget
                                    that is viewing a particular location.

   Copyright (C) 2000 Eazel, Inc.

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

   Author: Darin Adler <darin@bentspoon.com>
*/

#include <config.h>
#include "caja-directory-background.h"

#include <eel/eel-gdk-extensions.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-background.h>
#include "caja-dnd.h"
#include "caja-global-preferences.h"
#include "caja-metadata.h"
#include "caja-file-attributes.h"
#include <gtk/gtk.h>
#include <string.h>

static void background_changed_callback     (EelBackground *background,
        GdkDragAction  action,
        CajaFile  *file);
static void background_reset_callback       (EelBackground *background,
        CajaFile       *file);

static void saved_settings_changed_callback (CajaFile       *file,
        EelBackground *background);

static void caja_file_background_receive_mateconf_changes (EelBackground *background);

static void caja_file_background_theme_changed (gpointer user_data);

void
caja_connect_desktop_background_to_file_metadata (CajaIconContainer *icon_container,
        CajaFile *file)
{
    EelBackground *background;

    background = eel_get_widget_background (GTK_WIDGET (icon_container));

    eel_background_set_desktop (background, GTK_WIDGET (icon_container), TRUE);

    /* Strictly speaking, we don't need to know about metadata changes, since
     * the desktop setting aren't stored there. But, hooking up to metadata
     * changes is actually a small part of what this fn does, and we do need
     * the other stuff (hooked up to background & theme changes, initialize
     * the background). Being notified of metadata changes on the file is a
     * waste, but won't hurt, so I don't think it's worth refactoring the fn
     * at this point.
     */
    caja_connect_background_to_file_metadata (GTK_WIDGET (icon_container), file, CAJA_DND_ACTION_SET_AS_FOLDER_BACKGROUND);

    caja_file_background_receive_mateconf_changes (background);
}

static void
caja_file_background_get_default_settings (char **color,
        char **image,
        EelBackgroundImagePlacement *placement)
{
    gboolean background_set;

    background_set = eel_preferences_get_boolean
                     (CAJA_PREFERENCES_BACKGROUND_SET);

    if (background_set && color)
    {
        *color = eel_preferences_get (CAJA_PREFERENCES_BACKGROUND_COLOR);
    }

    if (background_set && image)
    {
        *image = eel_preferences_get (CAJA_PREFERENCES_BACKGROUND_FILENAME);
    }

    if (placement)
    {
        *placement = EEL_BACKGROUND_TILED;
    }
}


#define BG_PREFERENCES_DRAW_BACKGROUND    "/desktop/mate/background/draw_background"
#define BG_PREFERENCES_PRIMARY_COLOR      "/desktop/mate/background/primary_color"
#define BG_PREFERENCES_SECONDARY_COLOR    "/desktop/mate/background/secondary_color"
#define BG_PREFERENCES_COLOR_SHADING_TYPE "/desktop/mate/background/color_shading_type"
#define BG_PREFERENCES_PICTURE_OPTIONS    "/desktop/mate/background/picture_options"
#define BG_PREFERENCES_PICTURE_OPACITY    "/desktop/mate/background/picture_opacity"
#define BG_PREFERENCES_PICTURE_FILENAME   "/desktop/mate/background/picture_filename"

static void
read_color (MateConfClient *client, const char *key, GdkColor *color)
{
    gchar *tmp;

    tmp = mateconf_client_get_string (client, key, NULL);

    if (tmp != NULL)
    {
        if (!gdk_color_parse (tmp, color))
            gdk_color_parse ("black", color);
        g_free (tmp);
    }
    else
    {
        gdk_color_parse ("black", color);
    }

    gdk_rgb_find_color (gdk_rgb_get_colormap (), color);
}

static void
caja_file_background_read_desktop_settings (char **color,
        char **image,
        EelBackgroundImagePlacement *placement)
{
    MateConfClient *client;
    gboolean enabled;
    GdkColor primary, secondary;
    gchar *tmp, *filename;
    char	*end_color;
    char	*start_color;
    gboolean use_gradient;
    gboolean is_horizontal;

    filename = NULL;

    client = mateconf_client_get_default ();

    /* Get the image filename */
    enabled = mateconf_client_get_bool (client, BG_PREFERENCES_DRAW_BACKGROUND, NULL);
    if (enabled)
    {
        tmp = mateconf_client_get_string (client, BG_PREFERENCES_PICTURE_FILENAME, NULL);
        if (tmp != NULL)
        {
            if (g_utf8_validate (tmp, -1, NULL) && g_file_test (tmp, G_FILE_TEST_EXISTS))
            {
                filename = g_strdup (tmp);
            }
            else
            {
                filename = g_filename_from_utf8 (tmp, -1, NULL, NULL, NULL);
            }
        }
        g_free (tmp);

        if (filename != NULL && filename[0] != '\0')
        {
            *image = g_filename_to_uri (filename, NULL, NULL);
        }
        else
        {
            *image = NULL;
        }
        g_free (filename);
    }
    else
    {
        *image = NULL;
    }

    /* Get the placement */
    tmp = mateconf_client_get_string (client, BG_PREFERENCES_PICTURE_OPTIONS, NULL);
    if (tmp != NULL)
    {
        if (strcmp (tmp, "wallpaper") == 0)
        {
            *placement = EEL_BACKGROUND_TILED;
        }
        else if (strcmp (tmp, "centered") == 0)
        {
            *placement = EEL_BACKGROUND_CENTERED;
        }
        else if (strcmp (tmp, "scaled") == 0)
        {
            *placement = EEL_BACKGROUND_SCALED_ASPECT;
        }
        else if (strcmp (tmp, "stretched") == 0)
        {
            *placement = EEL_BACKGROUND_SCALED;
        }
        else if (strcmp (tmp, "zoom") == 0)
        {
            *placement = EEL_BACKGROUND_ZOOM;
        }
        else if (strcmp (tmp, "spanned") == 0)
        {
            *placement = EEL_BACKGROUND_SPANNED;
        }
        else if (strcmp (tmp, "none") == 0)
        {
            g_free (*image);

            *placement = EEL_BACKGROUND_CENTERED;
            *image = NULL;
        }
        else
        {
            *placement = EEL_BACKGROUND_CENTERED;
        }
    }
    else
    {
        *placement = EEL_BACKGROUND_CENTERED;
    }
    g_free (tmp);

    /* Get the color */
    tmp = mateconf_client_get_string (client, BG_PREFERENCES_COLOR_SHADING_TYPE, NULL);
    if (tmp != NULL)
    {
        if (strcmp (tmp, "solid") == 0)
        {
            use_gradient = FALSE;
            is_horizontal = FALSE;
        }
        else if (strcmp (tmp, "vertical-gradient") == 0)
        {
            use_gradient = TRUE;
            is_horizontal = FALSE;
        }
        else if (strcmp (tmp, "horizontal-gradient") == 0)
        {
            use_gradient = TRUE;
            is_horizontal = TRUE;
        }
        else
        {
            use_gradient = FALSE;
            is_horizontal = FALSE;
        }
    }
    else
    {
        use_gradient = FALSE;
        is_horizontal = FALSE;
    }
    g_free (tmp);

    read_color (client, BG_PREFERENCES_PRIMARY_COLOR, &primary);
    read_color (client, BG_PREFERENCES_SECONDARY_COLOR, &secondary);

    start_color   = eel_gdk_rgb_to_color_spec (eel_gdk_color_to_rgb (&primary));
    end_color     = eel_gdk_rgb_to_color_spec (eel_gdk_color_to_rgb (&secondary));

    if (use_gradient)
    {
        *color = eel_gradient_new (start_color, end_color, is_horizontal);
    }
    else
    {
        *color = g_strdup (start_color);
    }

    g_free (start_color);
    g_free (end_color);
}

static void
caja_file_background_write_desktop_default_settings (void)
{
    /* We just unset all the mateconf keys so they go back to
     * defaults
     */
    MateConfClient *client;
    MateConfChangeSet *set;

    client = mateconf_client_get_default ();
    set = mateconf_change_set_new ();

    /* the list of keys here has to be kept in sync with libmate
     * schemas, which isn't the most maintainable thing ever.
     */
    mateconf_change_set_unset (set, "/desktop/mate/background/picture_options");
    mateconf_change_set_unset (set, "/desktop/mate/background/picture_filename");
    mateconf_change_set_unset (set, "/desktop/mate/background/picture_opacity");
    mateconf_change_set_unset (set, "/desktop/mate/background/primary_color");
    mateconf_change_set_unset (set, "/desktop/mate/background/secondary_color");
    mateconf_change_set_unset (set, "/desktop/mate/background/color_shading_type");

    /* this isn't atomic yet so it'll be a bit inefficient, but
     * someday it might be atomic.
     */
    mateconf_client_commit_change_set (client, set, FALSE, NULL);

    mateconf_change_set_unref (set);

    g_object_unref (G_OBJECT (client));
}

static int
call_settings_changed (EelBackground *background)
{
    CajaFile *file;
    file = g_object_get_data (G_OBJECT (background), "eel_background_file");
    if (file)
    {
        saved_settings_changed_callback (file, background);
    }
    g_object_set_data (G_OBJECT (background), "desktop_mateconf_notification_timeout", GUINT_TO_POINTER (0));
    return FALSE;
}

static void
desktop_background_destroyed_callback (EelBackground *background, void *georgeWBush)
{
    guint notification_id;
    guint notification_timeout_id;

    notification_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (background), "desktop_mateconf_notification"));
    eel_mateconf_notification_remove (notification_id);

    notification_timeout_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (background), "desktop_mateconf_notification_timeout"));
    if (notification_timeout_id != 0)
    {
        g_source_remove (notification_timeout_id);
    }
}

static void
desktop_background_mateconf_notify_cb (MateConfClient *client, guint notification_id, MateConfEntry *entry, gpointer data)
{
    EelBackground *background;
    guint notification_timeout_id;

    background = EEL_BACKGROUND (data);

    notification_timeout_id = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (background), "desktop_mateconf_notification_timeout"));

    if (strcmp (entry->key, "/desktop/mate/background/stamp") == 0)
    {
        if (notification_timeout_id != 0)
            g_source_remove (notification_timeout_id);

        call_settings_changed (background);
    }
    else if (notification_timeout_id == 0)
    {
        notification_timeout_id = g_timeout_add (300, (GSourceFunc) call_settings_changed, background);

        g_object_set_data (G_OBJECT (background), "desktop_mateconf_notification_timeout", GUINT_TO_POINTER (notification_timeout_id));
    }
}

static void
caja_file_background_receive_mateconf_changes (EelBackground *background)
{
    guint notification_id;

    eel_mateconf_monitor_add ("/desktop/mate/background");
    notification_id = eel_mateconf_notification_add ("/desktop/mate/background", desktop_background_mateconf_notify_cb, background);

    g_object_set_data (G_OBJECT (background), "desktop_mateconf_notification", GUINT_TO_POINTER (notification_id));

    g_signal_connect (background, "destroy",
                      G_CALLBACK (desktop_background_destroyed_callback), NULL);
}

/* return true if the background is not in the default state */
gboolean
caja_file_background_is_set (EelBackground *background)
{
    char *color;
    char *image;

    gboolean is_set;

    color = eel_background_get_color (background);
    image = eel_background_get_image_uri (background);

    is_set = (color || image);

    g_free (color);
    g_free (image);

    return is_set;
}

/* handle the background changed signal */
static void
background_changed_callback (EelBackground *background,
                             GdkDragAction  action,
                             CajaFile   *file)
{
    char *color;
    char *image;

    g_assert (EEL_IS_BACKGROUND (background));
    g_assert (CAJA_IS_FILE (file));
    g_assert (g_object_get_data (G_OBJECT (background), "eel_background_file") == file);


    color = eel_background_get_color (background);
    image = eel_background_get_image_uri (background);

    if (eel_background_is_desktop (background))
    {
        eel_background_save_to_mateconf (background);
    }
    else
    {
        /* Block the other handler while we are writing metadata so it doesn't
         * try to change the background.
         */
        g_signal_handlers_block_by_func (
            file, G_CALLBACK (saved_settings_changed_callback), background);

        if (action != (GdkDragAction) CAJA_DND_ACTION_SET_AS_FOLDER_BACKGROUND &&
                action != (GdkDragAction) CAJA_DND_ACTION_SET_AS_GLOBAL_BACKGROUND)
        {
            GdkDragAction default_drag_action;

            default_drag_action = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (background), "default_drag_action"));


            action = default_drag_action;
        }

        if (action == (GdkDragAction) CAJA_DND_ACTION_SET_AS_GLOBAL_BACKGROUND)
        {
            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
                                    NULL,
                                    NULL);

            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
                                    NULL,
                                    NULL);

            eel_preferences_set
            (CAJA_PREFERENCES_BACKGROUND_COLOR, color ? color : "");
            eel_preferences_set
            (CAJA_PREFERENCES_BACKGROUND_FILENAME, image ? image : "");
            eel_preferences_set_boolean
            (CAJA_PREFERENCES_BACKGROUND_SET, TRUE);
        }
        else
        {
            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
                                    NULL,
                                    color);

            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
                                    NULL,
                                    image);
        }

        /* Unblock the handler. */
        g_signal_handlers_unblock_by_func (
            file, G_CALLBACK (saved_settings_changed_callback), background);
    }

    g_free (color);
    g_free (image);
}

static void
initialize_background_from_settings (CajaFile *file,
                                     EelBackground *background)
{
    char *color;
    char *image;
    EelBackgroundImagePlacement placement;

    g_assert (CAJA_IS_FILE (file));
    g_assert (EEL_IS_BACKGROUND (background));
    g_assert (g_object_get_data (G_OBJECT (background), "eel_background_file")
              == file);

    if (eel_background_is_desktop (background))
    {
        caja_file_background_read_desktop_settings (&color, &image, &placement);
    }
    else
    {
        color = caja_file_get_metadata (file,
                                        CAJA_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
                                        NULL);
        image = caja_file_get_metadata (file,
                                        CAJA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
                                        NULL);
        placement = EEL_BACKGROUND_TILED; /* non-tiled only avail for desktop, at least for now */

        /* if there's none, read the default from the theme */
        if (color == NULL && image == NULL)
        {
            caja_file_background_get_default_settings
            (&color, &image, &placement);
        }
    }

    /* Block the other handler while we are responding to changes
     * in the metadata so it doesn't try to change the metadata.
     */
    g_signal_handlers_block_by_func
    (background,
     G_CALLBACK (background_changed_callback),
     file);

    eel_background_set_color (background, color);
    eel_background_set_image_uri (background, image);
    eel_background_set_image_placement (background, placement);

    /* Unblock the handler. */
    g_signal_handlers_unblock_by_func
    (background,
     G_CALLBACK (background_changed_callback),
     file);

    g_free (color);
    g_free (image);
}

/* handle the file changed signal */
static void
saved_settings_changed_callback (CajaFile *file,
                                 EelBackground *background)
{
    initialize_background_from_settings (file, background);
}

/* handle the theme changing */
static void
caja_file_background_theme_changed (gpointer user_data)
{
    CajaFile *file;
    EelBackground *background;

    background = EEL_BACKGROUND (user_data);
    file = g_object_get_data (G_OBJECT (background), "eel_background_file");
    if (file)
    {
        saved_settings_changed_callback (file, background);
    }
}

/* handle the background reset signal by setting values from the current theme */
static void
background_reset_callback (EelBackground *background,
                           CajaFile  *file)
{
    char *color;
    char *image;

    if (eel_background_is_desktop (background))
    {
        caja_file_background_write_desktop_default_settings ();
    }
    else
    {
        /* Block the other handler while we are writing metadata so it doesn't
         * try to change the background.
         */
        g_signal_handlers_block_by_func (
            file,
            G_CALLBACK (saved_settings_changed_callback),
            background);

        color = caja_file_get_metadata (file,
                                        CAJA_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
                                        NULL);
        image = caja_file_get_metadata (file,
                                        CAJA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
                                        NULL);
        if (!color && !image)
        {
            eel_preferences_set_boolean (CAJA_PREFERENCES_BACKGROUND_SET,
                                         FALSE);
        }
        else
        {
            /* reset the metadata */
            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
                                    NULL,
                                    NULL);

            caja_file_set_metadata (file,
                                    CAJA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
                                    NULL,
                                    NULL);
        }
        g_free (color);
        g_free (image);

        /* Unblock the handler. */
        g_signal_handlers_unblock_by_func (
            file,
            G_CALLBACK (saved_settings_changed_callback),
            background);
    }

    saved_settings_changed_callback (file, background);
}

/* handle the background destroyed signal */
static void
background_destroyed_callback (EelBackground *background,
                               CajaFile *file)
{
    g_signal_handlers_disconnect_by_func
    (file,
     G_CALLBACK (saved_settings_changed_callback), background);
    caja_file_monitor_remove (file, background);
    eel_preferences_remove_callback (CAJA_PREFERENCES_THEME,
                                     caja_file_background_theme_changed,
                                     background);
    eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_SET,
                                     caja_file_background_theme_changed,
                                     background);
    eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_COLOR,
                                     caja_file_background_theme_changed,
                                     background);
    eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_FILENAME,
                                     caja_file_background_theme_changed,
                                     background);
}

/* key routine that hooks up a background and location */
void
caja_connect_background_to_file_metadata (GtkWidget    *widget,
        CajaFile *file,
        GdkDragAction default_drag_action)
{
    EelBackground *background;
    gpointer old_file;

    /* Get at the background object we'll be connecting. */
    background = eel_get_widget_background (widget);

    /* Check if it is already connected. */
    old_file = g_object_get_data (G_OBJECT (background), "eel_background_file");
    if (old_file == file)
    {
        return;
    }

    /* Disconnect old signal handlers. */
    if (old_file != NULL)
    {
        g_assert (CAJA_IS_FILE (old_file));
        g_signal_handlers_disconnect_by_func
        (background,
         G_CALLBACK (background_changed_callback), old_file);
        g_signal_handlers_disconnect_by_func
        (background,
         G_CALLBACK (background_destroyed_callback), old_file);
        g_signal_handlers_disconnect_by_func
        (background,
         G_CALLBACK (background_reset_callback), old_file);
        g_signal_handlers_disconnect_by_func
        (old_file,
         G_CALLBACK (saved_settings_changed_callback), background);
        caja_file_monitor_remove (old_file, background);
        eel_preferences_remove_callback (CAJA_PREFERENCES_THEME,
                                         caja_file_background_theme_changed,
                                         background);
        eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_SET,
                                         caja_file_background_theme_changed,
                                         background);
        eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_COLOR,
                                         caja_file_background_theme_changed,
                                         background);
        eel_preferences_remove_callback (CAJA_PREFERENCES_BACKGROUND_FILENAME,
                                         caja_file_background_theme_changed,
                                         background);

    }

    /* Attach the new directory. */
    caja_file_ref (file);
    g_object_set_data_full (G_OBJECT (background), "eel_background_file",
                            file, (GDestroyNotify) caja_file_unref);

    g_object_set_data (G_OBJECT (background), "default_drag_action", GINT_TO_POINTER (default_drag_action));

    /* Connect new signal handlers. */
    if (file != NULL)
    {
        g_signal_connect_object (background, "settings_changed",
                                 G_CALLBACK (background_changed_callback), file, 0);
        g_signal_connect_object (background, "destroy",
                                 G_CALLBACK (background_destroyed_callback), file, 0);
        g_signal_connect_object (background, "reset",
                                 G_CALLBACK (background_reset_callback), file, 0);
        g_signal_connect_object (file, "changed",
                                 G_CALLBACK (saved_settings_changed_callback), background, 0);

        /* arrange to receive file metadata */
        caja_file_monitor_add (file,
                               background,
                               CAJA_FILE_ATTRIBUTE_INFO);

        /* arrange for notification when the theme changes */
        eel_preferences_add_callback (CAJA_PREFERENCES_THEME,
                                      caja_file_background_theme_changed, background);
        eel_preferences_add_callback (CAJA_PREFERENCES_BACKGROUND_SET,
                                      caja_file_background_theme_changed, background);
        eel_preferences_add_callback (CAJA_PREFERENCES_BACKGROUND_COLOR,
                                      caja_file_background_theme_changed, background);
        eel_preferences_add_callback (CAJA_PREFERENCES_BACKGROUND_FILENAME,
                                      caja_file_background_theme_changed, background);
    }

    /* Update the background based on the file metadata. */
    initialize_background_from_settings (file, background);
}
