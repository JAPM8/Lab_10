/*
 * File:   main_Postlab.c
 *
 * Author: Javier Alejandro P�rez Mar�n
 * Comunicaci�n serial con OSSCON de 1 MHz y baud rate de 9600, donde PIC
 * envia informaci�n de lectura de un POT y devuelve un ASCII que se le envia
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
#include <stdlib.h>
#include <string.h>

/*
 * CONSTANTES
 */
#define _XTAL_FREQ 1000000

/*
 * VARIABLES 
 */
char pregunta_1 [] = "\n\rBienvenid@ - Seleccione ingresando solo el n�mero:\n\r1.Leer potenci�metro.\n\r2.Enviar Ascii.\n\r"; //Cadena de primer pregunta que ser� enviada
char pregunta_2 [] = "\n\r�Perfecto!\n\r�Qu� car�cter o n�mero quiere enviar (una letra o un d�gito)? \n\r"; //Cadena de segunda pregunta que ser� enviada
uint8_t val_pot = 0; //Valor del potenci�metro
char val_imp[]; //Array para almacenar valor del potenci�metro
int  estado = 0; //Variable que indicar� el estado en que se encuentra para realizar las diferentes opciones 
int dato =0, //Variable que almacena ASCII ingresado por usuario
    imp = 0; //Variable utilizada para impresi�n de feedback

/*
 * PROTOTIPO DE FUNCI�N
 */
void setup(void);
void impresion(char txt[]); //Funci�n para imprimir que recibe cadena de texto
void set_estado(void); //Funci�n para cambiar entre estados

void __interrupt() isr(void){
    //Interrupci�n del ADC
    if (PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0){ //Se verifica canal AN0        
            val_pot = ADRESH; //Se guardan los 8 bits de conversi�n
        }
        PIR1bits.ADIF = 0; // Limpiamos bandera ADC
    }
    //Interrupci�n UART
    if(PIR1bits.RCIF){ //Se verifica si hay un nuevo dato en el serial
        set_estado(); //Se pasa a funci�n para determinar opci�n seleccionada 
    }
    return;
}

void main(void) {  
   
    setup(); // Se pasa a configurar PIC
        
    while(1){
            if(ADCON0bits.GO == 0){ // Si no hay proceso de conversi�n
            ADCON0bits.GO = 1; // Se inicia proceso de conversi�n
            } 
            if(estado == 0){ // Si estado = 0
                impresion(pregunta_1); // Entonces mostrar men� en terminal
                estado = 1; //Pasamos a indicar valor del potenci�metro
            }
            if(estado == 2){ // Si estado = 2
                impresion(pregunta_2); // Entonces mostrar pregunta 2 en terminal
                impresion("Presione la barra espaciadora cuando est� listo \n\r\n\r"); //Instrucciones a usuario
                estado = 3; //Se pasa a recibir ASCII del usuario
            }
                        
        }
}

