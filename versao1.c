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
#define BOMBA  5

typedef struct t_sprites
{
	ALLEGRO_BITMAP* sheet;

	ALLEGRO_BITMAP* jogador[12];
	ALLEGRO_BITMAP* mapa[6];
	ALLEGRO_BITMAP* inimigos[6];
} t_sprites;

typedef struct t_quadrado
{
	int tipo;
	int fogo;
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

	for (i=0 ; i<2 ; i++)										//pega os sprites do mapa
		for (j=0 ; j<3 ; j++)
				sprites.mapa[i*3 + j] = carrega_sprite(j*16, 64+i*16, 16, 16);

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

void inicia_mapa ()
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
			if (( (i % 2) == 1)&&( (j % 2) == 1))
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
		}
	}

	mapa.m[0][0].tipo = LIMPO;
	mapa.m[0][1].tipo = LIMPO;
	mapa.m[1][0].tipo = LIMPO;
}

void desenha_mapa ()
{
	int i,j;
	for (i=0 ; i<11 ; i++)
		for (j=0 ; j<11 ; j++)
			al_draw_bitmap (sprites.mapa[ mapa.m[i][j].tipo ], 16 + j*16, 16 + i*16, 0);
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

int testa_colisao (int x1, int y1, int dir)
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
			if ((mapa.m[y1][x1].tipo != 0) || (mapa.m[y1][x2].tipo != 0))
				return 1;

		case (2):
			if ((mapa.m[y2][x1].tipo != 0) || (mapa.m[y2][x2].tipo != 0))
                        	return 1;

		case (3):
			if ((mapa.m[y1][x1].tipo != 0) || (mapa.m[y2][x1].tipo != 0))
                        	return 1;

		case (4):
			if ((mapa.m[y1][x2].tipo != 0) || (mapa.m[y2][x2].tipo != 0))
                        	return 1;
	}

	return 0;
}

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *fechar, int *dir, int *bomba)
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
		if (testa_colisao ( *x, *y, *dir))
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

void coloca_bomba (int *por_bomba, int x, int y)
{
	int quant;
	*por_bomba = 0;
	if (vbomba.quant < vbomba.max)		//!!! testar se ja tem bomba no lugar
	{
		quant = vbomba.quant;
		vbomba.v[ quant ].x     = (x - 8)/16;
		vbomba.v[ quant ].y     = (y - 8)/16;
		vbomba.v[ quant ].tempo = 100;

		mapa.m[ vbomba.v[ quant ].y ][ vbomba.v[ quant ].x ].tipo = BOMBA;

		vbomba.quant++;
	}
}

void passa_tempo_bombas ()
{
	int i;
	for (i=0 ; i < vbomba.quant ; i++)
	{
		vbomba.v[i].tempo--;
	}
}

void explode_bombas ()
{
	int i,j;
	for (i=0 ; i < vbomba.quant ; i++)
	{
		if (vbomba.v[i].tempo == 0)
		{
			//*a lógica da explosão vai aqui
			vbomba.v[i].x = 0;
			vbomba.v[i].y = 0;
			for (j=i ; j < vbomba.quant-1 ; j++)
			{
				vbomba.v[j].x     = vbomba.v[j+1].x;
				vbomba.v[j].y     = vbomba.v[j+1].y;
				vbomba.v[j].tempo = vbomba.v[j+1].tempo;
			}
			mapa.m[ vbomba.v[ i ].y ][ vbomba.v[ i ].x ].tipo = LIMPO;
			vbomba.quant--;
		}
	}
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

	int jogador_x = 16;
	int jogador_y = 16;
	int pre_x     = 16;
	int pre_y     = 16;
	int dir       = 2;
	int frame     = 0;
	int contframe = 0;

	int por_bomba = 0;
	inicia_vbomba ();

	srand(time(0));

	unsigned char keyb[ALLEGRO_KEY_MAX];
	memset (keyb, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));

	/*Loop do jogo--------------------------------------------------------------------------------------------------*/

	inicia_mapa();
	al_start_timer (timer);
	while (1)
	{
		al_wait_for_event (queue, &event);

		switch (event.type)
		{
			case ALLEGRO_EVENT_TIMER:
			
				pre_x = jogador_x;
				pre_y = jogador_y;

				interpreta_controle (keyb, &jogador_x, &jogador_y, &fechar, &dir, &por_bomba);

				contframe++;
				anima_jogador (&contframe, &frame, pre_x, pre_y, jogador_x, jogador_y);
				
				if (por_bomba)
					coloca_bomba (&por_bomba, jogador_x, jogador_y);
				passa_tempo_bombas ();
				explode_bombas();
                                                                
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

		if (fechar)
			break;		//sai do loop do jogo

		if ((desenha) && (al_is_event_queue_empty (queue)))
		{
			pre_escrita_display ();
                        
			al_clear_to_color (al_map_rgb(0, 0, 0));
			desenha_mapa();
			al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %d Y: %d QB: %d TB: %d", jogador_x, jogador_y, vbomba.quant, vbomba.v[0].tempo);
			al_draw_bitmap (sprites.jogador[ dir-1 + 4*frame ], jogador_x, jogador_y, 0);

                        pos_escrita_display ();

			desenha = 0;
		}
	}

	destroi_sprites();
	destroi_final();

	return 0;
}
