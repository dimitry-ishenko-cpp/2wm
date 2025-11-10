#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>

Display *dpy;
Window root;
XRRScreenResources *sres;

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

    const KeyCode full = XKeysymToKeycode(dpy, XK_f);
    const KeyCode next = XKeysymToKeycode(dpy, XK_Tab);
    const KeyCode kill = XKeysymToKeycode(dpy, XK_k);
    const KeyCode quit = XKeysymToKeycode(dpy, XK_q);

    const KeyCode *code[] = { &full, &next, &kill, &quit };
    for (int i = 0; i < sizeof(code) / sizeof(code[0]); ++i)
    {
        XGrabKey(dpy, *code[i], Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    }

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

            else if (ev.xkey.keycode == quit) break;
        }
    }

    XRRFreeScreenResources(sres);
    XCloseDisplay(dpy);
    return 0;
}
