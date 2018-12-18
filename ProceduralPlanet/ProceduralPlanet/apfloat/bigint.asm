.386
.model flat

include cpp.inc

extrn _tmpll:DWORD, _oldcw:DWORD, _c1one:DWORD, _c2p63:DWORD, _c2p64:DWORD

public _bigadd, _bigsub, _bigmul, _bigdiv, _bigcmp, _bigshr

.code

_bigadd proc

push ecx
push esi
push edi

mov edi, [16+esp]
mov esi, [20+esp]
mov ecx, [24+esp]
add ecx, ecx

jz bigaddcarry

lea esi, [esi+4*ecx]
lea edi, [edi+4*ecx]
not ecx
inc ecx

bigaddloop:
mov eax, [esi+4*ecx]
adc [edi+4*ecx], eax
inc ecx
jnz bigaddloop

bigaddcarry:

jnc bigaddend

mov ecx, [28+esp]
add ecx, 080000000h
add ecx, ecx

jz bigaddend

lea edi, [edi+4*ecx]
not ecx
inc ecx

bigaddcarryloop:
adc dword ptr [edi+4*ecx], 0
jnc bigaddend
inc ecx
jnz bigaddcarryloop

bigaddend:

mov eax, 0
mov edx, 0
adc eax, 0

pop edi
pop esi
pop ecx

ret

_bigadd endp
            
_bigsub proc

push ecx
push esi
push edi

mov edi, [16+esp]
mov esi, [20+esp]
mov ecx, [24+esp]
add ecx, ecx

jz bigsubcarry

lea esi, [esi+4*ecx]
lea edi, [edi+4*ecx]
not ecx
inc ecx

bigsubloop:
mov eax, [esi+4*ecx]
sbb [edi+4*ecx], eax
inc ecx
jnz bigsubloop

bigsubcarry:

jnc bigsubend

mov ecx, [28+esp]
add ecx, 080000000h
add ecx, ecx

jz bigsubend

lea edi, [edi+4*ecx]
not ecx
inc ecx

bigsubcarryloop:
sbb dword ptr [edi+4*ecx], 0
jnc bigsubend
inc ecx
jnz bigsubcarryloop

bigsubend:

mov eax, 0
mov edx, 0
adc eax, 0

pop edi
pop esi
pop ecx

ret

_bigsub endp

_bigmul proc

push ebx
push ecx
push esi
push edi
push ebp

mov edi, [24+esp]
mov esi, [28+esp]
mov ecx, [40+esp]
xor ebx, ebx
xor ebp, ebp
add ecx, ecx

jz bigmulend

lea edi, [edi+4*ecx]
lea esi, [esi+4*ecx]
neg ecx

bigmulloop:
mov eax, [32+esp]
mul dword ptr [esi+4*ecx]
add eax, ebx
mov ebx, 0
adc ebp, edx
mov edx, [esi+4*ecx]
adc ebx, ebx
mov [edi+4*ecx], eax

mov eax, [36+esp]
mul edx
add eax, ebp
mov ebp, ebx
adc ebp, edx
mov ebx, eax

inc ecx
jnz bigmulloop

bigmulend:

mov eax, ebx
mov edx, ebp

pop ebp
pop edi
pop esi
pop ecx
pop ebx

ret

_bigmul endp

_bigdiv proc

fnstcw word ptr _oldcw

push ebx
push ecx
push esi
push edi
push ebp

mov eax, _oldcw
sub esp, 4
and eax, 0ffff00ffh
or  eax, 0700h
mov [esp], eax
fldcw word ptr [esp]
add esp, 4

mov edx, [36+esp]
mov eax, [32+esp]
xor edx, 080000000h
mov _tmpll, eax
mov _tmpll+4, edx
fild qword ptr _tmpll
fld dword ptr _c2p63
fld dword ptr _c2p64
fxch st(2)
fadd st, st(1)

mov edi, [24+esp]
mov esi, [28+esp]
mov ecx, [40+esp]

fdivr dword ptr _c1one

xor eax, eax
xor edx, edx

test ecx, ecx
jz bigdivend

mov ebx, [-8+esi+8*ecx]
mov ebp, [-4+esi+8*ecx]
cmp ebp, [36+esp]
ja bigdivloop
jb bigdivsub
cmp ebx, [32+esp]
jae bigdivloop
bigdivsub:
mov [-8+edi+8*ecx], eax
mov [-4+edi+8*ecx], eax
mov eax, ebx
mov edx, ebp
dec ecx

