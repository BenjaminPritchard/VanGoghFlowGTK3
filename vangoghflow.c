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
// Almost everything in here is adapted from code originally from:
//  screen overlay: https://github.com/anko/hudkit
//  tray icon: https://github.com/3v1n0/indicators-examples-snaps/blob/master/gtk3-appindicator/simple-client.c
//  hot keys: https://github.com/anko/xkbcat/blob/master/xkbcat.c
//
// Writing programs like this is like standing on the shoulders of giants. I hope some people will find it cool and interesting.
//
// Benjamin Pritchard / Kundalini Software
// August 1 2021

#include "screen.h"
#include "tray.h"
#include "keyboard.h"
#include "vangoghflow.h"

#define VERSION "1.0"

// these default values can be overriden by command line arguments
float opacity = 0.25;
int visualizationIndex = 0;

const int numURLS = 3;
const gchar URLs[numURLS][12] = {"_hHwz1UWJmI", "9TbLJI7ja4s", "95FxKgcgjN0"}; // currently this is hardcoded!

/* This callback quits the program */
gint delete_event(GtkWidget *widget,
                  GdkEvent *event,
                  gpointer
                      data)
{
    shouldStop = true; // tell kb thread to quit
    gtk_main_quit();
    return FALSE;
}

void loadURL(WebKitWebView *web_view, int index)
{
    if (index < numURLS)
    {
        gchar YouTubeURL[80]; // we know 80 is bigger than the longest URL we will create

        snprintf(YouTubeURL, sizeof(YouTubeURL), "https://www.youtube.com/embed/%s?rel=0;&autoplay=1&mute=1", URLs[index]);
        webkit_web_view_load_uri(web_view, (const gchar *)YouTubeURL);
    }
    else
        fprintf(stderr, "invalid index passed to item_clicked_cb %d\n", index);
}

//Callback che crea lancia il thread
void startKeyBDThread()
{
    GThread *myThread = g_thread_new(NULL, (GThreadFunc)CheckForHotKeys, NULL);
}

// this routine is called by the thread created in startKeyBDThread()
// by using gdk_threads_add_idle_full()
// My understanding is that using gdk_threads_add_idle() is the way
// we are suppossed to pass data from the background thread back to our
// UI thread here
gboolean DoUpdate(void *data)
{
    char *key = (char *)data;
    if (strcmp(key, "F3") == 0)
    {
        if (opacity != 0.0)
            opacity -= 0.1;
    }
    else if (strcmp(key, "F4") == 0)
    {
        if (opacity != 1.0)
            opacity += 0.1;
    }

    gtk_widget_set_opacity(GTK_WIDGET(window), opacity);

    // i think we have to return false each time, so that this routine won't be called more than one
    // per invocation of gdk_threads_add_idle()
    return false;
}

void parseCmdLine(int argc, char **argv)
{
    bool printHelp;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0)
            {
                printf(
                    "VanGoghFlow, written by Benjamin Pritchard.\n"
                    "\n"
                    "This is the Linux GTK3 version of VanGoghFlow, a program for overlaying your display with beautiful visualizations pulled from YouTube. \n"
                    "In addition to being aesthetically pleasing, this software is useful for gently inducing psychological flow states.\n"
                    "It also can help stimulate insight & creativity.\n"
                    "\n"
                    "Usage: vangoghflow [OPTIONS]\n"
                    "   -h, --help                  Displays this information.\n"
                    "   -o, --opacity <0.0-1.0>     Sets initial opacity.\n"
                    "   -i, --index <0-3>           Sets initial visualization\n"
                    "   -v, --version               Displays version information\n"
                    "\n"
                    "Source code at: https://github.com/BenjaminPritchard/VanGoghFlowGTK3\n"
                    "\n");
                exit(0);
            }

            else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--opacity") == 0)
            {
                if (i + 1 < argc)
                {
                    opacity = atof(argv[i + 1]);
                    if (opacity < 0 || opacity > 1)
                    {
                        fprintf(stderr, "Error: o must be between 0 and 1.0\n");
                        exit(1);
                    }
                }
                else
                {
                    fprintf(stderr, "Error: -o needs a value\n");
                    exit(1);
                }
            }
            else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--index") == 0)
            {
                if (i + 1 < argc)
                {
                    visualizationIndex = atof(argv[i + 1]);
                    if (visualizationIndex < 0 || visualizationIndex > numURLS - 1)
                    {
                        fprintf(stderr, "Error: i must be between 0 and %d.\n", numURLS - 1);
                        exit(1);
                    }
                }
                else
                {
                    fprintf(stderr, "Error: -i needs a value\n");
                    exit(1);
                }
            }
            else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--v") == 0)
            {
                printf("vangoghflow version %s\n", VERSION);
                exit(0);
            }
            else
            {
                fprintf(stderr, "Error: unknown option %s\n", argv[i]);
                exit(1);
            }
        }
    }
}

int main(int argc, char **argv)
{
    parseCmdLine(argc, argv);
    gtk_init(&argc, &argv);

    // create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH_WEST);
    gtk_window_move(GTK_WINDOW(window), 0, 0);
    gtk_window_set_title(GTK_WINDOW(window), "VanGoghFlow");
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
    gtk_widget_set_opacity(GTK_WIDGET(window), opacity);

    MakeTrayIcon(&visualizationIndex);

    // setup background thread to listen for global keyboard events...
    if (initKB())
        startKeyBDThread();

    loadURL(web_view, visualizationIndex);
    gtk_main();
    return 0;
}
