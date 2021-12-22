/*
 *  modifier: Honrun
 *  date:     2021/12/22 17:00
 */
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "DevicesQueue.h"




QueueType g_TypeQueueKeyInput = {0};
uint8_t st_ucQueueKeyInputBuff[QUEUE_KEY_INPUT_LENGTH + 4] = {0};

QueueType g_TypeQueueKeyOutput = {0};
uint8_t st_ucQueueKeyOutputBuff[QUEUE_KEY_OUTPUT_LENGTH + 4] = {0};




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
 * Return:      �����Ƿ�ɹ�״ֵ̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; pucName: ��������; iLength: ���г���
 * Description: ��ʼ������
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
 * Return:      ���л������״̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��
 * Description: ��ȡ���л������״̬
 */
enumQueueState enumQueueGetState(QueueType *pTypeQueue)
{
    uint8_t * pNow = NULL;

    if(pTypeQueue == NULL)
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
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; ucStateFlag: ����״̬
 * Description: ���ö��л������״̬
 */
enumQueueState enumQueueSetState(QueueType *pTypeQueue, enumQueueState enumState)
{
    if(pTypeQueue == NULL)
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
 * Return:      ���л�������Ч���ݳ���
 * Parameters:  *pTypeQueue: ���нṹ��ָ��
 * Description: ��ȡ���л�������Ч���ݳ���
 */
int32_t iQueueGetLengthOfOccupy(QueueType *pTypeQueue, uint8_t ucByte)
{
    int32_t iLength = 0;
    char *pcHead = NULL;

    if(pTypeQueue == NULL)
        return 0;

    /* �ж��Ƿ���Ҫ��β���ţ��Լ��Ƿ��н�β���� */
    if((ucByte != 0) &&
       ((memchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pTail - pTypeQueue->pReadFrom) == 0) &&
        (memchr(pTypeQueue->pHead, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pHead) == 0)))
        return 0;

    if(ucByte != 0)
    {
        if((pcHead = strrchr((char *)(pTypeQueue->pReadFrom), ucByte)) != NULL)
            iLength = ((uint8_t *)pcHead - pTypeQueue->pReadFrom) + 1;

        else if((pcHead = strrchr((char *)(pTypeQueue->pHead), ucByte)) != NULL)
            iLength = ((uint8_t *)pcHead - pTypeQueue->pHead) + (pTypeQueue->pTail - pTypeQueue->pReadFrom) + 1;
    }
    else
    {
        if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
            iLength = pTypeQueue->pWriteTo - pTypeQueue->pReadFrom;
        else
            iLength = pTypeQueue->length - (pTypeQueue->pReadFrom - pTypeQueue->pWriteTo);
    }

    return iLength;
}

/*
 * Return:      ���л����е���һ���ָ�������Ч���ݳ���
 * Parameters:  *pTypeQueue: ���нṹ��ָ��
 * Description: ��ȡ���л�������Ч���ݳ���
 */
int32_t iQueueGetLengthOfSeparetor(QueueType *pTypeQueue, uint8_t ucByte)
{
    int32_t iLength = 0;
    uint8_t *pucHead = NULL;

    if(pTypeQueue == NULL)
        return 0;

    if((pucHead = memchr(pTypeQueue->pReadFrom, ucByte, pTypeQueue->pTail - pTypeQueue->pReadFrom)) != NULL)
        iLength = (pucHead - pTypeQueue->pReadFrom) + 1;

    else if((pucHead = memchr(pTypeQueue->pHead, ucByte, pTypeQueue->pWriteTo - pTypeQueue->pHead)) != NULL)
        iLength = (pucHead - pTypeQueue->pHead) + (pTypeQueue->pTail - pTypeQueue->pReadFrom) + 1;

    return iLength;
}

/*
 * Return:      ���л�����ʣ�೤��
 * Parameters:  *pTypeQueue: ���нṹ��ָ��
 * Description: ��ȡ���л�����ʣ�೤��
 */
int32_t iQueueGetLengthOfRemaining(QueueType *pTypeQueue)
{
    int32_t iLength = 0;

    if(pTypeQueue == NULL)
        return 0;

    if(pTypeQueue->pReadFrom <= pTypeQueue->pWriteTo)
        iLength = pTypeQueue->length - (pTypeQueue->pWriteTo - pTypeQueue->pReadFrom) - 1;
    else
        iLength = (pTypeQueue->pReadFrom - pTypeQueue->pWriteTo) - 1;

    return iLength;
}
/*
 * Return:      �Ƿ���ӳɹ�״̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; ucData: ������ֽ�����
 * Description: ���һ���ֽ�����
 */
enumQueueState enumQueuePushByte(QueueType *pTypeQueue, uint8_t ucData)
{
    enumQueueState enumPushState = queueNormal;

    if(pTypeQueue == NULL)
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

    /* �����ʱ����Ҫ��readָ��ָ��ǰ�����µ�ĩβ */
    if(enumPushState != queueNormal)
    {
        pTypeQueue->pReadFrom = pTypeQueue->pWriteTo + 1;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return queueNormal;
}

/*
 * Return:      �Ƿ���ӳɹ�״̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; *pucData: �������ֽ�����ָ��
 * Description: ����һ���ֽ�����
 */
enumQueueState enumQueuePopByte(QueueType *pTypeQueue, uint8_t *pucData)
{
    if(pTypeQueue == NULL)
        return queueNull;

    if(enumQueueGetState(pTypeQueue) == queueEmpty)
        return queueEmpty;

    *pucData = *pTypeQueue->pReadFrom++;
    pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);

    return queueNormal;
}

/*
 * Return:      �Ƿ���ӳɹ�״̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; *ppHead: ��������ݻ���ָ��; iLength: ���泤��
 * Description: ���һϵ������
 */
enumQueueState enumQueuePushDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength)
{
    uint8_t *pucHandle = pvBuff;
    enumQueueState enumPushState = queueNormal;

    if(pTypeQueue == NULL)
        return queueNull;

    /* ���ζ��У�ʵ���ܹ�ʹ�õ���󳤶ȱȳ�ʼ��������1������ԭ���������ݵõ� */
    if(iLength >= pTypeQueue->length)
        return queueError;

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

    /* �����ʱ����Ҫ��readָ��ָ��ǰ�����µ�ĩβ */
    if(enumPushState != queueNormal)
    {
        pTypeQueue->pReadFrom = pTypeQueue->pWriteTo + 1;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return queueNormal;
}

/*
 * Return:      �Ƿ���ӳɹ�״̬
 * Parameters:  *pTypeQueue: ���нṹ��ָ��; *ppHead: ���������ݻ���ָ��; iLength: ���泤��
 * Description: ����һϵ������
 */
enumQueueState enumQueuePopDatas(QueueType *pTypeQueue, void *pvBuff, int32_t iLength, uint8_t ucByte)
{
    uint8_t *pucHandle = pvBuff;

    if(pTypeQueue == NULL)
        return queueNull;

    if(pvBuff == NULL)
        return queueError;

    if(iQueueGetLengthOfOccupy(pTypeQueue, ucByte) < iLength)
        return queueError;

    while((iLength--) > 0)
    {
        *pucHandle++ = *pTypeQueue->pReadFrom++;
        pTypeQueue->pReadFrom = ((pTypeQueue->pReadFrom >= pTypeQueue->pTail) ? pTypeQueue->pHead : pTypeQueue->pReadFrom);
    }

    return queueNormal;
}
