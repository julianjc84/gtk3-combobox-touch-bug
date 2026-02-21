/* Combo Boxes - Original GTK3 Demo
 *
 * Extracted from gtk3-demo (GTK 3.24.41) and made standalone.
 * This is the UNMODIFIED demo — "Items with icons" and "String IDs"
 * fail on touchscreen.
 *
 * Build: gcc $(pkg-config --cflags --libs gtk+-3.0) -o gtk3-demo-combobox-original gtk3-demo-combobox-original.c
 * Run:   ./gtk3-demo-combobox-original
 */

#include <glib/gi18n.h>
#include <gtk/gtk.h>

enum
{
  ICON_NAME_COL,
  TEXT_COL
};

static GtkTreeModel *
create_icon_store (void)
{
  const gchar *icon_names[6] = {
    "dialog-warning",
    "process-stop",
    "document-new",
    "edit-clear",
    NULL,
    "document-open"
  };
  const gchar *labels[6] = {
    N_("Warning"),
    N_("Stop"),
    N_("New"),
    N_("Clear"),
    NULL,
    N_("Open")
  };

  GtkTreeIter iter;
  GtkListStore *store;
  gint i;

  store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

  for (i = 0; i < G_N_ELEMENTS (icon_names); i++)
    {
      if (icon_names[i])
        {
          gtk_list_store_append (store, &iter);
          gtk_list_store_set (store, &iter,
                              ICON_NAME_COL, icon_names[i],
                              TEXT_COL, _(labels[i]),
                              -1);
        }
      else
        {
          gtk_list_store_append (store, &iter);
          gtk_list_store_set (store, &iter,
                              ICON_NAME_COL, NULL,
                              TEXT_COL, "separator",
                              -1);
        }
    }

  return GTK_TREE_MODEL (store);
}

static void
set_sensitive (GtkCellLayout   *cell_layout,
               GtkCellRenderer *cell,
               GtkTreeModel    *tree_model,
               GtkTreeIter     *iter,
               gpointer         data)
{
  GtkTreePath *path;
  gint *indices;
  gboolean sensitive;

  path = gtk_tree_model_get_path (tree_model, iter);
  indices = gtk_tree_path_get_indices (path);
  sensitive = indices[0] != 1;
  gtk_tree_path_free (path);

  g_object_set (cell, "sensitive", sensitive, NULL);
}

static gboolean
is_separator (GtkTreeModel *model,
              GtkTreeIter  *iter,
              gpointer      data)
{
  GtkTreePath *path;
  gboolean result;

  path = gtk_tree_model_get_path (model, iter);
  result = gtk_tree_path_get_indices (path)[0] == 4;
  gtk_tree_path_free (path);

  return result;
}

static GtkTreeModel *
create_capital_store (void)
{
  struct {
    gchar *group;
    gchar *capital;
  } capitals[] = {
    { "A - B", NULL },
    { NULL, "Albany" },
    { NULL, "Annapolis" },
    { NULL, "Atlanta" },
    { NULL, "Augusta" },
    { NULL, "Austin" },
    { NULL, "Baton Rouge" },
    { NULL, "Bismarck" },
    { NULL, "Boise" },
    { NULL, "Boston" },
    { "C - D", NULL },
    { NULL, "Carson City" },
    { NULL, "Charleston" },
    { NULL, "Cheyenne" },
    { NULL, "Columbia" },
    { NULL, "Columbus" },
    { NULL, "Concord" },
    { NULL, "Denver" },
    { NULL, "Des Moines" },
    { NULL, "Dover" },
    { "E - J", NULL },
    { NULL, "Frankfort" },
    { NULL, "Harrisburg" },
    { NULL, "Hartford" },
    { NULL, "Helena" },
    { NULL, "Honolulu" },
    { NULL, "Indianapolis" },
    { NULL, "Jackson" },
    { NULL, "Jefferson City" },
    { NULL, "Juneau" },
    { "K - O", NULL },
    { NULL, "Lansing" },
    { NULL, "Lincoln" },
    { NULL, "Little Rock" },
    { NULL, "Madison" },
    { NULL, "Montgomery" },
    { NULL, "Montpelier" },
    { NULL, "Nashville" },
    { NULL, "Oklahoma City" },
    { NULL, "Olympia" },
    { "P - S", NULL },
    { NULL, "Phoenix" },
    { NULL, "Pierre" },
    { NULL, "Providence" },
    { NULL, "Raleigh" },
    { NULL, "Richmond" },
    { NULL, "Sacramento" },
    { NULL, "Salem" },
    { NULL, "Salt Lake City" },
    { NULL, "Santa Fe" },
    { NULL, "Springfield" },
    { NULL, "St. Paul" },
    { "T - Z", NULL },
    { NULL, "Tallahassee" },
    { NULL, "Topeka" },
    { NULL, "Trenton" },
    { NULL, NULL }
  };

  GtkTreeIter iter, iter2;
  GtkTreeStore *store;
  gint i;

  store = gtk_tree_store_new (1, G_TYPE_STRING);

  for (i = 0; capitals[i].group || capitals[i].capital; i++)
    {
      if (capitals[i].group)
        {
          gtk_tree_store_append (store, &iter, NULL);
          gtk_tree_store_set (store, &iter, 0, capitals[i].group, -1);
        }
      else if (capitals[i].capital)
        {
          gtk_tree_store_append (store, &iter2, &iter);
          gtk_tree_store_set (store, &iter2, 0, capitals[i].capital, -1);
        }
    }

  return GTK_TREE_MODEL (store);
}

