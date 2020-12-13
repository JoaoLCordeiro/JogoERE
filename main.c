#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>			
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

#include "libDefine.h"

#include "libTeste.h"
#include "libGeral.h"
#include "libBomMon.h"
#include "libDesenha.h"
#include "libInit.h"

int main ()
{
	/*inicia o que for necessario-----------------------------------------------------------------------------------*/

	inicia_comeco ();
	inicia_sprites ();

	al_register_event_source (queue, al_get_keyboard_event_source ());
	al_register_event_source (queue, al_get_display_event_source (display));
	al_register_event_source (queue, al_get_timer_event_source (timer));

	/*--------------------------------------------------------------------------------------------------------------*/

	/*parte das variáveis-------------------------------------------------------------------------------------------*/

	int fechar  = 0;
	int desenha = 1;

	ALLEGRO_EVENT event;

	int jogador_x = 16;		//variaveis que guardam a coordenada atual do jogador
	int jogador_y = 16;
	int pre_x     = 16;		//variaveis que guardam a coordenada do frame anterior do jogador
	int pre_y     = 16;
	int vidas     = 3;		//guarda quantas vidas o jogador tem
        int imune     = 0;		//controla a imunidade do jogador após receber um dano

	int dir       = 2;		//guarda a direção para onde o player ta se mexendo
	int frame     = 0;		//guarda qual variação do frame de movimento do jogador deve ser exibido
	int contfrmj  = 0;		//conta os frames do jogador para trocar a variação de frame que será exibido
	int contfrmm  = 0;		//conta os frames dos monstros para trocar a variação de frame que será exibido
	int morte     = 0;		//guarda se o jogador morreu
	int passou    = 0;		//guarda se o jogador passou de fase
	int fase      = 1;		//guarda a fase em que o jogador esta
	int pausa     = 0;		//flag para usar a pausa
	int ajuda     = 0;		//flag para usar o menu de ajuda
	int sair      = 0;		//flag para sair do jogo
	int code      = 0;		//flag para habilitar o codigo de bombas no maximo

	int portal_x  = 0;		//variaveis que guardam a coordenada do portal
	int portal_y  = 0;

	int porBomba  = 0;		//flag para por bomba
	int naBomba   = 0;		//flag que detecta se o player ainda está no quadrado da boma após colocá-la

	srand(time(0));

	unsigned char keyb[ALLEGRO_KEY_MAX];
	memset (keyb, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));

	int vscore[10];							//vetor de inteiros que guarda o score
	FILE* fscore;							//file onde os maiores scores ficam guardados
	fscore = fopen ("./resources/score.txt","r");

	if (! fscore)							//caso nao exista o arquivo que guarda os maiores scores
	{
		fscore = fopen ("./resources/score.txt","w+");		//cria o arquivo
		if (! fscore)
		{
			perror ("erro ao iniciar o arquivo de score");
			exit (1);
		}
		inicia_score (fscore);
	}

	pega_score (fscore,vscore);

	/*--------------------------------------------------------------------------------------------------------------*/

	/*Loop do jogo com menus e fases--------------------------------------------------------------------------------*/

	malloc_mapa_vbomba_vmonstros();		//da mallocs e inicia dois vetores: de bomba e de monstros
	inicia_vbomba ();
	inicia_vmonstros ();
	al_start_timer (timer);

	while (! fechar)					//loop do jogo inteiro: menu, fases e jogo em si
	{
		pontos = 0;

		faz_menu (&fechar, keyb, &event, &code);	//faz o menu principal	

		if (code)					//habilita o codigo
		{
			vbomba.max  = 5 ;
			vbomba.tamf = 10;
		}

		while ((! morte) && (! sair) && (! fechar))		//loop do jogo com as fases e o jogo em si
		{
			inicia_mapa (&portal_y, &portal_x);		//inicia o mapa da fase
			reseta_vbomba ();				//resta o vetor de bombas
			cria_monstros (fase);				//cria os monstros da fase

			while (contfrmj < 180)				//mostra FASE NUM, onde NUM eh o numero da fase
			{
				pre_escrita_display();
				
				al_draw_filled_rectangle (0, 0, 208, 208, al_map_rgb_f(0, 0, 0));		
				al_draw_textf(font, al_map_rgb(255, 255, 255), 83, 100, 0, "FASE %d", fase);

				pos_escrita_display();

				contfrmj++;
			}
			contfrmj = 0;

			while ((!morte) && (! sair) && (! passou))		//loop com o jogo em si, uma fase
			{
				al_wait_for_event (queue, &event);		//parte retirada do codigo do tutorial em allegro

				switch (event.type)
				{
					case ALLEGRO_EVENT_TIMER:
			
						pre_x = jogador_x;		//salva o lugar anterior que o jogador estava
						pre_y = jogador_y;
	
						interpreta_controle (keyb, &jogador_x, &jogador_y, &dir, &porBomba, naBomba, &pausa, &ajuda);

						if (pausa)
						{
							faz_pausa (keyb, &sair, &event);
							pausa = 0;
						}

						if (ajuda)
						{
							faz_ajuda (keyb, &event);
							ajuda = 0;
						}

						contfrmj++;			//aumenta o contador de frames pra monstros e pro jogador
						contfrmm++;
						if (contfrmm ==30)
							contfrmm = 0;

										//troca a variacao de sprites jogador caso ele se mexa
						anima_jogador (&contfrmj, &frame, pre_x, pre_y, jogador_x, jogador_y);
					
						if (porBomba)			//poe uma bomba caso precise e possa por
							coloca_bomba (jogador_x, jogador_y, &porBomba);

						verifica_na_bomba (&porBomba, &naBomba, jogador_y, jogador_x);
						passa_tempo_bombas ();		//passa o tempo das bombas e dos fogos das explosoes
						passa_tempo_fogos  ();
						explode_bombas();		//explode as bombas carregadas

						move_monstros();		//move e mata os monstros que devem morrer
						mata_monstros();

						if (imune == 0)			//testa se o jogador esta morto
							testa_morte (jogador_y, jogador_x, &morte, &vidas, &imune);
						else
							imune--;
				
										//verifica a situacao dos upgrades e liga o portal caso necessario
						testa_upgrades (jogador_y, jogador_x);
						liga_portal    (portal_y , portal_x);

										//caso o portal esteja ligado, verifica se o jogador
										//passou por ele
						if (mapa.m[portal_y][portal_x].upgd == 4)
							passou = verifica_passou (jogador_y, jogador_x, portal_y, portal_x);
                                                                
						desenha = 1;
						break;
	
					case ALLEGRO_EVENT_KEY_DOWN:		//parte pega do tutorial de jogo em allegro	
	        				keyb[event.keyboard.keycode] = KEY_VISTA | KEY_SOLTA;
        					break;

		   	 		case ALLEGRO_EVENT_KEY_UP:
        					keyb[event.keyboard.keycode] &= KEY_SOLTA;
	        				break;
				}	

				if ((desenha) && (al_is_event_queue_empty (queue)))	//desenha o jogo em si
				{
					pre_escrita_display ();
                        
					al_clear_to_color (al_map_rgb(0, 0, 0));					//pinta tudo de preto
					desenha_hud(vidas);								//desenha o hud
					desenha_mapa();									//desenha o mapa
					al_draw_textf(font, al_map_rgb(255, 255, 255), 80, 196, 0, "%d", pontos);	//desenha os pontos
					desenha_monstros(contfrmm);							//desenha os monstros
					desenha_jogador(imune, jogador_y, jogador_x, frame, dir);			//desenha o jogador
	
                	        	pos_escrita_display ();
		
					desenha = 0;
				}
			}
			//fim do loop do jogo em si, uma fase


			if (morte)					//caso o jogador tenha morrido
			{
				contfrmj = 0;				//desenha a tela de morte
				while (contfrmj != 180)
				{
					pre_escrita_display();

					al_clear_to_color (al_map_rgb(0, 0, 0));				
					al_draw_textf (font, al_map_rgb(255, 255, 255), 40, 50, 0, "VOCE FOI DESTRUIDO");
					if (contfrmj < 120)
						al_draw_bitmap (sprites.jogador[ 8 +  contfrmj/30 ], 96, 96, 0);
					else
						al_draw_bitmap (sprites.jogador[11], 96, 96, 0);

					pos_escrita_display();
					contfrmj++;
				}
			}

			if (passou)					//se o jogador passou de de fase
			{
				pontos += 1000;
				fase++;
				vidas++;
			}

			if (fase > 5)					//se o jogador passou por todas as fases
			{
				contfrmj = 60;
				while (contfrmj != 0)
				{
					pre_escrita_display();

					al_clear_to_color (al_map_rgb(0, 0, 0));
					if (code)
						al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 50, 0, "TRAPACEADOR NAO VENCE");
					else
                                        	al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 50, 0, "OS REBELDES VENCERAM!");

					pos_escrita_display();
					contfrmj--;
				}
				sair = 1;
			}

			passou = 0;	
			jogador_x = 16;
			jogador_y = 16;
		}
		//fim do loop com o jogo dentro e todas as fases

		if ((pontos > vscore[9]) && ((fase > 5) || (morte))) 	//caso o jogador morra ou passe por tudo e sua pontuacao for grande
		{							//o suficiente, escreve ela no arquivo de scores
			fscore = freopen ("./resources/score.txt","w+", fscore);
			adiciona_score (fscore, vscore, pontos);

			contfrmj = 300;
			while (contfrmj != 0)				//desenha a tela de scores
			{
				pre_escrita_display ();

				al_draw_filled_rectangle (0, 0, 208, 208, al_map_rgb_f(0, 0, 0));
				al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 50, 0, "SEU SCORE:");
				al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 58, 0, "%d", pontos);	
				al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 66, 0, "PONTOS MAIS ALTOS:");

				int i;												
                       	 	for (i=0 ; i<10 ; i++)
                        		al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 74+8*i, 0, "%d", vscore[i]);
	
				pos_escrita_display ();
				contfrmj--;
			}
		}

		morte = 0;
		fase = 0;
	}
	//fim do loop do jogo inteiro, com menus, fases e o jogo em si

					//destroi o que for necessario no final
	destroi_sprites();
	destroi_final();

	return 0;
}
