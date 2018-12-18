.386
.model flat

include cpp.inc

public _modmul, _modsub, _modadd

.code
                         
_modmul proc

push ebx
push ecx
push esi
push edi

mov eax, [20+esp]
mul dword ptr [28+esp]
mov edi, eax          
mov ebx, edx          
mov eax, [28+esp]     
mul dword ptr [24+esp]
add ebx, eax          
mov ecx, edx          
adc ecx, 0            
mov eax, [32+esp]     
mul dword ptr [20+esp]
add ebx, eax          
adc ecx, edx          
mov esi, 0            
adc esi, 0            
mov eax, [24+esp]     
mul dword ptr [32+esp]
add ecx, eax          
adc edx, esi          
mov eax, edi          
cmp dword ptr [40+esp], 0ffffffffh
je modmul2
cmp dword ptr [40+esp], 0fffffffch
je modmul1

sub eax, ecx
sbb ebx, edx
mov edi, ecx
sbb esi, esi
shl edi, 8  
shr ecx, 24 
add ebx, edi
adc ecx, 0  
mov edi, edx
shl edi, 8                  
shr edx, 24                 
add ecx, esi                
adc edx, esi
add ecx, edi
adc edx, 0
  
sub eax, ecx
sbb ebx, edx          
mov edi, ecx          
sbb esi, esi          
shl edi, 8            
shr ecx, 24           
add ebx, edi          
adc ecx, 0            
mov edi, edx          
shl edx, 8            
add ecx, esi          
add ecx, edx
                      
sub eax, ecx          
sbb ebx, 0            
mov edi, ecx          
sbb ecx, ecx          
shl edi, 8            
add ebx, edi          
adc ecx, 0            

mov esi, [36+esp]     
mov edx, [40+esp]     
sub eax, esi          
sbb ebx, edx          
sbb ecx, 0            
pop edi               
and esi, ecx          
and edx, ecx          
add eax, esi          
pop esi               
adc edx, ebx          
pop ecx               
pop ebx               

ret                   

modmul1:
sub eax, ecx          
sbb ebx, edx          
mov edi, ecx          
sbb esi, esi          
shl edi, 2            
shr ecx, 30           
add ebx, edi          
adc ecx, 0            
mov edi, edx          
shl edi, 2            
shr edx, 30           
add ecx, esi          
adc edx, esi
add ecx, edi          
adc edx, 0
            
sub eax, ecx          
sbb ebx, edx          
mov edi, ecx          
sbb esi, esi          
shl edi, 2            
shr ecx, 30           
add ebx, edi          
adc ecx, 0            
mov edi, edx          
shl edx, 2            
add ecx, esi          
add ecx, edx          

sub eax, ecx          
sbb ebx, 0            
mov edi, ecx          
sbb ecx, ecx          
shl edi, 2            
add ebx, edi          
adc ecx, 0            

mov esi, [36+esp]     
mov edx, [40+esp]     
sub eax, esi          
sbb ebx, edx          
sbb ecx, 0            
pop edi               
and esi, ecx          
and edx, ecx          
add eax, esi          
pop esi               
adc edx, ebx          
pop ecx               
pop ebx               

ret                   

modmul2:
sub eax, ecx          
sbb ebx, edx          
sbb edx, 0            
add ebx, ecx          
adc edx, 0            

sub eax, edx                  
sbb ebx, 0                    
mov ecx, 0                    
sbb ecx, 0                    
add ebx, edx                  
adc ecx, 0                    

mov esi, [36+esp]             
mov edx, [40+esp]             
sub eax, esi                  
sbb ebx, edx                  
sbb ecx, 0                    
pop edi                       
and esi, ecx                  
and edx, ecx                  
add eax, esi                  
pop esi                       
adc edx, ebx                  
pop ecx                       
pop ebx                       

ret
                              
_modmul endp

_modadd proc

push ebx                  
push ecx                  
push esi                  
xor esi, esi              

mov eax, [16+esp]         
mov edx, [20+esp]         
mov ebx, [24+esp]         
mov ecx, [36+esp]         
add eax, ebx              
mov ebx, [28+esp]         
adc edx, ebx              
mov ebx, [32+esp]         
adc esi, esi              
sub eax, ebx              
sbb edx, ecx              
sbb esi, 0                
and ebx, esi              
and ecx, esi              
add eax, ebx              
pop esi                   
adc edx, ecx              
pop ecx                   
pop ebx
                          
ret                       

_modadd endp

_modsub proc

push ebx
push ecx
push esi
                 
mov eax, [16+esp]             
mov edx, [20+esp]             
mov ebx, [24+esp]             
sub eax, ebx                  
mov ebx, [28+esp]             
sbb edx, ebx                  
mov ebx, [32+esp]             
sbb esi, esi                  
mov ecx, [36+esp]             
and ebx, esi                  
and ecx, esi                  
add eax, ebx                  
pop esi                       
adc edx, ecx                  
pop ecx                       
pop ebx                       

ret

_modsub endp

end
