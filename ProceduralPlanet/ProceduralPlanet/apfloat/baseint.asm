.386
.model flat

include cpp.inc

extrn _Base:DWORD, _tmpll:DWORD, _oldcw:DWORD, _c1one:DWORD, _c2p63:DWORD, _c2p64:DWORD

public _baseadd, _basesub, _basemuladd, _basediv

.code

_baseadd proc

push ebx
push ecx
push esi
push edi
push ebp

mov edi, [24+esp]
mov esi, [28+esp]
mov ecx, [36+esp]
mov eax, [40+esp]

test ecx, ecx
jz baseaddend

baseaddloop:
mov edx, [-8+esi+8*ecx]
mov ebp, [-4+esi+8*ecx]
add edx, eax
mov eax, 0
adc ebp, 0

mov ebx, [32+esp]
test ebx, ebx
jz baseaddloopnosrc2
add edx, [-8+ebx+8*ecx]
mov eax, [-4+ebx+8*ecx]
adc ebp, eax
mov eax, 0
adc eax, eax
baseaddloopnosrc2:

mov ebx, _Base
sub edx, ebx
mov esi, _Base+4
sbb ebp, esi
sbb eax, 0
dec ecx

and ebx, eax
and esi, eax
inc eax
add edx, ebx
adc ebp, esi

test ecx, ecx
mov esi, [28+esp]
mov [edi+8*ecx], edx
mov [4+edi+8*ecx], ebp
jnz baseaddloop

baseaddend:

xor edx, edx

pop ebp
pop edi
pop esi
pop ecx
pop ebx

ret

_baseadd endp

_basesub proc

push ebx
push ecx
push esi
push edi
push ebp

mov edi, [24+esp]
mov esi, [28+esp]
mov ecx, [36+esp]
mov eax, [40+esp]

test ecx, ecx
jz basesubend

basesubloop:
xor edx, edx
xor ebp, ebp

test esi, esi
jz basesubloopnosrc1
mov edx, [-8+esi+8*ecx]
mov ebp, [-4+esi+8*ecx]
basesubloopnosrc1:

xor esi, esi
mov ebx, [32+esp]

test ebx, ebx
jz basesubloopnosrc2
add eax, [-8+ebx+8*ecx]
mov esi, [-4+ebx+8*ecx]
adc esi, 0
basesubloopnosrc2:

sub edx, eax
sbb ebp, esi

mov ebx, _Base
sbb eax, eax
mov esi, _Base+4

and ebx, eax
and esi, eax
neg eax
add edx, ebx
adc ebp, esi

mov esi, [28+esp]
mov [-8+edi+8*ecx], edx
mov [-4+edi+8*ecx], ebp
dec ecx
jnz basesubloop

basesubend:

xor edx, edx

pop ebp
pop edi
pop esi
pop ecx
pop ebx

ret

_basesub endp

_basediv1 proc

push edi
push ebx
push eax
push edx

mov _tmpll, ebp
xor esi, 080000000h
mov _tmpll+4, esi
xor ebx, 080000000h
fild qword ptr _tmpll
mov _tmpll, edi
mov _tmpll+4, ebx
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
mov ebx, ebp
mov edx, _tmpll+4
xor edx, 080000000h
xor ebp, ebp
mov _tmpll+4, edx

mov eax, [4+esp]
mul dword ptr _tmpll
mov esi, eax
mov edi, edx
mov eax, [4+esp]
mul dword ptr _tmpll+4
add edi, eax
adc ebp, edx
mov eax, [esp]
mul dword ptr _tmpll
add edi, eax
adc ebp, edx
mov eax, [esp]
imul eax, _tmpll+4
add ebp, eax

mov eax, [12+esp]
sub eax, esi
mov edx, [8+esp]
sbb edx, edi
mov esi, [4+esp]
sbb ebx, ebp
mov edi, [esp]

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
mov esi, [4+esp]
mov edi, [esp]

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
mov esi, [4+esp]
mov edi, [esp]

sub eax, esi
sbb edx, edi
sbb ebx, 0
and esi, ebx
and edi, ebx
not ebx
add eax, esi
adc edx, edi
mov esi, _tmpll
sub esi, ebx
sbb ebp, ebx

add esp, 16
mov ebx, esi

ret

_basediv1 endp

_basemuladd proc

push ebx
push ecx
push esi
push edi
push ebp

mov eax, _Base
mov edx, _Base+4
cmp edx, 080000000h
jne basemuladdnofast
test eax, eax
jz basemuladdfast
basemuladdnofast:

fnstcw word ptr _oldcw

mov ebx, _oldcw
sub esp, 4
and ebx, 0ffff00ffh
or  ebx, 0700h
mov [esp], ebx
fldcw word ptr [esp]
add esp, 4

