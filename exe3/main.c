#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    double s1=0,s2=0,s3=0,s4=0,s5=0;
    int data = 0;
    int janela = 5;
    int contador = 0;
    double soma = 0.0;


    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            double x = data;
            // implementar filtro aqui!
            if(contador<janela){
                s1=s2;s2=s3;s3=s4;s4=s5;s5=x;
                soma +=x;
                contador++;
                
                if(contador == janela){
                    double media = soma/janela;
                    printf("%f\n",media);
                }
                
            }else{
                soma -=s1;
                s1 = s2; s2 = s3; s3 = s4; s4 = s5; s5 = x;
                soma+=x;

                double media = soma/janela;
                printf("%f\n",media);
            }

            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
