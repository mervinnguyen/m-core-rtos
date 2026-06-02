#include "led.h"
#include "uart.h"
#include <stdio.h>
#include "timebase.h"
#include "osKernel.h"
#include "osQueue.h"

#define		QUANTA 	10

typedef uint32_t TaskProfiler;

TaskProfiler Task0_Profiler, Task1_Profiler, Task2_Profiler, pTask1_Profiler, pTask2_Profiler;

void motor_run(void);
void motor_stop(void);
void valve_open(void);
void valve_close(void);

int32_t semaphore1, semaphore2;

typedef struct {
	uint32_t timestamp;
	uint32_t value;
} SensorData_t;

void task0(void){
	while(1){
		Task0_Profiler++;
		osThreadYield();
	}
}

void task1(void){
	/* Producer : builds a SensorData message and sends to the queue*/
	SensorData_t data;

	while(1){
		osSemaphoreWait(&semaphore1);
		
		data.timestamp = Task1_Profiler++;
		data.value = 42;

		osQueueSend(&sensorQueue, &data);
		motor_run();

		osSemaphoreSet(&semaphore1);
	}
}

void task2(void){
	/* Consumer : receives SensorData message from the queue and process it*/
	SensorData_t data;
	while(1){
		osSemaphoreWait(&semaphore2);
		
		osQueueReceive(&sensorQueue, &data);
		printf("Received data: timestamp = %lu, value = %lu\n", data.timestamp, data.value);

		valve_open();
		osSemaphoreSet(&semaphore1);
	}
}

void task3(void){
	while(1){
		pTask1_Profiler++;
	}
}

int main(){

	/*Initialize UART*/
	uart_tx_init();

	/*Initialize hardware timer*/
	tim2_1hz_interrupt_init();

	/*Initialize semaphores*/
	osSemaphoreInit(&semaphore1, 1);
	osSemaphoreInit(&semaphore2, 0);

	/*Initialize queue*/
	osQueueInit(&sensorQueue, sizeof(SensorData_t));

	/* Initialize Kernel*/
	osKernelInit();

	/*Add Thread*/
	osKernelAddThreads(&task0, &task1, &task2);

	/*Set RoundRobin time quanta*/
	osKernelLaunch(QUANTA);
}

void TIM2_IRQHandler(){
	/*Clear update interrupt flag*/
	TIM2->SR &= ~SR_UIF;

	/* Signal task2 via semaphore, task2 will then drain the queue */
	osSemaphoreSet(&semaphore2);
	
	/*Do something*/
	pTask2_Profiler++;
}

void motor_run(void){
	printf("Motor is starting ...\n");
}

void motor_stop(void){
	printf("Motor is stopping ...\n\r");
}

void valve_open(void){
	printf("Valve is opening...\n");
}

void valve_close(void){
	printf("Valve is closing...\n\r");
}

