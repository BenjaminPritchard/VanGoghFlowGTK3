// This code is from: XXX
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

#define LOCAL_ICON "simple-client-test-icon.png"

static void
item_clicked_cb(GtkWidget *widget, gpointer data)
{
    const char *tmp = (const char *)data;
    int index = atoi(tmp);

    // loadURL() does range checking
    loadURL(web_view, index);
}

void MakeTrayIcon()
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
    GtkWidget *item = gtk_menu_item_new_with_label("Visualization 1");
    g_signal_connect(item, "activate",
                     G_CALLBACK(item_clicked_cb), "0");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    item = gtk_menu_item_new_with_label("Visualization 2");
    g_signal_connect(item, "activate",
                     G_CALLBACK(item_clicked_cb), "1");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);

    item = gtk_menu_item_new_with_label("Visualization 3");
    g_signal_connect(item, "activate",
                     G_CALLBACK(item_clicked_cb), "2");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);

    app_indicator_set_menu(ci, GTK_MENU(menu));
}