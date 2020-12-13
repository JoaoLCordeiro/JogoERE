#include "libDefine.h"
#include "libBomMon.h"
#include "libTeste.h"

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
