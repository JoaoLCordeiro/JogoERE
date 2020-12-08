#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

#define BUFFER_H 208				//Tamanho dos pixels * blocos de altura
#define BUFFER_W 208				//Tamanho dos pixels * blocos de largura

#define DISPLAY_S 4				//Escala que o buffer aumenta para o display
#define	DISPLAY_H (BUFFER_H * DISPLAY_S)	//Tamanho da altura do display
#define	DISPLAY_W (BUFFER_W * DISPLAY_S)	//Tamanho da largura do display

#define KEY_VISTA 1				//defines para a interpretação do input	
#define KEY_SOLTA 2

#define LIMPO  0				//defines pros objetos do mapa
#define MURO   1
#define PALLET 2
#define BOMBA1 6
#define BOMBA2 7

#define TBOMBA 200

typedef struct t_sprites
{
	ALLEGRO_BITMAP* sheet;

	ALLEGRO_BITMAP* jogador[12];
	ALLEGRO_BITMAP* mapa[12];
	ALLEGRO_BITMAP* inimigos[6];
} t_sprites;

typedef struct t_quadrado
{
	int tipo;
	int fogo;
	int upgd;
} t_quadrado;

typedef struct t_mapa
{
	t_quadrado** m;				//matriz de quadrados
} t_mapa;

typedef struct t_monstro		
{
	int tipo;
	int x;
	int y;
} t_monstro;

typedef struct t_vmonstros
{
	int quant;
	t_monstro *v;
} t_vmonstros;

typedef struct t_bomba
{
       	int tempo;
       	int x;
       	int y;
} t_bomba;

typedef struct t_vbomba
{
	int tamf;
	int max;
	int quant;
	t_bomba *v;
} t_vbomba;

//Variáveis globais

ALLEGRO_DISPLAY* 	display;
ALLEGRO_BITMAP*  	buffer;
ALLEGRO_TIMER*   	timer;
ALLEGRO_EVENT_QUEUE*	queue;
ALLEGRO_FONT*		font;

t_sprites 		sprites;
t_mapa 	  		mapa;
t_vmonstros		vmonstros;
t_vbomba		vbomba;

void inicializa_geral (bool resultado, const char *testado)
{
	if (resultado)
		return;
	else
	{
		printf ("Erro ao inicializar %s\n", testado);	//!!!!!!! mandar na saida de erro
		exit (1);
	}
}

void inicia_comeco ()//!!!!! pensar num nome melhor
{
	//!!!!!linhas que suavizam o grafico, testar
	//al_set_new_display_option (ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	//al_set_new_display_option (ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

	inicializa_geral (al_init(), "allegro");			//inicializa o allegro em si e o teclado
	inicializa_geral (al_install_keyboard(), "keyboard");

	display = al_create_display 	(DISPLAY_W, DISPLAY_H);		//cria o display
	inicializa_geral (display, "display");

	buffer  = al_create_bitmap  	(BUFFER_W, BUFFER_H);		//cria o buffer
	inicializa_geral (buffer , "buffer" );

	timer   = al_create_timer   	(1.0/60.0);			//cria o timer
	inicializa_geral (timer  , "timer"  );

	queue   = al_create_event_queue ();				//cria a queue de eventos
	inicializa_geral (queue  , "queue"  );

	font    = al_create_builtin_font ();
	inicializa_geral (font   , "font"   );

	inicializa_geral (al_init_primitives_addon(), "primitives");	//inicializa o primitives
	inicializa_geral (al_init_image_addon(), "image addon");
}

ALLEGRO_BITMAP* carrega_sprite (int x, int y, int w, int h)
{
	ALLEGRO_BITMAP* sprite = al_create_sub_bitmap (sprites.sheet, x, y, w, h);
	inicializa_geral (sprite, "carrega sprite");
	return sprite;
}

