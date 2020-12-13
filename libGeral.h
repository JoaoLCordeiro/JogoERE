#ifndef __GEDEF__
#define __GEDEF__

//lib geral - funcoes gerais para o jogo											

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *dir, int *bomba, int naBomba, int *pausa, int *ajuda);
								//funcao que interpreta o controle do usurario dentro do jogo em si e altera flags

void limpa_upgrades (int y1, int y2, int x1, int x2);					//limpa o local que o jogador pegou upgrade

void liga_portal (int y, int x);							//verifica se os monstros morreram e, se sim, liga o portal

void faz_pausa (unsigned char *keyb, int *sair, ALLEGRO_EVENT *event);			//menu de pausa

void faz_menu (int *fechar, unsigned char *keyb, ALLEGRO_EVENT *event, int *code);	//faz o menu principal

void faz_ajuda (unsigned char *keyb, ALLEGRO_EVENT *event);				//faz o menu de ajuda

void inicia_score (FILE *score);							//inicia a file com os scores

void pega_score (FILE *file, int *vetor);						//coleta os scores da file score pra um vetor de inteiros

void adiciona_score (FILE *file, int *vetor, int pontos);				//adiciona o score novo no arquivo de scores

#endif
