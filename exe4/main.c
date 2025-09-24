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

 volatile int timer_flag = 0;

volatile int timer_run = 0;

bool timer_callback(repeating_timer_t *rt){
    timer_flag = 1;
    return true;
}


 int main() {
     stdio_init_all();
     gpio_init(PIN_LED_B);
     gpio_set_dir(PIN_LED_B,GPIO_OUT);
     int led =0;
     gpio_put(PIN_LED_B,led);

     adc_init();
     adc_gpio_init(28);
     adc_select_input(2);


     repeating_timer_t tim;

     
     int current = -1;
     while (1) {

        uint16_t result = adc_read();
        double volt = result *conversion_factor;

        int zone = 0;
        if(volt < 1.0){
            zone= 0;
        }else if(volt <2.0){
            zone = 1;
        }else{
            zone = 2;
        }


        if(zone != current){
            if(timer_run){
                cancel_repeating_timer(&tim);
                timer_run =0;
                gpio_put(PIN_LED_B,0);
            }
            if(zone == 1){
                if(add_repeating_timer_ms(300,timer_callback,NULL,&tim)){
                    timer_run = 1;
                }
            }
            if(zone == 2){
                if(add_repeating_timer_ms(500,timer_callback,NULL,&tim)){
                    timer_run = 1;
                }
            }

            current = zone;
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
