#include "libDefine.h"
#include <stdio.h>
#include "libInit.h"

ALLEGRO_BITMAP* carrega_sprite (int x, int y, int w, int h)
{
	ALLEGRO_BITMAP* sprite = al_create_sub_bitmap (sprites.sheet, x, y, w, h);
	inicializa_geral (sprite, "carrega sprite");
	return sprite;
}

void inicializa_geral (bool resultado, const char *testado)
{
	if (resultado)
		return;
	else
	{
		fprintf (stderr,"Erro ao inicializar %s\n", testado);
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

void destroi_final ()
{			//destroi tudo no final
	al_destroy_timer	(timer    );
	al_destroy_display	(display  );
	al_destroy_bitmap	(buffer   );
	al_destroy_event_queue  (queue    );
	al_destroy_sample       (sRobo    );
	al_destroy_sample       (sExplosao);
	al_destroy_sample       (sBomba   );
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
