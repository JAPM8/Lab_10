/*
 * File:   main_Lab.c
 * 
 * Author: Javier Alejandro Pérez Marín
 * Comunicación serial con OSSCON de 1 MHz y baud rate de 9600, donde PIC
 * envia cadena de carácteres a la computadora
 * 
 * Created on 4 de mayo de 2022, 12:13 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <string.h>

/*
 * CONSTANTES
 */
#define _XTAL_FREQ 1000000

/*
 * VARIABLES 
 */
char mensaje[]="HOLA MUNDO, ESTE ES MI LAB :D :V \r\n Profe póngame 100 ;)"; //Cadena que será enviada
int cont_car = sizeof(mensaje); //Almacena cantidad de carácteres
int i; //Contador de carácteres impresos

/*
 * PROTOTIPO DE FUNCIÓN
 */
void setup(void);
void impresion(void); //Función para imprimir cadena

void main(void) {  
   
    setup(); // Se pasa a configurar PIC
        
    while(1){
        impresion(); //Se llama función para impresión de la cadena
        }
}

void setup(void){
    ANSEL = 0;   //I/O DIGITALES
    ANSELH = 0; //I/O DIGITALES
    
    TRISD = 0;  //PORTD completo como OUTPUT
    PORTD = 0; //CLEAR de PORTD
    
    OSCCONbits.IRCF = 0b100;    //Oscilador interno de 1 mHz
    OSCCONbits.SCS = 1;        //Oscilador interno
    
    //Configuraciones de comunicación serial
    
    TXSTAbits.SYNC = 0; //Modo asíncrono
    TXSTAbits.BRGH = 1;//High speed Baud Rate
    BAUDCTLbits.BRG16 = 1; //Contador de 16 bits para el Baud Rate
    SPBRGH = 0;
    SPBRG = 25; //Baud rate aprox. en 9600
    
    RCSTAbits.SPEN = 1; //Se habilita comunicación serial
    RCSTAbits.RX9 = 0; //Se indica que solo 8 bits serán de transmisión
    
    TXSTAbits.TXEN = 1; //Se habilita transmisor
     
    return;
 }

void impresion(void){
    while(i<=cont_car){ //Siempre que se hayan impreso menos o la misma cantidad de carácteres de la cadena.
        if(PIR1bits.TXIF){ //Se verifica que el módulo esté libre para transmitir datos
            for(i=0; i<=cont_car; i++){
                __delay_ms(100); //Delay de 100 ms para que sea notoria la transmisión
                TXREG = mensaje[i]; //Se imprime carácter del mensaje
            }
        }
    }
}