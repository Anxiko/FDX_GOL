/*
 * FDX_project/FDX_KD7
 *
 * FDX_GOL (Game of life)
 * Simulador del "Juego de la vida" de John Conway (http://es.wikipedia.org/wiki/Juego_de_la_vida)
 * Versión indev (25/8/2013, dd/mm/yy)
 * Versión alpha (28/8/2013)
 * Versión 0.1 (12/10/2013)
 * Versión 0.2 (15/10/2013)
 * Versión 0.3 (20/10/2013)
 *
 *
 * TO DO list:
 *
 * Guardado y carga de CellA a ficheros de archivo - Pospuesto
 *
 *
 * Copyright 2014 Joaquín Monteagudo Gómez <kindos7@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/*
 * Convenciones
 */

/*Posición de una célula en el array*/
/*
 * Aunque las células se disponen en un tablero de dos dimensiones,
 * se guardan en un array de una única dimensión (porque son más sencillos)
 * La disposición de las células en el array es secuencial, y se guardan
 * de izquierda a derecha, y de arriba a abajo. Así, si el tablero tiene
 * NCOL columnas, y la posición de la célula en el array unidimensional
 * es cnt, las coordenadas de la célula serán:
 * -Posición en X=cnt%NCOL
 * -Posición en Y=(int) cnt/NCOL
 * La forma más sencilla de acceder a dichas coordenadas es a través de macros.
 * Nótese que las constantes y variables aquí utilizadas no tiene por qué
 * ser las que aparezcan en el programa
 */

/*Células muertas y libres*/
/*
 * El array de células no es completamente dinámico, sino libre:
 * su tamaño es fijo mientras el tablero no cambie de tamaño, ya que
 * tamaño y tablero coinciden. Por ello, hasta las células muertas tienen
 * su posición, pero esta no está libre, y no debe ser sobreescrita.
 *
 */

/*Estado en la próxima generación*/
/*
 * Por defecto, el estado de una célula en la próxima generación
 * es como muerta. Sin embargo, esto no es importante, ya que ese estado
 * es siempre calculado justo antes de ser usado.
 */

/*Nº de vecinas*/
/*
 * Al contrario que en otras versiones del juego de la vida, en las que
 * el número de células vecinas posibles para que una célula permanezca
 * viva o nazca una nueva se guarda en un intervalo, en esta versión dichos
 * parámetros se guardan en un array. De está forma, y por poner un ejemplo,
 * en esta versión se puede configurar el que una célula permanezca viva
 * si tiene 2,4,5 o 6 vecinos, y sin embargo está célula morirá si tiene 3 vecinos.
 * Por ellos, se usan dos array's (uno para decidir cuando una célula sigue viva
 * y otro para decidir si una célula muerta vuelve a nacer). Dichos array's son
 * semidinámicos, lo que quiere decir que pueden cambiar durante el transcurso del
 * programa, pero solo si el usuario así lo desea.
 * Hay que tener en cuenta que existe un número máximo de células vecinas, pero la
 * posibilidad de alcanzar este límite es mínima, y depende del tipos de dato que sea
 * el array. También es preciso una variable para cada array que indique
 * el número de elementos de este. Dado que el array se guarda en un puntero,
 * si este es NULL, se considera vacío.
 */

/*
 * Directivas de preprocesador
 */

/*Inclusiones*/

//Biblioteca estandar de C
//Entrada/salida
#include <stdio.h>
//Valores aleatorios
#include <stdlib.h>
//Fuente de valores aleatorios
#include <time.h>

//SDL
//Gráficos 2D y eventos
#include "SDL1.2/SDL1.2/include/SDL.h"
//Texto por pantalla
#include "SDL1.2/SDL1.2_ttf/include/SDL_ttf.h"

//FDX
//Gestión de memoria y array's
#include "../lib/FDX_Aml/FDX_Aml.c"

/*Definiciones*/

//Definición de NULL
#ifndef NULL
#define NULL 0
#endif

//Máscaras de bits para los colores de SDL
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

//Definiciones del programa
//Título del programa
#define WTITLE "FDX_GOL"
//Icono del prograna
#define WICON "../data/FDX_GOL_Icon.bmp"
//Versión del programa
#define VERSION "v 0.3"

//Fuente
//Fuente para escribir en pantalla (Ubuntu)
#define FONTFILE "../data/Ubuntu-B.ttf"
//Tamaño de la fuente
#define FONTSIZE 15
//Nombre de la fuente
#define FONTNAME "Ubuntu"
//Longitud máxima del mensaje a copiar a pantalla
#define FONTMESSMAX (150+1)

//Tamaño de la ventana
//Ancho
#define MAINSCREENSIZEX 800
//Alto
#define MAINSCREENSIZEY 500
//BPP
#define MAINSCREENBPP 8

//Estados de CellS.stat1/2
//Viva
#define CELLSTATALIVE 1
//Muerta
#define CELLSTATDEAD 0

//Estados en texto de CellS
//Viva
#define CELLSTATTEXTALIVE "alive"
//Muerta
#define CELLSTATTEXTDEAD "dead"

//Tamaño (inicial) del array de células
//Tamaño en X (nº columnas)
#define CELLASIZEX 500
//Tamaño en Y (nº filas)
#define CELLASIZEY 500

//Superficie de las células (puede ser la superficie principal)
//Ancho de la pantalla
#define CELLASCREENSIZEX 500
//Alto de la pantalla
#define CELLASCREENSIZEY MAINSCREENSIZEY
//Posición X de la superficie (lado izquierdo)
#define CELLASCREENPOSX 0
//Posición Y de la superficie (lado superior)
#define CELLASCREENPOSY 0
//Grosor de la línea que separa las células
#define CELLASCREENSHEIGHT 1

