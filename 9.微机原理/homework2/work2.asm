
stack segment
    dw   1000H  dup(0)
ends

code segment       
    assume cs:code,ss:stack
start:
    mov dx,8000H
    mov ax,0    
    mov ds,ax
    mov bx,0
    mov cx,0
while:
    out dx,ax
    inc dx
    inc ah
    inc al
    cmp al,102
    jl while
    mov ax,0
                                        .
ends

end start ; 
