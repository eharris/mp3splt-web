/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2009 Alexandru Munteanu
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

/**********************************************************
 * Filename: main_win.c
 *
 * main file that initialises the menubar, the toolbar, 
 * the tabs, about window, status error messages
 *
 *********************************************************/

//we include the "config.h" file from the config options
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "0.5.7"
#define PACKAGE_NAME "mp3splt-gtk"
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libmp3splt/mp3splt.h>
#include <gdk/gdkkeysyms.h>

#include "util.h"
#include "main_win.h"
#include "mp3splt-gtk.h"
#include "tree_tab.h"
#include "split_files.h"
#include "cddb_cue.h"
#include "utilities.h"
#include "preferences_tab.h"
#include "freedb_tab.h"
#include "special_split.h"
#include "utilities.h"
#include "player_tab.h"
#include "player.h"
#include "messages.h"

//main window
GtkWidget *window = NULL;
GtkAccelGroup *window_accel_group = NULL;

//status bar
GtkWidget *status_bar;

//if we are on the preferences tab, then TRUE
gint preferences_tab = FALSE;

//player box
GtkWidget *player_box;

//the split button
GtkWidget *split_button;
//the split freedb button
GtkWidget *split_freedb_button;

//new window for the progress bar
GtkWidget *percent_progress_bar;

//filename and path for the file to split
gchar *filename_to_split;
gchar *filename_path_of_split;

//if we are currently splitting
gint we_are_splitting = FALSE;
gint we_quit_main_program = FALSE;

GtkWidget *player_vbox = NULL;

//stop button to cancel the split
GtkWidget *cancel_button = NULL;

extern GtkWidget *mess_history_dialog;

extern GtkWidget *entry;
extern GtkWidget *directory_entry;
extern GArray *splitpoints;
extern gint selected_id;
extern splt_state *the_state;
extern splt_freedb_results *search_results;
extern GList *player_pref_list;
extern gchar **split_files;
extern gint max_split_files;
extern gint selected_player;
extern silence_wave *silence_points;
extern gint number_of_silence_points;

GtkWidget *playlist_box = NULL;

//close the window and exit button function
void quit(GtkWidget *widget, gpointer   data)
{
  //if we are in the middle of the split
  if (we_are_splitting)
  {
    gint err = SPLT_OK;
    mp3splt_stop_split(the_state,&err);
    print_status_bar_confirmation(err);

    //wait to finish split then quit
    we_quit_main_program = TRUE;
    put_status_message(_(" info: stopping the split process before exiting"));
  }

  //quit the player: currently closes gstreamer
  if (player_is_running())
  {
    player_quit();
  }

  //free player choices list
  g_list_free(player_pref_list);
  //we free the splitpoints
  g_array_free(splitpoints, TRUE);

  //we free the GUI silence points
  if (silence_points)
  {
    g_free(silence_points);
    silence_points = NULL;
    number_of_silence_points = 0;
  }

  gint err = SPLT_OK;
  //we free left variables in the library
  mp3splt_free_state(the_state,&err);
  print_status_bar_confirmation(err);

  //we definetly quit the program...
  gtk_main_quit();
}

//initializes window
void initialize_window()
{
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  window_accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), window_accel_group);
   
  //sets the title
  gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME" "VERSION);
  //sets the width
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(quit), NULL);
 
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
 
  //for the bitmap
  GdkPixbuf *pixbuf =
    gdk_pixbuf_new_from_file(PIXMAP_PATH"mp3splt-gtk_ico.png", NULL);
  gtk_window_set_default_icon(pixbuf);
}

void about_window(GtkWidget *widget, gpointer *data)
{
  GtkWidget *dialog = gtk_about_dialog_new();

  //for the bitmap
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (PIXMAP_PATH"mp3splt-gtk.png",
      NULL);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), (gchar *)PACKAGE_NAME);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 PACKAGE_NAME" (c) 2005-2009 Munteanu"
                                 " Alexandru \n mp3splt (c) 2002-2005 Matteo Trotta");
  
  gchar *b1 = NULL, *b2 = NULL;
  gchar b3[100] = { '\0' };
  b1 = (gchar *)_("using");
  b2 = (gchar *)_("created from");
  gchar library_version[20] = { '\0' };
  mp3splt_get_version(library_version);
  g_snprintf(b3, 100, "-%s 16/05/09-\n%s libmp3splt %s (%s mp3splt)",
             _("release of"), b1, library_version, b2);
  
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

  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net/");

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

