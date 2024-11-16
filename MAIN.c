#include <xc.h>
#include <pic16f887.h>
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "sensor.h"
#include "adc.h"

// Definición de los pines de los LEDs
#define LED_VERDE PORTEbits.RE0
#define LED_AZUL PORTEbits.RE1
#define LED_ROJO PORTEbits.RE2

#define UMBRAL_TEMPERATURA 30   // Valor de temperatura
#define UMBRAL_LUZ 700          // Valor de luz

unsigned int valor_temp = 0;
unsigned int valor_luz = 0;
unsigned int valor_Hall = 0;

// sensores de monitoreo
int temperatura;
int luz;

// sensores de evento
int hall;
int obstaculos;


const char CLAVE_SECRETA[5] = {'6','9','6','9',0};
char entrada_usuario[5];
unsigned char indice_entrada = 0;
int intentos_restantes = 3;

enum estado {
    INIT,
    MONITOREO,
    ALARMA,
    ALERTA,
    EVENTOS,
    BLOQUEO
};

enum estado estado_actual = INIT;

// Función para esperar en milisegundos
void espera_ms(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);
    }
}

// Función de parpadeo de un LED específico
void parpadeo_led(unsigned char led, unsigned int t_encendido, unsigned int t_apagado, unsigned int duracion) {
    unsigned int tiempo_transcurrido = 0;
    while (tiempo_transcurrido < duracion) {
        // Encender LED
        switch(led) {
            case 'V': LED_VERDE = 1; break;
            case 'A': LED_AZUL = 1; break;
            case 'R': LED_ROJO = 1; break;
        }
        
        espera_ms(t_encendido);

        // Apagar LED
        switch(led) {
            case 'V': LED_VERDE = 0; break;
            case 'A': LED_AZUL = 0; break;
            case 'R': LED_ROJO = 0; break;
        }
        espera_ms(t_apagado);

        tiempo_transcurrido += t_encendido + t_apagado;
    }
}

unsigned char flagClave = 0;

void Seguridad(void) {
    LCD_Clear();
    LCD_String_xy(0, 0, "Ingrese codigo:");
    LCD_Command(0xC0);

    while (indice_entrada < 4) {
        char tecla = keypad_getkey();
        if (tecla != 0) {
            LCD_Char('#'); 
            entrada_usuario[indice_entrada++] = tecla;
        }
        espera_ms(100);
    }
    entrada_usuario[indice_entrada++] = 0;

    if (strncmp(entrada_usuario, CLAVE_SECRETA, 4) == 0) {
        LCD_Clear();
        LCD_String_xy(0, 0, "Acceso Concedido");
        parpadeo_led('V', 500, 500, 3000);  // Parpadea LED verde
        intentos_restantes = 3;
        flagClave = 1;
    } else {
        LCD_Clear();
        LCD_String_xy(0, 0, "Clave Incorrecta");
        parpadeo_led('A', 30, 70, 200);  // Parpadea LED azul
        intentos_restantes--;
        flagClave = 0;

        if (intentos_restantes == 0) {
            intentos_restantes = 3;
            flagClave = 2;
        }
    }

    espera_ms(200);
    LCD_Clear();
    indice_entrada = 0;
}

void FuncionMonitoreo(void) {
    
    LCD_Clear();
    
    temperatura = leerTemperatura();
    luz = leerLuz();
    
    char buffer[17];        
            
    // Formatear y mostrar la temperatura en la primera línea del LCD
    sprintf(buffer, "Temperatura: %dC", temperatura);
    LCD_String_xy(0, 0, buffer);
    
    // Formatear y mostrar la luz en la segunda línea del LCD
    sprintf(buffer, "Luz: %d", luz);
    LCD_String_xy(1, 0, buffer);
 
}

void funcionBloqueo(void) {
    LCD_Clear();
    LCD_String_xy(0, 0, "Sistema Bloqueado");
    parpadeo_led('R', 300, 500, 8000);  // Parpadea LED rojo
    LED_ROJO = 0;
}

