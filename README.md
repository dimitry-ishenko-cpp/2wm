# 2wm — two-bit Window Manager

**2wm** is a tiny X11 window manager inspired by
[_tinywm_](http://incise.org/tinywm.html) and
[_1wm_](https://github.com/lslvr/1wm), designed to provide a minimalistic
graphical experience.

## Usage

Unlike traditional window managers, **2wm** is primarily intended to be used as
a prefix command to launch a specific X11 application within its own X server
session. For example:

```sh
2wm firefox
```

This command will:

1. Start a new X server instance running **2wm**.
2. Launch Firefox inside that X server.
3. Stop the X server automatically when Firefox exits.

Alternatively, you can start **2wm** standalone by running:

```sh
2wm
```

and launch applications inside it however you normally would (eg, `DISPLAY=:0 firefox`).

The following shortcuts can be used with **2wm**:

| Shortcut                                   | Action                                           |
|--------------------------------------------|--------------------------------------------------|
| <kbd>Meta</kbd> + 🖱️ click                 | Focus and raise the window under the cursor      |
| <kbd>Meta</kbd> + 🖱️ click-and-drag        | Move the window by dragging                      |
| <kbd>Meta</kbd> <kbd>Shift</kbd> + 🖱️ click-and-drag | Resize the window by dragging          |
| <kbd>Meta</kbd> + <kbd>Tab</kbd>           | Cycle (focus and raise) through all open windows |
| <kbd>Meta</kbd> + <kbd>F</kbd>             | Resize the window to full-screen                 |
| <kbd>Meta</kbd> + <kbd>K</kbd>             | Close (kill) the window under the cursor         |
| <kbd>Meta</kbd> + <kbd>Q</kbd>             | Quit **2wm** and stop the X server               |

## Installation

TODO

## Authors

* **Dimitry Ishenko** - dimitry (dot) ishenko (at) (gee) mail (dot) com

## License

This project is distributed under the GNU GPL license. See the
[LICENSE.md](LICENSE.md) file for details.
