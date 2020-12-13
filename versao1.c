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
	ALLEGRO_BITMAP* monstros[6];
	ALLEGRO_BITMAP* elHud[1];
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
	int dir;
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
ALLEGRO_BITMAP* 	hud;
ALLEGRO_BITMAP*  	menu;
ALLEGRO_SAMPLE*		sRobo;
ALLEGRO_SAMPLE*		sExplosao;
ALLEGRO_SAMPLE*		sBomba;
ALLEGRO_AUDIO_STREAM*   musica;

t_sprites 		sprites;
t_mapa 	  		mapa;
t_vmonstros		vmonstros;
t_vbomba		vbomba;
int 			pontos;

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

void inicia_comeco ()
{
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

	inicializa_geral (al_install_audio(), "audio");			//inicia o audio
	inicializa_geral (al_init_acodec_addon(), "acodec");
	inicializa_geral (al_reserve_samples(16), "reserve samples");
	inicializa_geral (al_init_primitives_addon(), "primitives");	//inicializa as imagens
	inicializa_geral (al_init_image_addon(), "image addon");

	sRobo     = al_load_sample ("./resources/robomorte.wav");	//inicializa os sons
	sExplosao = al_load_sample ("./resources/explosao.wav" );
	sBomba	  = al_load_sample ("./resources/bomba.wav"    );
	inicializa_geral (sRobo    , "sample robo"    );
	inicializa_geral (sExplosao, "sample explosao");
	inicializa_geral (sBomba   , "sample bomba"   );

	musica    = al_load_audio_stream ("./resources/music.opus", 2, 2048);
        inicializa_geral (musica   , "musica");
        al_attach_audio_stream_to_mixer (musica, al_get_default_mixer());
}

ALLEGRO_BITMAP* carrega_sprite (int x, int y, int w, int h)
{
	ALLEGRO_BITMAP* sprite = al_create_sub_bitmap (sprites.sheet, x, y, w, h);
	inicializa_geral (sprite, "carrega sprite");
	return sprite;
}

void inicia_sprites ()
{
	sprites.sheet = al_load_bitmap ("./resources/spritesheet.png");
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
                        	sprites.monstros[i*4 + j] = carrega_sprite(j*16, 128+i*16, 16, 16);

	sprites.elHud[0] = carrega_sprite (48, 64, 16, 16);

	hud = al_load_bitmap ("./resources/hud.png");
	inicializa_geral (hud, "hud");

	menu = al_load_bitmap ("./resources/menu.png");
	inicializa_geral (menu,"menu");
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
				al_destroy_bitmap (sprites.monstros[i*4 + j]);
}

