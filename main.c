#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "DevicesQueue.h"



int main(void)
{
    int i = 0;
    uint8_t ucPushDatas[1024] = {0}, ucPopDatas[1024] = {0};
    uint16_t x = 0, y = 0;


    printf("Hello Queue test.\r\n\n");

    enumQueueInit();

    for(i = 0; i < sizeof(ucPushDatas) / 2; ++i)
        ucPushDatas[i] = '0' + (i % 10);

    while(1)
    {
        x = rand();
        y = rand() % 128;

        if(x & 1)
        {
            /* 入队测试 */
            enumQueuePushDatas(&g_TypeQueueKeyInput, ucPushDatas, y);

            printf("Push: %02d,\tOccupy length: %d,\tRemaining length: %d\r\n", y, iQueueGetLengthOfOccupy(&g_TypeQueueKeyInput), iQueueGetLengthOfRemaining(&g_TypeQueueKeyInput));
            printf("Head \t%08X Data: %s\r\n", g_TypeQueueKeyInput.pHead, g_TypeQueueKeyInput.pHead);
            printf("Read \t%08X Data: %s\r\n", g_TypeQueueKeyInput.pReadFrom, g_TypeQueueKeyInput.pReadFrom);
            printf("Write \t%08X Data: %s\r\n\n", g_TypeQueueKeyInput.pWriteTo, g_TypeQueueKeyInput.pWriteTo);
        }
        else
        {
            /* 出队测试 */
            enumQueuePopDatas(&g_TypeQueueKeyInput, ucPopDatas, y);

            printf("Pop: %02d,\tOccupy length: %d,\tRemaining length: %d\r\n", y, iQueueGetLengthOfOccupy(&g_TypeQueueKeyInput), iQueueGetLengthOfRemaining(&g_TypeQueueKeyInput));
            printf("Head \t%08X Data: %s\r\n", g_TypeQueueKeyInput.pHead, g_TypeQueueKeyInput.pHead);
            printf("Read \t%08X Data: %s\r\n", g_TypeQueueKeyInput.pReadFrom, g_TypeQueueKeyInput.pReadFrom);
            printf("Write \t%08X Data: %s\r\n\n", g_TypeQueueKeyInput.pWriteTo, g_TypeQueueKeyInput.pWriteTo);
        }
    }

    return 0;
}
