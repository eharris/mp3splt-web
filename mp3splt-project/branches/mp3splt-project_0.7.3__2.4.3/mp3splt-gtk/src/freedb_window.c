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
 * The freedb tab
 *
 * this file is used for the cddb tab 
 *   (for searching on freedb)
 *********************************************************/

#include "freedb_window.h"

enum {
  ALBUM_NAME,
  NUMBER,
  FREEDB_TABLE
};

//!add a row to the table
static void add_freedb_row(gchar *album_name, gint album_id,
    gint *revisions, gint revisions_number, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->freedb_tree);

  GtkTreeIter iter;
  gtk_tree_store_append (GTK_TREE_STORE(model), &iter,NULL);
  gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
      ALBUM_NAME, album_name, NUMBER, album_id, -1);

  gint malloc_number = strlen(album_name) + 20;
  gchar *number = malloc(malloc_number * sizeof(gchar *));
  gint i;
  for(i = 0; i < revisions_number; i++)
  {
    g_snprintf(number,malloc_number, _("%s Revision %d"),album_name, revisions[i]);

    GtkTreeIter child_iter;
    gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
    gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
        ALBUM_NAME, number, NUMBER, album_id + i + 1, -1);
  }

  ui->infos->freedb_table_number++;
  g_free(number);
}

//!creates the model for the freedb tree
static GtkTreeModel *create_freedb_model()
{
  GtkTreeStore *model = gtk_tree_store_new(FREEDB_TABLE, G_TYPE_STRING, G_TYPE_INT);
  return GTK_TREE_MODEL(model);
}

//!creates the freedb tree
static GtkTreeView *create_freedb_tree()
{
  GtkTreeModel *model = create_freedb_model();
  return GTK_TREE_VIEW(gtk_tree_view_new_with_model(model));
}

//!creates freedb columns
static void create_freedb_columns(GtkTreeView *freedb_tree)
{
  GtkCellRendererText *renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(ALBUM_NAME));
  GtkTreeViewColumn *name_column = gtk_tree_view_column_new_with_attributes 
    (_("Album title"), GTK_CELL_RENDERER(renderer), "text", ALBUM_NAME, NULL);

  gtk_tree_view_insert_column(freedb_tree, GTK_TREE_VIEW_COLUMN(name_column), ALBUM_NAME);

  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column), 0.5);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(name_column), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(name_column), TRUE);

  gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(name_column), TRUE);
}

static void set_freedb_selected_id_safe(gint selected_id, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->infos->freedb_selected_id = selected_id;
  unlock_mutex(&ui->variables_mutex);
}

static gint get_freedb_selected_id_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint selected_id = ui->infos->freedb_selected_id;
  unlock_mutex(&ui->variables_mutex);
  return selected_id;
}

//!freedb selection has changed
static void freedb_selection_changed(GtkTreeSelection *selection, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->freedb_tree);

  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    gchar *info;
    gint selected_id;
    gtk_tree_model_get(model, &iter, ALBUM_NAME, &info, NUMBER, &selected_id, -1);
    g_free(info);

    set_freedb_selected_id_safe(selected_id, ui);

    gtk_widget_set_sensitive(ui->gui->freedb_add_button, TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(ui->gui->freedb_add_button, FALSE);
  }
}

//!removes all rows from the freedb table
static void remove_all_freedb_rows(ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->freedb_tree);
  while (ui->infos->freedb_table_number > 0)
  {
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
    ui->infos->freedb_table_number--;
  }
}

static gboolean freedb_search_start(ui_state *ui)
{
  gui_state *gui = ui->gui;

  gtk_widget_hide(gui->freedb_search_button);
  gtk_widget_show(gui->freedb_spinner);
  gtk_spinner_start(GTK_SPINNER(gui->freedb_spinner));

  gtk_widget_set_sensitive(gui->freedb_add_button, FALSE);
  gtk_widget_set_sensitive(gui->freedb_entry, FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->freedb_tree), FALSE);

  put_status_message(_("please wait... contacting tracktype.org"), ui);

  return FALSE;
}

static gboolean freedb_search_end(ui_with_err *ui_err)
{
  gui_state *gui = ui_err->ui->gui;
  ui_infos *infos = ui_err->ui->infos;

  remove_all_freedb_rows(ui_err->ui);

  if (ui_err->err >= 0 && infos->freedb_search_results)
  {
    gint i = 0;
    for (i = 0; i < infos->freedb_search_results->number;i++)
    {
      gint must_be_free = SPLT_FALSE;
      infos->freedb_search_results->results[i].name =
        transform_to_utf8(infos->freedb_search_results->results[i].name, TRUE, &must_be_free);
      add_freedb_row(infos->freedb_search_results->results[i].name,
          infos->freedb_search_results->results[i].id,
          infos->freedb_search_results->results[i].revisions,
          infos->freedb_search_results->results[i].revision_number, ui_err->ui);
    }

    if (infos->freedb_search_results->number > 0)
    {
      GtkTreeSelection *selection = gtk_tree_view_get_selection(gui->freedb_tree);
      GtkTreeModel *model = gtk_tree_view_get_model(gui->freedb_tree);
      GtkTreePath *path = gtk_tree_path_new_from_indices (0 ,-1);

      GtkTreeIter iter;
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_selection_select_iter(selection, &iter);
      gtk_tree_path_free(path);
    }
  }

  gtk_widget_show(gui->freedb_search_button);
  gtk_spinner_stop(GTK_SPINNER(gui->freedb_spinner));
  gtk_widget_hide(gui->freedb_spinner);

  gtk_widget_set_sensitive(gui->freedb_entry, TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->freedb_tree), TRUE);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

