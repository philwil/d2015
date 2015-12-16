CFLAGS += -g -Wall
VERSION = 1.0.0

all: my_user

my_user: my_user.o my_lib.o

romfs:
	$(ROMFSINST) /bin/my_user

#install:
#	$(ROMFSINST) /bin/my_user

clean:
	rm -f my_user *.elf *.gdb *.o *~

package: clean
	cd .. && tar cvzf ../dl/my_user-$(VERSION).tar.gz my_user

install:: 
	mkdir -p ${DESTDIR}/${BINDIR}
	install -m 0755 my_user ${DESTDIR}/${BINDIR}/


br_install: clean
	mkdir -p ../../adi-buildroot-code/output/build/my_user-$(VERSION)
	mkdir -p ../../adi-buildroot-code/package/my_user
	cp *.mk ../../adi-buildroot-code/package/my_user
	-cp  *.patch ../../adi-buildroot-code/package/my_user
	cp Config.in ../../adi-buildroot-code/package/my_user
	cp * ../../adi-buildroot-code/output/build/my_user-$(VERSION)
	rm -f ../../adi-buildroot-code/output/build/my_user-$(VERSION)/.stamp_*
	touch ../../adi-buildroot-code/output/build/my_user-$(VERSION)/.stamp_downloaded
	touch ../../adi-buildroot-code/output/build/my_user-$(VERSION)/.stamp_extracted
	touch ../../adi-buildroot-code/output/build/my_user-$(VERSION)/.stamp_configured

br_config: clean
	-cd ../../adi-buildroot-code && patch < ../sdcs/my_user/add_my_user