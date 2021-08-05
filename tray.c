// This code is from: https://github.com/3v1n0/indicators-examples-snaps/blob/master/gtk3-appindicator/simple-client.c
// The original license is as follows:

/*
A small piece of sample code demonstrating a very simple application
with an indicator.
Copyright 2009 Canonical Ltd.
Authors:
    Ted Gould <ted@canonical.com>
This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License version 3, as published 
by the Free Software Foundation.
This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along 
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma clang diagnostic ignored "-Wdeprecated"

#include "app-indicator.h"
#include "server.h"
#include "menuitem.h"

#include "screen.h"
#include "tray.h"
#include "keyboard.h"
#include "vangoghflow.h"

#define LOCAL_ICON "simple-client-test-icon.png"

static GtkWidget *item1;
static GtkWidget *item2;
static GtkWidget *item3;

void dialog(gchar *message)
{
    GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void
about_menu_clicked(GtkWidget *widget, gpointer data)
{
    // maybe there is a way to make a better dialog in the future!
    dialog("VanGoghFlow GTK3, written by Benjamin Pritchard.\n\n Use F3 and F4 to set opacity.\n\nhttps://www.kundalinisoftware.com/van-gogh-flow/");
}

static void
quit_menu_clicked(GtkWidget *widget, gpointer data)
{
    shouldStop = true; // tell kb thread to quit
    gtk_main_quit();
}

static void
item_clicked_cb(GtkWidget *widget, gpointer data)
{
    static bool noRecursion;

    // trying to set the menu items to checked/unchecked using gtk_check_menu_item_set_active()
    // causes this function to be called again!!
    if (noRecursion)
        return;

    noRecursion = true;
    const char *tmp = (const char *)data;
    int index = atoi(tmp);

    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item1), false);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item2), false);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item3), false);

    switch (index)
    {
    case 0:
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item1), true);
        break;
    case 1:
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item2), true);
        break;
    case 2:
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item3), true);
        break;
    }

    //loadURL() does range checking
    loadURL(web_view, index);

    noRecursion = false;
}

void MakeTrayIcon(int *visualizationIndex)
{
    GtkWidget *menu = NULL;
    AppIndicator *ci = NULL;

    ci = app_indicator_new("example-simple-client",
                           "indicator-messages",
                           APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    g_assert(IS_APP_INDICATOR(ci));
    g_assert(G_IS_OBJECT(ci));

    app_indicator_set_status(ci, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon_full(ci, "indicator-messages-new", "System Messages Icon Highlighted");
    app_indicator_set_title(ci, "Van Gogh Flow");

    menu = gtk_menu_new();

    item1 = gtk_check_menu_item_new_with_label("Visualization 1");
    g_signal_connect(item1, "activate",
                     G_CALLBACK(item_clicked_cb), "0");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), (GtkWidget *)item1);
    gtk_widget_show(item1);

    item2 = gtk_check_menu_item_new_with_label("Visualization 2");
    g_signal_connect(item2, "activate",
                     G_CALLBACK(item_clicked_cb), "1");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), (GtkWidget *)item2);
    gtk_widget_show(item2);

    item3 = gtk_check_menu_item_new_with_label("Visualization 3");
    g_signal_connect(item3, "activate",
                     G_CALLBACK(item_clicked_cb), "2");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), (GtkWidget *)item3);
    gtk_widget_show(item3);

    GtkWidget *menuitem = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
    gtk_widget_show(menuitem);

    GtkWidget *aboutMenu = gtk_menu_item_new_with_label("About");
    g_signal_connect(aboutMenu, "activate",
                     G_CALLBACK(about_menu_clicked), "");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), (GtkWidget *)aboutMenu);
    gtk_widget_show(aboutMenu);

    GtkWidget *quitMenu = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quitMenu, "activate",
                     G_CALLBACK(quit_menu_clicked), "");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), (GtkWidget *)quitMenu);
    gtk_widget_show(quitMenu);

    // make sure to do this down here, after all the items are created
    // otherwise item_clicked_cb() will blow up
    // (i spent like an hour trying to figure that one out)

    char str[3];
    sprintf(str, "%d", *visualizationIndex);
    item_clicked_cb(NULL, (void *)str); // item clicked needs a pointer to a string version of the index

    app_indicator_set_menu(ci, GTK_MENU(menu));
}