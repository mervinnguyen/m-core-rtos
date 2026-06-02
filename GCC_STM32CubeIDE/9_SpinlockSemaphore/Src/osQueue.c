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

    /*Copy item into head slot and advance */
    memcpy(q->buf[q->head], item, q->itemSize);
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count++;

    __enable_irq();
}

void osQueueReceive(osQueue_t *q, void *item) {
    /* Wait until there is data */
    while(1) {
        __disable_irq();
        if (q->count > 0) {
            break;          /*Data available, hold IRQ off */
        }
        __enable_irq();   /*Release IRQ and wait*/
        osThreadYield();
    }

    /* Copy item out of tail slot and advance */
    memcpy(item, q->buf[q->tail], q->itemSize);
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count--;

    __enable_irq();
}
