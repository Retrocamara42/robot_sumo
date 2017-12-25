/***************************************
 *                                     *
 * File:   Sumobot.c                   *
 * Author: Juan Manuel                 *
 *                                     *
 *                                     *
 ***************************************/

#include <stdlib.h>
#include <stdint.h>
#include <pic16f628a.h>

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include "user.h"

#pragma warning disable 752

int main(int argc, char** argv) {

    // CONFIGURACION
    /**************************************************************************
     * Entradas y salidas                                                     *
     *  Entradas:                                                             *
     *      Ultrasonido Echo : Puerto A Pin 7                                 *
     *      Fotoresistor     : Puerto B Pin 5 (con interrupciones)            *
     *      Botón            : Puerto B Pin 0                                 *
     *  Salidas:                                                              *  
     *      Motores          : Puerto A Pines 0,1(derecha) y 2,3(izquierda)   *
     *      Ultrasonido Trig : Puerto A Pin 6                                 *
     *      Foquito Debug    : Puerto B Pin 7                                 *
     *      PWM              : Puerto B Pin 3                                 *
     **************************************************************************/
    // Configuracion Puerto A
    CMCON = 0x07;                    // Turns comparators off and enable digital I/O
    TRISA &= ~((31<<0)|(1<<6));      // Pines 6 y 4:0 salidas
    PORTA &= ~((31<<0)|(1<<6));      // Apaga las salidas
    TRISA |= (1<<7);                 // Pin 7 es entrada
    // Configuracion Puerto B
    PIE1  &= ~(3<<4);                // Desabilita la función USART de los pines 1 y 2 del puerto B
    OPTION_REG |= (1<<7);            // Pulls up disabled puerto B
    TRISB |= (1<<0)|(1<<5);          // Pines 0 y 5 son entradas 
    TRISB &= ~((1<<4)|(1<<6)|(1<<7));  // Pines 4,6,7 salida, 3 es salida PWM (más abajo)
    
    // Interrupciones
    INTCONbits.RBIF = 0;        // Clear the flag RBIF
    INTCONbits.RBIE = 1;        // Activa las interrupciones en puerto B - pin 4-7

    
    // Timer 1
    T1CONbits.T1CKPS  = 1;      // 1:2 Prescale value
    T1CONbits.T1OSCEN = 0;      // Apaga oscilador
    T1CONbits.TMR1CS  = 0;      // Reloj interno: Fosc/4, Fosc = 4Mhz
    T1CONbits.TMR1ON  = 0;      // Detiene timer
    TMR1 = 0;                   // Cuenta se inicializa en cero
    
    // Configuracion Timer 2
    // Reloj interno: Fosc/4, Fosc = 4Mhz
    T2CONbits.T2CKPS = 0;           // 1:1 prescaler value
    // Se configura CCP1 para PWM
    CCP1CONbits.CCP1M = 1100;       // Modo PWM 
    // Se configura PWM
    PR2 = 49;                       // Periodo PWM de 10KHz = 0.1ms
    // Duty cycle max = 0.1ms - todo el periodo PWM
    // -> CCPR1L:CCP1CON<5:4> max = 100
    // -> Se requiere 50%   
    CCPR1L = 11001;              
    CCP1CONbits.CCP1X = 0;         // CCP1CON 5
    CCP1CONbits.CCP1Y = 0;         // CCP1CON 4
    TRISB &= ~(1<<3);
    // Se habilita timer 2
    T2CONbits.TMR2ON = 1;
    PORTBbits.RB3 = 1;             // Se activa la salida PWM
    
    // Parpadea el LED
    int32_t i = 0;
    PORTBbits.RB7 = 1;
    for(i = 0; i< 30000; i++);
    PORTBbits.RB7 = 0;           
    for(i = 0; i< 30000; i++);
    
    // Activar el botón para empezar
    while(PORTBbits.RB0 == 0);
    for(i = 0; i< 30000; i++); // Efecto rebote
    
    // Parpadea el LED
    PORTBbits.RB7 = 1;
    for(i = 0; i< 30000; i++);
    PORTBbits.RB7 = 0;           
    for(i = 0; i< 30000; i++);
    
    /************************************************************************************/
    // Movimiento
    while(1){
        int8_t direccion = detectarOponente();
        switch(direccion){
            case DIRECCION_ADELANTE: 
                PORTAbits.RA1 = 0;
                PORTAbits.RA3 = 0;
                delayMs(50);    // Para que RA1, RA3 se terminen de apagar
                PORTAbits.RA0 = 1;
                PORTAbits.RA2 = 1;
                delayMs(1000);   // Para que avance un poco
                break;
            case DIRECCION_DERECHA: 
                PORTAbits.RA1 = 0;
                PORTAbits.RA2 = 0;
                delayMs(50);   
                PORTAbits.RA0 = 1;
                PORTAbits.RA3 = 1;
                delayMs(1000);
                break;
            case DIRECCION_IZQUIERDA: 
                PORTAbits.RA0 = 0;
                PORTAbits.RA3 = 0;
                delayMs(50);   
                PORTAbits.RA1 = 1;
                PORTAbits.RA2 = 1;
                delayMs(1000);
                break;
            case DIRECCION_NINGUNA:
            default:
                break;
        }
        // Se apagan los motores
        PORTAbits.RA0 = 0;
        PORTAbits.RA1 = 0;
        PORTAbits.RA2 = 0;
        PORTAbits.RA3 = 0;
    }
    
    return (EXIT_SUCCESS);
}

