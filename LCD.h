#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

   
#define RS PORTCbits.RC0  /*PIN 0 of PORTD is assigned for register select Pin of LCD*/
#define EN PORTCbits.RC1  /*PIN 1 of PORTD is assigned for enable Pin of LCD */
#define ldata PORTC  /*PORTD(PD4-PD7) is assigned for LCD Data Output*/ 
#define LCD_Port TRISC  /*define macros for PORTD Direction Register*/
    
void LCD_Init(void);                   /*Initialize LCD*/
void LCD_Command(unsigned char );  /*Send command to LCD*/
void LCD_Char(unsigned char x);    /*Send data to LCD*/
void LCD_String(const char *);     /*Display data string on LCD*/
void LCD_String_xy(char, char , const char *);
void LCD_Clear(void);                  /*Clear LCD Screen*/
void LCD_Character(void);

void Lmain(void);

#ifdef	__cplusplus
}
#endif

#endif
