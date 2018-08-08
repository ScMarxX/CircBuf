/**
 * @file circular_buffer.h
 * @brief     circular buffer
 * @author Mark Xu<ChuiXiao.Xu@agilex.ai>
 * @version 1
 * @date 2018-08-01
 */
#ifndef __CIRC_BUF__
#define __CIRC_BUF__

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b)       (((a) > (b)) ? (b) : (a))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

typedef struct CIRC_BUF
{
    unsigned int Size;
    unsigned int Header;
    unsigned int Tailer;
    unsigned char *Buffer;
} CircBuf_t;

unsigned int IsPowerOf2 (unsigned int Num);
unsigned long RoundUp_PowerOf2 (unsigned long Num);
int CircBuf_Alloc(CircBuf_t *CBuf, unsigned int Size);
void CircBuf_Delete (CircBuf_t *CBuf);
unsigned int CircBuf_Put (CircBuf_t *CBuf, unsigned char *data, unsigned int LenToPut);
unsigned int CircBuf_Get (CircBuf_t *CBuf, unsigned char *data, unsigned int LenToGet);
unsigned int CircBuf_GetOneChar (CircBuf_t *CBuf, unsigned char *data);
unsigned int CircBuf_GetAvalaibleSize(CircBuf_t *CBuf);
unsigned int CircBuf_GetUsedSize(CircBuf_t *CBuf);
unsigned int CircBuf_IsEmpty(CircBuf_t *CBuf);
unsigned int CircBuf_IsFull(CircBuf_t *CBuf);

#ifdef __cplusplus
}
#endif

#endif

