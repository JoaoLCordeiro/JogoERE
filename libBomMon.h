#ifndef __BMDEF__
#define __BMDEF__

//lib de bomba e monstros - funcoes focadas nas bombas e nos monstros

int  tem_monstro (int i, int y, int x);			//testa se ja tem monstro no lugar

void coloca_bomba ( int x, int y, int *bomba);		//coloca uma bomba no lugar

void passa_tempo_bombas ();				//passa o tempo de todas as bombas

void passa_tempo_fogos  ();				//passa o tempo dos fogos das explosoes

void cria_explosao (int y, int x, int tamf, int dir);	//cria explosoes recursivamente

void inicia_explosoes (int y, int x, int tamf);		//inicia uma corrente de explosoes

void explode_bombas ();					//passa pelas bombas e e chama a inicia_explosoes pras que estao carregadas

void cria_monstros (int fase);				//cria os monstros dependendo da fase

void mata_monstros ();					//mata os monstros que devem ser mortos

void move_monstro (int i, int tipo);			//move um monstro dependendo do seu tipo

void move_monstros ();					//chama a move_monstro para todos os monstros

#endif
