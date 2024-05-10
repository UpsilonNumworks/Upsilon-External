# Epsilon

The base firmware image is a fork of https://github.com/numworks/epsilon including
the "external" application and an API for accessing base functionality of epsilon.

The modified sources can be found here : https://github.com/Delta-NumWorks/delta/tree/external

## Developer info

To create a new application, go to [the external applications creation guide](docs/new-app.md)

## If something goes wrong

Reinstall Omega from https://getomega.dev/, doing a device recovery from there if necessary.

## Included applications

### KhiCAS

The port included here is a port from the delta firmware : https://github.com/Delta-NumWorks/delta/tree/delta

### Periodic

This is a port of https://github.com/M4xi1m3/nw-atom

### Nofrendo

This is a port of Nofrendo from https://www.baisoku.org/

### Unit Circle

A simple app showing an unit circle for trigonometry

### CHIP-8

This is a port of CHIP-8 from https://github.com/f0lg0/CHIP-8

### PngBrowser

An application that allows you to view PNG files https://github.com/Oiha-dev/PngBrowser

## Technical concepts

- The firmware part is just a regular application called "External", looking for a tar archive at address 0x90200000. If found, each file is displayed in a menu.
- To launch a file, the external application fetches the firsts 4 bytes of the file, and use it as a function pointer to the address of the entrypoint. The entrypoint is then called with an array of function pointers as en API, and a pointer to a pre-allocated heap.
- Each application is totally independent (and needs to bring it's own libc, ...). data and bss sections are allocated in ITCM RAM, and the heap is pre-allocated from epsilon. The state of the application is totally reset upon entry.
- To build a usable tar archive, relocation needs to be done to account the real adresses in flash of the binaries. There is a script called "archive" in this repository, which calls the linker on partially linked elf, to build final correctly relocated elf files (then added to the tar archive).
- The file in the tar must be marked as executable to be executable.