static void
is_capital_sensitive (GtkCellLayout   *cell_layout,
                      GtkCellRenderer *cell,
                      GtkTreeModel    *tree_model,
                      GtkTreeIter     *iter,
                      gpointer         data)
{
  gboolean sensitive;

  sensitive = !gtk_tree_model_iter_has_child (tree_model, iter);

  g_object_set (cell, "sensitive", sensitive, NULL);
}

static void
fill_combo_entry (GtkWidget *combo)
{
  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo), "One");
  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo), "Two");
  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo), "2\302\275");
  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo), "Three");
}

static void
activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *vbox, *frame, *box, *combo, *entry;
  GtkTreeModel *model;
  GtkCellRenderer *renderer;
  GtkTreePath *path;
  GtkTreeIter iter;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Combo Boxes (original demo)");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  /* A combobox demonstrating cell renderers, separators and
   *  insensitive rows
   */
  frame = gtk_frame_new ("Items with icons");
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);
  gtk_container_add (GTK_CONTAINER (frame), box);

  model = create_icon_store ();
  combo = gtk_combo_box_new_with_model (model);
  g_object_unref (model);
  gtk_container_add (GTK_CONTAINER (box), combo);

  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "icon-name", ICON_NAME_COL,
                                  NULL);

  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
                                      renderer,
                                      set_sensitive,
                                      NULL, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "text", TEXT_COL,
                                  NULL);

  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
                                      renderer,
                                      set_sensitive,
                                      NULL, NULL);

  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (combo),
                                        is_separator, NULL, NULL);

  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

  /* A combobox demonstrating trees. */
  frame = gtk_frame_new ("Where are we ?");
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);
  gtk_container_add (GTK_CONTAINER (frame), box);

  model = create_capital_store ();
  combo = gtk_combo_box_new_with_model (model);
  g_object_unref (model);
  gtk_container_add (GTK_CONTAINER (box), combo);

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "text", 0,
                                  NULL);
  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
                                      renderer,
                                      is_capital_sensitive,
                                      NULL, NULL);

  path = gtk_tree_path_new_from_indices (0, 8, -1);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_path_free (path);
  gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);

  /* A GtkComboBoxEntry with validation */
  frame = gtk_frame_new ("Editable");
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);
  gtk_container_add (GTK_CONTAINER (frame), box);

  combo = gtk_combo_box_text_new_with_entry ();
  fill_combo_entry (combo);
  gtk_container_add (GTK_CONTAINER (box), combo);

  /* A combobox with string IDs */
  frame = gtk_frame_new ("String IDs");
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 5);
  gtk_container_add (GTK_CONTAINER (frame), box);

  combo = gtk_combo_box_text_new ();
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combo), "never", "Not visible");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combo), "when-active", "Visible when active");
  gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (combo), "always", "Always visible");
  gtk_container_add (GTK_CONTAINER (box), combo);

  entry = gtk_entry_new ();
  g_object_bind_property (combo, "active-id",
                          entry, "text",
                          G_BINDING_BIDIRECTIONAL);
  gtk_container_add (GTK_CONTAINER (box), entry);

  gtk_widget_show_all (window);
}

int
main (int argc, char *argv[])
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.test.combobox.demo.original",
                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return status;
}
