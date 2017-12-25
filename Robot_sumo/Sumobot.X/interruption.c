
#include "user.h"

void interrupt color(){
    
    if(INTCONbits.RBIF == 1){
        if(PORTBbits.RB5 == 1){
            int8_t timerPrendido = (T1CONbits.TMR1ON == 0)? 0 : 1;
            // Retroceso
            PORTAbits.RA0 = 0;
            PORTAbits.RA2 = 0;
            delayMs(50);    
            PORTAbits.RA1 = 1;
            PORTAbits.RA3 = 1;
            delayMs(1500);  
            PORTBbits.RB7 = 1;
            T1CONbits.TMR1ON = (timerPrendido == 0)? 0 : 1;
        }
        INTCONbits.RBIF = 0;
    }
}


