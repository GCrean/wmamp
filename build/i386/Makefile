##
##  this has got so ugly
##
##  make os=arm for wma11b version
##  make  for x86 linux test port
##
## We don't want to maintain 2 makefiles for each architecture
## unless there is a major difference, so arm and i386
## makefiles are linked together
##



CFLAGS = -Wall -g $(LIBPATH) $(INC) -DNOT_HAVE_SA_LEN -fstrict-aliasing -fomit-frame-pointer -DI386

#-Wstrict-prototypes -Wshadow -Os -fstrict-aliasing -fomit-frame-pointer -D_GNU_SOURCE -DNDEBUG 

include ../common.mk

DEPEND= makedepend 
#$(CFLAGS)

all: depend wmamp cores ppm2u16


hasher.o: ../../authentication/hasher.c 
	$(CC) -c $(CFLAGS) ../../authentication/hasher.c -o $@

md5.o: ../../authentication/md5.c ../../authentication/md5.h
	$(CC) -c $(CFLAGS) ../../authentication/md5.c -o $@

mDNS.o: $(RENDEZVOUS)/mDNSCore/mDNS.c
	$(CC) -c $(CFLAGS) $(RENDEZVOUS)/mDNSCore/mDNS.c -o $@

mDNSPosix.o: $(RENDEZVOUS)/mDNSPosix/mDNSPosix.c
	$(CC) -c $(CFLAGS) -I$(RENDEZVOUS)/mDNSCore $(RENDEZVOUS)/mDNSPosix/mDNSPosix.c -o $@

mDNSUNP.o: $(RENDEZVOUS)/mDNSPosix/mDNSUNP.c
	$(CC) -c $(CFLAGS) $(RENDEZVOUS)/mDNSPosix/mDNSUNP.c -o $@

wmamp: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBPATH) -lfreetype -lmad $(OBJS) -lpthread  /usr/local/lib/libhttpd.a -o $@

%.o : ../../%.c
	$(CC) $(CFLAGS) -c ../../$*.c

depend: $(SRCS)
	$(DEPEND) $(INC) $(SRCS)

cores:
	rm -f core.*

clean:
	rm -f *.o wmamp core.* core ppm2u16
# DO NOT DELETE