xor edx, 080000000h
mov _tmpll, eax
mov _tmpll+4, edx
fild qword ptr _tmpll
fld dword ptr _c2p63
fld dword ptr _c2p64
fxch st(2)
fadd st, st(1)
fdivr dword ptr _c1one
basemuladdfast:

mov esi, [28+esp]
mov ecx, [44+esp]
mov eax, [48+esp]
mov edx, [52+esp]

test ecx, ecx
jz basemuladdend

basemuladdloop:
mov eax, [-8+esi+8*ecx]
mul dword ptr [36+esp]
mov edi, eax
mov ebx, edx
mov eax, [-4+esi+8*ecx]
mul dword ptr [36+esp]
xor ebp, ebp
add ebx, eax
adc ebp, edx
mov eax, [-8+esi+8*ecx]
mul dword ptr [40+esp]
add ebx, eax
mov eax, [-4+esi+8*ecx]
adc ebp, edx
mov esi, 0
adc esi, 0
mul dword ptr [40+esp]
dec ecx
add ebp, eax
adc esi, edx

mov eax, [48+esp]
mov edx, [52+esp]
add edi, eax
adc ebx, edx
adc ebp, 0
adc esi, 0

mov eax, [32+esp]
test eax, eax
jz basemuladdloopnosrc2
add edi, [eax+8*ecx]
mov edx, [4+eax+8*ecx]
adc ebx, edx
adc ebp, 0
adc esi, 0
basemuladdloopnosrc2:

mov eax, _Base
mov edx, _Base+4

cmp edx, 080000000h
jne basemuladdloopnofast
test eax, eax
jnz basemuladdloopnofast
mov eax, edi
mov edx, ebx
shrd ebx, ebp, 31
shrd ebp, esi, 31
and edx, 07fffffffh
jmp basemuladdloopfast
basemuladdloopnofast:
call _basediv1
basemuladdloopfast:

mov edi, [24+esp]
mov esi, [28+esp]

mov [48+esp], ebx
mov [52+esp], ebp

mov [edi+8*ecx], eax
mov eax, ebx
mov [4+edi+8*ecx], edx
mov edx, ebp

test ecx, ecx
jnz basemuladdloop

basemuladdend:

mov ebx, _Base
mov ecx, _Base+4
cmp ecx, 080000000h
jne basemuladdnofastend
test ebx, ebx
jz basemuladdfastend
basemuladdnofastend:
fstp st
fstp st
fstp st

fldcw word ptr _oldcw

basemuladdfastend:

pop ebp
pop edi
pop esi
pop ecx
pop ebx

ret

_basemuladd endp

_basediv proc

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
fdivr dword ptr _c1one

mov edi, [24+esp]
mov esi, [28+esp]
mov ecx, [40+esp]
mov eax, [44+esp]
mov edx, [48+esp]

test ecx, ecx
jz basedivend

lea edi, [edi+8*ecx]
test esi, esi
mov [24+esp], edi
jz basedivnosrc1
lea esi, [esi+8*ecx]
mov [28+esp], esi
basedivnosrc1:
neg ecx

basedivloop:
mov edx, _Base+4
mov eax, _Base
cmp edx, 080000000h
jne basedivloopnofast
test eax, eax
jnz basedivloopnofast
mov ebp, [44+esp]
mov esi, [48+esp]
xor edi, edi
xor ebx, ebx
shrd ebx, ebp, 1
shrd ebp, esi, 1
shr esi, 1
jmp basedivloopfast
basedivloopnofast:
mul dword ptr [44+esp]
mov edi, eax
mov ebx, edx
mov eax, _Base+4
mul dword ptr [44+esp]
xor ebp, ebp
add ebx, eax
adc ebp, edx
mov eax, _Base
mul dword ptr [48+esp]
add ebx, eax
mov eax, _Base+4
adc ebp, edx
mov esi, 0
adc esi, 0
mul dword ptr [48+esp]
add ebp, eax
adc esi, edx
basedivloopfast:

mov eax, [28+esp]
test eax, eax
jz basedivloopnosrc1
add edi, [eax+8*ecx]
mov edx, [4+eax+8*ecx]
adc ebx, edx
adc ebp, 0
adc esi, 0
basedivloopnosrc1:

mov eax, [32+esp]
mov edx, [36+esp]

call _basediv1

mov edi, [24+esp]
mov esi, [28+esp]

mov [44+esp], eax
mov [48+esp], edx

mov [edi+8*ecx], ebx
mov [4+edi+8*ecx], ebp

inc ecx
jnz basedivloop

basedivend:

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

_basediv endp

end