void inicia_sprites ()
{
	sprites.sheet = al_load_bitmap ("./sprites/spritesheet.png");
	inicializa_geral (sprites.sheet, "spritesheet");

	int i,j;											
	for (i=0 ; i<3; i++)										//pega os sprites do jogador
		for (j=0 ; j<4 ; j++)
			sprites.jogador[i*4 + j] = carrega_sprite(j*16, i*16, 16, 16);

	for (i=0 ; i<3 ; i++)										//pega os sprites do mapa
		for (j=0 ; j<4 ; j++)
				sprites.mapa[i*4 + j] = carrega_sprite(j*16, 64+i*16, 16, 16);

	for (i=0 ; i<2 ; i++)										//pega os sprites dos inimigos
		for (j=0 ; j<4 ; j++)
			if (((i == 0)&&(j < 4)) || ((i == 1)&&(j <2)))
                        	sprites.inimigos[i*4 + j] = carrega_sprite(j*16, 128+i*16, 16, 16);
}

void destroi_sprites ()
{
	al_destroy_bitmap (sprites.sheet);

	int i,j;								//destroi os sprites do jogador	
        for (i=0 ; i<3; i++)
        	for (j=0 ; j<4 ; j++)
			al_destroy_bitmap (sprites.jogador[i*4 + j]);
                                                                                                     
        for (i=0 ; i<2 ; i++)							//destroi os sprites do mapa
        	for (j=0 ; j<3 ; j++)
			al_destroy_bitmap (sprites.mapa[i*3 + j]);
                                                                                                        
        for (i=0 ; i<2 ; i++)							//destroi os sprites dos inimigos
        	for (j=0 ; j<4 ; j++)
        		if (((i == 0)&&(j < 4)) || ((i == 1)&&(j <2)))
				al_destroy_bitmap (sprites.inimigos[i*4 + j]);
}

void inicia_mapa (int *portal_y, int *portal_x)
{
	int i,j;
	mapa.m = (t_quadrado **) malloc (11*sizeof(t_quadrado *));
	for (i=0 ; i<11 ; i++)
		mapa.m[i] = (t_quadrado *) malloc (11*sizeof(t_quadrado));

	int chance;
	for (i=0 ; i<11 ; i++)
	{
		for (j=0 ; j<11 ; j++)
		{
			if (( (i % 2) == 1)&&( (j % 2) == 1))		//parte que cria os muros e as pallets
				mapa.m[i][j].tipo = MURO;
			else
			{
				chance = rand() % 5;
				if (chance < 2)
					mapa.m[i][j].tipo = PALLET;
				else
					mapa.m[i][j].tipo = LIMPO;
			}
			mapa.m[i][j].fogo = 0;

			if ((mapa.m[i][j].tipo == PALLET) && (i > 1) && (j > 1))	//parte que poe os upgrades
			{
				chance = rand() % 5;
				if (chance == 0)
				{
					chance = rand() % 2; 
					if (chance == 0)
						mapa.m[i][j].upgd = 1;
					else
						mapa.m[i][j].upgd = 2;
				}
				else
					mapa.m[i][j].upgd = 0;
			}
			else
				mapa.m[i][j].upgd = 0;
		}
	}

	mapa.m[0][0].tipo = LIMPO;
	mapa.m[0][1].tipo = LIMPO;
	mapa.m[1][0].tipo = LIMPO;

	i = rand() % 9 + 2;
	j = rand() % 9 + 2;
	while ((mapa.m[i][j].tipo != PALLET) || (mapa.m[i][j].upgd != 0))
	{
		i = rand() % 9 + 2;
                j = rand() % 9 + 2;
	}

	*portal_y = i;
	*portal_x = j;

	mapa.m[i][j].upgd = 3;
}