void funcionEventos(void) {
    // Leer los sensores
    int hall_lectura = leer_sensor_hall();
    int obstaculos_lectura = leer_sensor_obstaculos();
    
    // Mostrar el estado del sensor Hall en la primera línea del LCD
    if (hall_lectura == 1) {  // Detecta algo cuando hall_lectura es 1
        LCD_String_xy(0, 0, "Hall: Detectado");
        hall = 1;
    } else {
        LCD_String_xy(0, 0, "Hall: No Detectado");
        hall = 0;
    }
    
    // Mostrar el estado del sensor de obstáculos en la segunda línea del LCD
    if (obstaculos_lectura == 0) {  // Detecta obstáculo cuando obstaculos_lectura es 0
        LCD_String_xy(1, 0, "Obstaculo: Detectado");
        obstaculos = 1;
    } else {
        LCD_String_xy(1, 0, "Obstaculo: No Detectado");
        obstaculos = 0;
    }
}


void alarma (void) {
    
    LCD_Clear();
    LCD_String_xy(0,0, "alarma");
    __delay_ms(500);
}

// Variables para el manejo de los temporizadores
void Timer_start();
int count_1000ms = 0;
int count_5000ms = 0;
unsigned char flag_1000ms = 0;
unsigned char flag_5000ms = 0;
#define TIMEOUT_1000 1000
#define TIMEOUT_5000 5000

void main(void) {
    OSCCON = 0x71; // Configura oscilador interno (FOSC = 8Mhz)
    
    LCD_Init(); // Inicializa el LCD
    keypad_init(); // Inicializa el teclado
    adc_begin(); // Inicializar el adc
    
    TRISE = 0x00;
    TRISA = 0xFF;   
    ANSEL = 0x03;
    ANSELH = 0x00;
    TRISD = 0b00000000;
    
    // Apagar los LEDs al inicio
    LED_VERDE = 0;
    LED_AZUL = 0;
    LED_ROJO = 0;
    
    Timer_start();
    
    // Ciclo principal
    while (1) {
        switch (estado_actual) {
            case INIT:
                Seguridad();
                if (flagClave == 1) {
                    estado_actual = MONITOREO;
                    count_5000ms = 0;
                    count_1000ms = 0;
                } else if (flagClave == 2) {
                    estado_actual = BLOQUEO;
                }
                break;
                
            case MONITOREO:
                
                if (flag_1000ms) {
                    FuncionMonitoreo();
                    flag_1000ms = 0;
                    count_1000ms = 0;
                }
                                
                if (flag_5000ms) {
                    flag_5000ms = 0;
                    estado_actual = EVENTOS;
                    count_5000ms = 0;  
                }
                
                if (temperatura > UMBRAL_TEMPERATURA ) {
                    estado_actual = ALARMA; 
                }
                
                
                break;
                
            case ALARMA:
                
                alarma();
                if (flag_5000ms) {
                    flag_5000ms = 0;
                    estado_actual = EVENTOS;
                    count_1000ms = 0;
                }
                
                break;
                
            case EVENTOS:
                funcionEventos();
                if (flag_5000ms) {
                    flag_5000ms = 0;
                    estado_actual = MONITOREO;
                    count_5000ms = 0;
                }
                break;
                
            case ALERTA:
                
                break;
                
            case BLOQUEO:
                funcionBloqueo();
                
                estado_actual = INIT;
                break;
                
            default:
                break;
        }
    }
}

// Interrupción del temporizador
void __interrupt() Timer1_ISR() {
    TMR1 = 0xFC16;
    count_1000ms++;
    count_5000ms++;

    if (count_1000ms >= TIMEOUT_1000) {
        count_1000ms = 0;
        flag_1000ms = 1;
    }
    if (count_5000ms >= TIMEOUT_5000) {
        count_5000ms = 0;
        flag_5000ms = 1;
    }
    
    PIR1bits.TMR1IF = 0; // Limpiar bandera de interrupción
}

// Inicialización del Timer1
void Timer_start() {
    GIE = 1;              // Habilitar interrupciones globales
    PEIE = 1;             // Habilitar interrupciones periféricas
    TMR1IE = 1;           // Habilitar interrupción de desbordamiento de Timer1
    TMR1IF = 0;
    T1CON = 0x00;         // Configurar Timer1 (16 bits, sin preescalador)
    TMR1 = 0xFC16;        // Cargar el valor para 1 ms
    TMR1ON = 1;           // Activar Timer1
    
}