//used  for the ok button event
//removes status bar message
void remove_status_message()
{
  guint status_id =
    gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "mess");
  gtk_statusbar_pop(GTK_STATUSBAR(status_bar), status_id);
}

void put_status_message(const gchar *text)
{
  put_status_message_with_type(text, SPLT_MESSAGE_INFO);
}

void put_status_message_with_type(const gchar *text, splt_message_type mess_type)
{
  if (mess_type == SPLT_MESSAGE_INFO)
  {
    guint status_id =
      gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "mess");

    gtk_statusbar_pop(GTK_STATUSBAR(status_bar), status_id);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_id, text);
  }

  put_message_in_history(text, mess_type);
}

//event for the split button
void cancel_button_event(GtkWidget *widget, gpointer data)
{
  gint err = SPLT_OK;
  mp3splt_stop_split(the_state,&err);
  print_status_bar_confirmation(err);
  
  if (widget != NULL)
  {
    gtk_widget_set_sensitive(widget, FALSE);
  }
  put_status_message(_(" info: stopping the split process.. please wait"));
}

//event for the split button
void split_button_event(GtkWidget *widget, gpointer data)
{
  //if we are not splitting
  if (!we_are_splitting)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_DEFAULT);

    gint err = SPLT_OK;

    put_options_from_preferences();

    //output format
    if (mp3splt_get_int_option(the_state, SPLT_OPT_SPLIT_MODE,&err)
        != SPLT_OPTION_NORMAL_MODE)
    {
      if (!get_checked_output_radio_box())
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
            SPLT_OUTPUT_FORMAT);
      }
    }

    filename_to_split = (gchar *) gtk_entry_get_text(GTK_ENTRY(entry));

    filename_path_of_split = (gchar *)
      gtk_entry_get_text(GTK_ENTRY(directory_entry));

    if (filename_path_of_split != NULL)
    {
      we_are_splitting = TRUE;
      g_thread_create((GThreadFunc)split_it, NULL, TRUE, NULL);
      gtk_widget_set_sensitive(GTK_WIDGET(cancel_button), TRUE);
    }
    else
    {
      put_status_message((gchar *)_(" error: no file selected"));
    }
  }
  else
  {
    put_status_message((gchar *)_(" error: split in progress..."));
  }
}

//creates the toolbar
GtkWidget *create_toolbar()
{
  //the toolbar
  GtkWidget *toolbar = gtk_toolbar_new();
  
  //separator
  GtkWidget *toolbar_button = (GtkWidget *)gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolbar_button), 0);
  gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolbar_button), TRUE);
  gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(toolbar_button),
                                   FALSE);
  
  //split button
  split_button = (GtkWidget *)gtk_tool_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_widget_set_tooltip_text(split_button, _("Split !"));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(split_button), 1);
  g_signal_connect(G_OBJECT(split_button), "clicked",
      G_CALLBACK(split_button_event), NULL);
  
  //separator
  toolbar_button = (GtkWidget *)gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(toolbar_button), 2);
  gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolbar_button), TRUE);
  gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(toolbar_button),
                                   FALSE);

  //toolbar preferences
  gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar), TRUE);
  gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  
  return toolbar;
}

//event for the split button
void show_messages_history_dialog(GtkWidget *widget, gpointer data)
{
  gtk_widget_show_all(GTK_WIDGET(mess_history_dialog));
}