../../main.o: /usr/include/unistd.h /usr/include/features.h
../../main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../main.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
../../main.o: /usr/include/bits/wordsize.h
../../main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../main.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
../../main.o: ../../screen.h ../../video.h /usr/include/fcntl.h
../../main.o: /usr/include/bits/fcntl.h /usr/include/sys/types.h
../../main.o: /usr/include/time.h /usr/include/linux/fb.h
../../main.o: /usr/include/linux/tty.h /usr/include/asm/types.h
../../main.o: /usr/include/sys/mman.h /usr/include/bits/mman.h ../../fonts.h
../../main.o: /usr/include/ft2build.h
../../main.o: /usr/include/freetype2/freetype/config/ftheader.h
../../main.o: /usr/include/freetype2/freetype/internal/internal.h
../../main.o: /usr/include/freetype2/freetype/freetype.h
../../main.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../main.o: /usr/include/freetype2/freetype/config/ftoption.h
../../main.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../main.o: /usr/include/limits.h
../../main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../main.o: /usr/include/ctype.h /usr/include/endian.h
../../main.o: /usr/include/bits/endian.h /usr/include/string.h
../../main.o: /usr/include/stdio.h /usr/include/libio.h
../../main.o: /usr/include/_G_config.h /usr/include/wchar.h
../../main.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../main.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../main.o: /usr/include/stdlib.h /usr/include/setjmp.h
../../main.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
../../main.o: /usr/include/freetype2/freetype/fterrors.h
../../main.o: /usr/include/freetype2/freetype/ftmoderr.h
../../main.o: /usr/include/freetype2/freetype/fterrdef.h
../../main.o: /usr/include/freetype2/freetype/fttypes.h
../../main.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../main.o: /usr/include/freetype2/freetype/ftsystem.h
../../main.o: /usr/include/freetype2/freetype/ftimage.h
../../main.o: /usr/include/freetype2/freetype/ftglyph.h
../../main.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../main.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../main.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../main.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../main.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../main.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../main.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../main.o: /usr/include/bits/sched.h /usr/include/signal.h
../../main.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/initspin.h
../../main.o: /usr/include/bits/sigthread.h ../../fonts.h ../../remote.h
../../main.o: ../../mp3.h ../../song.h ../../net.h ../../bitmap.h
../../main.o: ../../httpd.h ../../mixer.h ../../timer.h ../../version.h
../../http.o: /usr/include/stdio.h /usr/include/features.h
../../http.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../http.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../http.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../http.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../http.o: /usr/include/_G_config.h /usr/include/wchar.h
../../http.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../http.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../http.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../http.o: /usr/include/sys/types.h /usr/include/time.h
../../http.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
../../http.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
../../http.o: /usr/include/limits.h
../../http.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../http.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../http.o: /usr/include/asm/sockios.h /usr/include/netinet/in.h
../../http.o: /usr/include/stdint.h /usr/include/bits/in.h
../../http.o: /usr/include/endian.h /usr/include/bits/endian.h
../../http.o: /usr/include/bits/byteswap.h /usr/include/memory.h
../../http.o: /usr/include/string.h /usr/include/unistd.h
../../http.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
../../http.o: /usr/include/netdb.h /usr/include/bits/netdb.h
../../http.o: /usr/include/stdlib.h /usr/include/errno.h
../../http.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
../../http.o: /usr/include/asm/errno.h /usr/include/fcntl.h
../../http.o: /usr/include/bits/fcntl.h ../../http.h
../../http.o: ../../authentication/hasher.h ../../net.h
../../screen2.o: /usr/include/stdio.h /usr/include/features.h
../../screen2.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../screen2.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../screen2.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../screen2.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../screen2.o: /usr/include/_G_config.h /usr/include/wchar.h
../../screen2.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../screen2.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../screen2.o: /usr/include/bits/stdio_lim.h
../../screen2.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../screen2.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../screen2.o: /usr/include/bits/confname.h /usr/include/memory.h
../../screen2.o: /usr/include/string.h ../../screen.h ../../video.h
../../screen2.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../screen2.o: /usr/include/sys/types.h /usr/include/time.h
../../screen2.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../screen2.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../screen2.o: /usr/include/bits/mman.h ../../fonts.h
../../screen2.o: /usr/include/ft2build.h
../../screen2.o: /usr/include/freetype2/freetype/config/ftheader.h
../../screen2.o: /usr/include/freetype2/freetype/internal/internal.h
../../screen2.o: /usr/include/freetype2/freetype/freetype.h
../../screen2.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen2.o: /usr/include/freetype2/freetype/config/ftoption.h
../../screen2.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../screen2.o: /usr/include/limits.h
../../screen2.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../screen2.o: /usr/include/ctype.h /usr/include/endian.h
../../screen2.o: /usr/include/bits/endian.h /usr/include/setjmp.h
../../screen2.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
../../screen2.o: /usr/include/freetype2/freetype/fterrors.h
../../screen2.o: /usr/include/freetype2/freetype/ftmoderr.h
../../screen2.o: /usr/include/freetype2/freetype/fterrdef.h
../../screen2.o: /usr/include/freetype2/freetype/fttypes.h
../../screen2.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen2.o: /usr/include/freetype2/freetype/ftsystem.h
../../screen2.o: /usr/include/freetype2/freetype/ftimage.h
../../screen2.o: /usr/include/freetype2/freetype/ftglyph.h
../../screen2.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../screen2.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../screen2.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../screen2.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../screen2.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../screen2.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../screen2.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../screen2.o: /usr/include/bits/sched.h /usr/include/signal.h
../../screen2.o: /usr/include/bits/pthreadtypes.h
../../screen2.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../screen2.o: ../../daap.h ../../msgtypes.h ../../commands.h ../../tree.h
../../screen2.o: ../../listview.h ../../song.h ../../dialog.h
../../screen2.o: ../../nowplaying.h
../../daap.o: /usr/include/sys/types.h /usr/include/features.h
../../daap.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../daap.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../daap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../daap.o: /usr/include/bits/typesizes.h /usr/include/time.h
../../daap.o: /usr/include/stdio.h /usr/include/libio.h
../../daap.o: /usr/include/_G_config.h /usr/include/wchar.h
../../daap.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../daap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../daap.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../daap.o: /usr/include/stdlib.h /usr/include/string.h
../../daap.o: /usr/include/assert.h ../../daap.h ../../hexdump.h
../../hexdump.o: /usr/include/stdio.h /usr/include/features.h
../../hexdump.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../hexdump.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../hexdump.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../hexdump.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../hexdump.o: /usr/include/_G_config.h /usr/include/wchar.h
../../hexdump.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../hexdump.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../hexdump.o: /usr/include/bits/stdio_lim.h
../../hexdump.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
../../hexdump.o: /usr/include/stdlib.h /usr/include/ctype.h
../../hexdump.o: /usr/include/endian.h /usr/include/bits/endian.h
../../hexdump.o: ../../hexdump.h
../../commands.o: /usr/include/string.h /usr/include/features.h
../../commands.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../commands.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../commands.o: /usr/include/stdio.h /usr/include/bits/types.h
../../commands.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
../../commands.o: /usr/include/libio.h /usr/include/_G_config.h
../../commands.o: /usr/include/wchar.h /usr/include/bits/wchar.h
../../commands.o: /usr/include/gconv.h
../../commands.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../commands.o: /usr/include/bits/stdio_lim.h
../../commands.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../commands.o: ../../http.h /usr/include/netdb.h /usr/include/netinet/in.h
../../commands.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../commands.o: /usr/include/limits.h
../../commands.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../commands.o: /usr/include/sys/types.h /usr/include/time.h
../../commands.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../commands.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../commands.o: /usr/include/endian.h /usr/include/bits/endian.h
../../commands.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../commands.o: ../../daap.h ../../mp3.h ../../song.h ../../msgqueue.h
../../commands.o: /usr/include/pthread.h /usr/include/sched.h
../../commands.o: /usr/include/bits/sched.h /usr/include/signal.h
../../commands.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
../../commands.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../commands.o: ../../tree.h ../../commands.h
../../mp3.o: /usr/include/stdio.h /usr/include/features.h
../../mp3.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../mp3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../mp3.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../mp3.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../mp3.o: /usr/include/_G_config.h /usr/include/wchar.h
../../mp3.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../mp3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../mp3.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../mp3.o: /usr/include/stdlib.h /usr/include/unistd.h
../../mp3.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
../../mp3.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
../../mp3.o: /usr/include/sys/mman.h /usr/include/bits/mman.h
../../mp3.o: /usr/include/sys/types.h /usr/include/time.h
../../mp3.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../mp3.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
../../mp3.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h
../../mp3.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
../../mp3.o: /usr/include/asm/ioctls.h /usr/include/bits/ioctl-types.h
../../mp3.o: /usr/include/sys/ttydefaults.h /usr/include/sys/socket.h
../../mp3.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
../../mp3.o: /usr/include/bits/socket.h /usr/include/limits.h
../../mp3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../mp3.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../mp3.o: /usr/include/asm/sockios.h /usr/include/memory.h
../../mp3.o: /usr/include/string.h /usr/include/errno.h
../../mp3.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
../../mp3.o: /usr/include/asm/errno.h /usr/include/mad.h ../../screen.h
../../mp3.o: ../../video.h /usr/include/linux/fb.h /usr/include/linux/tty.h
../../mp3.o: /usr/include/asm/types.h ../../fonts.h /usr/include/ft2build.h
../../mp3.o: /usr/include/freetype2/freetype/config/ftheader.h
../../mp3.o: /usr/include/freetype2/freetype/internal/internal.h
../../mp3.o: /usr/include/freetype2/freetype/freetype.h
../../mp3.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../mp3.o: /usr/include/freetype2/freetype/config/ftoption.h
../../mp3.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../mp3.o: /usr/include/ctype.h /usr/include/endian.h
../../mp3.o: /usr/include/bits/endian.h /usr/include/setjmp.h
../../mp3.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
../../mp3.o: /usr/include/freetype2/freetype/fterrors.h
../../mp3.o: /usr/include/freetype2/freetype/ftmoderr.h
../../mp3.o: /usr/include/freetype2/freetype/fterrdef.h
../../mp3.o: /usr/include/freetype2/freetype/fttypes.h
../../mp3.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../mp3.o: /usr/include/freetype2/freetype/ftsystem.h
../../mp3.o: /usr/include/freetype2/freetype/ftimage.h
../../mp3.o: /usr/include/freetype2/freetype/ftglyph.h
../../mp3.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../mp3.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../mp3.o: /usr/include/stdint.h /usr/include/bits/in.h
../../mp3.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../mp3.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../mp3.o: /usr/include/bits/sched.h /usr/include/signal.h
../../mp3.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/initspin.h
../../mp3.o: /usr/include/bits/sigthread.h ../../hexdump.h ../../mp3.h
../../mp3.o: ../../song.h ../../commands.h ../../daap.h ../../tree.h
../../mp3.o: ../../msgtypes.h ../../dialog.h ../../nowplaying.h
../../tree.o: /usr/include/stdio.h /usr/include/features.h
../../tree.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../tree.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../tree.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../tree.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../tree.o: /usr/include/_G_config.h /usr/include/wchar.h
../../tree.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../tree.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../tree.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../tree.o: /usr/include/stdlib.h /usr/include/string.h
../../tree.o: /usr/include/ctype.h /usr/include/endian.h
../../tree.o: /usr/include/bits/endian.h ../../tree.h
../../fonts.o: ../../fonts.h ../../video.h /usr/include/fcntl.h
../../fonts.o: /usr/include/features.h /usr/include/sys/cdefs.h
../../fonts.o: /usr/include/gnu/stubs.h /usr/include/bits/fcntl.h
../../fonts.o: /usr/include/sys/types.h /usr/include/bits/types.h
../../fonts.o: /usr/include/bits/wordsize.h
../../fonts.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../fonts.o: /usr/include/bits/typesizes.h /usr/include/time.h
../../fonts.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../fonts.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../fonts.o: /usr/include/bits/mman.h
../../net.o: /usr/include/sys/time.h /usr/include/features.h
../../net.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../net.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../net.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../net.o: /usr/include/bits/typesizes.h /usr/include/time.h
../../net.o: /usr/include/bits/time.h /usr/include/sys/select.h
../../net.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../../net.o: /usr/include/sys/types.h /usr/include/sys/socket.h
../../net.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
../../net.o: /usr/include/bits/socket.h /usr/include/limits.h
../../net.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../net.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../net.o: /usr/include/asm/sockios.h /usr/include/signal.h
../../net.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../net.o: /usr/include/bits/confname.h ../../screen.h ../../video.h
../../net.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../net.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../net.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../net.o: /usr/include/bits/mman.h ../../fonts.h /usr/include/ft2build.h
../../net.o: /usr/include/freetype2/freetype/config/ftheader.h
../../net.o: /usr/include/freetype2/freetype/internal/internal.h
../../net.o: /usr/include/freetype2/freetype/freetype.h
../../net.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../net.o: /usr/include/freetype2/freetype/config/ftoption.h
../../net.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../net.o: /usr/include/ctype.h /usr/include/endian.h
../../net.o: /usr/include/bits/endian.h /usr/include/string.h
../../net.o: /usr/include/stdio.h /usr/include/libio.h
../../net.o: /usr/include/_G_config.h /usr/include/wchar.h
../../net.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../net.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../net.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../net.o: /usr/include/stdlib.h /usr/include/setjmp.h
../../net.o: /usr/include/bits/setjmp.h
../../net.o: /usr/include/freetype2/freetype/fterrors.h
../../net.o: /usr/include/freetype2/freetype/ftmoderr.h
../../net.o: /usr/include/freetype2/freetype/fterrdef.h
../../net.o: /usr/include/freetype2/freetype/fttypes.h
../../net.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../net.o: /usr/include/freetype2/freetype/ftsystem.h
../../net.o: /usr/include/freetype2/freetype/ftimage.h
../../net.o: /usr/include/freetype2/freetype/ftglyph.h
../../net.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../net.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../net.o: /usr/include/stdint.h /usr/include/bits/in.h
../../net.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../net.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../net.o: /usr/include/bits/sched.h /usr/include/bits/pthreadtypes.h
../../net.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../net.o: ../../bitmap.h ../../timer.h
../../listview.o: /usr/include/assert.h /usr/include/features.h
../../listview.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../listview.o: ../../listview.h ../../screen.h ../../video.h
../../listview.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../listview.o: /usr/include/sys/types.h /usr/include/bits/types.h
../../listview.o: /usr/include/bits/wordsize.h
../../listview.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../listview.o: /usr/include/bits/typesizes.h /usr/include/time.h
../../listview.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../listview.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../listview.o: /usr/include/bits/mman.h ../../fonts.h
../../listview.o: /usr/include/ft2build.h
../../listview.o: /usr/include/freetype2/freetype/config/ftheader.h
../../listview.o: /usr/include/freetype2/freetype/internal/internal.h
../../listview.o: /usr/include/freetype2/freetype/freetype.h
../../listview.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../listview.o: /usr/include/freetype2/freetype/config/ftoption.h
../../listview.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../listview.o: /usr/include/limits.h
../../listview.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../listview.o: /usr/include/ctype.h /usr/include/endian.h
../../listview.o: /usr/include/bits/endian.h /usr/include/string.h
../../listview.o: /usr/include/stdio.h /usr/include/libio.h
../../listview.o: /usr/include/_G_config.h /usr/include/wchar.h
../../listview.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../listview.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../listview.o: /usr/include/bits/stdio_lim.h
../../listview.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../listview.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h
../../listview.o: /usr/include/bits/sigset.h
../../listview.o: /usr/include/freetype2/freetype/fterrors.h
../../listview.o: /usr/include/freetype2/freetype/ftmoderr.h
../../listview.o: /usr/include/freetype2/freetype/fterrdef.h
../../listview.o: /usr/include/freetype2/freetype/fttypes.h
../../listview.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../listview.o: /usr/include/freetype2/freetype/ftsystem.h
../../listview.o: /usr/include/freetype2/freetype/ftimage.h
../../listview.o: /usr/include/freetype2/freetype/ftglyph.h
../../listview.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../listview.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../listview.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../listview.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../listview.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../listview.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../listview.o: ../../msgqueue.h /usr/include/pthread.h
../../listview.o: /usr/include/sched.h /usr/include/bits/sched.h
../../listview.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
../../listview.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../remote.o: /usr/include/stdio.h /usr/include/features.h
../../remote.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../remote.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../remote.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../remote.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../remote.o: /usr/include/_G_config.h /usr/include/wchar.h
../../remote.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../remote.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../remote.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../remote.o: /usr/include/stdlib.h /usr/include/string.h
../../remote.o: /usr/include/sys/types.h /usr/include/time.h
../../remote.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
../../remote.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
../../remote.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
../../remote.o: /usr/include/bits/ioctl-types.h
../../remote.o: /usr/include/sys/ttydefaults.h /usr/include/fcntl.h
../../remote.o: /usr/include/bits/fcntl.h /usr/include/sys/poll.h
../../remote.o: /usr/include/bits/poll.h /usr/include/sys/utsname.h
../../remote.o: /usr/include/bits/utsname.h /usr/include/unistd.h
../../remote.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
../../remote.o: ../../remote.h /usr/include/linux/unistd.h
../../remote.o: /usr/include/asm/unistd.h ../../msgqueue.h
../../remote.o: /usr/include/pthread.h /usr/include/sched.h
../../remote.o: /usr/include/bits/sched.h /usr/include/signal.h
../../remote.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
../../remote.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../remote.o: ../../msgtypes.h ../../http.h /usr/include/netdb.h
../../remote.o: /usr/include/netinet/in.h /usr/include/stdint.h
../../remote.o: /usr/include/bits/socket.h /usr/include/limits.h
../../remote.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../remote.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../remote.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../remote.o: /usr/include/endian.h /usr/include/bits/endian.h
../../remote.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../remote.o: ../../mp3.h ../../song.h ../../mixer.h ../../nowplaying.h
../../screen3.o: /usr/include/stdio.h /usr/include/features.h
../../screen3.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../screen3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../screen3.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../screen3.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../screen3.o: /usr/include/_G_config.h /usr/include/wchar.h
../../screen3.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../screen3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../screen3.o: /usr/include/bits/stdio_lim.h
../../screen3.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../screen3.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../screen3.o: /usr/include/bits/confname.h /usr/include/memory.h
../../screen3.o: /usr/include/string.h ../../screen.h ../../video.h
../../screen3.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../screen3.o: /usr/include/sys/types.h /usr/include/time.h
../../screen3.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../screen3.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../screen3.o: /usr/include/bits/mman.h ../../fonts.h
../../screen3.o: /usr/include/ft2build.h
../../screen3.o: /usr/include/freetype2/freetype/config/ftheader.h
../../screen3.o: /usr/include/freetype2/freetype/internal/internal.h
../../screen3.o: /usr/include/freetype2/freetype/freetype.h
../../screen3.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen3.o: /usr/include/freetype2/freetype/config/ftoption.h
../../screen3.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../screen3.o: /usr/include/limits.h
../../screen3.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../screen3.o: /usr/include/ctype.h /usr/include/endian.h
../../screen3.o: /usr/include/bits/endian.h /usr/include/setjmp.h
../../screen3.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
../../screen3.o: /usr/include/freetype2/freetype/fterrors.h
../../screen3.o: /usr/include/freetype2/freetype/ftmoderr.h
../../screen3.o: /usr/include/freetype2/freetype/fterrdef.h
../../screen3.o: /usr/include/freetype2/freetype/fttypes.h
../../screen3.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen3.o: /usr/include/freetype2/freetype/ftsystem.h
../../screen3.o: /usr/include/freetype2/freetype/ftimage.h
../../screen3.o: /usr/include/freetype2/freetype/ftglyph.h
../../screen3.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../screen3.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../screen3.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../screen3.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../screen3.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../screen3.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../screen3.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../screen3.o: /usr/include/bits/sched.h /usr/include/signal.h
../../screen3.o: /usr/include/bits/pthreadtypes.h
../../screen3.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../screen3.o: ../../daap.h ../../msgtypes.h ../../commands.h ../../tree.h
../../screen3.o: ../../listview.h ../../song.h ../../nowplaying.h
../../screen4.o: /usr/include/stdio.h /usr/include/features.h
../../screen4.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../screen4.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../screen4.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../screen4.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../screen4.o: /usr/include/_G_config.h /usr/include/wchar.h
../../screen4.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../screen4.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../screen4.o: /usr/include/bits/stdio_lim.h
../../screen4.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../screen4.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../screen4.o: /usr/include/bits/confname.h /usr/include/memory.h
../../screen4.o: /usr/include/string.h ../../screen.h ../../video.h
../../screen4.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../screen4.o: /usr/include/sys/types.h /usr/include/time.h
../../screen4.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../screen4.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../screen4.o: /usr/include/bits/mman.h ../../fonts.h
../../screen4.o: /usr/include/ft2build.h
../../screen4.o: /usr/include/freetype2/freetype/config/ftheader.h
../../screen4.o: /usr/include/freetype2/freetype/internal/internal.h
../../screen4.o: /usr/include/freetype2/freetype/freetype.h
../../screen4.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen4.o: /usr/include/freetype2/freetype/config/ftoption.h
../../screen4.o: /usr/include/freetype2/freetype/config/ftstdlib.h
../../screen4.o: /usr/include/limits.h
../../screen4.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../screen4.o: /usr/include/ctype.h /usr/include/endian.h
../../screen4.o: /usr/include/bits/endian.h /usr/include/setjmp.h
../../screen4.o: /usr/include/bits/setjmp.h /usr/include/bits/sigset.h
../../screen4.o: /usr/include/freetype2/freetype/fterrors.h
../../screen4.o: /usr/include/freetype2/freetype/ftmoderr.h
../../screen4.o: /usr/include/freetype2/freetype/fterrdef.h
../../screen4.o: /usr/include/freetype2/freetype/fttypes.h
../../screen4.o: /usr/include/freetype2/freetype/config/ftconfig.h
../../screen4.o: /usr/include/freetype2/freetype/ftsystem.h
../../screen4.o: /usr/include/freetype2/freetype/ftimage.h
../../screen4.o: /usr/include/freetype2/freetype/ftglyph.h
../../screen4.o: /usr/include/freetype2/freetype/freetype.h ../../http.h
../../screen4.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../screen4.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../screen4.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../screen4.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../screen4.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../screen4.o: ../../msgqueue.h /usr/include/pthread.h /usr/include/sched.h
../../screen4.o: /usr/include/bits/sched.h /usr/include/signal.h
../../screen4.o: /usr/include/bits/pthreadtypes.h
../../screen4.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../screen4.o: ../../daap.h ../../msgtypes.h ../../commands.h ../../tree.h
../../screen4.o: ../../listview.h ../../mp3.h ../../song.h ../../dialog.h
../../screen4.o: ../../nowplaying.h
../../discover.o: /usr/include/stdio.h /usr/include/features.h
../../discover.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../discover.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../discover.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../discover.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../discover.o: /usr/include/_G_config.h /usr/include/wchar.h
../../discover.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../discover.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../discover.o: /usr/include/bits/stdio_lim.h
../../discover.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
../../discover.o: /usr/include/string.h ../../discover.h
../../discover.o: /home/geert/Downloads/Linksys-WMA11B/WMAmp/mDNSResponder-58.8/mDNSCore/mDNSClientAPI.h
../../discover.o: /home/geert/Downloads/Linksys-WMA11B/WMAmp/mDNSResponder-58.8/mDNSCore/mDNSDebug.h
../../discover.o: /home/geert/Downloads/Linksys-WMA11B/WMAmp/mDNSResponder-58.8/mDNSPosix/mDNSPosix.h
../../discover.o: /usr/include/sys/time.h /usr/include/time.h
../../discover.o: /usr/include/bits/time.h /usr/include/sys/select.h
../../discover.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../../discover.o: ../../msgqueue.h /usr/include/pthread.h
../../discover.o: /usr/include/sched.h /usr/include/bits/sched.h
../../discover.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
../../discover.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../discover.o: ../../msgtypes.h ../../http.h /usr/include/netdb.h
../../discover.o: /usr/include/netinet/in.h /usr/include/stdint.h
../../discover.o: /usr/include/bits/socket.h /usr/include/limits.h
../../discover.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../discover.o: /usr/include/sys/types.h /usr/include/bits/sockaddr.h
../../discover.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
../../discover.o: /usr/include/bits/in.h /usr/include/endian.h
../../discover.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
../../discover.o: /usr/include/bits/netdb.h
../../msgqueue.o: /usr/include/pthread.h /usr/include/features.h
../../msgqueue.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../msgqueue.o: /usr/include/sched.h /usr/include/bits/types.h
../../msgqueue.o: /usr/include/bits/wordsize.h
../../msgqueue.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../msgqueue.o: /usr/include/bits/typesizes.h /usr/include/time.h
../../msgqueue.o: /usr/include/bits/sched.h /usr/include/signal.h
../../msgqueue.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
../../msgqueue.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../msgqueue.o: /usr/include/stdlib.h /usr/include/stdio.h
../../msgqueue.o: /usr/include/libio.h /usr/include/_G_config.h
../../msgqueue.o: /usr/include/wchar.h /usr/include/bits/wchar.h
../../msgqueue.o: /usr/include/gconv.h
../../msgqueue.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../msgqueue.o: /usr/include/bits/stdio_lim.h
../../msgqueue.o: /usr/include/bits/sys_errlist.h ../../msgqueue.h
../../bitmap.o: /usr/include/unistd.h /usr/include/features.h
../../bitmap.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../bitmap.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
../../bitmap.o: /usr/include/bits/wordsize.h
../../bitmap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../bitmap.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
../../bitmap.o: /usr/include/stdlib.h /usr/include/memory.h
../../bitmap.o: /usr/include/string.h ../../bitmap.h ../../video.h
../../bitmap.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../bitmap.o: /usr/include/sys/types.h /usr/include/time.h
../../bitmap.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../bitmap.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../bitmap.o: /usr/include/bits/mman.h /usr/include/stdio.h
../../bitmap.o: /usr/include/libio.h /usr/include/_G_config.h
../../bitmap.o: /usr/include/wchar.h /usr/include/bits/wchar.h
../../bitmap.o: /usr/include/gconv.h
../../bitmap.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../bitmap.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../dialog.o: /usr/include/stdlib.h /usr/include/features.h
../../dialog.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../dialog.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../dialog.o: ../../video.h /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../dialog.o: /usr/include/sys/types.h /usr/include/bits/types.h
../../dialog.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
../../dialog.o: /usr/include/time.h /usr/include/linux/fb.h
../../dialog.o: /usr/include/linux/tty.h /usr/include/asm/types.h
../../dialog.o: /usr/include/sys/mman.h /usr/include/bits/mman.h
../../dialog.o: ../../fonts.h ../../bitmap.h /usr/include/stdio.h
../../dialog.o: /usr/include/libio.h /usr/include/_G_config.h
../../dialog.o: /usr/include/wchar.h /usr/include/bits/wchar.h
../../dialog.o: /usr/include/gconv.h
../../dialog.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../dialog.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../dialog.o: ../../dialog.h ../../msgqueue.h /usr/include/pthread.h
../../dialog.o: /usr/include/sched.h /usr/include/bits/sched.h
../../dialog.o: /usr/include/signal.h /usr/include/bits/sigset.h
../../dialog.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/initspin.h
../../dialog.o: /usr/include/bits/sigthread.h ../../msgtypes.h ../../http.h
../../dialog.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../dialog.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../dialog.o: /usr/include/limits.h
../../dialog.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../dialog.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../dialog.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../dialog.o: /usr/include/endian.h /usr/include/bits/endian.h
../../dialog.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../nowplaying.o: /usr/include/stdio.h /usr/include/features.h
../../nowplaying.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../nowplaying.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../nowplaying.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../nowplaying.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../nowplaying.o: /usr/include/_G_config.h /usr/include/wchar.h
../../nowplaying.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../nowplaying.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../nowplaying.o: /usr/include/bits/stdio_lim.h
../../nowplaying.o: /usr/include/bits/sys_errlist.h /usr/include/pthread.h
../../nowplaying.o: /usr/include/sched.h /usr/include/time.h
../../nowplaying.o: /usr/include/bits/sched.h /usr/include/signal.h
../../nowplaying.o: /usr/include/bits/sigset.h
../../nowplaying.o: /usr/include/bits/pthreadtypes.h
../../nowplaying.o: /usr/include/bits/initspin.h
../../nowplaying.o: /usr/include/bits/sigthread.h /usr/include/sys/types.h
../../nowplaying.o: /usr/include/linux/unistd.h /usr/include/asm/unistd.h
../../nowplaying.o: ../../mp3.h ../../http.h /usr/include/netdb.h
../../nowplaying.o: /usr/include/netinet/in.h /usr/include/stdint.h
../../nowplaying.o: /usr/include/bits/socket.h /usr/include/limits.h
../../nowplaying.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../nowplaying.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../nowplaying.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../nowplaying.o: /usr/include/endian.h /usr/include/bits/endian.h
../../nowplaying.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../nowplaying.o: ../../song.h ../../msgqueue.h ../../nowplaying.h
../../nowplaying.o: ../../fonts.h ../../msgtypes.h ../../video.h
../../nowplaying.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../nowplaying.o: /usr/include/linux/fb.h /usr/include/linux/tty.h
../../nowplaying.o: /usr/include/asm/types.h /usr/include/sys/mman.h
../../nowplaying.o: /usr/include/bits/mman.h
../../song.o: /usr/include/stdio.h /usr/include/features.h
../../song.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../song.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../song.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../song.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../song.o: /usr/include/_G_config.h /usr/include/wchar.h
../../song.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../song.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../song.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../song.o: /usr/include/stdlib.h /usr/include/string.h ../../song.h
../../httpd.o: /usr/include/stdio.h /usr/include/features.h
../../httpd.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../httpd.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../httpd.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../httpd.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../httpd.o: /usr/include/_G_config.h /usr/include/wchar.h
../../httpd.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../httpd.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../httpd.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../httpd.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../httpd.o: /usr/include/bits/confname.h /usr/include/stdlib.h
../../httpd.o: /usr/include/sys/time.h /usr/include/time.h
../../httpd.o: /usr/include/bits/time.h /usr/include/sys/select.h
../../httpd.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../../httpd.o: ../../httpd.h /usr/include/fcntl.h /usr/include/bits/fcntl.h
../../httpd.o: /usr/include/sys/types.h /usr/include/string.h
../../httpd.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
../../httpd.o: ../../version.h ../../nowplaying.h ../../mp3.h ../../http.h
../../httpd.o: /usr/include/netdb.h /usr/include/netinet/in.h
../../httpd.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../httpd.o: /usr/include/limits.h
../../httpd.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../httpd.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../httpd.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../httpd.o: /usr/include/endian.h /usr/include/bits/endian.h
../../httpd.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../httpd.o: ../../song.h ../../msgqueue.h /usr/include/pthread.h
../../httpd.o: /usr/include/sched.h /usr/include/bits/sched.h
../../httpd.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
../../httpd.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../mixer.o: /usr/include/stdio.h /usr/include/features.h
../../mixer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../mixer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../mixer.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
../../mixer.o: /usr/include/bits/typesizes.h /usr/include/libio.h
../../mixer.o: /usr/include/_G_config.h /usr/include/wchar.h
../../mixer.o: /usr/include/bits/wchar.h /usr/include/gconv.h
../../mixer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../mixer.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../mixer.o: /usr/include/stdlib.h /usr/include/string.h
../../mixer.o: /usr/include/sys/types.h /usr/include/time.h
../../mixer.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
../../mixer.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
../../mixer.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h
../../mixer.o: /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h
../../mixer.o: /usr/include/asm/ioctls.h /usr/include/bits/ioctl-types.h
../../mixer.o: /usr/include/sys/ttydefaults.h /usr/include/fcntl.h
../../mixer.o: /usr/include/bits/fcntl.h /usr/include/sys/poll.h
../../mixer.o: /usr/include/bits/poll.h /usr/include/sys/utsname.h
../../mixer.o: /usr/include/bits/utsname.h /usr/include/unistd.h
../../mixer.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
../../mixer.o: ../../mixer.h ../../msgtypes.h ../../msgqueue.h
../../mixer.o: /usr/include/pthread.h /usr/include/sched.h
../../mixer.o: /usr/include/bits/sched.h /usr/include/signal.h
../../mixer.o: /usr/include/bits/sigset.h /usr/include/bits/pthreadtypes.h
../../mixer.o: /usr/include/bits/initspin.h /usr/include/bits/sigthread.h
../../mixer.o: ../../http.h /usr/include/netdb.h /usr/include/netinet/in.h
../../mixer.o: /usr/include/stdint.h /usr/include/bits/socket.h
../../mixer.o: /usr/include/limits.h
../../mixer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/limits.h
../../mixer.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
../../mixer.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
../../mixer.o: /usr/include/endian.h /usr/include/bits/endian.h
../../mixer.o: /usr/include/bits/byteswap.h /usr/include/bits/netdb.h
../../mixer.o: ../../bitmap.h ../../video.h /usr/include/linux/fb.h
../../mixer.o: /usr/include/linux/tty.h /usr/include/asm/types.h
../../mixer.o: /usr/include/sys/mman.h /usr/include/bits/mman.h ../../timer.h
../../timer.o: /usr/include/stdlib.h /usr/include/features.h
../../timer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../../timer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h
../../timer.o: /usr/include/stdio.h /usr/include/bits/types.h
../../timer.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
../../timer.o: /usr/include/libio.h /usr/include/_G_config.h
../../timer.o: /usr/include/wchar.h /usr/include/bits/wchar.h
../../timer.o: /usr/include/gconv.h
../../timer.o: /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h
../../timer.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
../../timer.o: /usr/include/sys/types.h /usr/include/time.h
../../timer.o: /usr/include/sys/time.h /usr/include/bits/time.h
../../timer.o: /usr/include/sys/select.h /usr/include/bits/select.h
../../timer.o: /usr/include/bits/sigset.h /usr/include/memory.h
../../timer.o: /usr/include/string.h /usr/include/signal.h
../../timer.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
../../timer.o: /usr/include/bits/confname.h ../../timer.h
