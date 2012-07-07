/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net/
 *
 *********************************************************/
/**********************************************************
 *
 * This program is free software; you can redistribute it and/or

 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 *********************************************************/

/*!********************************************************
 * \file 
 * The main window
 *
 * main file that initialises the menubar, the toolbar, 
 * the tabs, about window, status error messages
 *
 *********************************************************/

//we include the "config.h" file from the config options
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "0.7.2.1058"
#define PACKAGE_NAME "mp3splt-gtk"
#endif

#include "main_window.h"

static void main_window_drag_data_received(GtkWidget *window,
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint
    info, guint time, ui_state *ui)
{
  const gchar *received_data = (gchar *) gtk_selection_data_get_text(data);
  if (received_data == NULL)
  {
    return;
  }

  gchar **drop_filenames = g_strsplit(received_data, "\n", 0);

  gint current_index = 0;
  gchar *current_filename = drop_filenames[current_index];
  while (current_filename != NULL)
  {
    gchar *filename = NULL;
    if (strstr(current_filename, "file:") == current_filename)
    {
      filename = g_filename_from_uri(current_filename, NULL, NULL);
    }
    else
    {
      gint fname_malloc_size = strlen(current_filename) + 1;
      filename = g_malloc(sizeof(gchar) * fname_malloc_size);
      g_snprintf(filename, fname_malloc_size, "%s", current_filename);
    }

    remove_end_slash_n_r_from_filename(filename);

    if (file_exists(filename))
    {
      import_file(filename, ui);
    }

    if (filename)
    {
      g_free(filename);
      filename = NULL;
    }

    g_free(current_filename);
    current_index++;
    current_filename = drop_filenames[current_index];
  }

  if (drop_filenames)
  {
    g_free(drop_filenames);
    drop_filenames = NULL;
  }
}

//! Set the name of the input file
void set_input_filename(const gchar *filename, gui_state *gui)
{
  if (filename == NULL)
  {
    return;
  }

  if (gui->input_filename != NULL)
  {
    g_string_free(gui->input_filename,TRUE);
  }
  gui->input_filename = g_string_new(filename);

  if (gui->open_file_chooser_button != NULL)
  {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(gui->open_file_chooser_button), filename);
  }
}

/*! Get the name of the input file

\return 
 - The name of the input file, if set.
 - "", otherwise.
*/
gchar* get_input_filename(gui_state *gui)
{
  if (gui->input_filename != NULL)
  {
    return gui->input_filename->str;
  }

  return "";
}

static gboolean configure_window_callback(GtkWindow *window, GdkEvent *event, ui_state *ui)
{
  ui_set_main_win_position(ui, event->configure.x, event->configure.y); 
  ui_set_main_win_size(ui, event->configure.width, event->configure.height);

  refresh_drawing_area(ui->gui);
  refresh_preview_drawing_areas(ui->gui);

  return FALSE;
}

static void initialize_window(ui_state *ui)
{
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  ui->gui->window = window;

  g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(configure_window_callback), ui);

  gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME" "VERSION);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(exit_application), ui);
  g_signal_connect(G_OBJECT(window), "drag-data-received",
      G_CALLBACK(main_window_drag_data_received), ui);
  gtk_drag_dest_set(window, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
      drop_types, 3, GDK_ACTION_COPY | GDK_ACTION_MOVE);
 
  GString *imagefile = g_string_new("");
  build_path(imagefile, PIXMAP_PATH, "mp3splt-gtk_ico"ICON_EXT);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(imagefile->str, NULL);
  gtk_window_set_default_icon(pixbuf);
  g_string_free(imagefile, TRUE);
}

