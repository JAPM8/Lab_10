/*
 * File:   main_Postlab.c
 *
 * Author: Javier Alejandro Pérez Marín
 * Comunicación serial con OSSCON de 1 MHz y baud rate de 9600, donde PIC
 * envia información de lectura de un POT y devuelve un ASCII que se le envia
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
char pregunta_1 [] = "\n\rBienvenid@ - Seleccione ingresando solo el número:\n\r1.Leer potenciómetro.\n\r2.Enviar Ascii.\n\r"; //Cadena de primer pregunta que será enviada
char pregunta_2 [] = "\n\r¡Perfecto!\n\r¿Qué carácter o número quiere enviar (una letra o un dígito)? \n\r"; //Cadena de segunda pregunta que será enviada
uint8_t val_pot = 0; //Valor del potenciómetro
char val_imp[]; //Array para almacenar valor del potenciómetro
int  estado = 0; //Variable que indicará el estado en que se encuentra para realizar las diferentes opciones 
int dato =0, //Variable que almacena ASCII ingresado por usuario
    imp = 0; //Variable utilizada para impresión de feedback

/*
 * PROTOTIPO DE FUNCIÓN
 */
void setup(void);
void impresion(char txt[]); //Función para imprimir que recibe cadena de texto
void set_estado(void); //Función para cambiar entre estados

void __interrupt() isr(void){
    //Interrupción del ADC
    if (PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0){ //Se verifica canal AN0        
            val_pot = ADRESH; //Se guardan los 8 bits de conversión
        }
        PIR1bits.ADIF = 0; // Limpiamos bandera ADC
    }
    //Interrupción UART
    if(PIR1bits.RCIF){ //Se verifica si hay un nuevo dato en el serial
        set_estado(); //Se pasa a función para determinar opción seleccionada 
    }
    return;
}

void main(void) {  
   
    setup(); // Se pasa a configurar PIC
        
    while(1){
            if(ADCON0bits.GO == 0){ // Si no hay proceso de conversión
            ADCON0bits.GO = 1; // Se inicia proceso de conversión
            } 
            if(estado == 0){ // Si estado = 0
                impresion(pregunta_1); // Entonces mostrar menú en terminal
                estado = 1; //Pasamos a indicar valor del potenciómetro
            }
            if(estado == 2){ // Si estado = 2
                impresion(pregunta_2); // Entonces mostrar pregunta 2 en terminal
                impresion("Presione la barra espaciadora cuando esté listo \n\r\n\r"); //Instrucciones a usuario
                estado = 3; //Se pasa a recibir ASCII del usuario
            }
                        
        }
}

void setup(void){
    ANSEL = 0x01; //Se configura PORTA0/AN0 como entrada analógica
    ANSELH = 0; //I/O DIGITALES
    
    TRISA = 0x01; //PORTA0/AN0 como INPUT    
    PORTA = 0;    //CLEAR DE PUERTO A
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
    
    //Config ADC
    ADCON0bits.ADCS = 0b00; // Fosc/2
    ADCON1bits.VCFG0 = 0;  // Referencia VDD
    ADCON1bits.VCFG1 = 0;  // Referencia VSS
    ADCON0bits.CHS = 0b0000; // Se selecciona PORTA0/AN0
    ADCON1bits.ADFM = 0; // Se indica que se tendrá un justificado a la izquierda
    ADCON0bits.ADON = 1; // Se habilita el modulo ADC
    __delay_us(40);     // Delay para sample time
    
    //Config interrupciones
    INTCONbits.GIE = 1; //Se habilitan interrupciones globales
    INTCONbits.PEIE = 1; // Se habilitan interrupciones de periféricos
    PIE1bits.ADIE = 1;  //Se habilita interrupcion del ADC
    PIR1bits.ADIF = 0; // Limpieza de bandera del ADC
    PIE1bits.RCIE = 1; //Se habilitn interrupciones de recepción 
    return;
 }

void set_estado(void){
    if(estado == 1){
        if(RCREG == 0b110001){ //Si usuario ingresa "1"
            impresion ("1\n\r"); //Se imprime opción ingresada
            //Mandar val pot
            impresion ("\n\rEl valor actual del potenciómetro es: "); 
            itoa(val_imp,val_pot,10); //Conversión int a ASCII mediante función itoa de STDLIB (ASCCII, INT, FORMATO)
            impresion (val_imp); //Se imprime el valor del potenciómetro      
            impresion (" que equivale al ~");
            //Obtención de porcentaje del potenciómetro
            itoa(val_imp,(val_pot*20/51),10); //Conversión int a ASCII mediante función itoa de STDLIB (ASCCII, INT, FORMATO)
            impresion (val_imp); //Se imprime el valor del potenciómetro 
            impresion ("%\n\r");
            //Reinicio de menú
            estado = 0;
        }
        else if(RCREG == 0b110010){ //Si usuario ingresa "2"
            impresion ("2\n\r"); //Se imprime opción ingresada
            estado = 2; //Se pasa a preguntar que ASCII quiere enviar
        }
        else { //Si usuario ingresa opción no válida
            for(imp = 0; imp<1;imp++){ //Display de feedback al usuario pues no ingresó opción válida
                TXREG = RCREG;
            }
            impresion ("\n\r¡Error, ingrese 1 o 2!\n\r\n\r"); //Se indica el problema
            estado = 0; //Reinicio de menú
        }
    }
    if (estado == 3){ 
        while(RCREG != 32){ // Se verifica el carácter space  pues este indica que el usuario terminó el ingreso de datos.
            if (dato != RCREG){ //Siempre que se ingrese un valor diferente al ya guardado
                dato = RCREG; //Se almacena el nuevo ASCII
                for(imp = 0; imp<1;imp++){ //Se muestra en consola lo ingresado
                    impresion("\b"); //Backspace para borrar valor anterior
                    TXREG = dato;
                }
            } 
        }
        impresion("\n\r\n\rEl ASCII recibido fue: "); //Se indica el último ASCII recibido
        TXREG = dato;
        impresion(".\n\r");
        estado = 0; //Reinicio de menú
    }
    return;
}

void impresion(char txt[]){
    uint8_t i = 0; //Contador de carácteres impresos
    while(txt[i] != '\0'){ //Se verifica el carácter inválido pues este indica el fin de una cadena.
        if(PIR1bits.TXIF){ //Se verifica que el módulo esté libre para transmitir datos
            TXREG = txt[i]; //Se imprime carácter del mensaje
            i++; //Se incrementa posición
        }
    }
}
