#ifndef __INDEF__
#define __INDEF__

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

#endif