jz bigdivend

bigdivloop:
mov _tmpll, eax
xor edx, 080000000h
mov _tmpll+4, edx
fild qword ptr _tmpll
mov ebx, [-8+esi+8*ecx]
mov ebp, [-4+esi+8*ecx]
mov _tmpll, ebx
xor ebp, 080000000h
mov _tmpll+4, ebp
fild qword ptr _tmpll
fxch st(1)
fadd st, st(3)
fxch st(1)
fadd st, st(3)
fxch st(1)
fmul st, st(4)
faddp st(1), st
fmul st, st(1)
fsub st, st(2)
fistp qword ptr _tmpll
mov ebx, eax
mov edx, _tmpll+4
xor edx, 080000000h
xor ebp, ebp
mov _tmpll+4, edx

mov eax, [32+esp]
mul dword ptr _tmpll
mov esi, eax
mov edi, edx
mov eax, [32+esp]
mul dword ptr _tmpll+4
add edi, eax
adc ebp, edx
mov eax, [36+esp]
mul dword ptr _tmpll
add edi, eax
adc ebp, edx
mov eax, [36+esp]
imul eax, _tmpll+4
mov edx, esi
mov esi, [28+esp]
add ebp, eax

mov eax, [-8+esi+8*ecx]
sub eax, edx
mov edx, [-4+esi+8*ecx]
sbb edx, edi
mov esi, [32+esp]
sbb ebx, ebp
mov edi, [36+esp]

sub eax, esi
sbb edx, edi
sbb ebx, 0
mov ebp, ebx
sar ebp, 31
and esi, ebp
and edi, ebp
add eax, esi
adc edx, edi
mov esi, _tmpll
adc ebx, 0
not ebp
mov edi, _tmpll+4
sub esi, ebp
sbb edi, ebp
mov _tmpll, esi
mov _tmpll+4, edi
mov esi, [32+esp]
mov edi, [36+esp]

sub eax, esi
sbb edx, edi
sbb ebx, 0
mov ebp, ebx
sar ebp, 31
and esi, ebp
and edi, ebp
add eax, esi
adc edx, edi
mov esi, _tmpll
adc ebx, 0
not ebp
mov edi, _tmpll+4
sub esi, ebp
sbb edi, ebp
mov _tmpll, esi
mov ebp, edi
mov esi, [32+esp]
mov edi, [36+esp]

sub eax, esi
sbb edx, edi
sbb ebx, 0
and esi, ebx
and edi, ebx
not ebx
add eax, esi
adc edx, edi
mov esi, _tmpll
mov edi, [24+esp]
sub esi, ebx
sbb ebp, ebx

mov [-8+edi+8*ecx], esi
mov [-4+edi+8*ecx], ebp

mov esi, [28+esp]

dec ecx
jnz bigdivloop

bigdivend:

fstp st
fstp st
fstp st

fldcw word ptr _oldcw

pop ebp
pop edi
pop esi
pop ecx
pop ebx

ret

_bigdiv endp

_bigshr proc

push ecx
push esi
push edi

mov edi, [16+esp]
mov esi, [20+esp]
mov ecx, [24+esp]
add ecx, ecx

jz bigshrend

bigshrloop:
mov eax, [-4+esi+4*ecx]
rcr eax, 1
mov [-4+edi+4*ecx], eax
dec ecx
jnz bigshrloop

bigshrend:

mov eax, 0
adc eax, 0

pop edi
pop esi
pop ecx

ret

_bigshr endp

_bigcmp proc

push ecx
push esi
push edi

mov edi, [16+esp]
mov esi, [20+esp]
mov ecx, [24+esp]
add ecx, ecx

jz bigcmpend

sub edi, 4
sub esi, 4

bigcmploop:
mov eax, [edi+4*ecx]
cmp eax, [esi+4*ecx]
jb bigcmpb
ja bigcmpa
dec ecx
jnz bigcmploop

bigcmpend:
xor eax, eax

pop edi
pop esi
pop ecx

ret

bigcmpb:
mov eax, -1

pop edi
pop esi
pop ecx

ret

bigcmpa:
mov eax, 1

pop edi
pop esi
pop ecx

ret

_bigcmp endp

end