void inicia_mapa (int *portal_y, int *portal_x)
{
	int i,j,chance;								

	for (i=0 ; i<11 ; i++)							//passa por toda matriz do mapa
	{
		for (j=0 ; j<11 ; j++)
		{
			if (( (i % 2) == 1)&&( (j % 2) == 1))			//parte que cria os muros e as pallets
				mapa.m[i][j].tipo = MURO;
			else
			{
				chance = rand() % 5;
				if (chance < 2)
					mapa.m[i][j].tipo = PALLET;
				else
					mapa.m[i][j].tipo = LIMPO;
			}

			mapa.m[i][j].fogo = 0;						//poe o fogo no default, 0

			if ((mapa.m[i][j].tipo == PALLET) && (i > 1) && (j > 1))	//parte que poe os upgrades
			{
				chance = rand() % 8;
				if (chance == 0)					//10% de chance de um upgrade em um pallet
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

	mapa.m[0][0].tipo = LIMPO ;		//parte que deixa o local de nascimento do jogador sempre do mesmo jeito
	mapa.m[0][1].tipo = LIMPO ;
	mapa.m[1][0].tipo = LIMPO ;
	mapa.m[2][0].tipo = PALLET;
	mapa.m[0][2].tipo = PALLET;

	i = rand() % 9 + 2;			//procura um lugar para ficar o portal
	j = rand() % 9 + 2;
	while ((mapa.m[i][j].tipo != PALLET) || (mapa.m[i][j].upgd != 0))
	{
		i = rand() % 9 + 2;
                j = rand() % 9 + 2;
	}

	*portal_y = i;				//registra onde vai ficar o portal
	*portal_x = j;

	mapa.m[i][j].upgd = 3;			//poe o portal
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
				if ((mapa.m[i][j].fogo > 10)&&(mapa.m[i][j].fogo < 31))			//printa o fogo maior
					al_draw_bitmap (sprites.mapa[ 8 ], 16 + j*16, 16 + i*16, 0);
				else if (mapa.m[i][j].fogo < 41)					//printa o fogo menor
				{
					al_draw_bitmap (sprites.mapa[ 9 ], 16 + j*16, 16 + i*16, 0);
					if (mapa.m[i][j].fogo == 40)
						al_play_sample (sExplosao, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
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
	al_destroy_timer	(timer    );
	al_destroy_display	(display  );
	al_destroy_bitmap	(buffer   );
	al_destroy_event_queue  (queue    );
	al_destroy_sample       (sRobo    );
	al_destroy_sample       (sExplosao);
	al_destroy_sample       (sBomba   );
	al_destroy_audio_stream (musica   );
}

int testa_mapa (int y, int x, int naBomba)
{						//testa o mapa para ver se o jogador pode entrar no local
	if (mapa.m[y][x].tipo == LIMPO)
		return 0;
	else if (((mapa.m[y][x].tipo == BOMBA1) || (mapa.m[y][x].tipo == BOMBA2)) && (naBomba == 1))
		return 0;
	else
		return 1;
}

int testa_colisao (int x1, int y1, int dir, int naBomba)
{				//funcao que testa a colisao do jogador
	int x2 = x1 + 12;					
	int y2 = y1 + 15;
	x1 += 3;
	y1 += 1;

	x1 -= 16;
	x2 -= 16;
	y1 -= 16;
	y2 -= 16;

	//temos os dois cantos, sup esq e inf dir, do sprite do jogador;
	
	if ((x1 < 0) || (y1 < 0) || (x2 > 175) || (y2 > 175))	//se deu verdadeiro, está fora do mapa
		return 1;

	y1 += 4;

	x1 /= 16;
	x2 /= 16;
	y1 /= 16;
	y2 /= 16;

	switch (dir)		//testa o mapa dependendo da direcao que o jogador quer se mexer
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

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *dir, int *bomba, int naBomba, int *pausa, int *ajuda)
{
	int aux_x = *x;				//funcao que interpreta o input do jogador dentro do jogo e movimenta o personagem
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

void inicia_vbomba ()
{				//inicia o vetor de bombas
	vbomba.max   = 1;
	vbomba.quant = 0;
	vbomba.tamf  = 1;
	int i;					
	for (i=0 ; i<5 ; i++)
	{
		vbomba.v[i].tempo = 0;
		vbomba.v[i].x     = 0;
		vbomba.v[i].y     = 0;
	}
}

void coloca_bomba ( int x, int y, int *bomba)
{						//funcao que coloca bomba no local
	int quant;
	if (vbomba.quant < vbomba.max) 		//testa se pode por bomba quanto à quantidade
	{
		quant = vbomba.quant;
		if ((mapa.m[ (y - 8)/16 ][ (x - 8)/16 ].tipo != BOMBA1) && (mapa.m[ (y - 8)/16 ][ (x - 8)/16 ].tipo != BOMBA2))
		{
			vbomba.v[ quant ].x     = (x - 8)/16;
			vbomba.v[ quant ].y     = (y - 8)/16;
			vbomba.v[ quant ].tempo = TBOMBA;

			mapa.m[ vbomba.v[ quant ].y ][ vbomba.v[ quant ].x ].tipo = BOMBA1;

			al_play_sample (sBomba, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			vbomba.quant++;
		}
	}
	else
		*bomba = 0;
}

void passa_tempo_bombas ()
{
	int i;						//passa pelo vetor de bombas e passa o tempo delas
	for (i=0 ; i < vbomba.quant ; i++)
	{
		vbomba.v[i].tempo--;
		if (( vbomba.v[i].tempo <= TBOMBA/4 ) && ( mapa.m[ vbomba.v[i].y ][ vbomba.v[i].x ].tipo != BOMBA2 ))
			mapa.m[ vbomba.v[i].y ][ vbomba.v[i].x ].tipo = BOMBA2;
	}
}

void passa_tempo_fogos  ()
{							//passa pela matriz e passa o tempo dos fogos
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

	mapa.m[y][x].fogo = 40 + (vbomba.tamf - tamf + 1)*15;		//aumenta o tempo pro fogo comecar, fazend o delay de cada explosao

	if (mapa.m[y][x].tipo == PALLET)
	{
		pontos += 10;
		return;
	}

	if ((mapa.m[y][x].upgd > 0) && (mapa.m[y][x].upgd < 3) && (mapa.m[y][x].tipo == LIMPO))
	{
		mapa.m[y][x].upgd = 0;
		return;
	}

	if (dir == 1)							//chama a funcao dependendo da direcao
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
{								//inicia uma corrente de epxlosoes, chamando a funcao recursiva cria_explosao 
	mapa.m[y][x].fogo = 40;					//para cada lado que ela se espalha
	mapa.m[y][x].tipo = LIMPO;				//tranforma o local que a bomba estava em limpo

	if ((y-1 >= 0)  && (mapa.m[y-1][x].tipo != MURO))	//cima
		cria_explosao (y-1, x, tamf, 1);
	if ((y+1 <= 10) && (mapa.m[y+1][x].tipo != MURO))	//baixo
		cria_explosao (y+1, x, tamf, 2);
	if ((x-1 >= 0)  && (mapa.m[y][x-1].tipo != MURO))	//esquerda
		cria_explosao (y, x-1, tamf, 3);
	if ((x+1 <= 10) && (mapa.m[y][x+1].tipo != MURO))	//direita
		cria_explosao (y, x+1, tamf, 4);
}

void explode_bombas ()
{			//funcao que passa pelas bombas e, caso o tempo delas carregar acabe, explode elas
	int i,j;
	for (i=0 ; i < vbomba.quant ; i++)
	{
		if (vbomba.v[i].tempo == 0)							//testa se a bomba carregou
		{
			inicia_explosoes (vbomba.v[ i ].y, vbomba.v[ i ].x, vbomba.tamf);	//chama a funcao que inicia as explosoes

			for (j=i ; j < vbomba.quant-1 ; j++)					//traz as bombas adiante
			{
				vbomba.v[j].x     = vbomba.v[j+1].x;
				vbomba.v[j].y     = vbomba.v[j+1].y;
				vbomba.v[j].tempo = vbomba.v[j+1].tempo;
			}
			vbomba.quant--;								//diminui a quantidade de bombas
		}
	}
}

int esta_na_bomba (int jogador_y, int jogador_x)	//testa se o jogador esta na bomba
{
	if (((jogador_x + 3)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 5)/16 == vbomba.v[ vbomba.quant-1 ].y))
		return 1;
	if (((jogador_x + 12)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 15)/16 == vbomba.v[ vbomba.quant-1 ].y))
		return 1;
	return 0;
}

void verifica_na_bomba (int *porBomba, int *naBomba, int jogador_y, int jogador_x)
{							//altera algumas flags caso o jogador esteja na bomba que ele acabou de por
	jogador_x -= 16;
	jogador_y -= 16;
	if ((*porBomba == 1) || (*naBomba == 1))	//se o jogador acabou de por uma bomba ou ainda esta nela
	{
		if (esta_na_bomba(jogador_y, jogador_x))
		{
			*naBomba = 1;
			*porBomba = 0;
		}
		else
			*naBomba = 0;
	}
}

int testa_queimadura (int y, int x)
{				//testa se o jogador ou monstro encostou em uma explosao de bomba
	if ((y >= 0) && (y <= 10) && (x >= 0) && (x <= 10))	//primeiro testa se esta dentro do mapa
	{
		if ((mapa.m[y][x].fogo > 0) && (mapa.m[y][x].fogo < 41))	//testa se o local esta explodindo
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int testa_toque (int y1, int y2, int x1, int x2)
{				//funcao que testa se o jogador encostou em algum monstro
	int i;
	int y1m;
	int y2m;
	int x1m;
	int x2m;
	for (i=0 ; i<vmonstros.quant ; i++)
	{
		y1m = vmonstros.v[i].y;						//pega os contos do monstro
		y2m = y1m + 15;
		x1m = vmonstros.v[i].x;
		x2m = x1m + 15;

		if (! ((x1 > x2m) || (x2 < x1m) || (y1 > y2m) || (y2 < y1m)))	//testa a colisao
			return 1;
	}

	return 0;
}

void testa_morte (int y1, int x1, int *morte, int *vidas, int *imune)
{
	int x2 = x1 + 12;										
        int y2 = y1 + 15;

	//essas funcoes testam os pixels em que o jogador e os monstros estao, por isso sao executadas antes das operacoes abaixo
	if (testa_toque (y1,y2,x1,x2))
        {
        	*vidas -= 1;
        	*imune = 150;
        }

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

	//a testa queimadura testa as coordenadas onde o jogador esta dentro da matriz, por isso efetuamos as operacoes
	if ((*imune == 0) && (testa_queimadura (y1,x1) || testa_queimadura (y1,x2) || testa_queimadura (y2,x1) || testa_queimadura (y2,x2)))
	{
		*vidas -= 1;
		*imune = 150;
	}

	if (*vidas < 0)			//se o jogador fica sem vidas, ele morre
		*morte = 1;
}

int testa_colisao_upgd (int y, int x, int upgd)
{					//funcao que testa se o player colidiu com um upgrade
	if (mapa.m[y][x].upgd == upgd)
		return 1;
        else
        	return 0;
}

void limpa_upgrades (int y1, int y2, int x1, int x2)
{					//funcao q limpa o local que o jogador pegou o upgrade
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

	//temos as coordenadas de cada canto do jogador na matriz
	

	//essa parte testa a colisao com os upgrades e, caso colida com um, limpa ele e aumenta o que deve ser aumentado

	if (testa_colisao_upgd (y1,x1,1) || testa_colisao_upgd (y1,x2,1) || testa_colisao_upgd (y2,x1,1) || testa_colisao_upgd (y2,x2,1))
	{
		if (vbomba.tamf < 10)
			vbomba.tamf++;
		limpa_upgrades (y1, y2, x1, x2);
		pontos += 50;
	}
	if (testa_colisao_upgd (y1,x1,2) || testa_colisao_upgd (y1,x2,2) || testa_colisao_upgd (y2,x1,2) || testa_colisao_upgd (y2,x2,2))
	{
		if (vbomba.max < 5)
			vbomba.max++;
		limpa_upgrades (y1, y2, x1, x2);
		pontos += 50;
	}
}

void inicia_vmonstros ()
{					//inicia o vetor de monstros
	vmonstros.quant = 0;

	int i;
	for (i=0 ; i<5 ; i++)
	{
		vmonstros.v[i].dir  = 1;
		vmonstros.v[i].tipo = 0;
		vmonstros.v[i].y    = 0;
		vmonstros.v[i].x    = 0;
	}
}

void liga_portal (int y, int x)
{					//funcao que verifica se todos os monstros morreram e, se sim, liga o portal
	if (vmonstros.quant == 0)
		mapa.m[y][x].upgd = 4;
}

int verifica_passou (int y1, int x1, int p_y, int p_x)
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

	//pega a posicao dos cantos do jogador na matriz

	//se ele colidiu com o portal, passou de fase
	if (testa_colisao_upgd(y1,x1,4) || testa_colisao_upgd(y1,x2,4) || testa_colisao_upgd(y2,x1,4) || testa_colisao_upgd(y2,x2,4))
		return 1;
	else
		return 0;
}

void malloc_mapa_vbomba_vmonstros()
{					//funcao que faz o malloc dos vetores de bomba e monstros
	vbomba.v    = (t_bomba *) malloc (5 * sizeof(t_bomba));
	vmonstros.v = (t_monstro *) malloc (5 * sizeof (t_monstro));

	int i;								
        mapa.m = (t_quadrado **) malloc (11*sizeof(t_quadrado *));
        for (i=0 ; i<11 ; i++)
        	mapa.m[i] = (t_quadrado *) malloc (11*sizeof(t_quadrado));
}

void reseta_vbomba ()
{					//funcao que reseta o vetor de bombas, usada ao passar de fase
	int i;					
	for (i=0 ; i<5 ; i++)
	{
		vbomba.v[i].tempo = 0;
		vbomba.v[i].x     = 0;
		vbomba.v[i].y     = 0;
	}
}

int tem_monstro (int i, int y, int x)
{					//funcao que testa se o lugar ja tem monstro
	int j;
	for (j=0 ; j<i ; j++)
	{
		if ((y == vmonstros.v[j].y) && (x == vmonstros.v[j].x))
			return 1;
	}

	return 0;
}

void cria_monstros (int fase)
{
	int quant1 = 0;		//quantidade de cada tipo de bicho
	int quant2 = 0;		//por default elas sao zero e, dependo de qual fase, elas mudam
	int quant3 = 0;

	//parte que determina a quantidade do tipo 1
	if (fase < 3)
	{
		if (fase == 1)
			quant1 = 3;
		else
			quant1 = 2;
	}
	else
		quant1 = 6 - fase;

	//parte que determina a quantidade do tipo 2
	if (fase == 2)
		quant2 = 1;
	else if (fase > 2)
		quant2 = 2;

	//parte que determina a quantidade do tipo 3
	if (fase == 4)
		quant3 = 1;
	else if (fase == 5)
		quant3 = 2;

	vmonstros.quant = quant1 + quant2 + quant3;		//pega a quantidade total de monstros

	//definido o tanto de inimigos que terao na fase, vamos cria-los
	

	int i,x,y;
	for (i=0 ; i < vmonstros.quant ; i++)
	{
		if (i < quant1)				//determina o tipo do monstro
			vmonstros.v[i].tipo = 1;
		else if (i < quant1 + quant2)
			vmonstros.v[i].tipo = 2;
		else
			vmonstros.v[i].tipo = 3;

		y = rand() % 8 + 3;			//poe ele aleatoriamente em um lugar limpo do mapa e dá para ele uma direcao aleatoria
		x = rand() % 8 + 3;

		while ((mapa.m[y][x].tipo != LIMPO) || (tem_monstro (i,y,x)))
		{
			y = rand() % 8 + 3;		//o % 8 + 3 é para ele cobrir de 3 até 11, que sao os limites das coordenadas de
                        x = rand() % 8 + 3;		//onde os monstros podem surgir, ignorando o local que o jogador comeca
		}

		vmonstros.v[i].dir = rand() % 4 + 1;;
		vmonstros.v[i].y   = y;
		vmonstros.v[i].x   = x;

	}

	for (i=0 ; i < vmonstros.quant ; i++)			//passa a coordenada de luagr da matriz para pixel na tela do jogo
	{
		vmonstros.v[i].y += 1;
                vmonstros.v[i].x += 1;
		vmonstros.v[i].y = 16 * vmonstros.v[i].y;
		vmonstros.v[i].x = 16 * vmonstros.v[i].x;
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

void mata_monstros ()
{
	int i,j;
	int y1,y2,x1,x2;
	for (i=0 ; i < vmonstros.quant ; i++)	//passa por todos os monstros	
	{
		x1 = vmonstros.v[i].x     ;	//pega as cordenadas dos cantos dos monstros 
		y1 = vmonstros.v[i].y     ;
		x2 = vmonstros.v[i].x + 15;							
                y2 = vmonstros.v[i].y + 15;
                                                                                        
                x1 /= 16;
                x2 /= 16;
                y1 /= 16;
                y2 /= 16;

		x1--;
		x2--;
		y1--;
		y2--;

		if (testa_queimadura (y1,x1) || testa_queimadura (y1,x2) || testa_queimadura (y2,x1) || testa_queimadura (y2,x2))
		{				//testa se o monstro foi queimado pela explosao
			vmonstros.quant--;
			pontos = pontos + (vmonstros.v[i].tipo)*(vmonstros.v[i].tipo)*100;
			vmonstros.v[i].dir  = 0;
			vmonstros.v[i].x    = 0;
			vmonstros.v[i].y    = 0;
			vmonstros.v[i].tipo = 0;
			al_play_sample (sRobo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			for (j=i ; j < vmonstros.quant ; j++)		//tira o monstro queimado do vetor e traz os que estao mais adiante
			{
				vmonstros.v[j].dir  = vmonstros.v[j+1].dir ;
				vmonstros.v[j].x    = vmonstros.v[j+1].x   ;
				vmonstros.v[j].y    = vmonstros.v[j+1].y   ;
				vmonstros.v[j].tipo = vmonstros.v[j+1].tipo;
			}
		}
	}
}

int testa_fora_mapa (int y1, int y2, int x1, int x2)
{								//essa funcao simplesmente testa se os inteiros vao para fora da matriz do mapa
	if ((y1 < 0) || (y2 > 10) || (x1 < 0) || (x2 > 10))
		return 1;
	else
		return 0;
}

int testa_colisao_monstro (int y1, int y2, int x1, int x2, int dir, int tipo)
{
	y1 = y1/16;
	y2 = y2/16;
	x1 = x1/16;
	x2 = x2/16;

	y1--;
	y2--;
	x1--;
	x2--;

	if (testa_fora_mapa (y1,y2,x1,x2))	//primeiramente testa se o monstro vai para fora do mapa
		return 1;

	switch (dir)				//dependendo da direcao, testa se o monstro pode ir
	{
		case (1):			//por exemplo, pra cima testa os cantos de cima
			if (tipo < 3)		//tipo 1 e 2 só andam em lugares limpos, tipo 3 atravessa pallets
			{
				if ((mapa.m[y1][x1].tipo != LIMPO) || (mapa.m[y1][x2].tipo != LIMPO))
					return 1;
			}
			else if (tipo == 3)
			{
				if ((mapa.m[y1][x1].tipo == MURO) || (mapa.m[y1][x2].tipo == MURO))
					return 1;
				if ((mapa.m[y1][x1].tipo >= BOMBA1) || (mapa.m[y1][x2].tipo >= BOMBA1))
					return 1;
			}
			break;

		case (2):
			if (tipo < 3)									
                        {
                        	if ((mapa.m[y2][x1].tipo != LIMPO) || (mapa.m[y2][x2].tipo != LIMPO))
                        		return 1;
                        }
                        else if (tipo == 3)
                        {
                        	if ((mapa.m[y2][x1].tipo == MURO) || (mapa.m[y2][x2].tipo == MURO))
					return 1;
				if ((mapa.m[y2][x1].tipo >= BOMBA1) || (mapa.m[y2][x2].tipo >= BOMBA1))
                        		return 1;
                        }

			break;

		case (3):
			if (tipo < 3)									
                        {
                        	if ((mapa.m[y1][x1].tipo != LIMPO) || (mapa.m[y2][x1].tipo != LIMPO))
                        		return 1;
                        }
                        else if (tipo == 3)
                        {
                        	if ((mapa.m[y1][x1].tipo == MURO) || (mapa.m[y2][x1].tipo == MURO))
					return 1;
				if ((mapa.m[y1][x1].tipo >= BOMBA1) || (mapa.m[y2][x1].tipo >= BOMBA1))	
                        		return 1;
                        }

			break;

		case (4):
			if (tipo < 3)									
                        {
                        	if ((mapa.m[y1][x2].tipo != LIMPO) || (mapa.m[y2][x2].tipo != LIMPO))
                        		return 1;
                        }
                        else if (tipo == 3)
                        {
                        	if ((mapa.m[y1][x2].tipo == MURO) || (mapa.m[y2][x2].tipo == MURO))
					return 1;
				if ((mapa.m[y1][x2].tipo >= BOMBA1) || (mapa.m[y2][x2].tipo >= BOMBA1))
                        		return 1;
                        }

			break;
	}

	return 0;
}

void move_monstro (int i, int tipo)
{
	int y1  = vmonstros.v[i].y;							//primeiramente pegando os cantos do monstro
	int y2  = y1 + 15;
	int x1  = vmonstros.v[i].x;
	int x2  = x1 + 15;
	int dir = vmonstros.v[i].dir;

	if ((tipo == 2)||(tipo == 3))							//se for do tipo 2 ou 3, ele pode
	{										//trocar de caminho ao ver uma esquina
		if (((dir == 1)||(dir == 2)) && (y1 % 32 == 16))
			vmonstros.v[i].dir = rand() % 2 + 3;
		else if (((dir == 3)||(dir == 4)) && (x1 % 32 == 16))
			vmonstros.v[i].dir = rand() % 2 + 1;
	}

	switch (vmonstros.v[i].dir)							//dependendo da direcao atual do monstro, testa se pode mover
	{
		case (1):
			if (! testa_colisao_monstro(y1-1, y2-1, x1, x2, dir, tipo))
				vmonstros.v[i].y--;
			else
				vmonstros.v[i].dir = rand() % 4 + 1;			//troca de direcao se nao conseguir mover
			break;

		case (2):
			if (! testa_colisao_monstro(y1+1, y2+1, x1, x2, dir, tipo))
				vmonstros.v[i].y++;
			else
                        	vmonstros.v[i].dir = rand() % 4 + 1;
			break;

		case (3):
			if (! testa_colisao_monstro(y1, y2, x1-1, x2-1, dir, tipo))
				vmonstros.v[i].x--;
			else
                        	vmonstros.v[i].dir = rand() % 4 + 1;
			break;

		case (4):
			if (! testa_colisao_monstro(y1, y2, x1+1, x2+1, dir, tipo))
				vmonstros.v[i].x++;
			else
                        	vmonstros.v[i].dir = rand() % 4 + 1;
			break;
	}
}

void move_monstros ()
{						//uma funcao que chama para todos os monstros a funcao que move eles
	int i;
	for (i=0 ; i<vmonstros.quant ; i++)
	{
		move_monstro (i, vmonstros.v[i].tipo);
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
        int sair     = 0;
        while (! sair)
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
        				sair = 1;
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

void inicia_musica ()
{
	musica    = al_load_audio_stream ("./resources/music.opus", 2, 2048);
        inicializa_geral (musica   , "musica");
        al_attach_audio_stream_to_mixer (musica, al_get_default_mixer());
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

		while ((! morte) && (! sair) && (!fechar))		//loop do jogo com as fases
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
	}

	destroi_sprites();
	destroi_final();

	return 0;
}
