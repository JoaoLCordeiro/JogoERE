#ifndef __DEDEF__
#define __DEDEF__

//lib de desenho - funcoes que focam em desenho de sprites e menus			

void pre_escrita_display ();

void pos_escrita_display ();

void desenha_mapa ();

void anima_jogador (int *contframe, int *frame, int x1, int y1, int x2, int y2);

void desenha_monstros(int contframe);

void desenha_jogador (int imune, int jogador_y, int jogador_x, int frame, int dir);

void desenha_hud (int vidas);

#endif
