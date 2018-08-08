#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <thread>
#include <time.h>
#include "circular_buffer.h"

bool IsWthreadRun = false;
bool IsRthreadRun = false;
bool IsWNthreadRun = false;

static long Counter1 = 0, Counter2 = 0, Counter3 = 0;

void GenRandString(char *buf, int maxsize)
{
    srand(Counter1);

    for (int i = 0; i < maxsize-1; i++)
    {
        buf[i] = (rand() % (26)) + 'A';
    }
    buf[maxsize - 1] = 0;
}

void GenRandNum(char *buf, int maxsize)
{
    srand(Counter1);

    for (int i = 0; i < maxsize - 1; i++)
    {
        buf[i] = (rand() % (10)) + '0';
    }
    buf[maxsize - 1] = 0;
}

int writeNbuf(CircBuf_t *CBuf)
{
    char RndStr[33] = { 0 };
    while (IsWNthreadRun)
    {
        GenRandNum(RndStr, 33);

        unsigned int result = CircBuf_Put(CBuf, (unsigned char *)RndStr, 32);

        printf("Tx: %s | %s | Put: % 2d | Size: %d Used: % 3d\n",
            RndStr,
            (result > 0 ? "WS" : "WF"),
            result,
            CBuf->Size,
            CircBuf_GetUsedSize(CBuf));

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        Counter3++;
        if (Counter3 >= 10000) IsWNthreadRun = false;
    }
    return 0;
}

int writebuf(CircBuf_t *CBuf)
{
    char RndStr[33] = { 0 };
    while (IsWthreadRun)
    {
        GenRandString(RndStr, 33);

        unsigned int result = CircBuf_Put(CBuf, (unsigned char *)RndStr, 32);

        printf("Tx: %s | %s | Put: % 2d | Size: %d Used: % 3d\n",
            RndStr,
            (result > 0 ? "WS" : "WF"),
            result,
            CBuf->Size,
            CircBuf_GetUsedSize(CBuf));

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        Counter1++;
        if (Counter1 >= 10000) IsWthreadRun = false;
    }
    return 0;
}

int readbuf(CircBuf_t *CBuf)
{
    char RxBuf[67] = { 0 };
    while (IsRthreadRun)
    {
        memset(RxBuf, 0, 67);
        unsigned int result = CircBuf_Get(CBuf, (unsigned char *)RxBuf, 66);

        printf("Rx: %s | %s | Get: % 2d | Size: %d Used: % 3d\n",
            RxBuf,
            (result > 0 ? "RS" : "RF"),
            result,
            CBuf->Size,
            CircBuf_GetUsedSize(CBuf));

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        Counter2++;
        if (Counter2 >= 10000) IsRthreadRun = false;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    CircBuf_t CBuf;
    CircBuf_Alloc(&CBuf, 20 * 1024);
    printf("Size: %d, 0x%0x\n", CBuf.Size, CBuf.Size);

    /**/
    IsWthreadRun = true;
    std::thread Wthread(writebuf, &CBuf);
    Wthread.detach();
    /*
    IsWNthreadRun = true;
    std::thread WNthread(writeNbuf, &CBuf);
    WNthread.detach();
    */
    IsRthreadRun = true;
    std::thread Rthread(readbuf, &CBuf);
    Rthread.detach();
    /**/
    char Input[32] = { 0 };
    scanf_s("%s", Input, 32);
    IsWthreadRun = false;
    IsRthreadRun = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    CircBuf_Delete(&CBuf);
    return 0;
}