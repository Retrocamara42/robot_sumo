/* 
 * File:   user.h
 * Author: Juan Manuel
 *
 */

#ifndef USER_H
#define	USER_H

#include <stdlib.h>
#include <stdint.h>
#include <pic16f628a.h>

int32_t const DISTANCIA_EN_S = 882;       // DIAMETRO_DOYO*FACTOR_DETECTAR*1000/17=150*0.06*1000/17
//uint8_t const  ERROR_ANGULO    = 10;  
enum Direccion{
  DIRECCION_NINGUNA = 0,
  DIRECCION_ADELANTE = 1,
  DIRECCION_DERECHA = 2,
  DIRECCION_IZQUIERDA = 3
};
void    delayMs(int32_t s);
void inline delay_us(int32_t us);
int8_t detectarOponente();

#endif	/* USER_H */
