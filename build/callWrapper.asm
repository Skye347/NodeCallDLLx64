;============================================
; Addup.asm file
;============================================
.code

;--------------------------------------------
Addup PROC
local @count:QWORD, @addre:QWORD, @array:QWORD

mov r14,rcx
test r14,1
jz NOAD
AD:
add r14,1
NOAD:
imul r14,r14,8
add r14,8
mov r15,r14
add r15,200
push rbp
push rdi
sub rsp,r15
lea rbp,[rsp+r14]
mov @count,rcx
mov @addre,rdx
mov @array,r8
cmp @count,1
je ONE
cmp @count,2
je TWO
cmp @count,3
je THREE
cmp @count,4
je FOUR
jmp LARGER

ONE:

mov r15,@array
mov rcx,[r15]
jmp CALLADDR

TWO:

mov r15,@array
mov rcx,[r15]
mov rdx,[r15+8h]
jmp CALLADDR

THREE:

mov r15,@array
mov rcx,[r15]
mov rdx,[r15+8h]
mov r8,[r15+10h]
jmp CALLADDR

FOUR:

mov r15,@array
mov rcx,[r15]
mov rdx,[r15+8h]
mov r8,[r15+10h]
mov r9,[r15+18h]
jmp CALLADDR

LARGER:

mov r15,@array
mov r14,[r15]
mov rdx,[r15+8h]
mov r8,[r15+10h]
mov r9,[r15+18h]
mov rcx,@count
sub rcx,4
lop:
mov r13,rcx
add r13,3
imul r13,r13,8
mov r12,[r15+r13]
sub r13,8
mov [rsp+r13],r12
loop lop
mov rcx,r14
call CALLADDR

CALLADDR:
mov r12,@addre
call r12
lea rsp,[rbp+c8h]
pop rdi
pop rbp
ret

Addup ENDP
;----------------------------------------------

END

0x000000e9b5eff6d8
