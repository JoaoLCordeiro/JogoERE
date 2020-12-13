#include "libDefine.h"
#include "libTeste.h"
#include "libGeral.h"

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

int testa_colisao_upgd (int y, int x, int upgd)
{					//funcao que testa se o player colidiu com um upgrade
	if (mapa.m[y][x].upgd == upgd)
		return 1;
        else
        	return 0;
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

int esta_na_bomba (int jogador_y, int jogador_x)	//testa se o jogador esta na bomba
{
	if (((jogador_x + 3)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 5)/16 == vbomba.v[ vbomba.quant-1 ].y))
		return 1;
	if (((jogador_x + 12)/16 == vbomba.v[ vbomba.quant-1 ].x) && ((jogador_y + 15)/16 == vbomba.v[ vbomba.quant-1 ].y))
		return 1;
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
