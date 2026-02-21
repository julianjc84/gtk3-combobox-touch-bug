#include <gtk/gtk.h>

/*
 * GTK3 ComboBox Touch Bug Reproducer
 *
 * BUG: GtkComboBox popups dismiss immediately on touchscreen tap
 * when placed inside a GtkScrolledWindow whose content fits without
 * scrolling. When the window is resized small enough to require
 * scrolling, the combobox popup works correctly with touch.
 *
 * This demo has three tabs:
 *   Tab 1: GtkComboBox + GtkListStore + GtkCellRendererText
 *   Tab 2: GtkComboBoxText
 *   Tab 3: GtkScrolledWindow with POLICY_ALWAYS (workaround)
 *
 * To reproduce:
 *   1. Run this app on a touchscreen
 *   2. With the window large (no scrollbar visible), tap a combobox
 *      → popup appears and immediately dismisses
 *   3. Resize the window smaller until a scrollbar appears
 *   4. Tap the same combobox
 *      → popup stays open and works correctly
 *
 * Tested with GTK 3.24.41 on Linux Mint / Ubuntu
 */

static GtkWidget *make_model_combo(void)
{
    GtkListStore *store;
    GtkWidget *combo;
    GtkCellRenderer *renderer;
    GtkTreeIter iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Option A", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Option B", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Option C", -1);
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Option D", -1);

    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combo), renderer, "text", 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    return combo;
}

static GtkWidget *make_text_combo(void)
{
    GtkWidget *combo;

    combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Option A");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Option B");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Option C");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Option D");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    return combo;
}

static GtkWidget *make_page(const char *description,
                            GtkWidget *(*make_combo_fn)(void),
                            gboolean force_scrolling)
{
    GtkWidget *scrolled, *viewport, *vbox;
    GtkWidget *label, *hbox;
    int i;

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER,
                                   force_scrolling ? GTK_POLICY_ALWAYS
                                                   : GTK_POLICY_AUTOMATIC);

    viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), viewport);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    gtk_container_add(GTK_CONTAINER(viewport), vbox);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), description);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);

    for (i = 0; i < 4; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Dropdown %d:", i + 1);

        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

        label = gtk_label_new(buf);
        gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

        gtk_box_pack_start(GTK_BOX(hbox), make_combo_fn(), FALSE, FALSE, 0);
    }

    /* Padding content to make the page taller */
    for (i = 0; i < 8; i++) {
        char buf[64];
        GtkWidget *check;
        snprintf(buf, sizeof(buf), "Checkbox %d (padding to increase page height)", i + 1);
        check = gtk_check_button_new_with_label(buf);
        gtk_box_pack_start(GTK_BOX(vbox), check, FALSE, FALSE, 0);
    }

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
        "\n<i>Resize the window until the scrollbar appears/disappears\n"
        "and test combobox touch behavior in each state.</i>");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);

    gtk_widget_show_all(scrolled);
    return scrolled;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window, *notebook;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),
        "GTK3 Touch Bug: Resize to toggle scrolling");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 600);

    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkComboBox + GtkListStore</b>\n\n"
            "Uses GtkComboBox with a GtkListStore model\n"
            "and GtkCellRendererText.\n\n"
            "Scroll policy: <b>AUTOMATIC</b>",
            make_model_combo, FALSE),
        gtk_label_new("ComboBox (auto scroll)"));

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkComboBoxText</b>\n\n"
            "Uses GtkComboBoxText with items\n"
            "added via gtk_combo_box_text_append_text().\n\n"
            "Scroll policy: <b>AUTOMATIC</b>",
            make_text_combo, FALSE),
        gtk_label_new("ComboBoxText (auto scroll)"));

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkComboBoxText + forced scrolling</b>\n\n"
            "Same as tab 2 but with scroll policy set to\n"
            "<b>GTK_POLICY_ALWAYS</b> — scrollbar is always visible.\n\n"
            "This should <b>always work</b> with touch regardless\n"
            "of window size.",
            make_text_combo, TRUE),
        gtk_label_new("Workaround (always scroll)"));

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.test.combobox.touch",
                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