void setup(void){
    ANSEL = 0x01; //Se configura PORTA0/AN0 como entrada anal�gica
    ANSELH = 0; //I/O DIGITALES
    
    TRISA = 0x01; //PORTA0/AN0 como INPUT    
    PORTA = 0;    //CLEAR DE PUERTO A
    TRISD = 0;  //PORTD completo como OUTPUT
    PORTD = 0; //CLEAR de PORTD
    
    OSCCONbits.IRCF = 0b100;    //Oscilador interno de 1 mHz
    OSCCONbits.SCS = 1;        //Oscilador interno
    
    //Configuraciones de comunicaci�n serial
    
    TXSTAbits.SYNC = 0; //Modo as�ncrono
    TXSTAbits.BRGH = 1;//High speed Baud Rate
    BAUDCTLbits.BRG16 = 1; //Contador de 16 bits para el Baud Rate
    SPBRGH = 0;
    SPBRG = 25; //Baud rate aprox. en 9600
    
    RCSTAbits.SPEN = 1; //Se habilita comunicaci�n serial
    RCSTAbits.RX9 = 0; //Se indica que solo 8 bits ser�n de transmisi�n
    
    TXSTAbits.TXEN = 1; //Se habilita transmisor
    RCSTAbits.CREN = 1;//Se habilita el receptor
    
    //Config ADC
    ADCON0bits.ADCS = 0b00; // Fosc/2
    ADCON1bits.VCFG0 = 0;  // Referencia VDD
    ADCON1bits.VCFG1 = 0;  // Referencia VSS
    ADCON0bits.CHS = 0b0000; // Se selecciona PORTA0/AN0
    ADCON1bits.ADFM = 0; // Se indica que se tendr� un justificado a la izquierda
    ADCON0bits.ADON = 1; // Se habilita el modulo ADC
    __delay_us(40);     // Delay para sample time
    
    //Config interrupciones
    INTCONbits.GIE = 1; //Se habilitan interrupciones globales
    INTCONbits.PEIE = 1; // Se habilitan interrupciones de perif�ricos
    PIE1bits.ADIE = 1;  //Se habilita interrupcion del ADC
    PIR1bits.ADIF = 0; // Limpieza de bandera del ADC
    PIE1bits.RCIE = 1; //Se habilitn interrupciones de recepci�n 
    return;
 }

void set_estado(void){
    if(estado == 1){
        if(RCREG == 0b110001){ //Si usuario ingresa "1"
            impresion ("1\n\r"); //Se imprime opci�n ingresada
            //Mandar val pot
            impresion ("\n\rEl valor actual del potenci�metro es: "); 
            itoa(val_imp,val_pot,10); //Conversi�n int a ASCII mediante funci�n itoa de STDLIB (ASCCII, INT, FORMATO)
            impresion (val_imp); //Se imprime el valor del potenci�metro      
            impresion (" que equivale al ~");
            //Obtenci�n de porcentaje del potenci�metro
            itoa(val_imp,(val_pot*20/51),10); //Conversi�n int a ASCII mediante funci�n itoa de STDLIB (ASCCII, INT, FORMATO)
            impresion (val_imp); //Se imprime el valor del potenci�metro 
            impresion ("%\n\r");
            //Reinicio de men�
            estado = 0;
        }
        else if(RCREG == 0b110010){ //Si usuario ingresa "2"
            impresion ("2\n\r"); //Se imprime opci�n ingresada
            estado = 2; //Se pasa a preguntar que ASCII quiere enviar
        }
        else { //Si usuario ingresa opci�n no v�lida
            for(imp = 0; imp<1;imp++){ //Display de feedback al usuario pues no ingres� opci�n v�lida
                TXREG = RCREG;
            }
            impresion ("\n\r�Error, ingrese 1 o 2!\n\r\n\r"); //Se indica el problema
            estado = 0; //Reinicio de men�
        }
    }
    if (estado == 3){ 
        while(RCREG != 32){ // Se verifica el car�cter space  pues este indica que el usuario termin� el ingreso de datos.
            if (dato != RCREG){ //Siempre que se ingrese un valor diferente al ya guardado
                dato = RCREG; //Se almacena el nuevo ASCII
                for(imp = 0; imp<1;imp++){ //Se muestra en consola lo ingresado
                    impresion("\b"); //Backspace para borrar valor anterior
                    TXREG = dato;
                }
            } 
        }
        impresion("\n\r\n\rEl ASCII recibido fue: "); //Se indica el �ltimo ASCII recibido
        TXREG = dato;
        impresion(".\n\r");
        estado = 0; //Reinicio de men�
    }
    return;
}

void impresion(char txt[]){
    uint8_t i = 0; //Contador de car�cteres impresos
    while(txt[i] != '\0'){ //Se verifica el car�cter inv�lido pues este indica el fin de una cadena.
        if(PIR1bits.TXIF){ //Se verifica que el m�dulo est� libre para transmitir datos
            TXREG = txt[i]; //Se imprime car�cter del mensaje
            i++; //Se incrementa posici�n
        }
    }
}
