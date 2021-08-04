#pragma once

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkmonitor.h>
#include <webkit2/webkit2.h>

#define MIN_FONT_SIZE 4

GtkWidget *window;
WebKitWebView *web_view;

void screen_changed(GtkWidget *widget, GdkScreen *old_screen,
                    gpointer user_data);
int get_monitor_rects(GdkDisplay *display, GdkRectangle **rectangles);
void size_to_screen(GtkWindow *window);
void make_entire_window_clickthrough();
