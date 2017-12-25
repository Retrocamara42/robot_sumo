
#include "user.h"

// Utiliza el timer para hacer un delay de aproximadamente "s" ms, Fosc/4 = 1Mhz
// Utiliza un preescalador de 1:2
// Entonces 1 ciclo = 2 us -> 500 ciclos = 1ms
void delayMs(int32_t ms){
    TMR1 = 0;                   // Resetea timer
    int32_t i = 0;
    T1CONbits.TMR1ON = 1;       // Activa timer
    for(i = 0; i< ms; i++){  
        while(TMR1 < 500);     // 1ms - nota: 500 se transforma a hexadecimal automaticamente
        TMR1 = 0;
    }
    T1CONbits.TMR1ON = 0;
}

void inline delay_us(int32_t us){
    T1CONbits.TMR1ON = 1;                // Activa timer 
    while(TMR1<(us/2));                  // TMR1 = 2 <-> 4us
    T1CONbits.TMR1ON = 0;                // Detiene timer
}

int8_t detectarOponente(){
    // ULTRASONIDO
    delayMs(300);                        // Delay para que las ondas no interfieran
    
    // tiempo para que el ultrasonido se adapte (4us-5us)
    PORTAbits.RA6 = 0;                   
    TMR1=0;
    delay_us(4);

    // manda pulso por 10us
    PORTAbits.RA6 = 1;                   
    TMR1=0;
    T1CONbits.TMR1ON = 1;                // Activa timer 
    while(TMR1<5);                       // TMR1 = 5 <-> 10us
    PORTAbits.RA6 = 0;
    T1CONbits.TMR1ON = 0;                // Detiene timer

    // tiempo de ultrasonido
    TMR1 = 0;                            // Resetea timer
    while(!PORTAbits.RA7);
    T1CONbits.TMR1ON = 1;                // Activa timer 
    while(PORTAbits.RA7);
    T1CONbits.TMR1ON = 0;                            // Detiene timer
       
    if((TMR1*2)  <= DISTANCIA_EN_S) { return DIRECCION_ADELANTE; }
    else { return DIRECCION_DERECHA; }   // Modo búsqueda 
}