//creates the menu bar
GtkWidget *create_menu_bar()
{
  GtkWidget *menu_box;
  menu_box = gtk_hbox_new(FALSE,0);
  
  //define the menu
  static GtkActionEntry entries[] = {
    //name, stock id,   label
    { "FileMenu", NULL, N_("_File") },  
    { "HelpMenu", NULL, N_("_Help") },
    //name, stock id, label, accelerator, tooltip
    { "Split", GTK_STOCK_APPLY, N_("_Split !"), "<Ctrl>S", N_("Split"),
      G_CALLBACK(split_button_event) },
    { "Messages history", GTK_STOCK_INFO, N_("Messages _history"), "<Ctrl>H", N_("Messages history"),
      G_CALLBACK(show_messages_history_dialog) },
    { "Quit", GTK_STOCK_QUIT, N_("_Quit"), "<Ctrl>Q", N_("Quit"),
      G_CALLBACK(quit) },
    //name, stock id, label, accelerator
    { "About", GTK_STOCK_ABOUT, N_("_About"), "<Ctrl>A", N_("About"),
      G_CALLBACK(about_window)},
  };
  static guint n_entries = G_N_ELEMENTS (entries);

  //build the menu
  static const gchar *ui_info = 
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='FileMenu'>"
    "      <menuitem action='Split'/>"
    "      <menuitem action='Messages history'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";

  GtkUIManager *ui;
  GtkActionGroup *actions;
  
  actions = gtk_action_group_new ("Actions");
  //translation
  gtk_action_group_set_translation_domain(actions, NULL);
  //adding the GtkActionEntry to GtkActionGroup
  gtk_action_group_add_actions (actions, entries, n_entries, NULL);
  ui = gtk_ui_manager_new ();
  //set action to the ui
  gtk_ui_manager_insert_action_group (ui, actions, 0);
  //set the actions to the window
  gtk_window_add_accel_group (GTK_WINDOW (window), 
                              gtk_ui_manager_get_accel_group (ui));
  //add ui from string
  gtk_ui_manager_add_ui_from_string (ui, ui_info, -1, NULL);
  
  //attach the menu
  gtk_box_pack_start (GTK_BOX (menu_box), 
                      gtk_ui_manager_get_widget(ui, "/MenuBar"),
                      FALSE, FALSE, 0);
  
  //our toolbar
  GtkWidget *toolbar;
  toolbar = (GtkWidget *)create_toolbar();
  
  //attach the toolbar
  gtk_box_pack_start (GTK_BOX (menu_box), toolbar,
                      TRUE, TRUE, 0);
  
  return menu_box;
}

//creates a cool button with image from stock
//toggle_or_not = TRUE means we create a toggle button
GtkWidget *create_cool_button(gchar *stock_id, gchar *label_text,
    gint toggle_or_not)
{
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *image;
  GtkWidget *button;

  box = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER (box), 2);

  image = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 3);

  if (label_text != NULL)
  {
    label = gtk_label_new (label_text);
    gtk_label_set_text_with_mnemonic(GTK_LABEL(label),label_text);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);
  }
  
  if (toggle_or_not)
  {
    button = gtk_toggle_button_new();
  }
  else
  {
    button = gtk_button_new();
  }
 
  gtk_container_add(GTK_CONTAINER(button),box);
 
  return button;
}