static void activate_url(GtkAboutDialog *about, const gchar *link, ui_state *ui)
{
#ifdef __WIN32__
  char default_browser[512] = { '\0' };
  DWORD dwType, dwSize = sizeof(default_browser) - 1;

  SHGetValue(HKEY_CURRENT_USER,
        TEXT("Software\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);

  if (default_browser[0] != '\0')
  {
    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);
  }

  if (default_browser[0] != '\0')
  {
    char browser_exe[2048] = { '\0' };
    dwSize = sizeof(browser_exe) - 1;

    char browser_exe_registry[1024] = { '\0' };
    snprintf(browser_exe_registry, 1024,
        "SOFTWARE\\Clients\\StartMenuInternet\\%s\\shell\\open\\command\\",
        default_browser);

    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT(browser_exe_registry), TEXT(""),
        &dwType, browser_exe, &dwSize);

    if (browser_exe[0] != '\0')
    {
      gint browser_command_size = strlen(browser_exe) + strlen(link) + 2;
      char *browser_command = g_malloc(sizeof(char) * browser_command_size);
      if (browser_command)
      {
        snprintf(browser_command, browser_command_size, "%s %s",
            browser_exe, link);

        STARTUPINFO si;
        PROCESS_INFORMATION pinf;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pinf, sizeof(pinf));

        if (! CreateProcess(NULL, browser_command,
              NULL, NULL, FALSE, 0, NULL, NULL, &si, &pinf))
        {
          put_status_message(_("Error launching external command"), ui);
        }

        CloseHandle(pinf.hProcess);
        CloseHandle(pinf.hThread);

        g_free(browser_command);
        browser_command = NULL;
      }
    }
  }
#endif
}

static void about_window(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *dialog = gtk_about_dialog_new();

  GString *imagefile = g_string_new("");
  build_path(imagefile, PIXMAP_PATH, "mp3splt-gtk.png");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(imagefile->str, NULL);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  g_string_free(imagefile, TRUE);
  
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), (gchar *)PACKAGE_NAME);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 PACKAGE_NAME" : Copyright © 2005-2011 Alexandru"
                                 " Munteanu \n mp3splt : Copyright © 2002-2005 Matteo Trotta");

  gchar b3[100] = { '\0' };
  gchar *b1 = _("using");
  gchar library_version[20] = { '\0' };
  mp3splt_get_version(library_version);
  g_snprintf(b3, 100, "-%s-\n%s libmp3splt %s",
             _("release of "MP3SPLT_GTK_DATE), b1, library_version);
  
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), b3);
  
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),
                                "\n"
                                "This program is free software; you can "
                                "redistribute it and/or \n"
                                "modify it under the terms of the GNU General Public License\n"
                                "as published by the Free Software "
                                "Foundation; either version 2\n"
                                "of the License, or (at your option) "
                                "any later version.\n\n"
                                "This program is distributed in the "
                                "hope that it will be useful,\n"
                                "but WITHOUT ANY WARRANTY; without even "
                                "the implied warranty of\n"
                                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                "GNU General Public License for more details.\n\n"
                                "You should have received a copy of the GNU General Public License\n"
                                "along with this program; if not, write "
                                "to the Free Software\n"
                                "Foundation, Inc., 59 Temple Place -"
                                "Suite 330, Boston, MA  02111-1307, "
                                "USA.");

  g_signal_connect(G_OBJECT(dialog), "activate-link", G_CALLBACK(activate_url), ui);

  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");

  gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog),
      "Mario Blättermann <mariobl@gnome.org>");

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/*! Removes status bar message

Used for the ok button event.
*/
void remove_status_message(gui_state *gui)
{
  guint status_id = gtk_statusbar_get_context_id(gui->status_bar, "mess");
  gtk_statusbar_pop(gui->status_bar, status_id);
}

/*! Output a info message to the status message bar

The message type is automatically set to SPLT_MESSAGE_INFO.
If you don't want that use put_status_message instead.
\param text The text that has to be displayed.
*/

void put_status_message(const gchar *text, ui_state *ui)
{
  put_status_message_with_type(text, SPLT_MESSAGE_INFO, ui);
}

/*! Output a message to the status message bar.

\param text The text that has to be displayed.
\param splt_message_type The type of the message.

If the type is to be set to SPLT_MESSAGE_INFO put_status_message
can be used instead; The enum for the message types is defined in
libmp3splt.h
 */
void put_status_message_with_type(const gchar *text, splt_message_type mess_type, ui_state *ui)
{
  gui_state *gui = ui->gui;

  if (mess_type == SPLT_MESSAGE_INFO)
  {
    guint status_id = gtk_statusbar_get_context_id(gui->status_bar, "mess");
    gtk_statusbar_pop(gui->status_bar, status_id);
    gtk_statusbar_push(gui->status_bar, status_id, text);
  }

  put_message_in_history(text, mess_type, ui);
}

