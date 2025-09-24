/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/timer.h"
 #include "hardware/adc.h"
 
 const int PIN_LED_B = 4;
 
 const float conversion_factor = 3.3f / (1 << 12);
 
 /**
  * 0..1.0V: Desligado
  * 1..2.0V: 150 ms
  * 2..3.3V: 400 ms
  */
 double leitura = 0.0;

 volatile int timer_flag = 0;

volatile int timer_run;
 void adc_task(void *p) {
    adc_init();
    adc_gpio_init(28);

    
    while (1) {
        uint16_t result;
        adc_select_input(2); // Select ADC input 1 (GPIO27)
        result = adc_read();
        leitura = result*conversion_factor;
        //printf("%f",leitura);

    }
}


bool timer_callback(repeating_timer_t *rt){
    timer_flag = 1;
    return true;
}


 int main() {
     stdio_init_all();

     repeating_timer_t tim;

     int led =0;

     while (1) {

        if(leitura <= 1){
            if(timer_run){
                cancel_repeating_timer(&tim);
            }
        }
        if(leitura <= 2 && leitura > 1){
            if(add_repeating_timer_ms(300,timer_callback,NULL,&tim)){
                timer_run = 1;
            }
        }
        if(leitura <= 3.3 && leitura > 2){
            if(add_repeating_timer_ms(500,timer_callback,NULL,&tim)){
                timer_run = 1;
            }
        }

        if(timer_flag){
            timer_flag = 0;
            if(timer_run){
                led = !led;
                gpio_put(PIN_LED_B,led);
            }
        }
     }
 }