//Zoom de CellA
//Ancho de la superficie
#define ZOOMSCREENSIZEX (MAINSCREENSIZEX-CELLASCREENSIZEX-CELLSZSIZEX)
//Alto de la superficie
#define ZOOMSCREENSIZEY (MAINSCREENSIZEY-TEXTSCREENSIZEY-CELLSZSIZEY)
//Posición X de la superficie
#define ZOOMSCREENPOSX (CELLASCREENSIZEX+CELLSZSIZEX/2)
//Posición Y de la superficie
#define ZOOMSCREENPOSY (TEXTSCREENSIZEY+CELLSASIZEY/2)
//Nº de columnas en CellZ
#define CELLZSIZEX (ZOOMSCREENSIZEX/CELLSZSIZEX)
//Nº de filas en CellZ
#define CELLZSIZEY (ZOOMSCREENSIZEY/CELLSZSIZEY)
//Ancho de célula en CellZ
#define CELLSZSIZEX (10)
//Alto de célula CellZ
#define CELLSZSIZEY (10)

//Superficie del texto
//Ancho de la superficie
#define TEXTSCREENSIZEX (MAINSCREENSIZEX-CELLASCREENSIZEX)
//Alto de la superficie
#define TEXTSCREENSIZEY (MAINSCREENSIZEY-300)
//Posición X de la superficie (lado izquierdo)
#define TEXTSCREENPOSX CELLASCREENSIZEX
//Posición Y de la superficie (lado superior)
#define TEXTSCREENPOSY 0
//Posición X (relativa a la superficie del texto) en la que se empieza a escribir el texto
#define TEXTPOSX 10
//Posición Y (relativa a la superficie del texto) en la que se empieza a escribir el texto
#define TEXTPOSY 10
//Incremento de la posición en cada salto de línea
#define TEXTNLY FONTSIZE

//Tamaño de las células en CellA
//Ancho de la célula
#define CELLSASIZEX (CELLASCREENSIZEX/CELLASIZEX)
//Alto de la célula
#define CELLSASIZEY (CELLASCREENSIZEY/CELLASIZEY)

//Opciones de formato
//Formato de impresión de coordenadas (rango)
#define FMTIMPCORDRNG 1

//Parámetros de supervivencia

//Parámetros de distancia
//Distancia en X a la que se tiene en cuenta una célula como vecina
#define PARSUPDISTX 1
//Distancia en Y a la que se tiene en cuenta una célula como vecina
#define PARSUPDISTY 1

//Definciones tipo booleano
//Sí
#define SI 1
//No
#define NO 0

//Máscara de 16 bits de control
//Se necesita dibujar la pantalla
#define MSKDRAW ((msk16b) 1)
//Se necesita avanzar una generación
#define MSKSADV ((msk16b) 2)
//Avance automático
#define MSKAADV ((msk16b) 4)
//Se dibuja la malla
#define MSKGRID ((msk16b) 8)
//Botón del ratón pulsado
#define MSKMPSD ((msk16b) 16)
//Ratón movido
#define MSKMMOV ((msk16b) 32)
//Botón izquierdo/derecho pulsado
#define MSKMBUT ((msk16b) 64)
//Botón izquierdo
#define MSKLBUT ((msk16b) 0)
//Botón derecho
#define MSKRBUT ((msk16b) 1)
//Desplegar información por pantalla
#define MSKDINF ((msk16b) 128)
//Protección ante entrada de datos
#define MSKPINP ((msk16b) 256)
//Uso de zoom
#define MSKZOOM ((msk16b) 512)

//Teclas de control
//Tecla de avance automático
#define CTRLKEYAADV SDLK_a
//Tecla de avance simple
#define CTRLKEYSADV SDLK_q
//Tecla de malla
#define CTRLKEYGRID SDLK_m
//Tecla de información
#define CTRLKEYDINF SDLK_n
//Tecla de formateo de generación
#define CTRLKEYRGEN SDLK_r
//Tecla de protección ante entrada indeseada
#define CTRLKEYPINP SDLK_p
//Tecla de zoom
#define CTRLKEYZOOM SDLK_z

//Botones de control
//Añadir nuevas células
#define CTRLMOSADD MSKLBUT
//Eliminar células
#define CTRLMOSDEL MSKRBUT

//Configuración inicial
#define MSKINIT (MSKDRAW|MSKDINF|MSKZOOM)

//Definiciones de control
//En desarrollo (pruebas)
#define INDEV SI

//Configuración de teclas de configuración aleatoria en del array
//Primera tecla
#define INITRNDFIRST SDLK_0
//Última tecla
#define INITRNDLAST SDLK_9
//Sumando de la fórmula
#define INITRNDADD (- SDLK_0)
//Dividendo de la fórmula
#define INITRNDDVD (10.0)

/*Macros*/

//Macros de posición en array
//Obtiene la coordenada X a partir de la posición en el array
#define GETXCORD(pos) ((fnele) (pos%CELLASIZEX))
//Obtiene la coordenada Y a partir de la posición en el array
#define GETYCORD(pos) ((fnele) (pos/CELLASIZEX))
//Obtiene la posición en el array unidimensional CellA a través de las coordenadas X e Y
#define GETPOSA(x, y) ((fnele) (x+y*CELLASIZEX))
//Obtiene la posición en el array unidimensional CellZ a través de las coordenadas X e Y
#define GETPOSZ(x, y) ((fnele) (x+y*CELLZSIZEX))

//Macros matemáticas
//Calcula la mitad y la devuelve como entera
#define HALF(x) ((int) x/2)

//Macros de estado de CellS
//Cambia el estado en número por una palabra
#define RPLTEXTSTATCELLS(x) ((x==CELLSTATALIVE)?CELLSTATTEXTALIVE:CELLSTATTEXTDEAD)

//Macros de campos de bits
//Pone todos los bits a 1 excepto el especificado (máscara de 8 bits)
#define NEGATE(x) ((msk16b) (~x))
//Cambia el valor del bit especificado
#define SWITCHBIT(value,BITMASK) (value+=(msk16b) value&BITMASK?-BITMASK:BITMASK)
//Comprueba el valor del bit especificado en el campo de bits dado
#define CHECKBIT(value, BITMASK) (value&BITMASK?1:0)
//Devuelve 1 si CellA está avanzando o 0 si no
#define GETADVSTAT (((settings&MSKSADV)||(settings&MSKAADV))!=0)

