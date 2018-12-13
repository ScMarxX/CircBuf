#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <thread>
#include <time.h>
#include "circular_buffer.h"
#include <map>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

bool IsWthreadRun = false;
bool IsRthreadRun = false;
bool IsWNthreadRun = false;

static long Counter1 = 0, Counter2 = 0, Counter3 = 0;

void GenRandString(char *buf, int maxsize)
{
    srand(Counter1++);

    for (int i = 0; i < maxsize-1; i++)
    {
        buf[i] = (rand() % (26)) + 'A';
    }
    buf[maxsize - 1] = 0;
}

void GenRandNum(char *buf, int maxsize)
{
    srand(Counter1++);

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

        unsigned int result = CircBuf_Push(CBuf, (unsigned char *)RndStr, 32);

        printf("Tx: %s | %s | Push: % 2d | Size: %d Used: % 3d\n",
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

        unsigned int result = CircBuf_Push(CBuf, (unsigned char *)RndStr, 32);

        printf("Tx: %s | %s | Push: % 2d | Size: %d Used: % 3d\n",
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
        unsigned int result = CircBuf_Pop(CBuf, (unsigned char *)RxBuf, 66);

        printf("Rx: %s | %s | Pop: % 2d | Size: %d Used: % 3d\n",
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
#if 0
#define MAX_COUNT 0x800
#define TEST_COUNT 40960

int main(int argc, char *argv[])
{
    long i = 0;
    unsigned char c = 0;
    map<long, unsigned char> TestMap;
    chrono::steady_clock::time_point Start = chrono::steady_clock::now();
    printf("Creating Map: %d element\n", MAX_COUNT);
    srand((unsigned)Start.max);


    for (i = 0; i < MAX_COUNT; i++)
    {
        c = i & 0xff;
        TestMap[i] = c;
    }
    auto Elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - Start).count();
    printf("Create Map elapsed %lld ms\n", Elapsed);
    vector<long long> ElapsedTimes;
    long long tmp = 0;

    auto End = chrono::steady_clock::now();
    auto AvgElapsed = Elapsed;
    long rnd = 0;
    AvgElapsed = 0;

   for (int j = 0; j < TEST_COUNT; j++)
    {

       rnd = rand()*150;
       rnd &= MAX_COUNT - 1;

        Start = chrono::steady_clock::now();
        auto it = TestMap.find(rnd);
        End = chrono::steady_clock::now();
        Elapsed = chrono::duration_cast<chrono::nanoseconds>(End - Start).count();

        AvgElapsed += Elapsed;
        if (it != TestMap.end())
            printf("Found: [% 9d, 0x%02X], in % 7lld ns\n", it->first, it->second, Elapsed);
    }
    printf("Average Elapsed(find): %lld ns, TestCount %d\n\n", AvgElapsed / TEST_COUNT, TEST_COUNT);
    unsigned char tmp1 = 0;
    AvgElapsed = 0;
    for (int j = 0; j < TEST_COUNT; j++)
    {

        rnd = rand() * 150;
        rnd &= MAX_COUNT - 1;

        Start = chrono::steady_clock::now();
        tmp1 = TestMap[rnd];
        End = chrono::steady_clock::now();
        Elapsed = chrono::duration_cast<chrono::nanoseconds>(End - Start).count();

        AvgElapsed += Elapsed;
        printf("Found: [% 9d, 0x%02X], in % 7lld ns\n", rnd, tmp1, Elapsed);
    }
    printf("Average Elapsed([]): %lld ns, TestCount %d\n", AvgElapsed / TEST_COUNT, TEST_COUNT);
    //system("pause");
    return 0;
}
#endif

#if 1
int main(int argc, char *argv[])
{
    CircBuf_t CBuf;

    printf("RoundUp_PowerOf2:\n%u: %u\n%u: %d\n%u: %d\n",
        400, RoundUp_PowerOf2(400),
        (unsigned long)ULONG_MAX/2, RoundUp_PowerOf2(ULONG_MAX/2 -1),
        (unsigned long)ULONG_MAX, RoundUp_PowerOf2(ULONG_MAX)
        );


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
    CircBuf_Free(&CBuf);
    return 0;
}
#endif