//!event for the cancel button
void cancel_button_event(GtkWidget *widget, ui_state *ui)
{
  lmanager_stop_split(ui);

  if (widget != NULL)
  {
    gtk_widget_set_sensitive(widget, FALSE);
  }

  put_status_message(_(" info: stopping the split process.. please wait"), ui);
}

static void show_preferences_window(GtkWidget *widget, ui_state *ui)
{
  if (ui->gui->preferences_dialog == NULL)
  {
    GtkWidget *preferences_dialog = gtk_dialog_new_with_buttons(_("Preferences"), NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
    ui->gui->preferences_dialog = preferences_dialog;

    gtk_window_set_default_size(GTK_WINDOW(preferences_dialog), 650, 380);
    gtk_window_set_position(GTK_WINDOW(preferences_dialog), GTK_WIN_POS_CENTER);

    GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(preferences_dialog));
    gtk_box_pack_start(GTK_BOX(area), ui->gui->preferences_widget, TRUE, TRUE, 0);
  }

  gtk_widget_show_all(ui->gui->preferences_dialog);
  gtk_dialog_run(GTK_DIALOG(ui->gui->preferences_dialog));
  gtk_widget_hide(ui->gui->preferences_dialog);
}

static void show_tracktype_window(GtkWidget *widget, ui_state *ui)
{
  if (ui->gui->freedb_dialog == NULL)
  {
    GtkWidget *freedb_dialog = gtk_dialog_new_with_buttons(_("TrackType"), NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
    ui->gui->freedb_dialog = freedb_dialog;

    gtk_window_set_default_size(GTK_WINDOW(freedb_dialog), 500, 300);
    gtk_window_set_position(GTK_WINDOW(freedb_dialog), GTK_WIN_POS_CENTER);

    GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(freedb_dialog));
    gtk_box_pack_start(GTK_BOX(area), ui->gui->freedb_widget, TRUE, TRUE, 0);

    GtkWidget *action_area = gtk_dialog_get_action_area(GTK_DIALOG(freedb_dialog));
    gtk_box_pack_start(GTK_BOX(action_area), ui->gui->freedb_add_button, FALSE, FALSE, 0);

    gtk_box_reorder_child(GTK_BOX(action_area), ui->gui->freedb_add_button, 0);
  }

  gtk_widget_show_all(ui->gui->freedb_dialog);
  hide_freedb_spinner(ui->gui);
  gtk_dialog_run(GTK_DIALOG(ui->gui->freedb_dialog));
  gtk_widget_hide(ui->gui->freedb_dialog);
}

static void show_split_files_window(GtkWidget *widget, ui_state *ui)
{
  if (ui->gui->split_files_dialog == NULL)
  {
    GtkWidget *split_files_dialog = gtk_dialog_new_with_buttons(_("Split files"), NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
    ui->gui->split_files_dialog = split_files_dialog;

    gtk_window_set_default_size(GTK_WINDOW(split_files_dialog), 500, 300);
    gtk_window_set_position(GTK_WINDOW(split_files_dialog), GTK_WIN_POS_CENTER);

    GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(split_files_dialog));
    gtk_box_pack_start(GTK_BOX(area), ui->gui->split_files_widget, TRUE, TRUE, 0);

    GtkWidget *action_area = gtk_dialog_get_action_area(GTK_DIALOG(split_files_dialog));
    gtk_box_pack_start(GTK_BOX(action_area), ui->gui->queue_files_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_area), ui->gui->remove_file_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(action_area), ui->gui->remove_all_files_button, FALSE, FALSE, 0);

    gtk_box_reorder_child(GTK_BOX(action_area), ui->gui->queue_files_button, 0);
    gtk_box_reorder_child(GTK_BOX(action_area), ui->gui->remove_file_button, 1);
    gtk_box_reorder_child(GTK_BOX(action_area), ui->gui->remove_all_files_button, 2);
  }

  gtk_widget_show_all(ui->gui->split_files_dialog);
  gtk_dialog_run(GTK_DIALOG(ui->gui->split_files_dialog));
  gtk_widget_hide(ui->gui->split_files_dialog);
}

