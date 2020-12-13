#ifndef __DEDEF__
#define __DEDEF__

//lib de desenho - funcoes que focam em desenho de sprites e menus			

void pre_escrita_display ();								//funcao retirada do tutorial de allegro para usar antes de
											//escrever no display

void pos_escrita_display ();								//funcao retirada do tutorial de allegro para usar depois de
											//escrever no display
											
void desenha_mapa ();									//desenha o mapa passando pela matriz mapa

void anima_jogador (int *contframe, int *frame, int x1, int y1, int x2, int y2);	//muda a variacao de frame que sera desenhada do jogador

void desenha_monstros(int contframe);							//desenha os monstros no display

void desenha_jogador (int imune, int jogador_y, int jogador_x, int frame, int dir);	//desenha o jogador no display

void desenha_hud (int vidas);								//desenha o hud no display

#endif
