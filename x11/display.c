#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <memory.h>

#include "../common.h"

Display* display = NULL;
int screen = 0;
Window rootWindow;
Window window;

extern void init_display(STATE_T *state, int display_unused, int layer)
{
}

extern NativeWindowType init_window(STATE_T *state, int display_unused, int layer)
{
    log_verbose("XOpenDisplay");
    display = XOpenDisplay(NULL);
    assert(display != NULL);
    
    log_verbose("DefaultScreen");
    screen = DefaultScreen(display);
    log_verbose("RootWindow");
    rootWindow = RootWindow(display, screen);

    int width = 1024;
    int height = 600;

    state->screen_width = width;
    state->screen_height = height;

    log_verbose("XCreateSimpleWindow");
    window = XCreateSimpleWindow(
        display,
        rootWindow,
        0,
        0,
        width,
        height,
        0,
        BlackPixel(display, screen),
        BlackPixel(display, screen));

    assert(window);

    log_verbose("XStoreName");
    XStoreName(display, window, "cam_overlay");

    XCreateGC(display, window, 0, NULL);
    //XSetForeground(dpy, gc, WhitePixel(dpy, scr));

    //XSelectInput(dpy, win, ExposureMask | ButtonPressMask);

    log_verbose("XMapWindow");
    XMapWindow(display, window);

    XUndefineCursor(display, window);
    XMapRaised(display, window);

    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;

    XMapWindow(display, window);

    XSendEvent (display, DefaultRootWindow(display), False,
                    SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    long id = 25165866;
    XRaiseWindow(display, id);

    XFlush(display);

    return window;
}

extern void close_display(STATE_T *state)
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}