//Macros de inicialización del array
//Da el valor de probabilidad con el que llenar a CellA a través de la tecla pulsada
#define INITRNDPROBLTY(key) (((float)(key+INITRNDADD))/INITRNDDVD)

/*
 * Datos
 */

/*Tipos de datos*/

//Estructura de célula a tipo de dato
typedef struct Cell CellS;
//Coordendas de las células
typedef short unsigned int coord;
//Máscara de bits (8 bits)
typedef unsigned char msk8b;
//Máscara de bits (16bits)
typedef short unsigned int msk16b;
//Número de células
//No es necesario, para contar el número de elementos usamos datos tipo fnele
//Color
typedef Uint32 FDX_Color;

/*Estructuras*/

struct Cell
//Estructura de una célula
{
	msk8b stat1 :1;//Estado actual
	msk8b stat2 :1;//Próximo estado
};

/*Variables*/

//Variables de datos del programa
//Array de células
CellS *CellA=NULL;
//Array auxiliar de células para zoom
CellS *CellZ=NULL;
//Número de células en CellA
fnele EleCellA=CELLASIZEX*CELLASIZEY;
//Número de células en CellZ
fnele EleCellZ=CELLZSIZEX*CELLZSIZEY;
//Célula muerta
CellS CellDead={CELLSTATDEAD, CELLSTATDEAD};
//Célula viva
CellS CellAlive={CELLSTATALIVE, CELLSTATDEAD};

//Variables de parámetros
fnele *parsupra=NULL;//Array de número de células vecinas que permiten a una célula sobrevivir
fnele Eleparsupra=0;//Numero de elementos en el array de número de células vecinas que permiten que una célula sobreviva
fnele *parsupbn=NULL;//Array de número de células vecinas que permiten a una célula muerta nacer
fnele Eleparsupbn=0;//Número de elementos en el array de número de células vecinas que permiten a una célula muerta nacer

//Variables de SDL
//Pantalla principal
SDL_Surface *mainscreen=NULL;
//Superficie de células
SDL_Surface *cellascreen=NULL;
//Superficie del texto
SDL_Surface *textscreen=NULL;
//Superficie del zoom
SDL_Surface *zoomscreen=NULL;
//Color para células muertas
FDX_Color cellcolordead;
//Color para células vivas
FDX_Color cellcoloralive;
//Color de fondo para la superficie del texto (formato SDL)
SDL_Color textcolorbg;
//Color de la fuente del texto (formato SDL)
SDL_Color textcolorfg;
//Color para la malla que separa las células
FDX_Color gridcolor;
//Color para las células vivas del zoom
FDX_Color zoomcoloralive;
//Rectángulo de la pantalla principal
SDL_Rect mainrct;
//Rectángulo de la superficie de las células
SDL_Rect cellarct;
//Rectángulo de la superficie del texto
SDL_Rect textrct;
//Rectángulo de la superficie del zoom
SDL_Rect zoomrct;
//Evento de SDL
SDL_Event event;
//Fuente de la pantalla
TTF_Font *font=NULL;

//Variables de control y auxiliares
//Controla diversos parámetros en forma de máscara de 8 bits
msk16b settings=MSKINIT;
int xmos, ymos;//Variables del ratón
int amos;//Posición en CellA a la célula a la que apunta el ratón
int cnt;//Variable de uso auxiliar y general
char messbuff [FONTMESSMAX];//Buffer al que copiar los mensajes que van a pantalla
unsigned long int generation=0;//Generación de CellA


/*
 * Funciones
 */

/*Definiciones*/


double drand48 ()
//Genera números aleatorios flotantes de precisión doble entre [0,1] (no implementada en Windows por defecto)
{
    return ((double)rand())/RAND_MAX;
}

fnele getvecCellA (CellS CellA [], coord x, coord y, coord maxx, coord maxy)
//Devuelve el número de vecinos a la célula especicada
{
	if (CellA==NULL) return 0;//Si el array es nulo o vacío, no tiene vecinos
	fnele nvec=0;//Número de células vecinas, 0 por defecto
	int cntx, cnty;//Variable de subíndice en X e Y
	for (cntx=x-PARSUPDISTX; cntx<=x+PARSUPDISTX; cntx++)//Se cicla por columnas
	{
		//Se comprueba que lo coordenada x sea viable
		if (cntx>=0&&cntx<maxx)
		{
			for (cnty=y-PARSUPDISTY; cnty<=y+PARSUPDISTY; cnty++)//Dentro de cada columna, se cicla por filas
			{
				//Se comprueba que la coordenada Y sea viable, que no se esté usando a la propia célula y que esta esté viva
				if (cnty>=0&&cnty<maxy&&(cntx!=x||cnty!=y)&&CellA[GETPOSA(cntx,cnty)].stat1==CELLSTATALIVE)nvec++;
			}
		}
	}
	return nvec;
}

int find_array (fnele array [], fnele EleArray, fnele ele)
//Busca el elemento (ele) en el array y devuelve 1 si lo encuentra y 0 en caso contrario
{
	if (array==NULL||EleArray==0)return 0;//Si el array es nulo o vacío, el elemento no se encuentra en él
	int cnt;//Variable de subíndice
	for (cnt=0; cnt<EleArray; cnt++)
	{
		if (array[cnt]==ele)return 1;//Si se encuentra el elemento, se devuelve 1
	}
	return 0;//Si se llega a este punto, el elemento buscado no está en el array, por tanto se devuelve 0
}

int getstat2CellS (CellS CellA [], coord x, coord y, coord maxx, coord maxy, fnele parsupra [], fnele Eleparsupra, fnele parsupbn [], fnele Eleparsupbn)
//Obtiene el próximo estado de una célula de CellA indicada por sus coordenadas
{
	if (CellA==NULL||x>=maxx||y>=maxy) return -1;//Se comprueban los argumentos
	//El próximo estado de la célula depende de su estado actual
	if (CellA[GETPOSA(x,y)].stat1==CELLSTATALIVE)//Si está viva
	{
		//Se busca en el array de permancer vivo
		return find_array(parsupra, Eleparsupra,
		getvecCellA (CellA, x, y, maxx, maxy))?CELLSTATALIVE:CELLSTATDEAD;
	}
	else//Si no está vivo, está muerto
	{
		//Se busca en el array de nacer
		return find_array(parsupbn, Eleparsupbn,
		getvecCellA (CellA, x, y, maxx, maxy))?CELLSTATALIVE:CELLSTATDEAD;
	}
	return 0;
}

