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

### Binary

Ubuntu:

```shell
sudo add-apt-repository ppa:ppa-verse/package
sudo apt install 2wm
```

Debian:
```shell
p=2wm v=0.1
wget https://github.com/dimitry-ishenko-cpp/${p}/releases/download/${v}/${p}_${v}_amd64.deb
sudo apt install ./${p}_${v}_amd64.deb
```

RaspberryPi:

```shell
p=2wm v=0.1
wget https://github.com/dimitry-ishenko-cpp/${p}/releases/download/${v}/${p}_${v}_arm64.deb
sudo apt install ./${p}_${v}_arm64.deb
```

### From source

Stable version (requires [_CMake_](https://cmake.org/) >= 3.16, _libx11-dev_,
_libxrandr-dev_ and [_pkgconf_](http://pkgconf.org/)):

```shell
p=2wm v=0.1
wget https://github.com/dimitry-ishenko-cpp/${p}/archive/v${v}.tar.gz
tar xzf v${v}.tar.gz
mkdir ${p}-${v}/build
cd ${p}-${v}/build
cmake ..
make
sudo make install
```

Latest master (requires [_git_](https://git-scm.com/),
[_CMake_](https://cmake.org/) >= 3.16, _libx11-dev_, _libxrandr-dev_ and
[_pkgconf_](http://pkgconf.org/)):

```shell
p=2wm
git clone --recursive https://github.com/dimitry-ishenko-cpp/${p}.git
mkdir ${p}/build
cd ${p}/build
cmake ..
make
sudo make install
```

## Authors

* **Dimitry Ishenko** - dimitry (dot) ishenko (at) (gee) mail (dot) com

## License

This project is distributed under the GNU GPL license. See the
[LICENSE.md](LICENSE.md) file for details.