//!event for the split button
static void split_button_event(GtkWidget *widget, ui_state *ui)
{
  if (ui->status->splitting)
  {
    put_status_message(_(" error: split in progress..."), ui);
    return;
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);

  gint err = SPLT_OK;

  put_options_from_preferences(ui);

  if (mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE,&err) != SPLT_OPTION_NORMAL_MODE)
  {
    if (!get_checked_output_radio_box(ui))
    {
      mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
    }
  }

  ui->status->filename_to_split = get_input_filename(ui->gui);
  ui->infos->filename_path_of_split = get_output_directory(ui);
  if (ui->infos->filename_path_of_split != NULL)
  {
    ui->status->splitting = TRUE;
    create_thread(split_it, ui, TRUE, NULL);
    gtk_widget_set_sensitive(ui->gui->cancel_button, TRUE);
  }
  else
  {
    put_status_message(_(" error: no file selected"), ui);
  }
}

//!creates the toolbar
static GtkWidget *create_toolbar(ui_state *ui)
{
  GtkWidget *box = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_box_pack_start(GTK_BOX(box), 
      gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_SMALL_TOOLBAR), 
      FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(_("Split !")), FALSE, FALSE, 0);

  GtkWidget *split_button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(split_button), box);
  gtk_widget_set_tooltip_text(split_button,_("Split !"));
  gtk_container_set_border_width(GTK_CONTAINER(split_button), 0);
  gtk_button_set_relief(GTK_BUTTON(split_button), GTK_RELIEF_HALF);

  g_signal_connect(G_OBJECT(split_button), "clicked", G_CALLBACK(split_button_event), ui);

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), split_button, TRUE, FALSE, 0);

  GtkWidget *vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return vbox;
}

//!event for the "messages history" button
static void show_messages_history_dialog(GtkWidget *widget, ui_state *ui)
{
  gtk_widget_show_all(ui->gui->mess_history_dialog);
  gtk_dialog_run(GTK_DIALOG(ui->gui->mess_history_dialog));
  gtk_widget_hide(ui->gui->mess_history_dialog);
}

#ifndef NO_GNOME
static void ShowHelp(GtkWidget *widget, ui_state *ui)
{
  GError* gerror = NULL;
  gtk_show_uri(gdk_screen_get_default(), "ghelp:mp3splt-gtk",  gtk_get_current_event_time(), &gerror);
}
#endif

static gchar *my_dgettext(const gchar *key, const gchar *domain)
{
  return dgettext("mp3splt-gtk", key);
}

static void player_pause_action(GtkWidget *widget, ui_state *ui)
{
  pause_event(ui->gui->pause_button, ui);
}
 
static void player_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 2./100. * total_time * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 2./100. * total_time * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_big_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 15./100. * total_time * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_big_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 15./100. * total_time * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_small_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 100 * 3 * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_small_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat new_time = ui->infos->current_time * 10 - 100 * 3 * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_seek_to_next_splitpoint_action(GtkWidget *widget, ui_state *ui)
{
  gint time_left = -1;
  gint time_right = -1;
  get_current_splitpoints_time_left_right(&time_left, &time_right, NULL, ui);

  if (time_right != -1)
  {
    player_seek(time_right * 10, ui);
  }
}

static void player_seek_to_previous_splitpoint_action(GtkWidget *widget, ui_state *ui)
{
  gint time_left = -1;
  gint time_right = -1;
  get_current_splitpoints_time_left_right(&time_left, &time_right, NULL, ui);

  if (time_left != -1)
  {
    player_seek(time_left * 10, ui);
  }
}

static void delete_closest_splitpoint(GtkWidget *widget, ui_state *ui)
{
  gint left_index_point = -1;
  gint right_index_point = -1;

  gint i = 0;
  for (i = 0; i < ui->infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i, ui);
    if (current_point_hundr_secs <= ui->infos->current_time)
    {
      left_index_point = i;
      continue;
    }

    if (current_point_hundr_secs > ui->infos->current_time + (DELTA * 2))
    {
      right_index_point = i;
      break;
    }
  }

  if (left_index_point == -1 && right_index_point == -1)
  {
    return;
  }

  gint time_to_left = INT_MAX;
  if (left_index_point != -1)
  {
    time_to_left = ui->infos->current_time - get_splitpoint_time(left_index_point, ui);
  }

  gint time_to_right = INT_MAX;
  if (right_index_point != -1)
  {
    time_to_right = get_splitpoint_time(right_index_point, ui) - ui->infos->current_time;
  }

  if (time_to_right > time_to_left)
  {
    remove_splitpoint(left_index_point, TRUE, ui);
  }
  else
  {
    remove_splitpoint(right_index_point, TRUE, ui);
  }
}

