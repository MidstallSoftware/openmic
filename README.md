# OpenMic

![GitHub Workflow Status](https://img.shields.io/github/workflow/status/MidstallSoftware/openmic/Build?style=for-the-badge)
![GitHub last commit](https://img.shields.io/github/last-commit/MidstallSoftware/openmic?style=for-the-badge)

A free and open source real time voice effects software.

## Dependencies

* GTK (**Not needed if the GUI is disabled**)
* NCurses (**Not needed if the TUI is disabled**)
* GStreamer

## Building

Building can be achieved using `meson`, ex:

```
$ meson build
$ ninja -C build
```

A shared and static library are automatically produced along with the executables. To build just the library, you can run meson with these arguments: `-D enable_gui=false -D enable_cli=false -D enable_tui=false`.
