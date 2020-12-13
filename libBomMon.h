#ifndef __BMDEF__
#define __BMDEF__

//lib de bomba e monstros - funcoes focadas nas bombas e nos monstros

int  tem_monstro (int i, int y, int x);

void coloca_bomba ( int x, int y, int *bomba);

void passa_tempo_bombas ();

void passa_tempo_fogos  ();

void cria_explosao (int y, int x, int tamf, int dir);

void inicia_explosoes (int y, int x, int tamf);

void explode_bombas ();

void cria_monstros (int fase);

void mata_monstros ();

void move_monstro (int i, int tipo);

void move_monstros ();

#endif
