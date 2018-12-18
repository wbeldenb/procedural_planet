#pragma inline
#include "ap.h"


// Optimized routines for moving big blocks of data

extern "C"
{
void moveraw (void *d, void *s, size_t n)
{
    __asm
    {
        mov edi, d
        mov esi, s
        mov ecx, n
        add ecx, ecx

        cld

        rep movsd
    }
}

void swapraw (void *d, void *s, size_t n)
{
    __asm
    {
        mov edi, d
        mov esi, s
        mov ecx, n
        add ecx, ecx

        jz swaprawend

        swaprawloop:
        mov eax, [edi]
        mov ebx, [esi]
        mov [esi], eax
        mov [edi], ebx
        add esi, 4
        add edi, 4
        dec ecx
        jnz swaprawloop

        swaprawend:
    }
}
}
