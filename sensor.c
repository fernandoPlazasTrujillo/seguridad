#include <xc.h>
#include "SENSOR.h"

#define _XTAL_FREQ 8000000  // Frecuencia del oscilador

void sensor_init(void) {
    ANSELbits.ANS2 = 0;  // Configura RA2 como entrada digital para el sensor Hall
    ANSELbits.ANS3 = 0;  // Configura RA3 como entrada digital para el sensor de obstáculos
    TRISA2 = 1;          // Configura RA2 como entrada (Sensor Hall)
    TRISA3 = 1;          // Configura RA3 como entrada (Sensor de obstáculos)
}

// Función para leer el sensor Hall
int leer_sensor_hall(void) {
    __delay_us(30);       // Espera para asegurar la lectura
    return PORTAbits.RA2; // Lee el valor del sensor Hall desde RA2
}

// Función para leer el sensor de obstáculos
int leer_sensor_obstaculos(void) {
    __delay_us(30);         // Espera para asegurar la lectura
    return PORTAbits.RA3;   // Lee el valor del sensor de obstáculos desde RA3
}
