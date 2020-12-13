#ifndef __TEDEF__
#define __TEDEF__

//lib de teste - funcoes que testam ou verificam situacoes do jogo			

int  testa_mapa (int y, int x, int naBomba);						//testa se o jogador pode entrar no local do mapa

int  testa_colisao (int x1, int y1, int dir, int naBomba);				//testa colisao do jogador, chamando a tessta_mapa

int  testa_queimadura (int y, int x);							//testa se algo encostou no fogo da explosao

int  testa_toque (int y1, int y2, int x1, int x2);					//testa se o jogador e algum monstro colidiram

int  testa_colisao_upgd (int y, int x, int upgd);					//testa se o jogador colidiu com um upgrade, usado tambem
											//na colisao com o portal

int  verifica_passou (int y1, int x1, int p_y, int p_x);				//verifica se o jogador encostou no portal para passar de fase

int  testa_fora_mapa (int y1, int y2, int x1, int x2);					//testa se as coordenadas estao fora do mapa

int  testa_colisao_monstro (int y1, int y2, int x1, int x2, int dir, int tipo);		//testa colisao dos monstros

int  esta_na_bomba (int jogador_y, int jogador_x);					//testa se o jogador ainda esta em cima da bomba

void testa_morte (int y1, int x1, int *morte, int *vidas, int *imune);			//testa se o jogador morreu

void testa_upgrades (int y1, int x1);							//testa o jogador pegou um upgrade e, se sim, aumenta
											//seus status

void verifica_na_bomba (int *porBomba, int *naBomba, int jogador_y, int jogador_x);	//altera algumas flags caso o jogador esteja na bomba que
											//ele acabou de por

#endif
