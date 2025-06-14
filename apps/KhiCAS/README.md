# KhiCAS

KhiCAS is a swiss knife for the mathematics. It's a port of xCAS for the calculator.

It's a bit complex to use, but when you know how to use it, you can do a lot of things. The interface is a shell, with some menus.

## Updating KhiCAS

To update KhiCAS, you have some things to do:

1. Download the latest version of KhiCAS from Parisse's website using `wget https://www-fourier.univ-grenoble-alpes.fr/~parisse/numworks/khiext.tgz`
2. Extract the archive using `tar xvfz khiext.tgz`
3. Copy the contents of the folder `khiext/apps/KhiCAS` to the folder `apps/KhiCAS`
4. Now, you have the new version of KhiCAS, but you have to apply some changes to the code.
5. Replace `inline bool iskeydown(int key){ return getkey(key | 0x80000000); }` by `inline bool iskeydown(int key);`
6. Add this code in the file `apps/KhiCAS/src/main.cpp`:

```cpp
bool iskeydown(int key) {
  return extapp_isKeydown(key);
}
```

7. If you have build error, please check if the Makfile is modified. If yes, restore the file to the original state.
8. You can try to run `make cleanall` in the folder `apps/KhiCAS` to clean the build and rebuild the program.
9. If it continues to crash, please ask for help in an issue on the GitHub repository or on the Discord server.
