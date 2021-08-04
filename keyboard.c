#include "keyboard.h"
#include "vangoghflow.h"

#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

const char *DEFAULT_DISPLAY = ":0";
const bool DEFAULT_PRINT_UP = false;

int xiOpcode;
char xDisplayName[2];
bool printKeyUps = DEFAULT_PRINT_UP;
Display *disp;

bool initKB()
{

       // Connect to X display
    // this is just harded for now...
    disp = XOpenDisplay(":0");
    if (NULL == disp)
    {
        fprintf(stderr, "Cannot open X display: %s\n", xDisplayName);
        return false;
    }

    { // Test for XInput 2 extension
        int queryEvent, queryError;
        if (!XQueryExtension(disp, "XInputExtension", &xiOpcode,
                             &queryEvent, &queryError))
        {
            fprintf(stderr, "X Input extension not available\n");
            exit(2);
        }
    }
    { // Request XInput 2.0, to guard against changes in future versions
        int major = 2, minor = 0;
        int queryResult = XIQueryVersion(disp, &major, &minor);
        if (queryResult == BadRequest)
        {
            fprintf(stderr, "Need XI 2.0 support (got %d.%d)\n", major, minor);
            exit(3);
        }
        else if (queryResult != Success)
        {
            fprintf(stderr, "XIQueryVersion failed!\n");
            exit(4);
        }
    }
    { // Register to receive XInput events
        Window root = DefaultRootWindow(disp);
        XIEventMask m;
        m.deviceid = XIAllMasterDevices;
        m.mask_len = XIMaskLen(XI_LASTEVENT);
        m.mask = calloc(m.mask_len, sizeof(char));
        XISetMask(m.mask, XI_RawKeyPress);
        if (printKeyUps)
            XISetMask(m.mask, XI_RawKeyRelease);
        XISelectEvents(disp, root, &m, 1 /*number of masks*/);
        XSync(disp, false);
        free(m.mask);
    }
    return true;
}

void *CheckForHotKeys()
{
    while (!shouldStop)
    {
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie *)&event.xcookie;

        XNextEvent(disp, &event);

        if (XGetEventData(disp, cookie) &&
            cookie->type == GenericEvent &&
            cookie->extension == xiOpcode)
        {
            switch (cookie->evtype)
            {
            case XI_RawKeyRelease:
            case XI_RawKeyPress:
            {
                XIRawEvent *ev = cookie->data;

                // Ask X what it calls that key; skip if it doesn't know
                KeySym s = XkbKeycodeToKeysym(disp, ev->detail,
                                              0 /*group*/, 0 /*shift level*/);
                if (NoSymbol == s)
                    continue;
                char *str = XKeysymToString(s);
                if (NULL == str)
                    continue;

                DoUpdate(str);
                break;
            }
            }
        }
    }
    return NULL;
}