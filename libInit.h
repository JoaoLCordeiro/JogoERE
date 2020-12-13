#ifndef __INDEF__
#define __INDEF__

//lib de iniciacao - funcoes que iniciam, resetam ou destroem estruturas

ALLEGRO_BITMAP* carrega_sprite (int x, int y, int w, int h);		//carrega um sprite

void inicializa_geral (bool resultado, const char *testado);		//verifica se a inicializacao foi um sucesso

void inicia_comeco ();							//inicia o que for necessario no comeco do codigo

void inicia_sprites ();							//inicia os sprites

void destroi_sprites ();						//destroi os sprites

void inicia_mapa (int *portal_y, int *portal_x);			//inicia o mapa da fase

void destroi_final ();							//destroi o que for necessario no final do codigo

void inicia_vbomba ();							//inicia o vetor de bombas

void inicia_vmonstros ();						//inicia o vetor de monstros

void malloc_mapa_vbomba_vmonstros();					//faz os mallocs para o mapa, o vetor de bomba e o vetor de monstros

void reseta_vbomba ();							//reseta o vetor de bombas, executado quando passa de fase

#endif
