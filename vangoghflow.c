// GTK3 version of VanGoghFlow, a program for overlaying your display with beautiful visualizations pulled from YouTube.
// In addition to being aesthetically pleasing, it is useful for modulating brain states between focused and diffuse modes.
// For more information on that, please see here:
//
// This is the first Linux program I have written. There are probably lots of issues using it, building it, running it, etc. on
// different Linux Distros. All I know is that I use Linux Mint Cinnamon, and I can build it (using clang) and run it.
// If you know how to make a more robust build system or how to package it better, put submit a pull request.
//
// This program uses the GTK3 version of webkit to render the youtube visualizations.
// And this whole thing is a linux port of the original Win64 version I wrote previously, which is available on GitHub
// here: XXX.
//
// The original version was written in C#, and used the Chromium embedded framework. As this version is written
// in straight C, I guess you could say this code is a half step lower than the original. (haha)
//
// This code (i.e. this Linux port) is available on github here: XXX
//
// Almost everything in here is adapted from code originally from
//  screen overlay: XXX
//  tray icon:
//  global hot keys:
//
// Writing programs like this is like standing on the shoulders of giants. I hope some people will find it cool and useful.
//
// Benjamin Pritchard / Kundalini Software
// August 1 2021

#include "screen.h"
#include "tray.h"

/* This callback quits the program */
gint delete_event(GtkWidget *widget,
                  GdkEvent *event,
                  gpointer
                      data)
{
    gtk_main_quit();
    return FALSE;
}

void loadURL(WebKitWebView *web_view, char *target_url)
{
    webkit_web_view_load_uri(web_view, target_url);
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    char *target_url = "https://www.youtube.com/embed/_hHwz1UWJmI?rel=0;&autoplay=1&mute=1";

    // create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH_WEST);
    gtk_window_move(GTK_WINDOW(window), 0, 0);
    gtk_window_set_title(GTK_WINDOW(window), "VanGoghFlow");
    gtk_widget_set_opacity(GTK_WIDGET(window), 0.5);
    gtk_widget_set_app_paintable(window, TRUE);

    /* Set a handler for delete_event that immediately exits GTK. */
    g_signal_connect(G_OBJECT(window), "delete_event",
                     G_CALLBACK(delete_event), NULL);

    // setup webview
    WebKitWebContext *wk_context = webkit_web_context_get_default();
    webkit_web_context_set_cache_model(wk_context,
                                       WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);

    web_view = WEBKIT_WEB_VIEW(
        webkit_web_view_new_with_context(wk_context));

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));

    //webkit_web_view_load_uri(web_view, target_url);
    loadURL(web_view, target_url);

    // Initialise the window and make it active.  We need this so it can resize
    // it correctly.
    screen_changed(window, NULL, web_view);

    GdkDisplay *display = gdk_display_get_default();
    GdkRectangle *rectangles = NULL;
    int nRectangles = get_monitor_rects(display, &rectangles);

    gtk_widget_show_all(window);

    // Hide the window, so we can get our properties ready without the window
    // manager trying to mess with us.
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    gdk_window_hide(GDK_WINDOW(gdk_window));

    // setup as many flags as possible to tell the window manager to be always on top and transparent
    gdk_window_set_override_redirect(GDK_WINDOW(gdk_window), true);
    gtk_window_set_keep_above(GTK_WINDOW(window), true);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), true);
    gtk_window_set_accept_focus(GTK_WINDOW(window), true);
    gtk_window_set_decorated(GTK_WINDOW(window), false);
    gtk_window_set_resizable(GTK_WINDOW(window), false);

    make_entire_window_clickthrough();

    gdk_window_show(GDK_WINDOW(gdk_window));
    gtk_widget_set_opacity(GTK_WIDGET(window), 0.25);

    MakeTrayIcon();
    gtk_main();

    return 0;
}
