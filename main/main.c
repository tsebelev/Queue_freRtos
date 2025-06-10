#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_timer.h"

#define QUEUE_LENGTH 10
#define MESSAGE_LENGTH 30
//esp_timer_get_time(); возвращает время с запуска МК микросекундах с момента загрузки.
//printf("%lld\n", esp_timer_get_time());
QueueHandle_t myQueue;

void task_sender(void *pvParameters) {
    char message[MESSAGE_LENGTH];
    int i = 0;
    //ESP_LOGI("task_sender", "Start firts Task");
        

    while (1) {
		//ESP_LOGI("task_sender", "1");
        snprintf(message, MESSAGE_LENGTH, "Message %d", i++);

        if (xQueueSend(myQueue, message, pdMS_TO_TICKS(100)) != pdTRUE) {
            printf("Send failed\n");
        } else {
            printf("1 Sent: %s\n", message);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_sender1(void *pvParameters) {
    char message[MESSAGE_LENGTH];
    int i = 0;
    //ESP_LOGI("task_sender1", "Start 2 Task");
        

    while (1) {
		//ESP_LOGE("task_sender1", "2");
        snprintf(message, MESSAGE_LENGTH, "Another string %d", i++);

        if (xQueueSend(myQueue, message, pdMS_TO_TICKS(1000)) != pdTRUE) {
            printf("Send failed\n");
        } else {
            printf("2 Sent: %s\n", message);
            }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task_receiver(void *pvParameters) {
    char message[MESSAGE_LENGTH];
	ESP_LOGE("task_receiver", "Start second Task");
    while (1) {
        if (xQueueReceive(myQueue, message, pdMS_TO_TICKS(100)) == pdTRUE) {
            printf("3 Received: %s\n", message);
        } else {
            printf("Receive failed\n");
        }

        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

void app_main() {
    // Создаем очередь для сообщений фиксированного размера
   myQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_LENGTH);
    if (myQueue != NULL) {
		ESP_LOGI("myQueue", "Queue creation");
    }
    else {
        printf("Queue creation failed\n");
        return;
	}
	
ESP_LOGE("Make", "3 task");
	xTaskCreatePinnedToCore(task_sender, "Task Sender", 2048, NULL, 1, NULL, 0);
    xTaskCreate(task_sender1, "Task Sender1", 2048, NULL, 1, NULL);
    xTaskCreatePinnedToCore(task_receiver, "Task Receiver", 2048, NULL, 5, NULL, 1);

    printf("Tasks created\n");
}