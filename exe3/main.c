#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
#define WINDOW_SIZE 5

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
    int data;
    int index = 0;
    int soma  = 0;
    int temp  = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            if (index < WINDOW_SIZE) {
                soma += data;
                if (index == 0) {
                    temp = data;
                }
            }
            else {
                soma = soma - temp + data;
                temp = data;
            }
            if (index < WINDOW_SIZE) {
                index++;
            }
            int resultado = soma / WINDOW_SIZE;
            printf("%d\n", resultado);

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
