#include <initializer_list>

#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>

Display* dpy;
Window root;
XRRScreenResources* sres;
bool done = false;

void full_screen(Window win)
{
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, win, &wa);

    auto cx = wa.x + wa.width / 2;
    auto cy = wa.y + wa.height / 2;

    for (auto i = 0; i < sres->ncrtc; ++i)
        if (auto ci = XRRGetCrtcInfo(dpy, sres, sres->crtcs[i]))
        {
            if (ci->noutput > 0)
            {
                if (cx >= ci->x && cx < ci->x + ci->width && cy >= ci->y && cy < ci->y + ci->height)
                {
                    XMoveResizeWindow(dpy, win, ci->x, ci->y, ci->width, ci->height);
                    XRRFreeCrtcInfo(ci);
                    break;
                }
            }
            XRRFreeCrtcInfo(ci);
        }
}

int main(int argc, char *argv[])
{
    if (!(dpy = XOpenDisplay(0x0))) return 1;
    root = DefaultRootWindow(dpy);
    sres = XRRGetScreenResources(dpy, root);

    auto cursor = XCreateFontCursor(dpy, XC_left_ptr);
    XDefineCursor(dpy, root, cursor);

    if (argc > 1 && fork() == 0)
    {
        execvp(argv[1], &argv[1]);
        return 1;
    }

    XGrabButton(dpy, Button1, Mod4Mask, root, True,
        ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync,
        None, None
    );
    XGrabButton(dpy, Button1, Mod4Mask|ShiftMask, root, True,
        ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync,
        None, None
    );

    const auto full = XKeysymToKeycode(dpy, XK_f);
    const auto next = XKeysymToKeycode(dpy, XK_Tab);
    const auto kill = XKeysymToKeycode(dpy, XK_k);
    const auto quit = XKeysymToKeycode(dpy, XK_q);

    for (auto code : { full, next, kill, quit })
    {
        XGrabKey(dpy, code, Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    }

    XButtonEvent move{.subwindow = None};
    XWindowAttributes attrs;

    XEvent ev;
    while (!done && !XNextEvent(dpy, &ev))
        switch (ev.type)
        {
        case KeyPress:
            if (ev.xkey.keycode == quit)
                done = true;

            else if (ev.xkey.keycode == next)
            {
                XCirculateSubwindowsUp(dpy, root);
                XSetInputFocus(dpy, ev.xkey.window, RevertToParent, CurrentTime);
            }
            else if (ev.xkey.subwindow)
            {
                if (ev.xkey.keycode == full)
                    full_screen(ev.xkey.subwindow);

                else if (ev.xkey.keycode == kill)
                    XKillClient(dpy, ev.xkey.subwindow);
            }
            break;

        case ButtonPress:
            if (ev.xbutton.subwindow)
            {
                XRaiseWindow(dpy, ev.xbutton.subwindow);
                XSetInputFocus(dpy, ev.xbutton.subwindow, RevertToParent, CurrentTime);

                move = ev.xbutton;
                XGetWindowAttributes(dpy, move.subwindow, &attrs);
            }
            break;

        case MotionNotify:
            if (ev.xmotion.subwindow)
            {
                auto dx = ev.xmotion.x_root - move.x_root;
                auto dy = ev.xmotion.y_root - move.y_root;

                if (move.state & ShiftMask)
                {
                    auto w = attrs.width + dx;
                    auto h = attrs.height + dy;
                    if (w > 0 && h > 0) XResizeWindow(dpy, move.subwindow, w, h);
                }
                else XMoveWindow(dpy, move.subwindow, attrs.x + dx, attrs.y + dy);
            }
            break;
        
        case ButtonRelease:
            move.subwindow = None;
            break;
        }

    XRRFreeScreenResources(sres);
    XCloseDisplay(dpy);
    return 0;
}
