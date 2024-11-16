#ifndef ADC_H
#define ADC_H

void adc_begin(void);
int adc_conversion(int channel);
int leer_temperatura(void);     // Devuelve la temperatura como entero
int leer_fotoresistor(void);    // Devuelve el valor de lux como entero

#endif /* ADC_H */
