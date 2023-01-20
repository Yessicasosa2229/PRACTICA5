/*
 * ADC.c
 *
 *  Created on: 04/11/2019
 *      Author: ravenelco
 */
#include "lib/include.h"

extern void Configura_Reg_ADC0(void)
{
    /*
    Habilitar el modulo 0 del ADC con dos canales analogicos 
    en el puerto E a una velocidad de conversion de 250ksps
    dandole la mayor prioridad al secuenciador 2 con evento
    de procesador 
    */
    //pag352 Habilitar y proporcionar un reloj al Modulo ADC0
    SYSCTL->RCGCADC = (1<<0);  //modulo 0

    //Utilizaré pines PB4, PB6 Y E4

    //Pag 406 (RGCGPIO) Habilitar Puertos a utilizar (B Y E)
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (0<<5)|(1<<4)|(0<<3)|(0<<2)|(1<<1)|(0<<0)|(1<<12)|(1<<8);
    
    //Pag 663 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida
    //PARA ADC TIENEN QUE SER ENTRADAS
    //EL GPIO -DIR se hace para cada puerto 
    GPIOB_AHB->DIR = (0<<4) | (0<<6); //PB4 y PB6    
    GPIOE_AHB->DIR = (0<<4); //PE4
    
    //(GPIOAFSEL) pag.672 Enable alternate función para que el modulo analógico tenga control de esos pines
    //EL GPIO -AFSEL se hace para cada puerto
    GPIOB_AHB->AFSEL =  (1<<4) | (1<<6 ); //PB4 Y PB6
    GPIOE_AHB->AFSEL =  (1<<4); //PE4

    //(GPIODEN) pag.682 desabilitaR el modo digital
    GPIOB_AHB->DEN = (0<<4) | (0<<6); //PB4 Y PB6
    GPIOE_AHB->DEN = (0<<4); //PE4
    
    //Pag 787 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1808
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF00FFFF);
    //(GPIOAMSEL) pag.786 habilitar analogico
    GPIOE_AHB->AMSEL = (1<<5) | (1<<4);
    //Pag 1159 El registro (ADCPC) establece la velocidad de conversión por segundo
    ADC0->PC = (0<<2)|(0<<1)|(1<<0);//250ksps
    //Pag 1099 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores
    ADC0->SSPRI = 0x3210;
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);
    //Pag 1091 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger)
    ADC0->EMUX  = (0x0000);
    //Pag 1129 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado
    ADC0->SSMUX2 = 0x0089;
    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción
    ADC0->SSCTL2 = (1<<6) | (1<<5) ;
    /* Enable ADC Interrupt */
    ADC0->IM |= (1<<2); /* Unmask ADC0 sequence 2 interrupt pag 1082*/
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (1<<2) | (0<<1) | (0<<0);
    ADC0->PSSI |= (1<<2);
}
extern void ADC0_InSeq2(uint16_t *Result,uint16_t *duty){

    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004;
       while((ADC0->RIS&0x04)==0){}; // espera al convertidor
       Result[1] = ADC0->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en la pila2
       Result[0] = ADC0->SSFIFO2&0xFFF;
       duty[0] = (Result[0]*20000)/4096;
       duty[1] = (Result[1]*20000)/4096;
       ADC0->ISC = 0x0004;  //Conversion finalizada

}


