.386
.model flat

include cpp.inc

public _moveblock, _transposeblock, _transpose2blocks

.code

_moveblock proc

push ebx
push ecx
push edx
push esi
push edi
push ebp

mov edi, [28+esp]
mov esi, [32+esp]

mov edx, [36+esp]
mov eax, [40+esp]
mov ebx, [44+esp]
sub eax, edx
shl eax, 3
sub ebx, edx
shl ebx, 3

cld

and edx, edx
jz moveblockend

shr dword ptr [36+esp], 2

moveblockloop:

mov ecx, [36+esp]
mov ebp, [edi]

moveblockline:

mov ebp, [28+edi]
mov ebp, [esi]
mov [edi], ebp
mov ebp, [4+esi]
mov [4+edi], ebp
mov ebp, [8+esi]
mov [8+edi], ebp
mov ebp, [12+esi]
mov [12+edi], ebp
mov ebp, [16+esi]
mov [16+edi], ebp
mov ebp, [20+esi]
mov [20+edi], ebp
mov ebp, [24+esi]
mov [24+edi], ebp
mov ebp, [28+esi]
mov [28+edi], ebp

add esi, 32
add edi, 32

dec ecx
jnz moveblockline

add edi, eax
add esi, ebx

dec edx
jnz moveblockloop

moveblockend:

pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx

ret

_moveblock endp

_transposeblock proc

push ebx
push ecx
push edx
push esi
push edi
push ebp

mov edx, [36+esp]
mov ebp, [32+esp]

shl edx, 3
dec ebp
jz transposeblockend

transposeblockloop:

mov edi, [28+esp]
mov esi, [28+esp]
mov ecx, ebp

add edi, edx
add esi, 8

lea eax, [8+edi]
mov [28+esp], eax

transposeblockline:
mov eax, [esi]
mov ebx, [edi]
mov [edi], eax
mov [esi], ebx
mov eax, [4+esi]
mov ebx, [4+edi]
mov [4+edi], eax
mov [4+esi], ebx
add esi, 8
add edi, edx
dec ecx
jnz transposeblockline

dec ebp
jnz transposeblockloop

transposeblockend:

pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx

ret

_transposeblock endp

_transpose2blocks proc

push ebx
push ecx
push edx
push esi
push edi
push ebp

mov edx, [40+esp]

shl edx, 3

mov ebp, [36+esp]

transpose2blocksloop:
mov edi, [28+esp]
mov esi, [32+esp]
mov ecx, [36+esp]

add dword ptr [28+esp], 8
add [32+esp], edx

transpose2blocksline:
mov eax, [esi]
mov ebx, [edi]
mov [edi], eax
mov [esi], ebx
mov eax, [4+esi]
mov ebx, [4+edi]
mov [4+edi], eax
mov [4+esi], ebx
add esi, 8
add edi, edx
dec ecx
jnz transpose2blocksline

dec ebp
jnz transpose2blocksloop

pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx

ret

_transpose2blocks endp

end