//!search the freedb.org
static gpointer freedb_search(ui_state *ui)
{
  set_process_in_progress_and_wait_safe(TRUE, ui);

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)freedb_search_start, ui, NULL);

  gint err = SPLT_OK;

  enter_threads();
  const gchar *freedb_search_value = gtk_entry_get_text(GTK_ENTRY(ui->gui->freedb_entry));
  exit_threads();

  //freedb_search_results is only used in the idle of the end of the thread, so no mutex needed
  ui->infos->freedb_search_results = 
    mp3splt_get_freedb_search(ui->mp3splt_state, freedb_search_value, &err,
        SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI, "\0", -1);

  print_status_bar_confirmation_in_idle(err, ui);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = err;
  ui_err->ui = ui;
  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)freedb_search_end, ui_err, NULL);

  return NULL;
}

//! Start a thread for the freedb search
static void freedb_search_start_thread(ui_state *ui)
{
  create_thread((GThreadFunc)freedb_search, ui);
}

//!we push the search button
static void freedb_search_button_event(GtkWidget *widget, ui_state *ui)
{
  freedb_search_start_thread(ui);
}

/*!search entry backspace event

when we push Enter for the search entry
*/
static void freedb_entry_activate_event(GtkEntry *entry, ui_state *ui)
{
  freedb_search_start_thread(ui);
}

//!returns the seconds, minutes, and hudreths
static void get_secs_mins_hundr(gfloat time, gint *mins,gint *secs, gint *hundr)
{
  *mins = (gint)(time / 6000);
  *secs = (gint)(time - (*mins * 6000)) / 100;
  *hundr = (gint)(time - (*mins * 6000) - (*secs * 100));
}

/*!updates the current splitpoints in ui->mp3splt_state

Takes the splitpoints from the table displayed in the gui

max_splits is the maximum number of splitpoints to update
*/
void update_splitpoints_from_mp3splt_state(ui_state *ui)
{
  gint max_splits = 0;

  gint err = SPLT_OK;
  const splt_point *points = mp3splt_get_splitpoints(ui->mp3splt_state, &max_splits, &err);
  print_status_bar_confirmation(err, ui);

  if (max_splits <= 0)
  {
    return;
  }

  remove_all_rows(ui->gui->remove_all_button, ui);
  gint i;
  for (i = 0; i < max_splits;i++)
  {
    //ugly hack because we use maximum ints in the GUI
    //-GUI must be changed to accept long values
    long old_point_value = points[i].value;
    int point_value = (int) old_point_value;
    if (old_point_value > INT_MAX)
    {
      point_value = INT_MAX;
    }

    get_secs_mins_hundr(point_value, 
        &ui->status->spin_mins, &ui->status->spin_secs, &ui->status->spin_hundr_secs);

    gint must_be_free = FALSE;
    gchar *result_utf8 = points[i].name;
    if (result_utf8 != NULL)
    {
      result_utf8 = transform_to_utf8(result_utf8, FALSE, &must_be_free);
      g_snprintf(ui->status->current_description, 255, "%s", result_utf8);
    }
    else
    {
      g_snprintf(ui->status->current_description, 255, "%s", _("description here"));
    }

    if (must_be_free)
    {
      g_free(result_utf8);
      result_utf8 = NULL;
    }

    if (points[i].type == SPLT_SPLITPOINT)
    {
      add_row(TRUE, ui);
    }
    else if (points[i].type == SPLT_SKIPPOINT)
    {
      add_row(FALSE, ui);
    }
  }

  g_snprintf(ui->status->current_description, 255, "%s", _("description here"));

  update_minutes_from_spinner(ui->gui->spinner_minutes, ui);
  update_seconds_from_spinner(ui->gui->spinner_seconds, ui);
  update_hundr_secs_from_spinner(ui->gui->spinner_hundr_secs, ui);
}

static gboolean put_freedb_splitpoints_start(ui_state *ui)
{
  gtk_widget_set_sensitive(ui->gui->freedb_add_button, FALSE);  
  gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->freedb_tree), FALSE);

  put_status_message(_("please wait... contacting tracktype.org"), ui);

  return FALSE;
}

static gboolean put_freedb_splitpoints_end(ui_state *ui)
{
  update_splitpoints_from_mp3splt_state(ui);

  gtk_widget_set_sensitive(ui->gui->freedb_add_button, TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->freedb_tree), TRUE);

  set_process_in_progress_and_wait_safe(FALSE, ui);

  return FALSE;
}

