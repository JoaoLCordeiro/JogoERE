LDFLAGS= `pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 --libs --cflags` 

all: main.o libBomMon.o libDesenha.o libGeral.o libInit.o libTeste.o libDefine.h
	gcc -Wall main.o libBomMon.o libDesenha.o libGeral.o libInit.o libTeste.o libDefine.h -o jogo $(LDFLAGS)

main.o: main.c
	gcc -Wall -c main.c $(LDFLAGS)

libBomMon.o: libBomMon.c libBomMon.h libTeste.c libTeste.h
	gcc -Wall -c libBomMon.c $(LDFLAGS)

libDesenha.o: libDesenha.c libDesenha.h
	gcc -Wall -c libDesenha.c $(LDFLAGS)

libGeral.o: libGeral.c libGeral.h libDesenha.c libDesenha.h libTeste.c libTeste.h
	gcc -Wall -c libGeral.c $(LDFLAGS)

libInit.o: libInit.c libInit.h
	gcc -Wall -c libInit.c $(LDFLAGS)

libTeste.o: libTeste.c libTeste.h libGeral.c libGeral.h
	gcc -Wall -c libTeste.c $(LDFLAGS)

clean:
	-rm -f *~ *.o
