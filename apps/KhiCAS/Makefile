API=../../api
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
AR=arm-none-eabi-ar
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions -fno-strict-aliasing -fno-common -fno-threadsafe-statics
LDFLAGS=-Wl,-L$(API) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nano.specs -nostartfiles -Wl,-Ur -lc -lstdc++ -lm -lapi -lnosys

# Use the same shell for all the commands of a recipe. It applies to all the recipes of the Makefile.
.ONESHELL:

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

app.elf: src/main.o src/lib/libgiac.a src/lib/libmpfi.a src/lib/libmpfr.a src/lib/libgmp.a src/lib/libmicropy.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

libDirectory:
	mkdir -p src/lib
	mkdir -p src/include

src/lib/libgmp.a src/include/gmp.h: libDirectory
	cd src/gmp-6.2.1
# Rebuild the Makefile because cleanall removed it.
	./configure --host=arm-none-eabi --disable-assembly CFLAGS="-Os -fdata-sections -ffunction-sections -DNDEBUG -mthumb -march=armv7e-m -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -fno-strict-aliasing --specs=nosys.specs"
# Build the library.
	make
# Copy the library to the build directory.
	cp .libs/libgmp.a ../lib/
	cp gmp.h ../include/gmp.h

src/lib/libmpfi.a src/include/mpfi.h: src/lib/libmpfr.a src/include/mpfr.h src/lib/libgmp.a src/include/gmp.h libDirectory
	cd src/mpfi-1.5.4
# Rebuild the Makefile because cleanall removed it.
	autoreconf
# Quick and dirty hack for github pages to work
# Setup symlinks manually
	mkdir -p build-aux
	mkdir -p m4

	ln -sf /usr/share/libtool/build-aux/ltmain.sh    build-aux/ltmain.sh
	ln -sf /usr/share/automake-1.16/missing          build-aux/missing
	ln -sf /usr/share/automake-1.16/config.sub       build-aux/config.sub
	ln -sf /usr/share/automake-1.16/compile          build-aux/compile
	ln -sf /usr/share/automake-1.16/config.guess     build-aux/config.guess
	ln -sf /usr/share/automake-1.16/test-driver      build-aux/test-driver
	ln -sf /usr/share/automake-1.16/install-sh       build-aux/install-sh
	ln -sf /usr/share/automake-1.16/ar-lib           build-aux/ar-lib
	ln -sf /usr/share/automake-1.16/depcomp          build-aux/depcomp
	ln -sf /usr/share/aclocal/ltoptions.m4           m4/ltoptions.m4
	ln -sf /usr/share/aclocal/ltsugar.m4             m4/ltsugar.m4
	ln -sf /usr/share/aclocal/libtool.m4             m4/libtool.m4
	ln -sf /usr/share/aclocal/lt~obsolete.m4         m4/lt~obsolete.m4
	ln -sf /usr/share/aclocal/ltversion.m4           m4/ltversion.m4


	./configure --with-mpfr=$(shell pwd)/src/ --with-gmp=$(shell pwd)/src/ --host=arm-none-eabi CFLAGS="-Os -fdata-sections -ffunction-sections -DNDEBUG -mthumb -march=armv7e-m -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -fno-strict-aliasing --specs=nosys.specs"
# Build the library
	make
# Copy the library to the correct location
	cp src/.libs/libmpfi.a ../lib/
# Copy the header files to the correct location
	cp src/mpfi.h ../include/mpfi.h
	# rm -r build-aux
	# rm -r m4

src/lib/libmpfr.a src/include/mpfr.h: src/lib/libgmp.a src/include/gmp.h libDirectory
	cd src/mpfr-4.1.0
#	Rebuild the Makefile because cleanall removed it.
	./configure --with-gmp=$(shell pwd)/src/ --host=arm-none-eabi --disable-shared CFLAGS="-Os -fdata-sections -ffunction-sections -DNDEBUG -mthumb -march=armv7e-m -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -fno-strict-aliasing --specs=nosys.specs"
	autoreconf
# Now, we can build the library
	make
# Copy the library to the correct location
	cp src/.libs/libmpfr.a ../lib/
# Copy the header files to the correct location
	cp src/mpfr.h ../include/mpfr.h
src/lib/libmicropy.a: libDirectory
	cd src/micropython-1.12/numworks

# Due to the update of compiler, we must ignore the error
	sed -i 's/\$$(CC) \$$(CFLAGS)/\$$(CC) \$$(CFLAGS) -Wno-error/g' ../py/mkrules.mk || true
# We need to use a script to build MicroPython, because the make command crashes in MicroPython because it tries to link without the required libraries
	./mklib

src/lib/libgiac.a: src/lib/libgmp.a src/lib/libmpfi.a src/lib/libmpfr.a src/include/gmp.h src/include/mpfi.h src/include/mpfr.h libDirectory
	cd src/giac-1.6.0/src
	make -f Makefile.numworks.n0110
	cp libgiac.a ../../lib/

clean:
	rm -f *.elf src/*.o src/include/*.h src/lib/*.a

cleanall: clean
# Clean all the build directories.
	cd src/gmp-6.2.1
	make distclean
	cd ../mpfr-4.1.0
	make distclean
	cd ../mpfi-1.5.4
	make distclean
	rm aclocal.m4
	cd ../micropython-1.12/numworks
	make clean
	cd ../../giac-1.6.0/src
	rm -f *.o *.a
