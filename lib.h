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


//lib de iniciacao - funcoes que iniciam, resetam ou destroem estruturas

ALLEGRO_BITMAP* carrega_sprite (int x, int y, int w, int h);

void inicializa_geral (bool resultado, const char *testado);

void inicia_comeco ();

void inicia_sprites ();

void destroi_sprites ();

void inicia_mapa (int *portal_y, int *portal_x);

void destroi_final ();

void inicia_vbomba ();

void inicia_vmonstros ();

void malloc_mapa_vbomba_vmonstros();

void reseta_vbomba ();


//lib de desenho - funcoes que focam em desenho de sprites e menus			

void pre_escrita_display ();

void pos_escrita_display ();

void desenha_mapa ();

void anima_jogador (int *contframe, int *frame, int x1, int y1, int x2, int y2);

void desenha_monstros(int contframe);

void desenha_jogador (int imune, int jogador_y, int jogador_x, int frame, int dir);

void desenha_hud (int vidas);


//lib de teste - funcoes que testam ou verificam situacoes do jogo			

int  testa_mapa (int y, int x, int naBomba);

int  testa_colisao (int x1, int y1, int dir, int naBomba);

int  testa_queimadura (int y, int x);

int  testa_toque (int y1, int y2, int x1, int x2);

int  testa_colisao_upgd (int y, int x, int upgd);

int  verifica_passou (int y1, int x1, int p_y, int p_x);

int  testa_fora_mapa (int y1, int y2, int x1, int x2);

int  testa_colisao_monstro (int y1, int y2, int x1, int x2, int dir, int tipo);

int  esta_na_bomba (int jogador_y, int jogador_x);

void testa_morte (int y1, int x1, int *morte, int *vidas, int *imune);

void testa_upgrades (int y1, int x1);

void verifica_na_bomba (int *porBomba, int *naBomba, int jogador_y, int jogador_x);


//lib de bomba e monstros - funcoes focadas nas bombas e nos monstros

int  tem_monstro (int i, int y, int x);

void coloca_bomba ( int x, int y, int *bomba);

void passa_tempo_bombas ();

void passa_tempo_fogos  ();

void cria_explosao (int y, int x, int tamf, int dir);

void inicia_explosoes (int y, int x, int tamf);

void explode_bombas ();

void cria_monstros (int fase);

void mata_monstros ();

void move_monstro (int i, int tipo);

void move_monstros ();


//lib geral - funcoes gerais para o jogo											

void interpreta_controle (unsigned char *keyb, int *x, int *y, int *dir, int *bomba, int naBomba, int *pausa, int *ajuda);

void limpa_upgrades (int y1, int y2, int x1, int x2);

void liga_portal (int y, int x);

void faz_pausa (unsigned char *keyb, int *sair, ALLEGRO_EVENT *event);

void faz_menu (int *fechar, unsigned char *keyb, ALLEGRO_EVENT *event);

void faz_ajuda (unsigned char *keyb, ALLEGRO_EVENT *event);
