#ifndef _DevicesQueue_H_
#define _DevicesQueue_H_




#define QUEUE_KEY_INPUT_LENGTH   (64)
#define QUEUE_KEY_OUTPUT_LENGTH  (64)



/* 使x对n字节对齐 */
#define queueRoundUp(x, n) (((x) + (n) - 1) & (~((n) - 1)))



/* 覆盖模式 */
#define QUEUE_MODE_NORMAL 0
/* 锁定模式 */
#define QUEUE_MODE_LOCK   1

#define QUEUE_MODE QUEUE_MODE_NORMAL

#if ((QUEUE_MODE != QUEUE_MODE_NORMAL) && (QUEUE_MODE != QUEUE_MODE_LOCK))
#error "need check mode!"
#endif



typedef enum {
    queueNull = 0,
    queueNormal,
    queueEmpty,
    queueFull,
    queueError,
}enumQueueState;



typedef struct{
    char *pcName;

    uint8_t *pHead;
    uint8_t *pTail;
    uint8_t *pReadFrom;
    uint8_t *pWriteTo;

    int32_t length;
}QueueType;



extern QueueType g_TypeQueueKeyInput;
extern QueueType g_TypeQueueKeyOutput;



enumQueueState enumQueueInit(void);
enumQueueState enumQueueCreate(QueueType *pTypeQueue, char *pcName, uint8_t *pucBuff, int32_t iLength);
enumQueueState enumQueueGetState(QueueType *pTypeQueue);
enumQueueState enumQueueSetState(QueueType *pTypeQueue, enumQueueState enumState);
int32_t iQueueGetLengthOfOccupy(QueueType *pTypeQueue, uint8_t ucByte);
int32_t iQueueGetLengthOfSeparetor(QueueType *pTypeQueue, uint8_t ucByte);
int32_t iQueueGetLengthOfRemaining(QueueType *pTypeQueue);
enumQueueState enumQueuePushByte(QueueType *pTypeQueue, uint8_t ucData);
enumQueueState enumQueuePopByte(QueueType *pTypeQueue, uint8_t *pucData);
enumQueueState enumQueuePushDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength);
enumQueueState enumQueuePopDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength, uint8_t ucByte);

#endif
