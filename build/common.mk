
MDNSOBJ=mDNS.o mDNSPosix.o mDNSUNP.o
AUTH=md5.o hasher.o
OBJS = commands.o discover.o main.o msgqueue.o screen1.o screen2.o http.o daap.o hexdump.o $(MDNSOBJ) $(AUTH) mp3.o tree.o fonts.o video.o net.o listview.o remote.o screen3.o screen4.o bitmap.o dialog.o nowplaying.o song.o httpd.o mixer.o timer.o
SRCS = ../../main.c ../../http.c ../../screen2.c ../../daap.c ../../hexdump.c ../../commands.c ../../mp3.c ../../tree.c ../../fonts.c ../../net.c ../../listview.c ../../remote.c ../../screen3.c ../../screen4.c ../../discover.c ../../msgqueue.c ../../bitmap.c ../../dialog.c ../../nowplaying.c ../../song.c ../../httpd.c ../../mixer.c ../../timer.c
