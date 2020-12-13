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
	inicia_comeco ();
	inicia_sprites ();

	al_register_event_source (queue, al_get_keyboard_event_source ());
	al_register_event_source (queue, al_get_display_event_source (display));
	al_register_event_source (queue, al_get_timer_event_source (timer));

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
	int pausa     = 0;
	int ajuda     = 0;
	int sair      = 0;
	int code      = 0;

	int portal_x  = 0;		//variaveis que guardam a coordenada do portal
	int portal_y  = 0;

	int porBomba  = 0;		//flag para por bomba
	int naBomba   = 0;		//flag que detecta se o player ainda está no quadrado da boma após colocá-la

	srand(time(0));

	unsigned char keyb[ALLEGRO_KEY_MAX];
	memset (keyb, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));

	FILE* score;
	score = fopen ("./resources/score.txt","r");

	if (! score)
	{
		score = freopen ("./resources/score.txt","w+");
		inicia_score (score);
	}

	/*--------------------------------------------------------------------------------------------------------------*/

	/*Loop do jogo--------------------------------------------------------------------------------------------------*/

	malloc_mapa_vbomba_vmonstros();
	inicia_vbomba ();
	inicia_vmonstros ();
	al_start_timer (timer);
	while (! fechar)	//loop do jogo inteiro com menu e tudo
	{
		pontos = 0;

		faz_menu (&fechar, keyb, &event, &code);

		if (code)
		{
			vbomba.max  = 5 ;
			vbomba.tamf = 10;
		}

		while ((! morte) && (! sair))		//loop do jogo com as fases
		{
			inicia_mapa (&portal_y, &portal_x);
			reseta_vbomba ();
			cria_monstros (fase);

			while (contfrmj < 180)
			{
				pre_escrita_display();
				
				al_draw_filled_rectangle (0, 0, 208, 208, al_map_rgb_f(0, 0, 0));		
				al_draw_textf(font, al_map_rgb(255, 255, 255), 83, 100, 0, "FASE %d", fase);
				contfrmj++;

				pos_escrita_display();
			}
			contfrmj = 0;

			while ((!morte) && (! sair) && (! passou))
			{
				al_wait_for_event (queue, &event);

				switch (event.type)
				{
					case ALLEGRO_EVENT_TIMER:
			
						pre_x = jogador_x;
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

						contfrmj++;
						contfrmm++;
						if (contfrmm ==30)
							contfrmm = 0;

						anima_jogador (&contfrmj, &frame, pre_x, pre_y, jogador_x, jogador_y);
					
						if (porBomba)
							coloca_bomba (jogador_x, jogador_y, &porBomba);

						verifica_na_bomba (&porBomba, &naBomba, jogador_y, jogador_x);
						passa_tempo_bombas ();
						passa_tempo_fogos  ();
						explode_bombas();

						move_monstros();
						mata_monstros();

						if (imune == 0)
							testa_morte (jogador_y, jogador_x, &morte, &vidas, &imune);
						else
							imune--;

						testa_upgrades (jogador_y, jogador_x);
						liga_portal    (portal_y , portal_x);

						if (mapa.m[portal_y][portal_x].upgd == 4)
							passou = verifica_passou (jogador_y, jogador_x, portal_y, portal_x);
                                                                
						desenha = 1;
						break;
	
					case ALLEGRO_EVENT_KEY_DOWN:					
	        				keyb[event.keyboard.keycode] = KEY_VISTA | KEY_SOLTA;
        					break;

		   	 		case ALLEGRO_EVENT_KEY_UP:
        					keyb[event.keyboard.keycode] &= KEY_SOLTA;
	        				break;
				}	

				if ((desenha) && (al_is_event_queue_empty (queue)))
				{
					pre_escrita_display ();
                        
					al_clear_to_color (al_map_rgb(0, 0, 0));
					desenha_hud(vidas);
					desenha_mapa();
					al_draw_textf(font, al_map_rgb(255, 255, 255), 80, 196, 0, "%d", pontos);
					desenha_monstros(contfrmm);
					desenha_jogador(imune, jogador_y, jogador_x, frame, dir);
	
                	        	pos_escrita_display ();
		
					desenha = 0;
				}
			}

			if (morte)
			{
				contfrmj = 0;
				while (contfrmj != 180)
				{
					pre_escrita_display();

					al_draw_filled_rectangle (0, 0, 208, 208, al_map_rgb_f(0, 0, 0));
					al_draw_textf (font, al_map_rgb(255, 255, 255), 40, 50, 0, "VOCE FOI DESTRUIDO");
					if (contfrmj < 120)
						al_draw_bitmap (sprites.jogador[ 7 +  contfrmj/30 ], 96, 96, 0);
					else
						al_draw_bitmap (sprites.jogador[11], 96, 96, 0);

					pos_escrita_display();
					contfrmj++;
				}
			}

			if (passou)
			{
				pontos += 1000;
				fase++;
				vidas++;
			}

			if (fase > 5)
			{
				contfrmj = 60;
				while (contfrmj != 0)
				{
					pre_escrita_display();

					al_draw_filled_rectangle (0, 0, 208, 208, al_map_rgb_f(0, 0, 0));
					if (code)
						al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 100, 0, "TRAPACEADOR NAO VENCE");
					else
                                        	al_draw_textf(font, al_map_rgb(255, 255, 255), 30, 100, 0, "OS REBELDES VENCERAM!");

					pos_escrita_display();
					contfrmj--;
				}
				sair = 1;
			}

			passou = 0;	
			jogador_x = 16;
			jogador_y = 16;
		}
		morte = 0;
		fase = 0;
	}

	destroi_sprites();
	destroi_final();

	return 0;
}
