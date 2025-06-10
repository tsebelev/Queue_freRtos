#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include <freertos/semphr.h>//использование семафоров

#define QUEUE_LENGTH 10
#define MESSAGE_LENGTH 30
//esp_timer_get_time(); возвращает время с запуска МК микросекундах с момента загрузки.
//printf("%lld\n", esp_timer_get_time());
QueueHandle_t myQueue;

//создаем семафоры для учета колличества
SemaphoreHandle_t xSemaphoreSend;
SemaphoreHandle_t xSemaphoreReceive;

void task_sender(void *pvParameters) {
    char message[MESSAGE_LENGTH];
    int i = 0;
    //ESP_LOGI("task_sender", "Start firts Task");
        

    while (1) {
		//ждем разрешения на отправку по семафору
		if (xSemaphoreTake(xSemaphoreSend, portMAX_DELAY) == pdTRUE){
		//ESP_LOGI("task_sender", "1");
        snprintf(message, MESSAGE_LENGTH, "Message %d", i++);

        if (xQueueSend(myQueue, message, pdMS_TO_TICKS(100)) != pdTRUE) {
            printf("Send failed\n");
        } else {
            printf("1 Sent: %s\n", message);
        }
		}
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_sender1(void *pvParameters) {
    char message[MESSAGE_LENGTH];
    int i = 0;
    //ESP_LOGI("task_sender1", "Start 2 Task");
        

    while (1) {
		//ждем разрешения на отправку по семафору
		if (xSemaphoreTake(xSemaphoreSend, portMAX_DELAY) == pdTRUE){
		//ESP_LOGE("task_sender1", "2");
        snprintf(message, MESSAGE_LENGTH, "Another string %d", i++);

        if (xQueueSend(myQueue, message, pdMS_TO_TICKS(100)) != pdTRUE) {
            printf("Send failed\n");
        } else {
            printf("2 Sent: %s\n", message);
            }
		}
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_receiver(void *pvParameters) {
    char message[MESSAGE_LENGTH];
	ESP_LOGE("task_receiver", "Start second Task");
    while (1) {
		//ждем разрешения на чтение по семафору
		if (xSemaphoreTake(xSemaphoreSend, portMAX_DELAY) == pdTRUE){
        if (xQueueReceive(myQueue, message, pdMS_TO_TICKS(100)) == pdTRUE) {
            printf("3 Received: %s\n\n", message);
        } else {
            printf("Receive failed\n");
        }
		}
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_controller(void *pvParametrs){
	while(1){
		//разрешаем две отправки
		xSemaphoreGive(xSemaphoreSend);
		xSemaphoreGive(xSemaphoreSend);
		vTaskDelay(pdMS_TO_TICKS(10)); //даем время на выполнение
		
		//разрешаем два чтения
		xSemaphoreGive(xSemaphoreReceive);
		xSemaphoreGive(xSemaphoreReceive);
		vTaskDelay(pdMS_TO_TICKS(10)); //даем время на выполнение
		
		//ESP_LOGI("controller", "Cycle completed");
		vTaskDelay(pdMS_TO_TICKS(10)); //пауза между циклами
	}
}

void app_main() {
    // Создаем очередь для сообщений фиксированного размера
   myQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_LENGTH);
    if (myQueue == NULL) {
        ESP_LOGE("app_main", "Queue creation failed");
        return;
	}
	
	//создаем бинарные семафоры
	xSemaphoreSend = xSemaphoreCreateBinary();
	xSemaphoreReceive = xSemaphoreCreateBinary();
	if (xSemaphoreSend == NULL || xSemaphoreReceive == NULL){
	ESP_LOGE("app_main", "Semaphor creation failed");
	return;	
	}
	

	xTaskCreatePinnedToCore(task_sender, "Task Sender", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(task_sender1, "Task Sender1", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(task_receiver, "Task Receiver", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_controller, "task_controller", 2048,NULL,3,NULL, 1);
	ESP_LOGE("app_main", "All task createt");

}