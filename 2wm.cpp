#include <initializer_list>

#include <sys/select.h> // select
#include <sys/syscall.h> // pidfd_open, syscall
#include <unistd.h> // execvp, fork
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>

Display* dpy;
Window root;
XRRScreenResources* sres;

auto get(KeySym sym){ return XKeysymToKeycode(dpy, sym); };
void grab(KeyCode code){ XGrabKey(dpy, code, Mod4Mask, root, True, GrabModeAsync, GrabModeAsync); };

void grab(int button, int mask){ XGrabButton(dpy, button, mask, root, True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
};

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

    pid_t child = 0;
    if (argc > 1 && (child = fork()) == 0)
    {
        execvp(argv[1], &argv[1]);
        return 1;
    }

    const auto full = get(XK_f), next = get(XK_Tab), kill = get(XK_k), quit = get(XK_q);
    for (auto code : {full, next, kill, quit}) grab(code);

    grab(Button1, Mod4Mask);
    grab(Button1, Mod4Mask|ShiftMask);

    fd_set fds;
    auto fd_c = syscall(SYS_pidfd_open, child, 0);
    auto fd_x = ConnectionNumber(dpy);
    auto fd_max = fd_c > fd_x ? fd_c : fd_x;

    XButtonEvent move{.subwindow = None};
    XWindowAttributes attrs;

    for (bool done = false; !done;)
    {
        FD_ZERO(&fds);
        if (fd_c > 0) FD_SET(fd_c, &fds);
        FD_SET(fd_x, &fds);

        XFlush(dpy);
        select(fd_max + 1, &fds, NULL, NULL, NULL);
        if (FD_ISSET(fd_c, &fds)) done = true;

        while (XPending(dpy))
        {
            XEvent ev;
            XNextEvent(dpy, &ev);

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
        }
    }

    XRRFreeScreenResources(sres);
    XCloseDisplay(dpy);
    return 0;
}
