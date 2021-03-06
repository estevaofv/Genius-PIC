/*
 * File:   main.c
 * Author: jonas
 *
 * Created on 1 de Outubro de 2017, 10:37
 */
#include "config.h"
#include "gpio.h"
#include "timer.h"
#include "genius.h"


// Func
void interrupt ISR_Handler(void);

// Vars
struct Genius_t Genius;
unsigned char frame = 0;

int main(void) {
    
    initOscillator();
    initInterrupts();
    initTimer0();
    initExtInt();
    initGPIO();   
        
    signed char res = -1;
    while(1){
        
        while(!Genius.teclado);
        
        switch(~BOTAO_PORT & BOTAO_MASK) {
            case BOTAO_ESQUERDA:
                buttonOnRisingEdge(BOTAO_PORT_PTR, BOTAO_ESQUERDA);
                setLed(3);
                res = 3;
            break;
            case BOTAO_BAIXO: 
                buttonOnRisingEdge(BOTAO_PORT_PTR, BOTAO_BAIXO);
                setLed(2);
                res = 2;
            break;
            case BOTAO_DIREITA:
                buttonOnRisingEdge(BOTAO_PORT_PTR, BOTAO_DIREITA);
                setLed(1);
                res = 1;
            break;
            case BOTAO_CIMA:
                buttonOnRisingEdge(BOTAO_PORT_PTR, BOTAO_CIMA);
                setLed(0);
                res = 0;
            break;
        }
        
        if(res >= 0) {
            if(res == Genius.sequencia[Genius.passo]) { // acertou o passo da sequencia
                Genius.passo++;
                if(Genius.passo > Genius.level) { // completou a sequencia, inicia novo nivel
                    Genius.passo = 0;
                    Genius.teclado = 0;
                    Genius.level++;                    
                    T1CONbits.TMR1ON = 1;
                }
            }
            else { // errou o passo da sequencia
                setLed(TODOS);
            }
            res = -1;
        }
        
    }
    
    return 0;
}


void interrupt ISR_Handler(void) {
    if(INTCONbits.INTE && INTCONbits.INTF) {
        INTCONbits.INTF = 0;
        initGenius(&Genius);
    }
        
    if(PIR1bits.TMR1IF && PIE1bits.TMR1IE) {
        PIR1bits.TMR1IF = 0;
        TMR1 = PER_TMR1;
        if(frame % 2 == 0) {
            if(Genius.passo <= Genius.level) {
                setLed(Genius.sequencia[Genius.passo]);
                Genius.passo++;
            }
            else {
                setLed(NENHUM);
                T1CONbits.TMR1ON = 0;
                Genius.passo = 0;
                Genius.teclado = 1;
            }
        }
        else {
            setLed(NENHUM);
        }
        frame++;
    } 
}