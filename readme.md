# ABOUT

GTK3 version of VanGoghFlow, a program for overlaying your display with beautiful visualizations pulled from YouTube. In addition to being aesthetically pleasing, it is useful for gently inducing psychological flow states.

For more information on that, please see here: https://www.kundalinisoftware.com/van-gogh-flow/

# USAGE

Just run the executable. You can use your F3 / F4 keys like "volume keys" to set the opacity of the visualizations.

To change visualizations, use the tray icon.

# BUILDING

Just run make, which builds using clang.

You can see the dependancies needed by looking at the makefile:

    clang -g -O0 -std=c11 -Wdeprecated vangoghflow.c tray.c screen.c keyboard.c -o vangoghflow -lappindicator3 -I/usr/include/libdbusmenu-glib-0.4/libdbusmenu-glib/ -I/usr/include/libappindicator-0.1/libappindicator `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -lX11 -lXi

# TODO

1.  Use local .HTML instead of just setting the YouTube URL directly. This will allow the use of the YouTube API to know when the videos are done playing, so that we can automatically advance to the next one.

    Additionally, using local .HTML opens up the possibility to do realtime visualizations using Javascript, instead of just using prerendered ones from YouTube.

2.  Allow user-configurable YouTube video configuration. Currently this is hardcoded.

3.  It would be nice to package this up as a .deb file.

# CAVEAT

I am not sure what all systems this builds / works under. But for sure you need a [compositing window manager](https://en.wikipedia.org/wiki/Compositing_window_manager) for this to work, and I think a GKT desktop. I use Linux Mint (which comes with a compositing window manager called Muffin) and everything works fine.

So maybe it is possible to get this working on different distros. But I have not tried. If you do something like that, please submit a pull request so others can benefit.

# GITHUB

Source code available on GitHub here: https://github.com/BenjaminPritchard/VanGoghFlowGTK3. If you have any ideas or improvements, please feel free to fork and submit a pull request.

# CREDITS

Original author: Benjamin Pritchard, August 2021.