void desenha_mapa ()
{
	int i,j;
	for (i=0 ; i<11 ; i++)
		for (j=0 ; j<11 ; j++)
		{
			if ((mapa.m[i][j].fogo == 0) || (mapa.m[i][j].fogo > 40))
			{
				if ((mapa.m[i][j].tipo != LIMPO) || (mapa.m[i][j].upgd == 0))
					al_draw_bitmap (sprites.mapa[ mapa.m[i][j].tipo ], 16 + j*16, 16 + i*16, 0);
				else
				{
					if ((mapa.m[i][j].upgd == 1) || (mapa.m[i][j].upgd == 2))
						al_draw_bitmap (sprites.mapa[ 9 + mapa.m[i][j].upgd ], 16 + j*16, 16 + i*16, 0);
					else
						al_draw_bitmap (sprites.mapa[ 1 + mapa.m[i][j].upgd ], 16 + j*16, 16 + i*16, 0);
				}
			}
			else
			{
				if ((mapa.m[i][j].fogo > 10)&&(mapa.m[i][j].fogo < 31))
					al_draw_bitmap (sprites.mapa[ 8 ], 16 + j*16, 16 + i*16, 0);
				else if (mapa.m[i][j].fogo < 41)
					al_draw_bitmap (sprites.mapa[ 9 ], 16 + j*16, 16 + i*16, 0);
			}
		}
}

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

void destroi_final ()
{			//destroi tudo no final
	al_destroy_timer	(timer  );
	al_destroy_display	(display);
	al_destroy_bitmap	(buffer );
	al_destroy_event_queue  (queue  );
}

int testa_mapa (int y, int x, int naBomba)
{
	if (mapa.m[y][x].tipo == LIMPO)
		return 0;
	else if (((mapa.m[y][x].tipo == BOMBA1) || (mapa.m[y][x].tipo == BOMBA2)) && (naBomba == 1))
		return 0;
	else
		return 1;
}

int testa_colisao (int x1, int y1, int dir, int naBomba)
{
	int x2 = x1 + 12;										
	int y2 = y1 + 15;
	x1 += 3;
	y1 += 1;

	x1 -= 16;
	x2 -= 16;
	y1 -= 16;
	y2 -= 16;

	//temos os dois cantos, sup esq e inf dir, do sprite do jogador;
	
	if ((x1 < 0) || (y1 < 0) || (x2 > 175) || (y2 > 175))	//se deu negativo, está fora do mapa
		return 1;

	y1 += 4;

	x1 /= 16;
	x2 /= 16;
	y1 /= 16;
	y2 /= 16;

	switch (dir)
	{
		case (1):
			if ((testa_mapa(y1, x1, naBomba)) || (testa_mapa(y1, x2, naBomba)))
				return 1;

		case (2):
			if ((testa_mapa(y2, x1, naBomba)) || (testa_mapa(y2, x2, naBomba)))
                        	return 1;

		case (3):
			if ((testa_mapa(y1, x1, naBomba)) || (testa_mapa(y2, x1, naBomba)))
                        	return 1;

		case (4):
			if ((testa_mapa(y1, x2, naBomba)) || (testa_mapa(y2, x2, naBomba)))
                        	return 1;
	}

	return 0;
}

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *fechar, int *dir, int *bomba, int naBomba)
{
	int aux_x = *x;
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
        	*fechar = 1;
	}
	else if(keyb[ALLEGRO_KEY_SPACE])
	{
		*bomba = 1;
	}

	if (dir)
	{
		if (testa_colisao ( *x, *y, *dir, naBomba))
		{
			*x = aux_x;
			*y = aux_y;
		}
	}

	int i;
        for( i = 0 ; i < ALLEGRO_KEY_MAX; i++ )
        	keyb[i] &= KEY_VISTA;
}

void anima_jogador (int *contframe, int *frame, int x1, int y1, int x2, int y2)
{
	if (*contframe > 30)		
        {
		if ((x1 != x2) || (y1 != y2))
		{
        		*contframe = 0;
        		if (*frame)
        			*frame = 0;
        		else
        			*frame = 1;
		}
        }

}

