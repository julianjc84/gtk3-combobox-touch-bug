#include <gtk/gtk.h>

/*
 * GTK4 DropDown Touch Test
 *
 * GTK4 equivalent of the GTK3 ComboBox touch bug reproducer.
 * GTK4 replaces GtkComboBox/GtkComboBoxText with GtkDropDown.
 *
 * Tests whether the same bug exists in GTK4:
 *   GtkDropDown inside GtkScrolledWindow dismissing on touch
 *   when content fits without scrolling.
 *
 * Three tabs:
 *   Tab 1: GtkDropDown + GtkStringList (auto scroll)
 *   Tab 2: GtkDropDown + GtkStringList (auto scroll, duplicate)
 *   Tab 3: GtkScrolledWindow with POLICY_ALWAYS (workaround)
 */

static GtkWidget *make_dropdown(void)
{
    const char *items[] = { "Option A", "Option B", "Option C", "Option D", NULL };
    GtkStringList *model = gtk_string_list_new(items);
    GtkWidget *dropdown = gtk_drop_down_new(G_LIST_MODEL(model), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), 0);
    return dropdown;
}

static GtkWidget *make_page(const char *description,
                            gboolean force_scrolling)
{
    GtkWidget *scrolled, *vbox;
    GtkWidget *label, *hbox;
    int i;

    scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER,
                                   force_scrolling ? GTK_POLICY_ALWAYS
                                                   : GTK_POLICY_AUTOMATIC);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), vbox);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), description);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(vbox), label);

    for (i = 0; i < 4; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Dropdown %d:", i + 1);

        hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_append(GTK_BOX(vbox), hbox);

        label = gtk_label_new(buf);
        gtk_box_append(GTK_BOX(hbox), label);

        gtk_box_append(GTK_BOX(hbox), make_dropdown());
    }

    /* Padding content to make the page taller */
    for (i = 0; i < 8; i++) {
        char buf[64];
        GtkWidget *check;
        snprintf(buf, sizeof(buf), "Checkbox %d (padding to increase page height)", i + 1);
        check = gtk_check_button_new_with_label(buf);
        gtk_box_append(GTK_BOX(vbox), check);
    }

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
        "\n<i>Resize the window until the scrollbar appears/disappears\n"
        "and test dropdown touch behavior in each state.</i>");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(vbox), label);

    return scrolled;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window, *notebook;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),
        "GTK4 Touch Test: Resize to toggle scrolling");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 600);

    notebook = gtk_notebook_new();
    gtk_window_set_child(GTK_WINDOW(window), notebook);

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkDropDown + GtkStringList</b>\n\n"
            "Uses GtkDropDown with a GtkStringList model.\n\n"
            "Scroll policy: <b>AUTOMATIC</b>",
            FALSE),
        gtk_label_new("DropDown (auto scroll)"));

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkDropDown + GtkStringList (duplicate)</b>\n\n"
            "Same as Tab 1 for comparison.\n\n"
            "Scroll policy: <b>AUTOMATIC</b>",
            FALSE),
        gtk_label_new("DropDown 2 (auto scroll)"));

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook),
        make_page(
            "<b>GtkDropDown + forced scrolling</b>\n\n"
            "Same as other tabs but with scroll policy set to\n"
            "<b>GTK_POLICY_ALWAYS</b> — scrollbar is always visible.\n\n"
            "If the GTK3 bug is fixed in GTK4, all tabs should work.\n"
            "If not, this tab should still work as a workaround.",
            TRUE),
        gtk_label_new("Workaround (always scroll)"));

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.test.dropdown.touch",
                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
