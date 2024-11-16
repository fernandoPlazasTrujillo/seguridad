#include <xc.h>
#include "ADC.h"
#include <math.h>

#define _XTAL_FREQ 8000000 // Ajusta si tu oscilador tiene una frecuencia diferente

void adc_begin(void) {
    ADCON1bits.ADFM = 1; // Justificación a la derecha
    ADCON1bits.VCFG0 = 0; // Referencia de voltaje 0V a 5V
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.ADCS = 0b01; // Tiempo de conversión Fosc/8
}

int adc_conversion(int channel) {
    ADCON0bits.CHS = (0x0F & channel); // Seleccionar canal
    ADCON0bits.ADON = 1; // Encender el módulo ADC
    __delay_us(30); // Tiempo de adquisición
    ADCON0bits.GO = 1; // Iniciar conversión
    while (ADCON0bits.GO); // Esperar que termine
    ADCON0bits.ADON = 0; // Apagar el ADC
    return ((ADRESH << 8) | ADRESL); // Retornar valor de 10 bits
}

int leerTemperatura(void) {
    int raw_value = adc_conversion(0); // Lee el canal donde está el sensor de temperatura
    
    // Constante beta para el cálculo de temperatura
    long beta = 3975;
    
    // Ajusta el valor de resistencia para el cálculo
    long a = 1023 - raw_value; // Calcula el valor de resistencia en base al valor ADC
    
    // Calcula la temperatura en grados Celsius utilizando la fórmula de Steinhart-Hart
    float TempC = (float)(beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0);
    
    // Retorna la temperatura en grados Celsius como un entero
    return (int) TempC;
}

int leerLuz(void) {
    return adc_conversion(1); // Lee el valor ADC del fotoresistor
}