int setstat2CellA (CellS CellA [], coord maxx, coord maxy, fnele parsupra [], fnele Eleparsupra, fnele parsupbn [], fnele Eleparsupbn)
//Fija el próximo valor de todas las células de CellA
{
	if (CellA==NULL||maxx==0||maxy==0) return -1;//Si el array es nulo o vacío, se devuelve error
	int cnt;//Variable de subíndice
	int EleCellA=maxx*maxy;//Se calculan los elementos del array
	int rv;//Recoge el valor de retorno de una función para comprobarlo
	for (cnt=0; cnt<EleCellA;cnt++)
	{
		rv=getstat2CellS (CellA, GETXCORD(cnt), GETYCORD(cnt), maxx, maxy, parsupra, Eleparsupra, parsupbn, Eleparsupbn);//Se recoge el próximo estado
		if (rv>=0)//Si el valor es válido
		CellA[cnt].stat2=rv;
		else//Si el valor es inválido
		return -2;//Se devuelve el error
	}
	return 0;//Todo correcto
}

int updstatCellA (CellS CellA [], fnele EleCellA)
//Actualiza el array de células a la siguiente generación
{
	if (CellA==NULL||EleCellA==0) return -1;//Array nulo o vacío
	int cnt;//Variable de subíndice
	for (cnt=0; cnt<EleCellA; cnt++)
	CellA[cnt].stat1=CellA[cnt].stat2;//Se actualiza el estado
	generation++;//Se aumenta el número de la generación
	return 0;//Todo correcto
}

int drawCellA (CellS CellA [], fnele EleCellA, FDX_Color cellcolordead, FDX_Color cellcoloralive, SDL_Surface *screen, fsele cellssizex, fsele cellssizey, fnele cellasizex, fnele cellasizey)
//Dibuja todo el array de células
{
	if (CellA==NULL||EleCellA==0||screen==NULL)return -1;//Se comprueban los argumentos
	SDL_FillRect (screen, NULL, cellcolordead);//Se borra la pantalla, y solo se dibujan las células vivas
	int cnt;//Varible de subíndice
	SDL_Rect rctcell;//Rectángulo a rellenar
	rctcell.w=cellssizex;//Ancho de la célula
	rctcell.h=cellssizey;//Alto de la célula
	for (cnt=0; cnt<EleCellA; cnt++)
	{
		if (CellA[cnt].stat1==CELLSTATDEAD) continue;//Si la célula está muerta, se salta a la siguiente
		rctcell.x=((fnele) (cnt%cellasizex))*cellssizex;//Se obtiene la posición X
		rctcell.y=((fnele) (cnt/cellasizex))*cellssizey;//Se obtiene la posición Y
		SDL_FillRect (screen, &rctcell, cellcoloralive);//Se dibuja con el color correspondiente
	}
	return 0;
}

int setrndCellA (CellS CellA [], fnele EleCellA, float posalive)
//LLena el array de valores aleatorios (la posibilidad en tanto por 1 de que una célula salga viva se guarda en posalive)
{
	if (CellA==NULL||EleCellA==0)return -1;//Se comprueban los argumentos
	int cnt;//Variable de subíndice
	if (posalive>0&&posalive<1)//Se comprueba que no sea ni 1 ni 0, se usan valores aleatorios
	{
		for (cnt=0; cnt<EleCellA; cnt++)
		CellA[cnt].stat1=drand48()<posalive?CELLSTATALIVE:CELLSTATDEAD;
		return 0;
	}
	else
	{
		int stat=posalive>=1?CELLSTATALIVE:CELLSTATDEAD;//Dado que el estado es constante, solo se calcula una vez
		for (cnt=0; cnt<EleCellA; cnt++)
		CellA[cnt].stat1=stat;
		return 0;
	}
}

int posmos (int x, int y)
//Devuelve la posición en el array a la que apunta el ratón o -1 en caso de error
{
	//Primero se adapta las coordendas, dado que la superficie de las células puede estar desplazada
	x-=CELLASCREENPOSX; y-=CELLASCREENPOSY;
	if (x<0||y<0||x>=CELLASCREENSIZEX||y>=CELLASCREENSIZEY)//Si no está apuntando a la superficie, no apunta a ninguna célula
	return -1;//No apunta a ninguna posición
	fnele pos=GETPOSA((fnele) x/CELLSASIZEX, y/CELLSASIZEY);//Se calcula la posición
	if (pos>=CELLASIZEX*CELLASIZEY) return -2;//Se comprueba el valor que se va a devolver
	return pos;//Se devuelve el valor
}

int modCellS (CellS CellA [], int pos, int stat)
//Modifica el estado de la célula indicada al estado indicado
{
	if (pos<0)return -1;
	if (stat==CELLSTATALIVE||stat==CELLSTATDEAD)//Se comprueba el valor
	{
		CellA[pos].stat1=stat;//Se asigna el estado
		return 0;//Todo correcto
	}
	else return -2;
}

void drawgridCellA (int x, int y, fnele cellax, fnele cellay, unsigned int cellsx, unsigned int cellsy, unsigned int strokeheight, SDL_Surface *screen, FDX_Color color)
//Dibuja la malla que separa a las células
{
	int cnt;//Variable de subíndice
	int maxx=cellax*cellsx+HALF(strokeheight), maxy=cellay*cellsy+HALF(strokeheight);//Ancho y alto de la superficie
	SDL_Rect rct;//Rectángulo que se colorea

	//Barras verticales

	//Se configura el rectángulo
	rct.w=strokeheight;
	rct.h=maxy;
	rct.x=x-HALF(strokeheight);
	rct.y=y;
	for (cnt=0; cnt<=cellax; cnt++)//Ciclo para dibujar las barras verticales
	{
		SDL_FillRect (screen, &rct, color);//Se dibuja la línea
		rct.x+=cellsx;
	}

	//Barra horizontales

	//Se configura el rectángulo
	rct.w=maxx;
	rct.h=strokeheight;
	rct.x=x;
	rct.y=y-HALF(strokeheight);
	for (cnt=0; cnt<=cellay; cnt++)//Ciclo para dibujar las barras horizontales
	{
		SDL_FillRect (screen, &rct, color);
		rct.y+=cellsy;
	}
	return;
}

