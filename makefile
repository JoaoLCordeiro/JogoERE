teste: 
	gcc -Wall teste.c -o teste $(pkg-config allegro-5 allegro_primitives-5 allegro_font-5 --libs --cflags)
