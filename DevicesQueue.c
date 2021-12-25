/*
 *  author: Honrun
 *  date:   2021/12/25 11:30
 */
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "DevicesQueue.h"




QueueType g_TypeQueueKeyInput = {0};
static uint8_t st_ucQueueKeyInputBuff[QUEUE_KEY_INPUT_LENGTH + 4] = {0};

QueueType g_TypeQueueKeyOutput = {0};
static uint8_t st_ucQueueKeyOutputBuff[QUEUE_KEY_OUTPUT_LENGTH + 4] = {0};




enumQueueState enumQueueInit(void)
{
    enumQueueState enumState = queueNormal;


    if(enumQueueCreate(&g_TypeQueueKeyInput, "Key Input", st_ucQueueKeyInputBuff, QUEUE_KEY_INPUT_LENGTH) != queueNormal)
        enumState = queueError;

    if(enumQueueCreate(&g_TypeQueueKeyOutput, "Key Output", st_ucQueueKeyOutputBuff, QUEUE_KEY_OUTPUT_LENGTH) != queueNormal)
        enumState = queueError;


    if(enumState != queueNormal)
        printf("enumQueueInit error.\r\n");


    return enumState;
}

/*
 * Return:      创建是否成功状态值
 * Parameters:  *pTypeQueue: 队列结构体指针; pucName: 队列名称; iLength: 队列长度
 * Description: 初始化队列
 */
enumQueueState enumQueueCreate(QueueType *pTypeQueue, char *pcName, uint8_t *pucBuff, int32_t iLength)
{
    if(pTypeQueue == NULL)
        return queueNull;

    if(iLength < 1)
        return queueEmpty;

    pTypeQueue->pcName    = pcName;
    pTypeQueue->pHead     = pucBuff;
    pTypeQueue->length    = iLength + 1;
    pTypeQueue->pReadFrom = pTypeQueue->pHead;
    pTypeQueue->pWriteTo  = pTypeQueue->pHead;
    pTypeQueue->pTail     = pTypeQueue->pHead + pTypeQueue->length;

    return queueNormal;
}

/*
 * Return:      队列缓存空满状态
 * Parameters:  *pTypeQueue: 队列结构体指针
 * Description: 获取队列缓存空满状态
 */
enumQueueState enumQueueGetState(QueueType *pTypeQueue)
{
    uint8_t * pNow = NULL;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
      return queueNull;

    if(pTypeQueue->pReadFrom == pTypeQueue->pWriteTo)
      return queueEmpty;

    pNow = pTypeQueue->pWriteTo + 1;
    pNow = (pNow >= pTypeQueue->pTail) ? pTypeQueue->pHead : pNow;

    if(pNow == pTypeQueue->pReadFrom)
      return queueFull;

    return queueNormal;
}

/*
 * Return:      void
 * Parameters:  *pTypeQueue: 队列结构体指针; ucStateFlag: 空满状态
 * Description: 设置队列缓存空满状态
 */
enumQueueState enumQueueSetState(QueueType *pTypeQueue, enumQueueState enumState)
{
    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    switch(enumState)
    {
        case queueEmpty:
            memset(pTypeQueue->pHead, 0, pTypeQueue->length);
            pTypeQueue->pReadFrom = pTypeQueue->pWriteTo = pTypeQueue->pHead;
            break;

        default : break;
    }

    return queueNormal;
}

/*
 * Return:      查找到的数据位置
 * Parameters:
 * Description: memrchr替代函数
 */
static void *vQueueMemrchr(void *pvHandle, uint8_t ucValue, int32_t iCount)
{
    uint8_t *pucCheck = pvHandle;

    pucCheck += iCount - 1;

    while((iCount--) > 0)
    {
        if(*pucCheck == ucValue)
            return pucCheck;

        --pucCheck;
    }

    return NULL;
}

/*
 * Return:      队列缓存中有效数据长度
 * Parameters:  *pTypeQueue: 队列结构体指针
 * Description: 获取队列缓存中有效数据长度
 */