void drawcross (int xc, int yc, unsigned int height, int sizex, int sizey, SDL_Surface *srf, FDX_Color col)
//Dibuja una cruz en la superficie especificada
{
	if (srf==NULL||sizex<=0||sizey<=0) return;//Se comprueban los argumentos
	SDL_Rect hoz, vec;//Rectángulos horizontales y verticales

	//Se configura el rectángulo horizontal
	hoz.x=xc-sizex/2;
	hoz.y=yc-height/2;
	hoz.w=sizex;
	hoz.h=height;

	//Se configura el rectángulo vertical
	vec.x=xc-height/2;
	vec.y=yc-sizey/2;
	vec.h=sizey;
	vec.w=height;

	//Se dibujan los rectángulos
	SDL_FillRect (srf, &hoz, col);//Horizontal
	SDL_FillRect (srf, &vec, col);//Vertical
	return;
}

void printtexttosurf (char *string, SDL_Surface *textscreen, int xpos, int ypos)
//Imprime el texto dado a la superficie especificada
{
	//Primero se comprueban los argumentos
	if (string==NULL||string[0]=='\0'||textscreen==NULL) return;
	SDL_Surface *auxtextscreen=NULL;//Puntero a superficie (auxiliar)
	SDL_Rect auxtextrct;//Rectángulo de la superficie del texto (auxiliar)
	//Se configura el rectángulo
	auxtextrct.x=xpos;
	auxtextrct.y=ypos;

	//Se renderiza el texto
	auxtextscreen=TTF_RenderText_Shaded (font, messbuff, textcolorfg, textcolorbg);
	if (auxtextscreen!=NULL)//Si la superficie creada no es nula
	{
		SDL_BlitSurface (auxtextscreen, NULL, textscreen, &auxtextrct);//Se copia la superficie al puntero dado
		SDL_FreeSurface (auxtextscreen);//Se libera la superficie recién creada (ya ha sido copiado)
	}
}

void displinf (int xrat, int yrat, SDL_Surface *textscreen)
//Imprime toda la información relativa a la célula a la que apunta el ratón
{
	//Se comprueban los argumentos
	if (textscreen==NULL) return;
	unsigned long int xpos, ypos, apos;//Posición X e Y de la célula en el tablero, y la posición en el array
	int textypos=TEXTPOSY;//Posición Y en la que se escribe el texto

	//Se establecen las coordenadas
	apos=posmos (xmos, ymos);//Se obtiene la posición de la célula a la que apunta
	xpos=GETXCORD(apos);//Se obtiene la coordenada X de la célula
	ypos=GETYCORD(apos);//Se obtiene la coordenada Y de la célula

	//Se imprime el estado actual
	if (GETADVSTAT) sprintf (messbuff, "Status: running (%s)", (settings&MSKAADV)?"auto":"single");//Avanzando
	else sprintf (messbuff, "Status: paused");//Parado
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Se imprime el número de generación
	sprintf (messbuff, "Generation = %lu", generation);
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Se imprime si CellA está o no protegida
	if (settings&MSKPINP)
	{
		sprintf (messbuff, "CellA is protected");
		printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
		textypos+=TEXTNLY;
	}

	//Se imprime si se usa el zoom o no
	sprintf (messbuff, "Zoom is%s", settings&MSKZOOM?" enabled":" not enabled");
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Se imprime la posición y los estados de la célula
	if (apos<0||apos>=CELLASIZEX*CELLASIZEY)//Si la célula no está en el tablero
	{
		//Se imprime un mensaje de advertencia. No hay más mensajes. Se sale de la función
		sprintf (messbuff, "Not pointing at any cell");
		printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
		return;
	}

	//Se imprime la posición
	sprintf (messbuff, "Cell position");
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Posición X
	sprintf (messbuff, "X [%i/%i] = %lu)",
	FMTIMPCORDRNG, CELLASIZEX-1+FMTIMPCORDRNG, xpos+FMTIMPCORDRNG);
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Posición Y
	sprintf (messbuff, "Y [%i/%i] = %lu)",
	FMTIMPCORDRNG, CELLASIZEY-1+FMTIMPCORDRNG, ypos+FMTIMPCORDRNG);
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Posición A
	sprintf (messbuff, "A [%i/%i] = %lu)",
	FMTIMPCORDRNG, CELLASCREENSIZEX*CELLASIZEY-1+FMTIMPCORDRNG, apos+FMTIMPCORDRNG);
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Estado de la célula
	sprintf (messbuff, "Cell status");
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Estado actual
	sprintf (messbuff, "Current status: %s", RPLTEXTSTATCELLS(CellA[apos].stat1));
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Número de vecinas
	sprintf (messbuff, "Number of neighbours = %i", getvecCellA (CellA, xpos, ypos, CELLASIZEX, CELLASIZEY));
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	textypos+=TEXTNLY;

	//Estado en la próxima generación
	sprintf (messbuff, "Next status = %s", RPLTEXTSTATCELLS(getstat2CellS (CellA, xpos, ypos, CELLASIZEX, CELLASIZEY,
	parsupra, Eleparsupra, parsupbn, Eleparsupbn)));
	printtexttosurf (messbuff, textscreen, TEXTPOSX, textypos);
	return;
}

