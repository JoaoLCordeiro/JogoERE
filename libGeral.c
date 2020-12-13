#include <stdio.h>
#include "libDefine.h"
#include "libGeral.h"
#include "libTeste.h"
#include "libDesenha.h"

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *dir, int *bomba, int naBomba, int *pausa, int *ajuda)
{
	int aux_x = *x;				//funcao que interpreta o input do jogador dentro do jogo, movimenta o personagem e mexe algumas flags
	int aux_y = *y;

	if(keyb[ALLEGRO_KEY_UP])
	{
        	*y = *y - 1;
		*dir = 1;
	}
	else if(keyb[ALLEGRO_KEY_DOWN])
	{
        	*y = *y + 1;
		*dir = 2;
	}				
	else if(keyb[ALLEGRO_KEY_LEFT])
	{
        	*x = *x - 1;
		*dir = 3;
	}
	else if(keyb[ALLEGRO_KEY_RIGHT])
	{
        	*x = *x + 1;
		*dir = 4;
	}
	else if(keyb[ALLEGRO_KEY_ESCAPE])
	{
        	*pausa = 1;
	}
	else if(keyb[ALLEGRO_KEY_SPACE])
	{
		*bomba = 1;
	}
	else if(keyb[ALLEGRO_KEY_H])
	{
		*ajuda = 1;
	}
							//testa a colisao do jogador e, se colide, deixa o jogador no mesmo lugar
	if (testa_colisao ( *x, *y, *dir, naBomba))	
	{
		*x = aux_x;
		*y = aux_y;
	}

	int i;
        for( i = 0 ; i < ALLEGRO_KEY_MAX; i++ )
        	keyb[i] &= KEY_VISTA;
}

void limpa_upgrades (int y1, int y2, int x1, int x2)
{					//funcao q limpa o local que o jogador pegou o upgrade
	mapa.m[y1][x1].upgd = 0;
	mapa.m[y2][x1].upgd = 0;
	mapa.m[y1][x2].upgd = 0;
	mapa.m[y2][x2].upgd = 0;
}

void liga_portal (int y, int x)
{					//funcao que verifica se todos os monstros morreram e, se sim, liga o portal
	if (vmonstros.quant == 0)
		mapa.m[y][x].upgd = 4;
}

void faz_pausa (unsigned char *keyb, int *sair, ALLEGRO_EVENT *event)
{
	int j;													
	int contfrm  = 0;
	int i 	     = 0;
	int despausa = 0;
	while (! despausa)
	{
		al_wait_for_event (queue, event);
                                                  
                switch (event->type)
		{
			case ALLEGRO_EVENT_TIMER:
				if (((keyb[ALLEGRO_KEY_UP]) || (keyb[ALLEGRO_KEY_DOWN]))&&(contfrm == 0))
        			{					//parte que interpreta o input e nao deixa o input ser lido muitas vezes
        				i = (i + 1) % 2;
					contfrm = 15;
        			}
				else if (keyb[ALLEGRO_KEY_ENTER])
				{
					despausa = 1;
				}
	
				pre_escrita_display();			//desenha o menu de pausa
	
				al_draw_filled_rectangle (56, 87, 155, 121, al_map_rgb_f(0, 0, 0));
				al_draw_textf(font, al_map_rgb(255, 255, 255), 73, 97 , 0, "CONTINUAR");
				al_draw_textf(font, al_map_rgb(255, 255, 255), 73, 105, 0, "SAIR");
	
				al_draw_textf(font, al_map_rgb(255, 255, 255), 66, 97+7*i, 0,">");
	
				pos_escrita_display();

				if (contfrm != 0)
					contfrm--;

				for( j = 0 ; j < ALLEGRO_KEY_MAX; j++ )
	                		keyb[j] &= KEY_VISTA;
				break;
	
			case ALLEGRO_EVENT_KEY_DOWN:					
                		keyb[event->keyboard.keycode] = KEY_VISTA | KEY_SOLTA;	
                		break;
                                                                                
           	     	case ALLEGRO_EVENT_KEY_UP:
                		keyb[event->keyboard.keycode] &= KEY_SOLTA;
                		break;
		}
	}

	if (i)
		*sair = 1;
}