int32_t iQueueGetLengthOfOccupy(QueueType *pTypeQueue)
{
    int32_t iLength = 0;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return 0;

    if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
        iLength = pTypeQueue->pWriteTo - pTypeQueue->pReadFrom;
    else
        iLength = pTypeQueue->length - (pTypeQueue->pReadFrom - pTypeQueue->pWriteTo);

    return iLength;
}

/*
 * Return:      队列缓存中有效数据长度
 * Parameters:  *pTypeQueue: 队列结构体指针; ucByte: 指定的有效字节
 * Description: 获取队列缓存中有效数据长度，需要有指定的有效字节
 */
int32_t iQueueGetLengthOfOccupyNeed(QueueType *pTypeQueue, uint8_t ucByte)
{
    int32_t iLength = 0;
    uint8_t *pucHead = NULL;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return 0;

    /* pHead|-------pReadFrom===============pWriteTo-------------|pTail */
    if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
    {
        if((pucHead = vQueueMemrchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pReadFrom)) != NULL)
            iLength = (pucHead - pTypeQueue->pReadFrom) + 1;
    }
    /* pHead|=======pWriteTo---------------pReadFrom=============|pTail */
    else
    {
        /* pHead|=======|pWriteTo */
        if((pucHead = vQueueMemrchr(pTypeQueue->pHead, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pHead)) != NULL)
            iLength = (pucHead - pTypeQueue->pHead) + (pTypeQueue->pTail - pTypeQueue->pReadFrom) + 1;

        /* pReadFrom|=============|pTail */
        else if((pucHead = vQueueMemrchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pTail - pTypeQueue->pReadFrom)) != NULL)
            iLength = (pucHead - pTypeQueue->pReadFrom) + 1;
    }

    return iLength;
}

/*
 * Return:      队列缓存中到下一个分隔符的有效数据长度
 * Parameters:  *pTypeQueue: 队列结构体指针
 * Description: 获取队列缓存中有效数据长度
 */
int32_t iQueueGetLengthOfSeparetor(QueueType *pTypeQueue, uint8_t ucByte)
{
    int32_t iLength = 0;
    uint8_t *pucHead = NULL;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return 0;

    /* pHead|-------pReadFrom===============pWriteTo-------------|pTail */
    if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
    {
        if((pucHead = memchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pReadFrom)) != NULL)
            iLength = (pucHead - pTypeQueue->pReadFrom) + 1;
    }
    /* pHead|=======pWriteTo---------------pReadFrom=============|pTail */
    else
    {
        /* pReadFrom|=============|pTail */
        if((pucHead = memchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pTail - pTypeQueue->pReadFrom)) != NULL)
            iLength = (pucHead - pTypeQueue->pReadFrom) + 1;

        /* pHead|=======|pWriteTo */
        else if((pucHead = memchr(pTypeQueue->pHead, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pHead)) != NULL)
            iLength = (pucHead - pTypeQueue->pHead) + (pTypeQueue->pTail - pTypeQueue->pReadFrom) + 1;
    }

    return iLength;
}

/*
 * Return:      队列缓存中剩余长度
 * Parameters:  *pTypeQueue: 队列结构体指针
 * Description: 获取队列缓存中剩余长度
 */
int32_t iQueueGetLengthOfRemaining(QueueType *pTypeQueue)
{
    int32_t iLength = 0;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return 0;

    if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
        iLength = pTypeQueue->length - (pTypeQueue->pWriteTo - pTypeQueue->pReadFrom) - 1;
    else
        iLength = (pTypeQueue->pReadFrom - pTypeQueue->pWriteTo) - 1;

    return iLength;
}

/*
 * Return:      是否入队成功状态
 * Parameters:  *pTypeQueue: 队列结构体指针; ucData: 待入队字节数据
 * Description: 入队一个字节数据
 */