void inicia_vbomba ()
{
	vbomba.max   = 1;
	vbomba.quant = 0;
	vbomba.tamf  = 1;
	vbomba.v = (t_bomba *) malloc (5 * sizeof(t_bomba));
	int i;
	for (i=0 ; i<5 ; i++)
	{
		vbomba.v[i].tempo = 0;
		vbomba.v[i].x     = 0;
		vbomba.v[i].y     = 0;
	}
}

void coloca_bomba ( int x, int y, int *bomba)
{
	int quant;
	if (vbomba.quant < vbomba.max)
	{
		quant = vbomba.quant;
		if ((mapa.m[ (y - 8)/16 ][ (x - 8)/16 ].tipo != BOMBA1) && (mapa.m[ (y - 8)/16 ][ (x - 8)/16 ].tipo != BOMBA2))
		{
			vbomba.v[ quant ].x     = (x - 8)/16;
			vbomba.v[ quant ].y     = (y - 8)/16;
			vbomba.v[ quant ].tempo = TBOMBA;

			mapa.m[ vbomba.v[ quant ].y ][ vbomba.v[ quant ].x ].tipo = BOMBA1;

			vbomba.quant++;
		}
	}
	else
		*bomba = 0;
}

void passa_tempo_bombas ()
{
	int i;
	for (i=0 ; i < vbomba.quant ; i++)
	{
		vbomba.v[i].tempo--;
		if (( vbomba.v[i].tempo <= TBOMBA/4 ) && ( mapa.m[ vbomba.v[i].y ][ vbomba.v[i].x ].tipo != BOMBA2 ))
			mapa.m[ vbomba.v[i].y ][ vbomba.v[i].x ].tipo = BOMBA2;
	}
}

void passa_tempo_fogos  ()
{
	int i,j;
	for (i=0 ; i < 11 ; i++)
	{
		for (j=0 ; j < 11 ; j++)
		{
			if (mapa.m[i][j].fogo != 0)
				mapa.m[i][j].fogo--;
			if ((mapa.m[i][j].fogo < 40) && (mapa.m[i][j].fogo > 0) && (mapa.m[i][j].tipo != LIMPO))
				mapa.m[i][j].tipo = LIMPO;
		}
	}
}

void cria_explosao (int y, int x, int tamf, int dir)
{
	if (tamf == 0)
        	return;

	mapa.m[y][x].fogo = 40 + (vbomba.tamf - tamf + 1)*15;


	if (dir == 1)
	{
		if ((y-1 >= 0)  && (mapa.m[y-1][x].tipo != MURO))
                	cria_explosao (y-1, x, tamf-1, 1);
	}
	else if (dir == 2)
	{
		if ((y+1 <= 10) && (mapa.m[y+1][x].tipo != MURO))
                	cria_explosao (y+1, x, tamf-1, 2);
	}
	else if (dir == 3)
	{
		if ((x-1 >= 0)  && (mapa.m[y][x-1].tipo != MURO))
                	cria_explosao (y, x-1, tamf-1, 3);
	}
	else if (dir == 4)
	{
		if ((x+1 <= 10) && (mapa.m[y][x+1].tipo != MURO))
                	cria_explosao (y, x+1, tamf-1, 4);
	}
}

void inicia_explosoes (int y, int x, int tamf)
{
	mapa.m[y][x].fogo = 40;
	mapa.m[y][x].tipo = LIMPO;

	if ((y-1 >= 0)  && (mapa.m[y-1][x].tipo != MURO))
		cria_explosao (y-1, x, tamf, 1);
	if ((y+1 <= 10) && (mapa.m[y+1][x].tipo != MURO))
		cria_explosao (y+1, x, tamf, 2);
	if ((x-1 >= 0)  && (mapa.m[y][x-1].tipo != MURO))
		cria_explosao (y, x-1, tamf, 3);
	if ((x+1 <= 10) && (mapa.m[y][x+1].tipo != MURO))
		cria_explosao (y, x+1, tamf, 4);
}

