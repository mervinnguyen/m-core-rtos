#include "osQueue.h"
#include "osKernel.h"

void osQueueInit(osQueue_t *q, uint8_t itemSize) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->itemSize = itemSize;
    q->mutex = 1; /* Unlocked */
}

void osQueueSend(osQueue_t *q, const void *item) {
    /* Wait until there is space */
    while(1) {
        __disable_irq();
        if (q->count < QUEUE_SIZE) {
            break;          /*Space available, hold IRQ off */
        }
        __enable_irq();   /*Release IRQ and wait*/
        osThreadYield();
    }
}