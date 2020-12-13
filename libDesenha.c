#include "libDesenha.h"
#include "libDefine.h"

void pre_escrita_display ()
{
	al_set_target_bitmap (buffer);					//faz desenhar no buffer
}

void pos_escrita_display ()
{
	al_set_target_backbuffer (display);								//faz desenhar na tela de novo
	al_draw_scaled_bitmap (buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISPLAY_W, DISPLAY_H, 0);	//escala o buffer para encaixar na tela

	al_flip_display();										//desenha na tela
}

void desenha_mapa ()
{
	int i,j;
	for (i=0 ; i<11 ; i++)			//passa por toda a matriz
		for (j=0 ; j<11 ; j++)
		{
			if ((mapa.m[i][j].fogo == 0) || (mapa.m[i][j].fogo > 40))	//se nao for explosao
			{
				if ((mapa.m[i][j].tipo != LIMPO) || (mapa.m[i][j].upgd == 0))		//printa muros e pallets
					al_draw_bitmap (sprites.mapa[ mapa.m[i][j].tipo ], 16 + j*16, 16 + i*16, 0);
				else
				{
					if ((mapa.m[i][j].upgd == 1) || (mapa.m[i][j].upgd == 2))	//printa os upgds
						al_draw_bitmap (sprites.mapa[ 9 + mapa.m[i][j].upgd ], 16 + j*16, 16 + i*16, 0);
					else								//printa os portais
						al_draw_bitmap (sprites.mapa[ 1 + mapa.m[i][j].upgd ], 16 + j*16, 16 + i*16, 0);
				}
			}
			else								//se for explosao
			{
				if ((mapa.m[i][j].fogo > 10)&&(mapa.m[i][j].fogo < 31))			//printa o fogo menor
					al_draw_bitmap (sprites.mapa[ 8 ], 16 + j*16, 16 + i*16, 0);
				else if (mapa.m[i][j].fogo < 41)					//printa o fogo maior
					al_draw_bitmap (sprites.mapa[ 9 ], 16 + j*16, 16 + i*16, 0);
			}
		}
}

void anima_jogador (int *contframe, int *frame, int x1, int y1, int x2, int y2)
{				//funcao que troca a variacao de frames que sera usada ao desenhar o jogador
	if (*contframe > 30)			//se o tanto de frames precisos pra rolar a animacao ja passou
        {
		if ((x1 != x2) || (y1 != y2))	//se o jogador andou
		{
        		*contframe = 0;
        		if (*frame)
        			*frame = 0;
        		else
        			*frame = 1;
		}
        }

}

void desenha_monstros(int contframe)
{
	int i;					//desenha os sprites dos monstros
	for (i=0 ; i < vmonstros.quant ; i++)
	{
		if (contframe < 15)		//usa a variavel contfrmm da main pra alternar entre os sprites dos monstros
			al_draw_bitmap (sprites.monstros[ 2*( vmonstros.v[i].tipo - 1 )    ], vmonstros.v[i].x, vmonstros.v[i].y, 0);
		else
			al_draw_bitmap (sprites.monstros[ 2*( vmonstros.v[i].tipo - 1 ) + 1], vmonstros.v[i].x, vmonstros.v[i].y, 0);
	}
}

void desenha_jogador (int imune, int jogador_y, int jogador_x, int frame, int dir)
{						//essa funcao desenha o personagem mais transparente se tiver imune e opaco se nao tiver
	if (imune == 0)
        	al_draw_bitmap (sprites.jogador[ dir-1 + 4*frame ], jogador_x, jogador_y, 0);
        else
        	al_draw_tinted_bitmap (sprites.jogador[ dir-1 + 4*frame], al_map_rgba_f (0.7, 0.7, 0.7, 0.5), jogador_x, jogador_y, 0);
}

void desenha_hud (int vidas)
{
	al_draw_bitmap (hud, 0, 0, 0);					//desenha o hud maior
	int i;
	for (i=0 ; i<vidas ; i++)					//for que desenha as vidas disponiveis
	{
		al_draw_bitmap (sprites.elHud[0], 67+16*i, 0, 0);
	}
}