static gpointer put_freedb_splitpoints(ui_state *ui)
{
  set_process_in_progress_and_wait_safe(TRUE, ui);

  gint selected_id = get_freedb_selected_id_safe(ui);

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)put_freedb_splitpoints_start, ui, NULL);

  gchar mp3splt_dir[14] = ".mp3splt-gtk";
  const gchar *home_dir = g_get_home_dir();
  gint malloc_number = strlen(mp3splt_dir) + strlen(home_dir) + 20;
  gchar *filename = malloc(malloc_number * sizeof(gchar));
  g_snprintf(filename, malloc_number, "%s%s%s%s%s", home_dir,
      G_DIR_SEPARATOR_S, mp3splt_dir,
      G_DIR_SEPARATOR_S, "query.cddb");

  gint err = SPLT_OK;

  mp3splt_write_freedb_file_result(ui->mp3splt_state, selected_id,
      filename, &err, SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI, "\0",-1);
  print_status_bar_confirmation_in_idle(err, ui);

  enter_threads();
  if (get_checked_output_radio_box(ui))
  {
    exit_threads(); 
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  }
  else
  {
    const char *data = gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry));
    exit_threads(); 

    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);

    gint error = SPLT_OUTPUT_FORMAT_OK;
    mp3splt_set_oformat(ui->mp3splt_state, data, &error);
    print_status_bar_confirmation_in_idle(error, ui);
  }

  err = SPLT_OK;
  mp3splt_put_cddb_splitpoints_from_file(ui->mp3splt_state, filename, &err);
  print_status_bar_confirmation_in_idle(err, ui);

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)put_freedb_splitpoints_end, ui, NULL);

  return NULL;
}

//!event for the freedb add button when clicked
static void freedb_add_button_clicked_event(GtkButton *button, ui_state *ui)
{
  create_thread((GThreadFunc)put_freedb_splitpoints, ui);
}

//!creates the freedb box
GtkWidget *create_freedb_frame(ui_state *ui)
{
  GtkWidget *freedb_hbox = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(freedb_hbox), 0);
 
  GtkWidget *freedb_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(freedb_hbox), freedb_vbox, TRUE, TRUE, 4);
  
  /* search box */
  GtkWidget *search_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(freedb_vbox), search_hbox, FALSE, FALSE, 2);

  GtkWidget *label = gtk_label_new(_("Search tracktype.org:"));
  gtk_box_pack_start(GTK_BOX(search_hbox), label, FALSE, FALSE, 0);

  GtkWidget *freedb_entry = gtk_entry_new();
  ui->gui->freedb_entry = freedb_entry;
  gtk_editable_set_editable(GTK_EDITABLE(freedb_entry), TRUE);
  gtk_box_pack_start(GTK_BOX(search_hbox), freedb_entry, TRUE, TRUE, 6);
  g_signal_connect(G_OBJECT(freedb_entry), "activate",
      G_CALLBACK(freedb_entry_activate_event), ui);

  GtkWidget *freedb_search_button = wh_create_cool_button(GTK_STOCK_FIND, _("_Search"),FALSE);
  ui->gui->freedb_search_button = freedb_search_button;
  g_signal_connect(G_OBJECT(freedb_search_button), "clicked",
      G_CALLBACK(freedb_search_button_event), ui);
  gtk_box_pack_start(GTK_BOX(search_hbox), freedb_search_button, FALSE, FALSE, 0);
 
  GtkWidget *freedb_spinner = gtk_spinner_new();
  ui->gui->freedb_spinner = freedb_spinner;
  gtk_box_pack_start(GTK_BOX(search_hbox), freedb_spinner, FALSE, FALSE, 4);

  /* freedb scrolled window and the tree */
  GtkTreeView *freedb_tree = create_freedb_tree();
  ui->gui->freedb_tree = freedb_tree;

  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(freedb_vbox), scrolled_window, TRUE, TRUE, 1);

  create_freedb_columns(freedb_tree);

  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(freedb_tree));
  
  GtkTreeSelection *freedb_tree_selection = gtk_tree_view_get_selection(freedb_tree);
  g_signal_connect(G_OBJECT(freedb_tree_selection), "changed",
                    G_CALLBACK(freedb_selection_changed), ui);

  /* add button */
  GtkWidget *freedb_add_button = wh_create_cool_button(GTK_STOCK_ADD,_("_Add splitpoints"), FALSE);
  ui->gui->freedb_add_button = freedb_add_button;

  gtk_widget_set_sensitive(freedb_add_button, FALSE);
  g_signal_connect(G_OBJECT(freedb_add_button), "clicked",
      G_CALLBACK(freedb_add_button_clicked_event), ui);
  gtk_widget_set_tooltip_text(freedb_add_button, _("Set splitpoints to the splitpoints table"));
 
  return freedb_hbox;
}

void hide_freedb_spinner(gui_state *gui)
{
  gtk_widget_hide(gui->freedb_spinner);
}


