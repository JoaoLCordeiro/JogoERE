#ifndef __TEDEF__
#define __TEDEF__

//lib de teste - funcoes que testam ou verificam situacoes do jogo			

int  testa_mapa (int y, int x, int naBomba);

int  testa_colisao (int x1, int y1, int dir, int naBomba);

int  testa_queimadura (int y, int x);

int  testa_toque (int y1, int y2, int x1, int x2);

int  testa_colisao_upgd (int y, int x, int upgd);

int  verifica_passou (int y1, int x1, int p_y, int p_x);

int  testa_fora_mapa (int y1, int y2, int x1, int x2);

int  testa_colisao_monstro (int y1, int y2, int x1, int x2, int dir, int tipo);

int  esta_na_bomba (int jogador_y, int jogador_x);

void testa_morte (int y1, int x1, int *morte, int *vidas, int *imune);

void testa_upgrades (int y1, int x1);

void verifica_na_bomba (int *porBomba, int *naBomba, int jogador_y, int jogador_x);

#endif
