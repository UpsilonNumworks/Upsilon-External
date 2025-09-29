.PHONY: %_flash
%_flash: %_rebuild
	./tool/archive apps.tar $@
	@echo "Waiting for the calculator to be connected, use the bootloader to flash on Upsilon if your app is bigger than 2MB"
	@until dfu-util -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	dfu-util -i 0 -a 0 -s 0x90200000 -D apps.tar

.PHONY: %_rebuild
%_rebuild: api/libapi.a
	@echo "Rebuilding..."
# 	Because the Makefile can't interpret the `%` operator, we have
#   to use the following command to rebuild the application.
	./tool/rebuild.sh $@

.PHONE: %_clean
%_clean:
	@echo "Cleaning..."
	rm -rf apps.tar
	rm -rf api/libapi.a
	./tool/clean.sh $@

api/libapi.a:
	make -C api

apps/%/app.elf: api/libapi.a
	make -C apps/$@

clean:
	@echo "make clean is deprecated, use make app-name_clean instead"
	rm -f apps.tar
	make -C api clean
	make -C apps/Periodic clean
	make -C apps/KhiCAS clean
	make -C apps/Nofrendo clean
	make -C apps/Peanut-GB clean
	make -C apps/HexEdit clean
	make -C apps/UnitCircle clean
	make -C apps/BadApple clean
	make -C apps/CHIP-8 clean
	make -C apps/Example clean
	make -C apps/Example-Cpp clean
	make -C apps/FlappyBird clean

