
// all the code in here is taken (and maybe slightly adapted) from: https://github.com/anko/hudkit

#include "screen.h"

gulong monitors_changed_handler_id = 0;

// This callback runs when the window is first set to appear on some screen, or
// when it's moved to appear on another.
void screen_changed(GtkWidget *widget, GdkScreen *old_screen,
                    gpointer user_data)
{
    GdkScreen *screen = gtk_widget_get_screen(widget);

    WebKitWebView *web_view = (WebKitWebView *)user_data;

    // Die unless the screen supports compositing (alpha blending)
    if (!gdk_screen_is_composited(screen))
    {
        fprintf(stderr, "Your screen does not support transparency.\n");
        fprintf(stderr, "Maybe your compositor isn't running?\n");
        exit(69); // memes
    }

    // Ensure the widget can take RGBA
    gtk_widget_set_visual(widget, gdk_screen_get_rgba_visual(screen));

    // Switch monitors-changed subscription from the old screen (if applicable)
    // to the new one
    if (old_screen)
        g_signal_handler_disconnect(old_screen, monitors_changed_handler_id);

    size_to_screen(GTK_WINDOW(widget));
}

int get_monitor_rects(GdkDisplay *display, GdkRectangle **rectangles)
{
    int n = gdk_display_get_n_monitors(display);
    GdkRectangle *new_rectangles = (GdkRectangle *)malloc(n * sizeof(GdkRectangle));
    for (int i = 0; i < n; ++i)
    {
        GdkMonitor *monitor = gdk_display_get_monitor(display, i);
        gdk_monitor_get_geometry(monitor, &new_rectangles[i]);
    }
    *rectangles = new_rectangles;
    // Ownership of the malloc'd memory transfers out
    return n;
}

// tell GTK that the entire window should be a clickthough region
void make_entire_window_clickthrough()
{
    cairo_rectangle_int_t rect = {0, 0, 0, 0};

    cairo_region_t *shape =
        cairo_region_create_rectangle(&rect);

    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    if (gdk_window)
        gdk_window_input_shape_combine_region(gdk_window, shape, 0, 0);

    cairo_region_destroy(shape);
}

void size_to_screen(GtkWindow *window)
{
    GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(window));

    // Get total screen size.  This involves finding all physical monitors
    // connected, and examining their positions and sizes.  This is as complex
    // as it is because monitors can be configured to have relative
    // positioning, causing overlapping areas and a non-rectangular total
    // desktop area.
    //
    // We want our window to cover the minimum axis-aligned bounding box of
    // that total desktop area.  This means it's too large (even large bits of
    // it may be outside the accessible desktop) but it's easier to manage than
    // multiple windows.

    GdkDisplay *display = gdk_display_get_default();
    GdkRectangle *rectangles = NULL;
    int nRectangles = get_monitor_rects(display, &rectangles);

    // I can't think of a reason why someone's monitor setup might have a
    // monitor positioned origin at negative x, y coordinates, but just in case
    // someone does, we'll cover for it.
    int x = 0, y = 0, width = 0, height = 0;
    for (int i = 0; i < nRectangles; ++i)
    {
        GdkRectangle rect = rectangles[i];
        int left = rect.x;
        int top = rect.y;
        int right = rect.x + rect.width;
        int bottom = rect.y + rect.height;
        if (left < x)
            x = left;
        if (top < y)
            y = top;
        if (width < right)
            width = right;
        if (height < bottom)
            height = bottom;
    }
    free(rectangles);

    gtk_window_move(GTK_WINDOW(window), x, y);
    gtk_window_set_default_size(window, width, height);
    gtk_window_resize(window, width, height);
    gtk_window_set_resizable(window, false);
}