#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include "Config.h"
#include "linInterp.h"
//typedef unsigned char uint8_t;
//typedef unsigned int uint16_t;
#define true 1;
#define false 0;
const int X[] = { 400, 600, 750, 900, 1200, 1600, 2000, 2400, 3100, 3500, 4100, 4700, 5400, 6100, 8100 };
const int Y[] = { 20, 30, 40 };
const int Z[sizeof(Y)/sizeof(0)][sizeof(X)/sizeof(0)] =
{
  {120, 290, 410, 410, 610, 730, 860, 1120, 1240, 1270, 1370, 1610, 1750, 1860},
  {290, 530, 630, 650, 840, 940, 1180, 1430, 1550, 1610, 1710, 1940,2080,2140,2200},
  {430, 670, 730, 840,1040,1200, 1450, 1690, 1820,}
};
const int size_X = (sizeof(X)/sizeof(0));
const int size_Y = (sizeof(Y)/sizeof(0));

int linInterp2D(int x1,  int y1,int x2,  int y2,int x)
{

    int y = y1 + (x - x1) * (y2 - y1) / (x2 - x1);

    return y;
}

int linInterp3D(
    int x1,  int y1,
    int x2,  int y2,
    int z11, int z21, int z12, int z22,
    int x,   int y
    ) {

    /*

         |
      y2-| z12       z22
         |
       y-|      z
         |
      y1-| z11       z21
         |_______________
           |    |    |
           x1   x    x2

    */

    int z1 = z11 + (z12 - z11) * (y - y1) / (y2 - y1);
    int z2 = z21 + (z22 - z21) * (y - y1) / (y2 - y1);
    int z = z1 + (z2 - z1) * (x - x1) / (x2 - x1);

    return z;
}

int findZ(int x, int y) {

    int x1 = 0; int x1i = 0;
    int y1 = 0; int y1i = 0;
    int x2 = 0; int x2i = 0;
    int y2 = 0; int y2i = 0;
    int z11 = 0;
    int z21 = 0;
    int z12 = 0;
    int z22 = 0;

  static bit x_inside_range = false;
  static bit y_inside_range = false;

    for (size_t i=0; i<size_X-1; i++)
{

        if ( x == X[i] ) {
            x1 = x2 = X[i];
            x1i = x2i = i;
            x_inside_range = true;
            break;
        }
        else if ( (x > X[i]) && (x < X[i+1]) ) {
            x1 = X[i];
            x1i = i;
            x2 = X[i+1];
            x2i = i+1;
            x_inside_range = true;
            break;
        }
        else if ( x == X[i+1] ) {
            x1 = x2 = X[i+1];
            x1i = x2i = i+1;
            x_inside_range = true;
            break;
        }
    }

    if ( !x_inside_range ) {
        //cout << "x is outside of X axis range!\n";
        return -666;
    }

    for (size_t i=0; i<size_X-1; i++) {

        if ( y == Y[i] ) {
            y1 = y2 = Y[i];
            y1i = y2i = i;
            y_inside_range = true;
            break;
        }
        else if ( (y > Y[i]) && (y < Y[i+1]) ) {
            y1 = Y[i];
            y1i = i;
            y2 = Y[i+1];
            y2i = i+1;
            y_inside_range = true;
            break;
        }
        else if ( y == Y[i+1] ) {
            y1 = y2 = Y[i+1];
            y1i = y2i = i+1;
            y_inside_range = true;
            break;
        }
    }

    if ( !y_inside_range ) {
        //cout << "y is outside of Y axis range!\n";
        return -666;
    }

    z11 = Z[y1i][x1i];
    z21 = Z[y1i][x2i];
    z12 = Z[y2i][x1i];
    z22 = Z[y2i][x2i];

    if ( (x1 == x2) && (y1 == y2) ) {
        return Z[y1i][x1i];
    }
    else if ( (x1 == x2) && (y1 != y2) ) {
        return linInterp2D(y1, z11, y2, z12, y);
    }
    else if ( (x1 != x2) && (y1 == y2) ) {
        return linInterp2D(x1, z11, x2, z21, x);
    }
    else {
        return linInterp3D(x1, y1, x2, y2, z11, z21, z12, z22, x, y);
    }
}
/***************************************************************************/
/*** linear interpolation                                                ***/
/*                                                                         */
/*   x1 - нижн¤¤ граница исходного диапазона                               */
/*   x2 - верхн¤¤ граница исходного диапазона                              */
/*   y1 - нижн¤¤ граница искомого диапазона                                */
/*   y2 - верхн¤¤ граница искомого диапазона                               */
/*   x - исходное значение                                                 */
/*   функци¤ возвращает искомое значение                                   */
/*                                                                         */
/***************************************************************************/
/*uint8_t linear_interp(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t x) {
  uint8_t x21, dx;
  div_t d;

  if (x <= x1)  					// меньше/равно нижней границе
    return y1;

  if (x >= x2)  					// больше/равно верхней границе
    return y2;

  x21 = x2 - x1;
  if (x21 == 0) 					// на ноль делить нельз¤!
    return y1;

  dx = x - x1;

  if (y2 < y1) {  					// отрицательный наклон хар-ки
    d = div( (y1-y2)*dx, x21 );
    if (d.rem >= (x21/2))  			// округление результата
      d.quot++;
    return y1 - d.quot;
  } else {        					// положительный наклон хар-ки
    d = div( (y2-y1)*dx, x21 );
    if (d.rem >= (x21/2))  			// округление результата
      d.quot++;
    return y1 + d.quot;
  }
}

/***************************************************************************/
/*** search table                                                        ***/
/***************************************************************************/
/*void search_table(uint8_t *tbl, uint8_t tbl_length, uint8_t item, struct search_table_t *r) {
  uint8_t i, searching;

  searching = 1;
  r->index = 0;

  for (i=0; (i<tbl_length) && searching; i++) {
    r->index = i;
    if (item < tbl[i]) {
      searching = 0;
    }
  }

  if (r->index == 0) { // lbound and ubound can't point to the same element
    r->lbound = tbl[0];
    r->ubound = tbl[1];
    r->index = 1;
  } else {
    r->lbound = tbl[r->index-1];
    r->ubound = tbl[r->index];
  }
}


/***************************************************************************/
/*** VE table lookup                                                     ***/
/***************************************************************************/
/*void ve_table_lookup(void) {
  uint8_t ve_11, ve_12, ve_21, ve_22;
  uint8_t ve_low_kpa, ve_high_kpa;
  struct search_table_t kpa, rpm;


  search_table(config.kparangeve, sizeof(config.kparangeve), engine.kpa, &kpa);
  search_table(config.rpmrangeve, sizeof(config.rpmrangeve), engine.rpm, &rpm);

  ve_11 = *(config.VE+8*(kpa.index-1)+(rpm.index-1));
  ve_12 = *(config.VE+8*(kpa.index-1)+rpm.index);
  ve_21 = *(config.VE+8*kpa.index+(rpm.index-1));
  ve_22 = *(config.VE+8*kpa.index+rpm.index);
  
  ve_low_kpa = linear_interp(rpm.lbound, rpm.ubound, ve_11, ve_12, engine.rpm);
  ve_high_kpa = linear_interp(rpm.lbound, rpm.ubound, ve_21, ve_22, engine.rpm);

  corr.ve = linear_interp(kpa.lbound, kpa.ubound, ve_low_kpa, ve_high_kpa, engine.kpa);
}*/

