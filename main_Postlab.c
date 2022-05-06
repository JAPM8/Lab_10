/*
 * File:   main_Postlab.c
 *
 * Author: Javier Alejandro Pérez Marín
 * Comunicación serial con OSSCON de 1 MHz y baud rate de 9600, donde PIC
 * envia información de lectura de un POT y devuelve una cadena que se le envia
 *
 * Created on 4 de mayo de 2022, 09:02 AM
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
#include <stdio.h>
#include <string.h>

/*
 * CONSTANTES
 */
#define _XTAL_FREQ 1000000

/*
 * VARIABLES 
 */
char pregunta_1 [] = "\n\rBienvenid@ - Seleccione solo con el número de opción:\n\r1.Leer potenciómetro.\n\r2.Enviar Ascii."; //Cadena de primer pregunta que será enviada
char pregunta_2 []= "\n\r¡Perfecto! ¿Qué carácter o cadena quiere enviar? \n\rPresione asterisco cuando haya concluido"; //Cadena de segunda pregunta que será enviada
char cadena_imp[]; //Array que almacenará cadena ingresada por el usuario
int estado = 0; //Variable que indicará el estado en que se encuentra para realizar las diferentes opciones */
int i, //Contador de carácteres impresos
    val_pot, //Valor del potenciómetro
    cont; //Contador de carácteres

/*
 * PROTOTIPO DE FUNCIÓN
 */
void setup(void);
void impresion(char texto []); //Función para imprimir menú
//void set_estado(void); //Función para cambiar entre estados*/

/*void __interrupt() isr(void){
    if(PIR1bits.RCIF){ //Se verifica si hay un nuevo dato en el serial
        set_estado(); //Se pasa a función para determinar opción seleccionada
    }
    return;
}*/

void main(void) {  
   
    setup(); // Se pasa a configurar PIC
        
    while(1){
            impresion(pregunta_1);
            __delay_ms(100);
            impresion(pregunta_2);
            /*if(estado == 0){ // Si estado = 0
                impresion_menu(); // Entonces mostrar menú en terminal
            }
            if(estado == 2){ // Si estado = 2
                impresion_p2(); // Entonces mostrar pregunta 2 en terminal
            }*/
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
    RCSTAbits.CREN = 1;//Se habilita el receptor
    
    //Config interrupciones
    INTCONbits.GIE = 1; //Se habilitan interrupciones globales
    INTCONbits.PEIE = 1; // Se habilitan interrupciones de periféricos
    PIE1bits.RCIE = 1; //Se habilitn interrupciones de recepción 
    return;
 }

/*void set_estado(void){
    if(estado == 1){
        if(RCREG == 0x1){
            //Mandar val pot
            cadena_user[] = ("El valor actual del potenciometro es: %i o a su %i", val_pot, (val_pot*20/51));
        }
        else if(RCREG == 0x2){
            estado = 2;
            
        }
        else {
            //error
        }
    }
    return;
}*/

void impresion(char texto){
    i = 0;
    int cont = sizeof(texto); //Almacena cantidad de carácteres
    while(i<=cont){ //Siempre que se hayan impreso menos o la misma cantidad de carácteres de la cadena.
        if(PIR1bits.TXIF){ //Se verifica que el módulo esté libre para transmitir datos
            for(i=0; i<=cont; i++){
                __delay_ms(100); //Delay de 100 ms para que sea notoria la transmisión
                TXREG = texto[i]; //Se imprime carácter por carácter
            }
        }
    }
    //estado = 1; //Se pasa a estado de input de usuario
    return;
}
