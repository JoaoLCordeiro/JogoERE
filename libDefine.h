#include <allegro5/allegro5.h>			
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

#ifndef __DFDEF__
#define __DFDEF__

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

t_sprites 		sprites;
t_mapa 	  		mapa;
t_vmonstros		vmonstros;
t_vbomba		vbomba;
int 			pontos;

#endif