void getCellZ (CellS *CellA, CellS *CellZ, coord xcent, coord ycent, fnele CellAsizex, fnele CellAsizey, fnele CellZsizex, fnele CellZsizey)
//Copia una zona de CellA a CellZ, especificada por el centro de (xcent,ycent)
{
	//Se comprueban los argumentos
	if (CellA==NULL||CellZ==NULL||CellAsizex<1||CellAsizey<1||CellZsizex<1||CellZsizey<1) return;

	//Se formatea el array (todos muertos)
	setrndCellA (CellZ, EleCellZ, 0);

	int cntx, cnty;//Contador X e Y
	int Axpos, Aypos;//Posición de la célula en el array CellA

	for (cntx=0; cntx<CellZsizex; cntx++)//Se circula por X
	{
		//Se obtiene la coordenada X dentro de CellA
		Axpos=xcent-(CellZsizex-1)/2+cntx;
		//Si no es válida, se salta a la siguiente coordenada X
		if (Axpos<0||Axpos>=CellAsizex)continue;
		for (cnty=0; cnty<CellZsizey; cnty++)//Se circula por Y
		{
			//Se obtiene la coordenada Y dentro de CellA
			Aypos=ycent-(CellZsizey-1)/2+cnty;
			//Si no es válida, se salta a la siguiente coordenada Y
			if (Aypos<0||Aypos>=CellAsizey)continue;
			//Si todo es correcto, se copian las células
			CellZ[GETPOSZ(cntx, cnty)]=CellA[GETPOSA(Axpos, Aypos)];
		}
	}
}

void exitprogram ()
//Realiza todos los procedimientos necesarios antes de salir del programa
{
	//Se imprime el mensaje de aviso
	printf ("\nExiting...\n");
	//Se libera el array de células
	free (CellA);
	//Se libera el array de zomm
	free (CellZ);
	//Se liberan los array dinámicos de números de células vecinas
	free (parsupbn);
	free (parsupra);
	//Se cierra la librería SDL_ttf y la fuente en uso
	TTF_CloseFont (font);
	TTF_Quit ();
	//Se cierra la librería SDL
	SDL_Quit();
	return;
}

/*
 * Función principal
 */