static void zoom_in(GtkWidget *widget, ui_state *ui)
{
  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff += fraction;
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui);
}

static void zoom_out(GtkWidget *widget, ui_state *ui)
{
  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff -= fraction; 
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui);
}

static gboolean window_key_press_event(GtkWidget *window, GdkEventKey *event, ui_state *ui)
{
  if (event->type != GDK_KEY_PRESS) { return; }

  if (event->state != 0)
  {
    return FALSE;
  }

  switch (event->keyval)
  {
    case GDK_Left:
      player_seek_backward_action(NULL, ui);
      return TRUE;
    case GDK_Right:
      player_seek_forward_action(NULL, ui);
      return TRUE;
    default:
      return FALSE;
  }
}


static void add_filters_to_file_chooser(GtkWidget *file_chooser)
{
  GtkFileFilter *our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name(our_filter, _("mp3 and ogg files (*.mp3 *.ogg)"));
  gtk_file_filter_add_pattern(our_filter, "*.mp3");
  gtk_file_filter_add_pattern(our_filter, "*.ogg");
  gtk_file_filter_add_pattern(our_filter, "*.MP3");
  gtk_file_filter_add_pattern(our_filter, "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);

  our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("mp3 files (*.mp3)"));
  gtk_file_filter_add_pattern(our_filter, "*.mp3");
  gtk_file_filter_add_pattern(our_filter, "*.MP3");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);

  our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("ogg files (*.ogg)"));
  gtk_file_filter_add_pattern(our_filter, "*.ogg");
  gtk_file_filter_add_pattern(our_filter, "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);
}

/*! \brief Events for browse button

Also used for the cddb and cue browses.
*/
static void open_file_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *file_chooser = gtk_file_chooser_dialog_new(_("Choose File"), NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN,
      GTK_RESPONSE_ACCEPT, NULL);

  add_filters_to_file_chooser(file_chooser);
  wh_set_browser_directory_handler(ui, file_chooser);

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    file_chooser_ok_event(filename, ui);
    if (filename)
    {
      g_free(filename);
      filename = NULL;
    }
  }

  gtk_widget_destroy(file_chooser);
  remove_status_message(ui->gui);
}

