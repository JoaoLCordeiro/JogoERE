#ifndef __GEDEF__
#define __GEDEF__

//lib geral - funcoes gerais para o jogo											

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *dir, int *bomba, int naBomba, int *pausa, int *ajuda);

void limpa_upgrades (int y1, int y2, int x1, int x2);

void liga_portal (int y, int x);

void faz_pausa (unsigned char *keyb, int *sair, ALLEGRO_EVENT *event);

void faz_menu (int *fechar, unsigned char *keyb, ALLEGRO_EVENT *event, int *code);

void faz_ajuda (unsigned char *keyb, ALLEGRO_EVENT *event);

void inicia_score (FILE *score);

#endif