//main vbox
GtkWidget *create_main_vbox()
{
  //big ain box contailning all with statusbar
  GtkWidget *main_vbox;
  //used for pages
  GtkWidget *frame;
  //the tree view
  GtkTreeView *tree_view;
  //the main window tabbed notebook
  GtkWidget *notebook;
  /* label for the notebook */
  GtkWidget *notebook_label;

  /* main vertical box with statusbar */
  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 0);

  frame = (GtkWidget *)create_choose_file_frame();
  gtk_box_pack_start(GTK_BOX(main_vbox), frame, FALSE, FALSE, 0);

  /* tabbed notebook */
  notebook = gtk_notebook_new();
  gtk_box_pack_start (GTK_BOX (main_vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  
  //creating the tree view
  GtkWidget *splitpoints_vbox;
  splitpoints_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (splitpoints_vbox), 0);
  tree_view = (GtkTreeView *)create_tree_view();
  frame = (GtkWidget *)create_choose_splitpoints_frame(tree_view);
  gtk_container_add(GTK_CONTAINER(splitpoints_vbox), frame);
  
  /* player page */
  player_vbox = gtk_vbox_new(FALSE,0);
  notebook_label = gtk_label_new((gchar *)_("Player"));
      
  //player control frame
  player_box = (GtkWidget *)create_player_control_frame(tree_view);
  gtk_box_pack_start(GTK_BOX(player_vbox), player_box, FALSE, FALSE, 0);

  //playlist control frame
  playlist_box = (GtkWidget *)create_player_playlist_frame();

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_vbox,
      (GtkWidget *)notebook_label);
      
  /* splitpoints page */
  notebook_label = gtk_label_new((gchar *)_("Splitpoints"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           splitpoints_vbox,
                           (GtkWidget *)notebook_label);

  /* split files frame */
  GtkWidget *split_files_vbox;
  split_files_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (split_files_vbox), 0);
  
  frame = (GtkWidget *)create_split_files();
  gtk_container_add(GTK_CONTAINER(split_files_vbox), frame);
  
  notebook_label = gtk_label_new((gchar *)_("Split files"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           split_files_vbox,
                           (GtkWidget *)notebook_label);
  
  /* cddb and cue page */
  GtkWidget *cddb_cue_vbox;
  cddb_cue_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (cddb_cue_vbox), 0);

  frame = (GtkWidget *)create_cddb_cue_frame();
  gtk_container_add(GTK_CONTAINER(cddb_cue_vbox), frame);
  
  notebook_label = gtk_label_new((gchar *)_("cddb & cue"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           cddb_cue_vbox,
                           (GtkWidget *)notebook_label);

  /* freedb page */
  GtkWidget *freedb_vbox;
  freedb_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (freedb_vbox), 0);
  
  frame = (GtkWidget *)create_freedb_frame();
  gtk_container_add(GTK_CONTAINER(freedb_vbox), frame);
  
  notebook_label = gtk_label_new((gchar *)_("FreeDB"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           freedb_vbox,
                           (GtkWidget *)notebook_label);
  
  /* special split page */
  GtkWidget *special_split_vbox;
  special_split_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (special_split_vbox), 0);
  frame = (GtkWidget *)create_special_split_page();
  gtk_container_add(GTK_CONTAINER(special_split_vbox), frame);
  notebook_label = gtk_label_new(_("Type of split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           special_split_vbox,
                           (GtkWidget *)notebook_label);
 
  /* preferences page */
  GtkWidget *preferences_vbox;
  preferences_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (preferences_vbox), 0);

  frame = (GtkWidget *)create_choose_preferences();
  gtk_container_add(GTK_CONTAINER(preferences_vbox), frame);

  notebook_label = gtk_label_new((gchar *)_("Preferences"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           preferences_vbox,
                           (GtkWidget *)notebook_label);
  
  /* progress bar */
  percent_progress_bar = gtk_progress_bar_new();
  //we begin at 0
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
                                0.0);
  //we write 0 on the bar
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
                            "");
  
  //hbox for progress bar and cancel button
  GtkWidget *hbox;
  hbox = gtk_hbox_new (FALSE,0);
  //we put the progress bar in the hbox
  gtk_box_pack_start(GTK_BOX(hbox), percent_progress_bar, TRUE, TRUE, 3);
  
  //stop button
  cancel_button = create_cool_button(GTK_STOCK_CANCEL,_("S_top"), FALSE);
  //action for the cancel button
  g_signal_connect(G_OBJECT(cancel_button), "clicked",
                   G_CALLBACK(cancel_button_event), NULL);
  
  //we put the stop button in the hbox
  gtk_box_pack_start(GTK_BOX(hbox), cancel_button, FALSE, TRUE, 3);
  gtk_widget_set_sensitive(GTK_WIDGET(cancel_button), FALSE);
  
  //we put progress bar hbox in the main box
  gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 3);  

  /* show messages history dialog */
  create_mess_history_dialog();
 
  /* statusbar */
  status_bar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(status_bar), FALSE);

  GtkWidget *mess_history_button =
    create_cool_button(GTK_STOCK_INFO, NULL, FALSE);
  gtk_button_set_relief(GTK_BUTTON(mess_history_button), GTK_RELIEF_NONE);
  gtk_widget_set_tooltip_text(mess_history_button,_("Messages history"));
  gtk_box_pack_start(GTK_BOX(status_bar), mess_history_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(mess_history_button), "clicked",
      G_CALLBACK(show_messages_history_dialog), NULL);

  gtk_box_pack_start(GTK_BOX(main_vbox), status_bar, FALSE, FALSE, 0);

  return main_vbox;
}

//main function, creates all the window
void create_all()
{
#ifdef __WIN32__
  set_language();
#endif

  //main vbox containing all + statusbar
  GtkWidget *main_vbox;
  
  initialize_window();
  
  /* vertical box */
  GtkWidget *window_vbox;
  window_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), window_vbox);

  /* menu bar */
  GtkWidget *menu_bar;
  menu_bar = (GtkWidget *)create_menu_bar();
  gtk_box_pack_start(GTK_BOX(window_vbox), menu_bar, FALSE, FALSE, 0);  
  
  /* main vbox */
  main_vbox = (GtkWidget *)create_main_vbox();
  gtk_box_pack_start(GTK_BOX(window_vbox), main_vbox, TRUE, TRUE, 0);
  
  gtk_widget_show_all(window);
 
  hide_disconnect_button();
  gtk_widget_hide(playlist_box);

  //load preferences
  load_preferences();
  combo_remove_unavailable_players();

  //hide connect button if player is gstreamer
  if (selected_player == PLAYER_GSTREAMER)
  {
    hide_connect_button();
  }
}

//print the status bar confirmation
void print_status_bar_confirmation(gint confirmation)
{
  char *error_from_library = mp3splt_get_strerror(the_state, confirmation);
  if (error_from_library != NULL)
  {
    put_status_message(error_from_library);
    free(error_from_library);
    error_from_library = NULL;
  }
}