void explode_bombas ()
{
	int i,j;
	for (i=0 ; i < vbomba.quant ; i++)
	{
		if (vbomba.v[i].tempo == 0)
		{
			inicia_explosoes (vbomba.v[ i ].y, vbomba.v[ i ].x, vbomba.tamf);
			mapa.m[ vbomba.v[ i ].y ][ vbomba.v[ i ].x ].tipo = LIMPO;
			vbomba.v[i].x = 0;
			vbomba.v[i].y = 0;
			for (j=i ; j < vbomba.quant-1 ; j++)
			{
				vbomba.v[j].x     = vbomba.v[j+1].x;
				vbomba.v[j].y     = vbomba.v[j+1].y;
				vbomba.v[j].tempo = vbomba.v[j+1].tempo;
			}
			vbomba.quant--;
		}
	}
}

void verifica_na_bomba (int *porBomba, int *naBomba, int jogador_x, int jogador_y)
{
	jogador_x -= 16;
	jogador_y -= 16;
	if ((*porBomba == 1) || (*naBomba == 1))
	{
		if (((jogador_x + 3)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 5)/16 == vbomba.v[ vbomba.quant-1 ].y) || ((jogador_x + 12)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 15)/16 == vbomba.v[ vbomba.quant-1 ].y))
		{				//!! melhorar essa verificação
			*naBomba = 1;
			*porBomba = 0;
		}
		else
			*naBomba = 0;
	}
}

int testa_queimadura (int y, int x)
{
	if ((mapa.m[y][x].fogo > 0) && (mapa.m[y][x].fogo < 41))
		return 1;
	else
		return 0;
}

void testa_morte (int y1, int x1, int *morte, int *vidas, int *imune)
{
	int x2 = x1 + 12;										
        int y2 = y1 + 15;
        x1 += 3;
        y1 += 5;

        x1 -= 16;
        x2 -= 16;
        y1 -= 16;
        y2 -= 16;
                                                                                                        
        //temos os dois cantos, sup esq e inf dir, do sprite do jogador;
                                                                                                        
        x1 /= 16;
        x2 /= 16;
        y1 /= 16;
        y2 /= 16;

	if (testa_queimadura (y1,x1) || testa_queimadura (y1,x2) || testa_queimadura (y2,x1) || testa_queimadura (y2,x2))
	{
		*vidas -= 1;
		*imune = 150;
	}

	if (*vidas <= 0)
		*morte = 1;
}

int testa_colisao_upgd (int y, int x, int upgd)
{
	if (mapa.m[y][x].upgd == upgd)
		return 1;
        else
        	return 0;
}

void limpa_upgrades (int y1, int y2, int x1, int x2)
{
	mapa.m[y1][x1].upgd = 0;
	mapa.m[y2][x1].upgd = 0;
	mapa.m[y1][x2].upgd = 0;
	mapa.m[y2][x2].upgd = 0;
}

void testa_upgrades (int y1, int x1)
{
	int x2 = x1 + 12;										
        int y2 = y1 + 15;
        x1 += 3;
        y1 += 5;
                                                                                                        
        x1 -= 16;
        x2 -= 16;
        y1 -= 16;
        y2 -= 16;
                                                                                                        
        //temos os dois cantos, sup esq e inf dir, do sprite do jogador;
                                                                                                        
        x1 /= 16;
        x2 /= 16;
        y1 /= 16;
        y2 /= 16;

	if (testa_colisao_upgd (y1,x1,1) || testa_colisao_upgd (y1,x2,1) || testa_colisao_upgd (y2,x1,1) || testa_colisao_upgd (y2,x2,1))
	{
		vbomba.tamf++;
		limpa_upgrades (y1, y2, x1, x2);
	}
	if (testa_colisao_upgd (y1,x1,2) || testa_colisao_upgd (y1,x2,2) || testa_colisao_upgd (y2,x1,2) || testa_colisao_upgd (y2,x2,2))
	{
		vbomba.max++;
		limpa_upgrades (y1, y2, x1, x2);
	}
}

