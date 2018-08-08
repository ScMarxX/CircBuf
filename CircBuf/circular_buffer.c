/**
 * @file circular_buffer.c
 * @brief
 * @author Mark Xu<ChuiXiao.Xu@agilex.ai>
 * @version 1
 * @date 2018-08-01
 */
#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"

/**
 * @brief     Check if Num is power of 2
 *
 * @param[in] Num   the number to check
 *
 * @return          1 if Num is power of 2
 */
unsigned int IsPowerOf2(unsigned int Num)
{
    return (Num > 0 && !(Num & (Num - 1)));
}

/**
 * @brief     calculate the minimum number that round up to the next power of 2
 *
 * @param[in] Num   the number to calculate
 *
 * @return          the number that round up to the next power of 2 (0x100 if Num is 0xf0, 0x81, 0xa3 ... )
 */
unsigned long RoundUp_PowerOf2(unsigned long Num)
{
    unsigned long result = 1;

    if (IsPowerOf2(Num) || Num == 0)
        return Num;

    while (Num)
    {
        Num >>= 1;
        result <<= 1;
    }

    return result;   // WARN: if Num biger than 0x8000 0000 0000 0000 then result will equals to 0
}

/**
 * @brief     Circular Buffer initialization
 *
 * @param[in] CBuf      the circular buffer to initialization
 * @param[in] Size      size of the circular buffer
 *
 * @return    1 if memery allocation success
 *            0 if fail
 */
int CircBuf_Alloc(CircBuf_t *CBuf, unsigned int Size)
{
    int result = 0;

    if(!IsPowerOf2(Size))
    {
        if(Size >= 0x80000000)
            Size = 0x80000000;
        else
            Size = RoundUp_PowerOf2(Size);
    }
    CBuf->Buffer = (unsigned char *) calloc(Size, sizeof(char));    // Buffer will set to 0

    CBuf->Tailer = 0;
    CBuf->Header = 0;

    if(CBuf->Buffer != NULL)
    {
        CBuf->Size = Size;
        result = 1;
    }

    return result;
}

/**
 * @brief     delete circular buffer and release the memery
 *
 * @param[in] CBuf  the circular buffer to delete
 */
void CircBuf_Delete(CircBuf_t *CBuf)
{
    free(CBuf->Buffer);
    CBuf = NULL;
}

/**
 * @brief     put data into the circular buffer
 *
 * @param[in] CBuf      the circular buffer that will store the data
 * @param[in] data      the data to store into circular buffer
 * @param[in] LenToPut  the length of data to store into circular buffer
 *
 * @return      the actual size stored into circular buffer
 */
unsigned int CircBuf_Put(CircBuf_t *CBuf, unsigned char *data, unsigned int LenToPut)
{
    unsigned int len;

    LenToPut = MIN(LenToPut, (CBuf->Size - (CBuf->Header - CBuf->Tailer)));

    len = MIN(LenToPut, CBuf->Size - (CBuf->Header & (CBuf->Size - 1)));

    memcpy(CBuf->Buffer + (CBuf->Header & CBuf->Size - 1), data, len);
    memcpy(CBuf->Buffer, data+len, LenToPut - len);

    CBuf->Header += LenToPut;

    return LenToPut;
}

/**
 * @brief     get data from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[in] data      target buffer that will store the data that from circular buffer
 * @param[in] LenToGet  the length that wan't to get from circular buffer
 *
 * @return      actual length that get from circular buffer
 */
unsigned int CircBuf_Get(CircBuf_t *CBuf, unsigned char *data, unsigned int LenToGet)
{
    unsigned int len;

    LenToGet = MIN(LenToGet, CBuf->Header - CBuf->Tailer);

    len = MIN(LenToGet, CBuf->Size - (CBuf->Tailer & (CBuf->Size - 1)));

    memcpy(data, CBuf->Buffer + (CBuf->Tailer & (CBuf->Size - 1)), len);
    memcpy(data + len, CBuf->Buffer, LenToGet - len);

    CBuf->Tailer += LenToGet;

    return LenToGet;
}

/**
 * @brief     get one char from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[n] data       target buffer that will store the data that from circular buffer
 *
 * @return              actual length that get from circular buffer
 */
unsigned int CircBuf_GetOneChar(CircBuf_t *CBuf, unsigned char *data)
{
    return CircBuf_Get(CBuf, data, 1);
}

/**
 * @brief     get the Available memery size of circular buffer
 *
 * @param[in] CBuf  the circular buffer to get size
 *
 * @return          Available size of the circular buffer
 */
unsigned int CircBuf_GetAvalaibleSize(CircBuf_t *CBuf)
{
    return ((CBuf->Size > 0) ? (CBuf->Size - (CBuf->Header - CBuf->Tailer)) : 0);
}

/**
* @brief     get the used memery size of circular buffer
*
* @param[in] CBuf  the circular buffer to get size
*
* @return          used size of the circular buffer
*/
unsigned int CircBuf_GetUsedSize(CircBuf_t *CBuf)
{
    return (CBuf->Header - CBuf->Tailer);
}

/**
 * @brief     check if the circular buffer is empty
 *
 * @param[in] CBuf  the circular buffer to check
 *
 * @return          1   if no data stored in the circular buffer
 *                  0   if the size of circular buffer equals to 0
 *                      or some data stored in the circular buffer
 */
unsigned int CircBuf_IsEmpty(CircBuf_t *CBuf)
{
    return ((CBuf->Size > 0) && (CBuf->Header == CBuf->Tailer));
}

/**
 * @brief     check if the circular buffer is full
 *
 * @param[in] CBuf  the circular buffer to check
 *
 * @return      1 if the size of circular buffer equals to 0
 *                  or no Available space of circular buffer
 */
unsigned int CircBuf_IsFull(CircBuf_t *CBuf)
{
    return ((CBuf->Size == 0) || (CBuf->Size == (CBuf->Header - CBuf->Tailer)));
}
