/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2011 Perberos <perberos@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __MATE_ABOUT_C__
#define __MATE_ABOUT_C__

#include "mate-about.h"

// what a mess!
#ifndef DISABLE_NYANCAT

// Thanks! http://www.gtkforums.com/viewtopic.php?t=1639
typedef struct _iter_arg {
	GtkWidget* widget;
	GdkPixbufAnimation* animation;
	GdkPixbufAnimationIter* iter;
} iter_arg_t;

gboolean on_animation_frame(iter_arg_t* object)
{
	if (object->widget == NULL)
	{
		return FALSE;
	}

	static gint frame = 1;

	if (gdk_pixbuf_animation_iter_advance(object->iter, NULL))
	{
		frame++;

		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(object->widget), gdk_pixbuf_animation_iter_get_pixbuf(object->iter));
	}

	return TRUE;
}

#endif

#if GTK_CHECK_VERSION(3, 0, 0) && !defined(UNIQUE)

static void mate_about_on_activate(GtkApplication* app)
{
	GList* list;
	GtkWidget* window;

	list = gtk_application_get_windows(app);

	if (list)
	{
		gtk_window_present(GTK_WINDOW(list->data));
	}
	else
	{
		mate_about_run();
	}
}

#elif GLIB_CHECK_VERSION(2, 26, 0) && !defined(UNIQUE)
// es un callback
static void mate_about_on_activate(GApplication* app)
{
	if (!mate_about_dialog)
	{
		mate_about_run();
	}
	else
	{
		gtk_window_present(GTK_WINDOW(mate_about_dialog));
	}
}

#endif

void mate_about_run(void)
{
	/**
	 * Es importante llamar gtk_init, si no, no se puede iniciar bien el dialogo
	 */
	mate_about_dialog = gtk_about_dialog_new();

	gtk_window_set_default_icon_name(icon);

	// logo
	#ifndef DISABLE_NYANCAT
		iter_arg_t animation_object;
		GdkPixbufAnimation* animation;
		GdkPixbufAnimationIter *iter;
		GtkWidget* image;

		animation = gdk_pixbuf_animation_new_from_file(PIXMAPS_DIR "nyan-cat.gif", NULL);

		if (animation != NULL)
		{
			iter = gdk_pixbuf_animation_get_iter(animation, NULL);

			animation_object.animation = animation;
			animation_object.iter = iter;
			animation_object.widget = mate_about_dialog;

			gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(mate_about_dialog), gdk_pixbuf_animation_iter_get_pixbuf(iter));

			g_timeout_add(gdk_pixbuf_animation_iter_get_delay_time(iter), (GSourceFunc) on_animation_frame, (gpointer) &animation_object);
		}

	#elif GTK_CHECK_VERSION(3, 0, 0) || GTK_CHECK_VERSION(2, 6, 0)

		gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(mate_about_dialog), icon);

	#else

		GtkIconTheme* icon_theme = gtk_icon_theme_get_default();

		if (gtk_icon_theme_has_icon(icon_theme, icon))
		{
			GdkPixbuf* pixbuf = gtk_icon_theme_load_icon(icon_theme, icon, 64, 0, NULL);
			gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(mate_about_dialog), pixbuf);
			g_object_unref(pixbuf);
		}

	#endif

	//name
	#if GTK_CHECK_VERSION(3, 0, 0) || GTK_CHECK_VERSION(2, 12, 0)
		gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(mate_about_dialog), gettext(program_name));
	#else
		gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(mate_about_dialog), gettext(program_name));
	#endif

	// version
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(mate_about_dialog), version);

	// creditos y pagina web
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(mate_about_dialog), copyright);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(mate_about_dialog), website);

	/**
	 * This generate a random message.
	 * The comments index must not be more than comments_count - 1
	 */
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(mate_about_dialog), gettext(comments[g_random_int_range(0, comments_count - 1)]));

	// autores
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(mate_about_dialog), authors);
	// I comment this because the list is empty
	//gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(mate_about_dialog), artists);
	//gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(mate_about_dialog), documenters);

	#if GTK_CHECK_VERSION(3, 0, 0)
		gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(mate_about_dialog), GTK_LICENSE_GPL_3_0);
		gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(mate_about_dialog), TRUE);
	#endif

	#ifdef USE_UNIQUE
		unique_app_watch_window(mate_about_application, GTK_WINDOW(mate_about_dialog));
	#elif GTK_CHECK_VERSION(3, 0, 0) && !defined(UNIQUE)
		gtk_window_set_application(GTK_WINDOW(mate_about_dialog), mate_about_application);
	#endif

	// start and destroy
	gtk_dialog_run(GTK_DIALOG(mate_about_dialog));
	gtk_widget_destroy(mate_about_dialog);
}

void mate_about_release_version(void)
{
	g_printf("%s %s\n", gettext(program_name), version);
}

// ...
gint main(gint argc, gchar** argv)
{
	gint status = 0;
	/**
	 * Solo utilizado para option parse
	 */
	GError* error = NULL;

	/**
	 * Con esto se inicia gettext
	 */
	bindtextdomain(gettext_package, locale_dir);
	bind_textdomain_codeset(gettext_package, "UTF-8");
	textdomain(gettext_package);

	/**
	 * http://www.gtk.org/api/2.6/glib/glib-Commandline-option-parser.html
	 */
	GOptionContext* context = g_option_context_new(NULL);
	g_option_context_add_main_entries(context, command_entries, gettext_package);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	g_option_context_parse(context, &argc, &argv, &error);

	/**
	 * Not necesary at all, program just run and die.
	 * But it free a little memory.
	 */
	g_option_context_free(context);

	gtk_init(&argc, &argv);

	if (mate_about_nogui == TRUE)
	{
		mate_about_release_version();
	}
	else
	{
		/**
		 * Ejemplos tomados de:
		 * http://developer.gnome.org/gtk3/3.0/gtk-migrating-GtkApplication.html
		 */
		#if defined(UNIQUE)

			mate_about_application = unique_app_new("org.mate.about", NULL);

			if (unique_app_is_running(mate_about_application))
			{
				UniqueResponse response = unique_app_send_message(mate_about_application, UNIQUE_ACTIVATE, NULL);

				if (response != UNIQUE_RESPONSE_OK)
				{
					status = 1;
				}
			}
			else
			{
				mate_about_run();
			}

			g_object_unref(mate_about_application);

		#elif GTK_CHECK_VERSION(3, 0, 0)

			mate_about_application = gtk_application_new("org.mate.about", 0);
			g_signal_connect(mate_about_application, "activate", G_CALLBACK(mate_about_on_activate), NULL);

			status = g_application_run(G_APPLICATION(mate_about_application), argc, argv);

			g_object_unref(mate_about_application);

		#elif GLIB_CHECK_VERSION(2, 26, 0)

			mate_about_application = g_application_new("org.mate.about", G_APPLICATION_FLAGS_NONE);
			g_signal_connect(mate_about_application, "activate", G_CALLBACK(mate_about_on_activate), NULL);

			status = g_application_run(G_APPLICATION(mate_about_application), argc, argv);

			g_object_unref(mate_about_application);

		#else
			mate_about_run();
		#endif
	}

	return status;
}

#endif