void inicia_vmonstros ()
{
	vmonstros.quant = 0;
	vmonstros.v     = (t_monstro *) malloc (5 * sizeof (t_monstro));

	int i;
	for (i=0 ; i<5 ; i++)
	{
		vmonstros.v[i].tipo = 0;
		vmonstros.v[i].x    = 0;
		vmonstros.v[i].y    = 0;
	}
}

void liga_portal (int y, int x)
{
	if (vmonstros.quant == 0)
		mapa.m[y][x].upgd = 4;
}

int main ()
{
	inicia_comeco ();
	inicia_sprites ();

	al_register_event_source (queue, al_get_keyboard_event_source ());
	al_register_event_source (queue, al_get_display_event_source (display));
	al_register_event_source (queue, al_get_timer_event_source (timer));

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
	int contframe = 0;		//conta os frames para trocar a variação de frame que será exibido
	int morte     = 0;		//guarda se o jogador morreu
	int passou    = 0;		//guarda se o jogador passou de fase
	int fase      = 1;		//guarda a fase em que o jogador esta

	int portal_x  = 0;		//variaveis que guardam a coordenada do portal
	int portal_y  = 0;

	int porBomba  = 0;		//flag para por bomba
	int naBomba   = 0;		//flag que detecta se o player ainda está no quadrado da boma após colocá-la
	inicia_vbomba ();

	srand(time(0));

	unsigned char keyb[ALLEGRO_KEY_MAX];
	memset (keyb, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));

	/*Loop do jogo--------------------------------------------------------------------------------------------------*/

	inicia_vmonstros ();
	inicia_mapa (&portal_y, &portal_x);
	al_start_timer (timer);
	while ((! fechar) && (! morte))	//loop do jogo inteiro com menu e tudo
	{
		while ((! morte) && (! fechar))		//loop do jogo com as fases
		{
			while ((!morte) && (! fechar) && (! passou))
			{
				al_wait_for_event (queue, &event);

				switch (event.type)
				{
					case ALLEGRO_EVENT_TIMER:
			
						pre_x = jogador_x;
						pre_y = jogador_y;
	
						interpreta_controle (keyb, &jogador_x, &jogador_y, &fechar, &dir, &porBomba, naBomba);
	
						contframe++;
						anima_jogador (&contframe, &frame, pre_x, pre_y, jogador_x, jogador_y);
					
						if (porBomba)
							coloca_bomba (jogador_x, jogador_y, &porBomba);

						verifica_na_bomba (&porBomba, &naBomba, jogador_x, jogador_y);
						passa_tempo_bombas ();
						passa_tempo_fogos  ();
						explode_bombas();

						if (imune == 0)
							testa_morte (jogador_y, jogador_x, &morte, &vidas, &imune);
						else
							imune--;

						testa_upgrades (jogador_y, jogador_x);
						liga_portal    (portal_y , portal_x);
                                                                
						desenha = 1;
						break;
	
					case ALLEGRO_EVENT_KEY_DOWN:
	        				keyb[event.keyboard.keycode] = KEY_VISTA | KEY_SOLTA;	//!!!!! entender melhor isso
        					break;

		   	 		case ALLEGRO_EVENT_KEY_UP:
        					keyb[event.keyboard.keycode] &= KEY_SOLTA;
	        				break;

					case ALLEGRO_EVENT_DISPLAY_CLOSE:
						fechar = 1;
						break;
				}

				if ((desenha) && (al_is_event_queue_empty (queue)))
				{
					pre_escrita_display ();
                        
					al_clear_to_color (al_map_rgb(0, 0, 0));
					desenha_mapa();
					al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %d Y: %d IM: %d VI: %d", jogador_x, jogador_y, imune, vidas);
					al_draw_bitmap (sprites.jogador[ dir-1 + 4*frame ], jogador_x, jogador_y, 0);
	
                	        	pos_escrita_display ();
		
					desenha = 0;
				}
			}	
		}
	}

	destroi_sprites();
	destroi_final();

	return 0;
}