enumQueueState enumQueuePushByte(QueueType *pTypeQueue, uint8_t ucData)
{
    enumQueueState enumPushState = queueNormal;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    if(enumQueueGetState(pTypeQueue) == queueFull)
    {
#if (QUEUE_MODE == QUEUE_MODE_NORMAL)
        enumPushState = queueFull;
#else
        return queueFull;
#endif
    }

    *pTypeQueue->pWriteTo++ = ucData;
    pTypeQueue->pWriteTo = ((pTypeQueue->pWriteTo >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pWriteTo);

    /* 在溢出时，需要把read指针指向当前队列新的末尾 */
    if(enumPushState != queueNormal)
    {
        pTypeQueue->pReadFrom = pTypeQueue->pWriteTo + 1;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return enumPushState;
}

/*
 * Return:      是否出队成功状态
 * Parameters:  *pTypeQueue: 队列结构体指针; *pucData: 待出队字节数据指针
 * Description: 出队一个字节数据
 */
enumQueueState enumQueuePopByte(QueueType *pTypeQueue, uint8_t *pucData)
{
    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    if(pTypeQueue->pReadFrom == pTypeQueue->pWriteTo)
      return queueEmpty;

    *pucData = *pTypeQueue->pReadFrom++;
    pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);

    return queueNormal;
}

/*
 * Return:      是否入队成功状态
 * Parameters:  *pTypeQueue: 队列结构体指针; *ppHead: 待入队数据缓存指针; iLength: 缓存长度
 * Description: 入队一系列数据
 */
enumQueueState enumQueuePushDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength)
{
    uint8_t *pucHandle = pvBuff;
    enumQueueState enumPushState = queueNormal;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    if(iQueueGetLengthOfRemaining(pTypeQueue) < iLength)
    {
#if (QUEUE_MODE == QUEUE_MODE_NORMAL)
        enumPushState = queueFull;
#else
        return queueFull;
#endif
    }

    while((iLength--) > 0)
    {
        *pTypeQueue->pWriteTo++ = *pucHandle++;
        pTypeQueue->pWriteTo = ((pTypeQueue->pWriteTo >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pWriteTo);
    }

    /* 在溢出时，需要把read指针指向当前队列新的末尾 */
    if(enumPushState != queueNormal)
    {
        pTypeQueue->pReadFrom = pTypeQueue->pWriteTo + 1;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return enumPushState;
}

/*
 * Return:      是否出队成功状态
 * Parameters:  *pTypeQueue: 队列结构体指针; *ppHead: 待出队数据缓存指针; iLength: 缓存长度
 * Description: 出队一系列数据
 */
enumQueueState enumQueuePopDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength)
{
    uint8_t *pucHandle = pvBuff;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    if(pvBuff == NULL)
        return queueError;

    if(iQueueGetLengthOfOccupy(pTypeQueue) < iLength)
        return queueError;

    while((iLength--) > 0)
    {
        *pucHandle++ = *pTypeQueue->pReadFrom++;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return queueNormal;
}

/*
 * Return:      是否出队成功状态
 * Parameters:  *pTypeQueue: 队列结构体指针; *ppHead: 待出队数据缓存指针; iLength: 缓存长度; ucByte: 指定的有效字节
 * Description: 出队一系列数据，需要有指定的有效字节
 */
enumQueueState enumQueuePopDatasNeed(QueueType *pTypeQueue, void *pvBuff, int32_t iLength, uint8_t ucByte)
{
    uint8_t *pucHandle = pvBuff;

    if((pTypeQueue == NULL) || (pTypeQueue->pHead == NULL))
        return queueNull;

    if(pvBuff == NULL)
        return queueError;

    if(iQueueGetLengthOfOccupyNeed(pTypeQueue, ucByte) < iLength)
        return queueError;

    while((iLength--) > 0)
    {
        *pucHandle++ = *pTypeQueue->pReadFrom++;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return queueNormal;
}
