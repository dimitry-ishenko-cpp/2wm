#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>

Display *dpy;
Window root;
XRRScreenResources *sres;

KeyCode grab_keycode(const char *key)
{
    KeyCode code = XKeysymToKeycode(dpy, XStringToKeysym(key));
    XGrabKey(dpy, code, Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    return code;
}

void full_screen(Window win)
{
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, win, &wa);

    int cx = wa.x + wa.width / 2;
    int cy = wa.y + wa.height / 2;

    XRRCrtcInfo *ci;
    for (int i = 0; i < sres->ncrtc; ++i)
        if ((ci = XRRGetCrtcInfo(dpy, sres, sres->crtcs[i])))
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

    Cursor cursor = XCreateFontCursor(dpy, XC_left_ptr);
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

    const KeyCode full = grab_keycode("f");
    const KeyCode next = grab_keycode("Tab");
    const KeyCode kill = grab_keycode("k");
    const KeyCode exit = grab_keycode("x");

    XButtonEvent move = {.subwindow = None};
    XWindowAttributes attrs;

    XEvent ev;
    while (!XNextEvent(dpy, &ev))
    {
        if (ev.type == ButtonPress)
        {
            XRaiseWindow(dpy, ev.xbutton.subwindow);
            XSetInputFocus(dpy, ev.xbutton.subwindow, RevertToParent, CurrentTime);

            move = ev.xbutton;
            XGetWindowAttributes(dpy, move.subwindow, &attrs);
        }
        else if (ev.type == MotionNotify)
        {
            int dx = ev.xmotion.x_root - move.x_root;
            int dy = ev.xmotion.y_root - move.y_root;

            if (move.state & ShiftMask)
            {
                int w = attrs.width + dx;
                int h = attrs.height + dy;
                if (w > 0 && h > 0) XResizeWindow(dpy, move.subwindow, w, h);
            }
            else XMoveWindow(dpy, move.subwindow, attrs.x + dx, attrs.y + dy);
        }
        else if (ev.type == ButtonRelease)
            move.subwindow = None;

        else if (ev.type == KeyPress)
        {
            if (ev.xkey.keycode == full)
                full_screen(ev.xkey.subwindow);

            else if (ev.xkey.keycode == next)
            {
                XCirculateSubwindowsUp(dpy, root);
                XSetInputFocus(dpy, ev.xkey.window, RevertToParent, CurrentTime);
            }
            else if (ev.xkey.keycode == kill)
                XKillClient(dpy, ev.xkey.subwindow);

            else if (ev.xkey.keycode == exit) break;
        }
    }

    XRRFreeScreenResources(sres);
    XCloseDisplay(dpy);
    return 0;
}