//!creates the menu bar
static GtkWidget *create_menu_bar(ui_state *ui)
{
  static const GtkActionEntry entries[] = {
    //name, stock id, label, accelerator, tooltip
    { "FileMenu", NULL, N_("_File"), NULL, NULL },
    { "ViewMenu", NULL, N_("_View"), NULL, NULL },
    { "PlayerMenu", NULL, N_("_Player"), NULL, NULL },
    { "HelpMenu", NULL, N_("_Help"), NULL, NULL },

    { "Open", GTK_STOCK_OPEN, N_("_Open..."), "<Ctrl>O", N_("Open"),
      G_CALLBACK(open_file_button_event) },

    { "Import", GTK_STOCK_FILE, N_("_Import splitpoints from file..."), "<Ctrl>I", 
      N_("Import splitpoints from file..."), G_CALLBACK(import_event) },

    { "ImportFromTrackType", GTK_STOCK_FIND, N_("_Import splitpoints from TrackType.org..."), "<Ctrl>T",
      N_("Import splitpoints from TrackType.org..."),
      G_CALLBACK(show_tracktype_window) },

    { "Export", GTK_STOCK_SAVE_AS, N_("_Export splitpoints..."), "<Ctrl>E",
      N_("Export splitpoints"), G_CALLBACK(ChooseCueExportFile) },

    { "Preferences", GTK_STOCK_PREFERENCES, N_("_Preferences"), "<Ctrl>P", N_("Preferences"),
      G_CALLBACK(show_preferences_window) },

    { "SplitFiles", NULL, N_("Split _Files"), "<Ctrl>F", N_("Split Files"),
      G_CALLBACK(show_split_files_window) },

    { "Split", GTK_STOCK_APPLY, N_("_Split !"), "<Ctrl>S", N_("Split"),
      G_CALLBACK(split_button_event) },

    { "Quit", GTK_STOCK_QUIT, N_("_Quit"), "<Ctrl>Q", N_("Quit"),
      G_CALLBACK(exit_application) },

#ifndef NO_GNOME
    { "Contents", GTK_STOCK_HELP, N_("_Contents"), "F1", N_("Contents"),
      G_CALLBACK(ShowHelp)},
#endif

    { "Messages history", GTK_STOCK_INFO, N_("Messages _history"), "<Ctrl>H", N_("Messages history"),
      G_CALLBACK(show_messages_history_dialog) },

    { "About", GTK_STOCK_ABOUT, N_("_About"), "<Ctrl>A", N_("About"),
      G_CALLBACK(about_window)},

    //player key bindings
    { "Player_pause", NULL, N_("P_ause / Play"), "space", N_("Pause/Play"),
      G_CALLBACK(player_pause_action)},

    { "Player_forward", GTK_STOCK_MEDIA_FORWARD, N_("Seek _forward"), "Right", N_("Seek forward"),
      G_CALLBACK(player_seek_forward_action)},
    { "Player_backward", GTK_STOCK_MEDIA_REWIND, N_("Seek _backward"), "Left", N_("Seek backward"),
      G_CALLBACK(player_seek_backward_action)},

    { "Player_small_forward", NULL, N_("Small seek f_orward"), "<Alt>Right", N_("Small seek forward"),
      G_CALLBACK(player_small_seek_forward_action)},
    { "Player_small_backward", NULL, N_("Small seek back_ward"), "<Alt>Left", N_("Small seek backward"),
      G_CALLBACK(player_small_seek_backward_action)},

    { "Player_big_forward", NULL, N_("Big seek fo_rward"), "<Shift>Right", N_("Big seek forward"),
      G_CALLBACK(player_big_seek_forward_action)},
    { "Player_big_backward", NULL, N_("Big seek bac_kward"), "<Shift>Left", N_("Big seek backward"),
      G_CALLBACK(player_big_seek_backward_action)},

    { "Player_next_splitpoint", GTK_STOCK_MEDIA_NEXT, N_("Seek to _next splitpoint"), "<Ctrl>Right", 
      N_("Seek to next splitpoint"), G_CALLBACK(player_seek_to_next_splitpoint_action)},
    { "Player_previous_splitpoint", GTK_STOCK_MEDIA_PREVIOUS, N_("Seek to _previous splitpoint"), "<Ctrl>Left", 
      N_("Seek to previous splitpoint"), G_CALLBACK(player_seek_to_previous_splitpoint_action)},

    { "Add_splitpoint", GTK_STOCK_ADD, N_("Add _splitpoint"), "s", 
      N_("Add splitpoint"), G_CALLBACK(add_splitpoint_from_player)},

    { "Delete_closest_splitpoint", GTK_STOCK_REMOVE, N_("_Delete closest splitpoint"), "d", 
      N_("Delete closest splitpoint"), G_CALLBACK(delete_closest_splitpoint)},

    { "Zoom_in", GTK_STOCK_ZOOM_IN, N_("Zoom _in"), "<Ctrl>plus", N_("Zoom in"), G_CALLBACK(zoom_in)},
    { "Zoom_out", GTK_STOCK_ZOOM_OUT, N_("Zoom _out"), "<Ctrl>minus", N_("Zoom out"), G_CALLBACK(zoom_out)},
  };

  static const gchar *ui_info = 
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='FileMenu'>"
    "      <menuitem action='Open'/>"
    "      <separator/>"
    "      <menuitem action='Import'/>"
    "      <menuitem action='ImportFromTrackType'/>"
    "      <menuitem action='Export'/>"
    "      <separator/>"
    "      <menuitem action='Preferences'/>"
    "      <separator/>"
    "      <menuitem action='Split'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='ViewMenu'>"
    "      <menuitem action='SplitFiles'/>"
    "    </menu>"
    "    <menu action='PlayerMenu'>"
    "      <menuitem action='Player_pause'/>"
    "      <separator/>"
    "      <menuitem action='Player_forward'/>"
    "      <menuitem action='Player_backward'/>"
    "      <menuitem action='Player_small_forward'/>"
    "      <menuitem action='Player_small_backward'/>"
    "      <menuitem action='Player_big_forward'/>"
    "      <menuitem action='Player_big_backward'/>"
    "      <menuitem action='Player_next_splitpoint'/>"
    "      <menuitem action='Player_previous_splitpoint'/>"
    "      <separator/>"
    "      <menuitem action='Add_splitpoint'/>"
    "      <menuitem action='Delete_closest_splitpoint'/>"
    "      <separator/>"
    "      <menuitem action='Zoom_in'/>"
    "      <menuitem action='Zoom_out'/>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
#ifndef NO_GNOME
    "      <menuitem action='Contents'/>"
    "      <separator/>"
#endif
    "      <menuitem action='Messages history'/>"
    "      <separator/>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";

  GtkActionGroup *action_group = gtk_action_group_new("Actions");
  ui->gui->action_group = action_group;

  gtk_action_group_set_translation_domain(action_group, "mp3splt-gtk");
  gtk_action_group_set_translate_func(action_group,
                  (GtkTranslateFunc)my_dgettext, NULL, NULL);

  gtk_action_group_add_actions(action_group, entries, G_N_ELEMENTS(entries), ui);
  GtkUIManager *uim = gtk_ui_manager_new();
  gtk_ui_manager_insert_action_group(uim, action_group, 0);

  g_signal_connect(G_OBJECT(ui->gui->window), "key_press_event",
      G_CALLBACK(window_key_press_event), ui);

  gtk_window_add_accel_group(GTK_WINDOW(ui->gui->window), gtk_ui_manager_get_accel_group(uim));
  gtk_ui_manager_add_ui_from_string(uim, ui_info, -1, NULL);
 
  GtkWidget *menu_box = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(menu_box), gtk_ui_manager_get_widget(uim, "/MenuBar"), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(menu_box), create_toolbar(ui), TRUE, TRUE, 0);
 
  player_key_actions_set_sensitivity(FALSE, ui->gui);

  return menu_box;
}