void faz_menu (int *fechar, unsigned char *keyb, ALLEGRO_EVENT *event, int *code)
{
	int j;														
        int contfrm  = 0;
        int i 	     = 0;
        int laco     = 0;
        while (! laco)
        {
        	al_wait_for_event (queue, event);
                                                  
                switch (event->type)
        	{
        		case ALLEGRO_EVENT_TIMER:
        			if (((keyb[ALLEGRO_KEY_UP]) || (keyb[ALLEGRO_KEY_DOWN]))&&(contfrm == 0))
        			{					//parte que interpreta o input e nao repete ele varias vezes
        				i = (i + 1) % 2;
        				contfrm = 15;
        			}
        			else if (keyb[ALLEGRO_KEY_ENTER])
        			{
        				laco = 1;
        			}
				else if (keyb[ALLEGRO_KEY_F])
				{
					*code = 1;
				}
        
        			pre_escrita_display();			//desenha o menu
       
			       	al_draw_bitmap(menu, 0, 0, 0);
        			al_draw_textf(font, al_map_rgb(255, 255, 255), 113, 97 , 0, "INICIAR");
        			al_draw_textf(font, al_map_rgb(255, 255, 255), 113, 105, 0, "SAIR");
        
        			al_draw_textf(font, al_map_rgb(255, 255, 255), 106, 97+8*i, 0,">");
        
        			pos_escrita_display();
                                                                                                                
        			if (contfrm != 0)
        				contfrm--;
                                                                                                                
        			for( j = 0 ; j < ALLEGRO_KEY_MAX; j++ )
                        		keyb[j] &= KEY_VISTA;
        			break;
        
        		case ALLEGRO_EVENT_KEY_DOWN:					
                		keyb[event->keyboard.keycode] = KEY_VISTA | KEY_SOLTA;	
                		break;
                                                                                
           	     	case ALLEGRO_EVENT_KEY_UP:
                		keyb[event->keyboard.keycode] &= KEY_SOLTA;
                		break;
        	}
        }

	if (i == 1)
		*fechar = 1;
}

void faz_ajuda (unsigned char *keyb, ALLEGRO_EVENT *event)
{
	int j;														
        int contfrm  = 0;
        int sair     = 0;
        while (! sair)
        {
        	al_wait_for_event (queue, event);
                                                  
                switch (event->type)
        	{
        		case ALLEGRO_EVENT_TIMER:
        			if ((keyb[ALLEGRO_KEY_ENTER]) && (contfrm == 0))
        			{
        				sair = 1;
					contfrm = 30;
        			}
        
        			pre_escrita_display();			//desenha o menu de ajuda
                                                                                                                        
        		       	al_draw_filled_rectangle(0, 73, 208, 130, al_map_rgb_f(0, 0, 0));
        			al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 84 , 0, "PARA ANDAR, USE AS SETAS");
        			al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 92 , 0, "PARA POR BOMBA, ESPACO");
				al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 100, 0, "ESC PAUSA, ETR SAI D HELP");
				al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 108, 0, "MATE TODOS OS INIMIGOS");
				al_draw_textf(font, al_map_rgb(255, 255, 255), 5, 116, 0, "ENCONTRE O PORTAL");
        
        			pos_escrita_display();
                                                                                                                
        			if (contfrm != 0)
        				contfrm--;
                                                                                                                
        			for( j = 0 ; j < ALLEGRO_KEY_MAX; j++ )
                        		keyb[j] &= KEY_VISTA;
        			break;
        
        		case ALLEGRO_EVENT_KEY_DOWN:					
                		keyb[event->keyboard.keycode] = KEY_VISTA | KEY_SOLTA;	
                		break;
                                                                                
           	     	case ALLEGRO_EVENT_KEY_UP:
                		keyb[event->keyboard.keycode] &= KEY_SOLTA;
                		break;
        	}
        }
}

void inicia_score (FILE *score)
{
	int i;
	for (i=0 ; i<10 ; i++)
		fprintf (score, "0\n");
}

void pega_score (FILE *file, int *vetor)
{
	int i;
	rewind (file);

	for (i=0 ; i<10 ; i++)
		fscanf (file, "%d\n", &vetor[i]);
}

void adiciona_score (FILE *file, int *vetor, int pontos)
{
	int i,aux;
	vetor[9] = pontos;
	for (i=8 ; i >= 0 ; i--)
	{
		if (vetor[i] < vetor[i+1])
		{
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}

	for (i=0 ; i<10 ; i++)
	{
		fprintf (file, "%d\n", vetor[i]);
	}
}
