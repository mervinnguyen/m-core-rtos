#ifndef OSQUEUE_H
#define OSQUEUE_H

#include <stdint.h>
#include <string.h>

#define QUEUE_SIZE    8         /* Max number of items in the queue */
#define ITEM_MAX_SIZE 16        /* Max bytes per item*/

typedef struct {
    uint8_t buf[QUEUE_SIZE][ITEM_MAX_SIZE]; /* Ring buffer */
    uint8_t head;                           /* Next write index */
    uint8_t tail;                           /* Next read index */
    uint8_t count;                          /* Number of items in the queue */
    uint8_t itemSize;                       /* Size of each item in bytes */
    int32_t mutex;                          /* Protexts head/tail/count */
} osQueue_t;

void osQueueInit(osQueue_t *q, uint8_t itemSize);
void osQueueSend(osQueue_t *q, const void *item);
void osQueueReceive(osQueue_t *q, void *item);

#endif /* OSQUEUE_H */