static void file_selection_changed(GtkFileChooser *open_file_chooser, ui_state *ui)
{
  gchar *filename = gtk_file_chooser_get_filename(open_file_chooser);
  gchar *previous_fname = get_input_filename(ui->gui);
  if (previous_fname != NULL && filename != NULL && 
      strcmp(filename, previous_fname) == 0)
  {
    return;
  }

  if (filename != NULL)
  {
    file_chooser_ok_event(filename, ui);
    g_free(filename);
    filename = NULL;
    return;
  }

  if (previous_fname != NULL && strlen(previous_fname) != 0)
  {
    //gtk_file_chooser_set_filename() does not work here.
    ui->status->queue_set_filename_to_file_chooser_button = TRUE;
  }
}

static void file_set_event(GtkFileChooserButton *open_file_chooser_button, ui_state *ui)
{
  file_selection_changed(GTK_FILE_CHOOSER(open_file_chooser_button), ui);
}

static GtkWidget *create_choose_file_frame(ui_state *ui)
{
  GtkWidget *open_file_chooser_button = gtk_file_chooser_button_new(("_Open..."), GTK_FILE_CHOOSER_ACTION_OPEN);
  ui->gui->open_file_chooser_button = open_file_chooser_button;
  add_filters_to_file_chooser(open_file_chooser_button);
  wh_set_browser_directory_handler(ui, open_file_chooser_button);

  g_signal_connect(G_OBJECT(open_file_chooser_button), "file-set", G_CALLBACK(file_set_event), ui);
  g_signal_connect(G_OBJECT(open_file_chooser_button), "selection-changed",
      G_CALLBACK(file_selection_changed), ui);

  gchar *fname = get_input_filename(ui->gui);
  if (fname != NULL && strlen(fname) != 0)
  {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(open_file_chooser_button), get_input_filename(ui->gui));
  }

  return open_file_chooser_button;
}

