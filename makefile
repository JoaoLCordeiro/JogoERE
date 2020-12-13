LDFLAGS= `pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs --cflags` 

all: main.o libBomMon.o libDesenha.o libGeral.o libInit.o libTeste.o libDefine.h
	gcc -Wall teste.c -o teste $(pkg-config allegro-5 allegro_primitives-5 allegro_font-5 --libs --cflags)

main.o: main.c
	gcc -Wall -c main.c $(LDFLAGS)

libBomMon.o: 
	gcc -Wall -c libBomMon.c $(LDFLAGS)

libDesenha.o:
	gcc -Wall -c libDesenha.c $(LDFLAGS)

libGeral.o:
	gcc -Wall -c libGeral.c $(LDFLAGS)

libInit.o:
	gcc -Wall -c libInit.c $(LDFLAGS)

libTeste.o:
	gcc -Wall -c libTeste.c $(LDFLAGS)