int main (void)
{
	/*Bienvenida*/

	printf ("FDX_GOL (%s) starting...\n", VERSION);

	/*Configuración*/

	//Se inicializa la fuente de valores aleatorios
	puts ("Inicializating random seed");
	srand (time (NULL));

	//Se crea el array principal
	puts ("Creating CellA");
	CellA= (CellS *) calloc (EleCellA, sizeof (CellS));
	if (CellA==NULL)//Se comprueba el puntero devuelto
	{
		printf ("No se pudo crear el array principal de %u elemento(s) (%lu bytes cada uno)\n", EleCellA, sizeof (CellS));
		return 1;//Se devuelve error
	}

	//Se formatea el array principal
	puts ("Formatting CellA");
	setrndCellA (CellA, EleCellA, 0);

	//Se crea el array del zoom
	puts ("Creating CellZ");
	CellZ= (CellS *) calloc (EleCellZ, sizeof (CellS));
	if (CellZ==NULL)//Se comprueba el puntero devuelto
	{
		printf ("No se pudo crear el array del zoom de %u elemento(s) (%lu bytes cada uno)\n", EleCellZ, sizeof (CellS));
		return 1;//Se devuelve error
	}

	//Se formatea el array del zoom
	puts ("Formatting CellZ");
	setrndCellA (CellZ, EleCellZ, 0);

	//Se inicializa SDL
	puts ("Starting SDL");
	if (SDL_Init (SDL_INIT_VIDEO)<0)
	{
		printf ("No se pudo inicializar el sistema de vídeo de SDL: %s\n", SDL_GetError());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se pone el título y el icono a la ventana
	puts ("Setting window title and icon");
	SDL_WM_SetCaption (WTITLE, NULL);//Se pone el título
	SDL_Surface *icon;//Superficie del icono
	icon=SDL_LoadBMP (WICON);//Se lee el icono
	if (icon==NULL)//Se compruebe el icono
	{
		printf ("Couldn't load icon surface in %s: %s\n", WICON, SDL_GetError ());
		printf ("Not a fatal error, so not exiting\n");
	}
	else
	SDL_WM_SetIcon (icon, 0);

	//Se crea la pantalla principal
	puts ("Creating main screen");
	mainscreen=SDL_SetVideoMode (MAINSCREENSIZEX, MAINSCREENSIZEY, MAINSCREENBPP, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_ANYFORMAT);
	if (mainscreen==NULL)
	{
		printf ("No se pudo crear una pantalla de %ix%i a %i BPP: %s\n", MAINSCREENSIZEX, MAINSCREENSIZEY, MAINSCREENBPP, SDL_GetError());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se inicializa el sistema de texto por pantalla (SDL_ttf)
	puts ("Starting SDL_ttf");
	if (TTF_Init ()<0)
	{
		printf ("No se puedo abrir el sistema de texto en pantalla (SDL_ttf): %s\n", SDL_GetError ());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se carga la fuente para SDL_ttf (Ubuntu)
	printf ("Loading %s font\n", FONTNAME);
	font=TTF_OpenFont (FONTFILE, FONTSIZE);
	if (font==NULL)
	{
		printf ("No se pudo cargar la fuente %s desde %s, a tamaño %i", FONTNAME, FONTFILE, FONTSIZE);
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se crea la superficie para CellA
	puts ("Creating CellA surface");
	cellascreen=SDL_CreateRGBSurface(SDL_HWSURFACE, CELLASCREENSIZEX, CELLASCREENSIZEY, MAINSCREENBPP, RMASK, GMASK, BMASK, 0);
	if  (cellascreen==NULL)
	{

		printf ("No se pudo crear la superficie para guardar las células de %ix%i a %iBPP: %s\n", CELLASCREENSIZEX, CELLASCREENSIZEY, MAINSCREENBPP, SDL_GetError ());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se convierte la superficie para CellA al mismo formato de la pantalla
	puts ("Converting CellA surface");
	cellascreen=SDL_ConvertSurface (cellascreen, mainscreen->format, SDL_HWSURFACE);
	if (cellascreen==NULL)
	{
		printf ("No se pudo convertir la superficie de las células al formato de la superfcie principal: %s\n", SDL_GetError());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se crea la superficie para el texto
	puts ("Creating text surface");
	textscreen=SDL_CreateRGBSurface (SDL_HWSURFACE, TEXTSCREENSIZEX, TEXTSCREENSIZEY, MAINSCREENBPP, RMASK, GMASK, BMASK, 0);
	if  (textscreen==NULL)
	{
		printf ("No se pudo crear la superficie para el texto de %ix%i a %iBPP: %s\n", TEXTSCREENSIZEX, TEXTSCREENSIZEY, MAINSCREENBPP, SDL_GetError ());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se convierte la superficie para el texto al mismo formato de la pantalla
	puts ("Converting text surface");
	textscreen=SDL_ConvertSurface (textscreen, mainscreen->format, SDL_HWSURFACE);
	if (textscreen==NULL)
	{
		printf ("No se pudo convertir la superficie del texto al formato de la superfcie principal: %s\n", SDL_GetError());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se crea la superfice para el zoom
	puts ("Creating zoom surface");
	zoomscreen=SDL_CreateRGBSurface (SDL_HWSURFACE, ZOOMSCREENSIZEX, ZOOMSCREENSIZEY, MAINSCREENBPP, RMASK, GMASK, BMASK, 0);
	if  (zoomscreen==NULL)
	{
		printf ("No se pudo crear la superficie para el zoom de %ix%i a %iBPP: %s\n", ZOOMSCREENSIZEX, ZOOMSCREENSIZEY, MAINSCREENBPP, SDL_GetError ());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	//Se convierte la superficie para el zoom al mismo formato de la pantalla
	puts ("Converting zoom surface");
	zoomscreen=SDL_ConvertSurface (zoomscreen, mainscreen->format, SDL_HWSURFACE);
	if (zoomscreen==NULL)
	{
		printf ("No se pudo convertir la superficie del zoom al formato de la superfcie principal: %s\n", SDL_GetError());
		//Se sale del programa
		exitprogram ();
		return 1;
	}

	puts ("Configuring colors");
	//Se establecen los colores
	//Célula muerta=negro
	cellcolordead=SDL_MapRGB (mainscreen->format, 0, 0, 0);
	//Célula viva=blanca
	cellcoloralive=SDL_MapRGB (mainscreen->format, 255, 255, 255);
	//Color de fondo del texto
	textcolorbg.r=0;
	textcolorbg.g=0;
	textcolorbg.b=0;
	//Color de la fuente del texto
	textcolorfg.r=100;
	textcolorfg.g=255;
	textcolorfg.b=100;
	//Malla de células
	gridcolor=SDL_MapRGB (mainscreen->format, 0, 255, 0);
	//Células del zoom vivas
	zoomcoloralive=SDL_MapRGB (mainscreen->format, 100, 100, 255);

	puts ("Setting survival parameters");
	//Se establecen los parámetros de supervivencia
	//Células vecinas para sobrevivir
	Eleparsupra=2;
	parsupra=calloc (Eleparsupra, sizeof (fnele));
	if (parsupra==NULL)
	{
		printf ("No se pudo crear el array dinámico parsupra [%i]\n", Eleparsupra);
		exitprogram ();
		return 1;
	}
	parsupra[0]=2;
	parsupra[1]=3;


	//Células vecinas para nacer una nueva
	Eleparsupbn=1;
	parsupbn=calloc (Eleparsupbn, sizeof (fnele));
	if (parsupbn==NULL)
	{
		printf ("No se pudo crear el array dinámico parsupbn [%i]\n", Eleparsupbn);
		exitprogram ();
		return 1;
	}
	parsupbn[0]=3;

	//Se configuran los rectángulos de las pantallas

	puts ("Setting surfaces rects'");
	//Pantalla principal
	mainrct.x=mainrct.y=0;
	mainrct.w=MAINSCREENSIZEX;
	mainrct.h=MAINSCREENSIZEY;

	//Superficie de las células
	cellarct.x=CELLASCREENPOSX;
	cellarct.y=CELLASCREENPOSY;
	cellarct.w=CELLASCREENSIZEX;
	cellarct.h=CELLASCREENSIZEY;

	//Superficie del texto
	textrct.x=TEXTSCREENPOSX;
	textrct.y=TEXTSCREENPOSY;
	textrct.w=TEXTSCREENSIZEX;
	textrct.h=TEXTSCREENSIZEY;

	//Superficie del zoom
	zoomrct.x=ZOOMSCREENPOSX;
	zoomrct.y=ZOOMSCREENPOSY;
	zoomrct.w=ZOOMSCREENSIZEX;
	zoomrct.h=ZOOMSCREENSIZEY;

	puts ("Finished loading");

	/*Bucle principal*/

	while (1)
	{

		//Estados por defecto
		//El ratón no se ha movido
		settings&=NEGATE(MSKMMOV);

		//Formateo de pantalla
		SDL_FillRect (mainscreen, NULL, cellcolordead);//Pantalla principal
		SDL_FillRect (textscreen, NULL, cellcolordead);//Pantalla del texto
		SDL_FillRect (zoomscreen, NULL, cellcolordead);//Pantalla del zoom

		//Se recogen y procesan los eventos
		while (SDL_PollEvent (&event))
		{
			//Switch en función del tipo de evento
			switch (event.type)
			{
				//Se pide salir
				case SDL_QUIT:
				{
					if (settings&MSKPINP)//Si el sistema está bloqueado, no se puede salir
					break;
					exitprogram ();//Se sale de la función
					return 0;
				}
				//Ratón movido
				case SDL_MOUSEMOTION:
				{
					//Se guarda el ratón movido
					settings|=MSKMMOV;
					//Se guarda la posición
					xmos=event.motion.x;
					ymos=event.motion.y;
					break;
				}

				//Botón ratón pulsado
				case SDL_MOUSEBUTTONDOWN:
				{
					//Se comprueba que el botón pulsado sea útil
					if (event.button.button==SDL_BUTTON_LEFT||event.button.button==SDL_BUTTON_RIGHT)
					{
						//Se establece el botón como pulsado (y movido)
						settings|=MSKMPSD;
						settings|=MSKMMOV;
						//Botón izquierdo
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							if (MSKLBUT)//Si el botón se configura como sí
							settings|=MSKMBUT;
							else//Si el botón se configura como no
							settings&=NEGATE(MSKMBUT);
						}
						//Botón derecho
						else
						{
							if (MSKRBUT)//Si el botón se configura como sí
							settings|=MSKMBUT;
							else//Si se configura como no
							settings&=NEGATE(MSKMBUT);
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					//Se comprueba si se ha levantado un botón útil
					if (settings&MSKMPSD)
					{
						//Solo se levanta el botón si el botón levantado es el guardado
						if (
							((event.button.button==SDL_BUTTON_LEFT)&&(CHECKBIT(settings, MSKMBUT)==MSKLBUT))
							||
							((event.button.button==SDL_BUTTON_RIGHT)&&(CHECKBIT(settings, MSKMBUT)==MSKRBUT))
						   )
						settings&=NEGATE(MSKMPSD);
					}
					break;
				}
				//Tecla pulsada
				case SDL_KEYDOWN:
				{
					//Menú en función de la tecla
					switch (event.key.keysym.sym)
					{
						//Avance automático
						case CTRLKEYAADV:
						{
							SWITCHBIT(settings, MSKAADV);
							break;
						}
						//Avance simple
						case CTRLKEYSADV:
						{
							settings|=MSKSADV;
							break;
						}
						//Dibujar malla o no
						case CTRLKEYGRID:
						{
							SWITCHBIT(settings, MSKGRID);
							break;
						}
						//Desplegar información o no
						case CTRLKEYDINF:
						{
							SWITCHBIT(settings, MSKDINF);
							break;
						}
						//Poner a cero el contador de células
						case CTRLKEYRGEN:
						{
							if (!(settings&MSKPINP))
							generation=0;
							break;
						}
						//Proteger CellA ante entrada de teclado/ratón
						case CTRLKEYPINP:
						{
							SWITCHBIT (settings, MSKPINP);
							break;
						}
						//Usar el zoom
						case CTRLKEYZOOM:
						{
							SWITCHBIT (settings, MSKZOOM);
							break;
						}
						//Otras opciones
						default:
						{
							//Inicialización con valores del 0% al 90%
							if ((!(settings&MSKPINP))&&event.key.keysym.sym>=INITRNDFIRST&&event.key.keysym.sym<=INITRNDLAST)
							{
								setrndCellA (CellA, EleCellA, INITRNDPROBLTY(event.key.keysym.sym));
								break;

							}
							break;
						}
					}
					break;
				}
			}
		}

		//Se insertan células, si procede
		if (settings&MSKMPSD&&settings&MSKMMOV&&(!(settings&MSKPINP))&&posmos (xmos, ymos)>=0&&posmos (xmos, ymos)<CELLASIZEX*CELLASIZEY)
		{
			modCellS (CellA, posmos (xmos, ymos), CHECKBIT(settings, MSKMBUT)==CTRLMOSADD?CELLSTATALIVE:CELLSTATDEAD);
		}

		//Se dibujan las células y la malla, si procede
		if (settings&MSKDRAW)
		{
			//Dibujar las células
			drawCellA (CellA, EleCellA, cellcolordead, cellcoloralive, cellascreen, CELLSASIZEX, CELLSASIZEY, CELLASIZEX, CELLASIZEY);

			if (settings&MSKGRID)//Se dibuja la malla, si procede
			drawgridCellA (CELLASCREENPOSX, CELLASCREENPOSY, CELLASCREENSIZEX,
			CELLASCREENSIZEY, CELLSASIZEX, CELLSASIZEY, CELLASCREENSHEIGHT,
			cellascreen, gridcolor);

			if (settings&MSKDINF)//Se despliega información de la célula por pantalla, si procede
			displinf (xmos, ymos, textscreen);

			if (settings&MSKZOOM)//Se usa el zoom, si procede
			{
				amos=posmos (xmos, ymos);//Se obtiene la coordenada
				if (amos>=0&&amos<(EleCellA))//Se comprueba la coordenada
				{
					getCellZ (CellA, CellZ, GETXCORD (amos), GETYCORD (amos), CELLASIZEX, CELLASIZEY, CELLZSIZEX, CELLZSIZEY);
					drawCellA (CellZ, EleCellZ, cellcolordead, zoomcoloralive, zoomscreen, CELLSZSIZEX, CELLSZSIZEY, CELLZSIZEX, CELLZSIZEY);
					drawgridCellA (0, 0, CELLZSIZEX, CELLZSIZEY, CELLSZSIZEX, CELLSZSIZEY, CELLASCREENSHEIGHT, zoomscreen, gridcolor);
					drawcross (ZOOMSCREENSIZEX/2, ZOOMSCREENSIZEY/2, 1, CELLSZSIZEX, CELLSZSIZEY, zoomscreen, gridcolor);
				}
			}

			//Renderizado de pantallas
			//Se copian las superficies
			SDL_BlitSurface (cellascreen, NULL, mainscreen, &cellarct);//Superficie de las células
			if (settings&MSKDINF)SDL_BlitSurface (textscreen, NULL, mainscreen, &textrct);//Superficie del texto
			if (settings&MSKZOOM)SDL_BlitSurface (zoomscreen, NULL, mainscreen, &zoomrct);//Superficie del zoom
			//Se dibuja la pantalla
			SDL_Flip (mainscreen);
		}
		if ((settings&MSKSADV)||(settings&MSKAADV))//Se avanza de generación si procede
		{
			//Se apaga el avance único
			settings&=NEGATE(MSKSADV);
			//Se obtienen los próximos estados
			setstat2CellA (CellA, CELLASIZEX, CELLASIZEY, parsupra, Eleparsupra, parsupbn, Eleparsupbn);
			//Se actualiza a la próxima generación
			updstatCellA (CellA, EleCellA);
		}
	}
	return 0;//Fin del programa
}