//!main vbox
static GtkWidget *create_main_vbox(ui_state *ui)
{
  GtkWidget *main_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 0);

  gtk_box_pack_start(GTK_BOX(main_vbox), 
      create_choose_file_frame(ui), FALSE, FALSE, 0);

  /* tabbed notebook */
  GtkWidget *notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
   
  /* player page */
  GtkWidget *player_vbox = wh_vbox_new();
  GtkWidget *notebook_label = gtk_label_new(_("Player"));

  ui->gui->player_box = create_player_control_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->player_box, FALSE, FALSE, 0);

  ui->gui->playlist_box = create_player_playlist_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->playlist_box, TRUE, TRUE, 0);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_vbox, notebook_label);

  /* splitpoints page */
  GtkWidget *splitpoints_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(splitpoints_vbox), 0);
  gtk_box_pack_start(GTK_BOX(splitpoints_vbox), create_splitpoints_frame(ui), TRUE, TRUE, 0);
 
  notebook_label = gtk_label_new(_("Splitpoints"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), splitpoints_vbox, notebook_label);

  /* split files page */
  ui->gui->split_files_widget = create_split_files_frame(ui);
  
  /* freedb page */
  ui->gui->freedb_widget = create_freedb_frame(ui);
  
  /* special split page */
  GtkWidget *special_split_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(special_split_vbox), 0);
  GtkWidget *frame = create_special_split_page(ui);
  gtk_box_pack_start(GTK_BOX(special_split_vbox), frame, TRUE, TRUE, 0);
  notebook_label = gtk_label_new(_("Type of split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), special_split_vbox, notebook_label);
 
  /* preferences widget */
  ui->gui->preferences_widget = create_choose_preferences(ui);

  /* progress bar */
  GtkProgressBar *percent_progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
  ui->gui->percent_progress_bar = percent_progress_bar;
  gtk_progress_bar_set_fraction(percent_progress_bar, 0.0);
  gtk_progress_bar_set_text(percent_progress_bar, "");

#if GTK_MAJOR_VERSION >= 3
  gtk_progress_bar_set_show_text(percent_progress_bar, TRUE);
#endif

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(percent_progress_bar), TRUE, TRUE, 0);

  //stop button
  GtkWidget *cancel_button = wh_create_cool_button(GTK_STOCK_CANCEL,_("S_top"), FALSE);
  ui->gui->cancel_button = cancel_button;
  g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(cancel_button_event), ui);
  gtk_box_pack_start(GTK_BOX(hbox), cancel_button, FALSE, TRUE, 3);
  gtk_widget_set_sensitive(cancel_button, FALSE);

  gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 2);

  /* show messages history dialog */
  create_mess_history_dialog(ui);
 
  /* statusbar */
  GtkStatusbar *status_bar = GTK_STATUSBAR(gtk_statusbar_new());
  ui->gui->status_bar = status_bar;

  gtk_box_pack_start(GTK_BOX(main_vbox), GTK_WIDGET(status_bar), FALSE, FALSE, 0);

  return main_vbox;
}

static void move_and_resize_main_window(ui_state *ui)
{
  const ui_main_window *main_win = ui_get_main_window_infos(ui);

  gint x = main_win->root_x_pos;
  gint y = main_win->root_y_pos;

  if (x != 0 && y != 0)
  {
    gtk_window_move(GTK_WINDOW(ui->gui->window), x, y);
  }
  else
  {
    gtk_window_set_position(GTK_WINDOW(ui->gui->window), GTK_WIN_POS_CENTER);
  }

  gtk_window_resize(GTK_WINDOW(ui->gui->window), main_win->width, main_win->height);
}

void create_application(ui_state *ui)
{
  initialize_window(ui);

  GtkWidget *window_vbox = wh_vbox_new();
  gtk_container_add(GTK_CONTAINER(ui->gui->window), window_vbox);

  gtk_box_pack_start(GTK_BOX(window_vbox), create_menu_bar(ui), FALSE, FALSE, 0);  
  gtk_box_pack_start(GTK_BOX(window_vbox), create_main_vbox(ui), TRUE, TRUE, 0);

  ui_load_preferences(ui);

  move_and_resize_main_window(ui);

  gtk_widget_show_all(ui->gui->window);

  if (ui->infos->selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_hide(ui->gui->playlist_box);
  }

  hide_freedb_spinner(ui->gui);
}

/*!Output an error message from libmp3splt to the status bar

  \param The error number from the library.
 */
void print_status_bar_confirmation(gint error, ui_state *ui)
{
  char *error_from_library = mp3splt_get_strerror(ui->mp3splt_state, error);
  if (error_from_library == NULL) { return; }

  put_status_message(error_from_library, ui);
  free(error_from_library);
  error_from_library = NULL;
}

