data segment
    arr db 1,2,4,8,16,32,64,128,1             
ends             
stack segment
    dw   1000H  dup(0)
ends
                      

code segment       
    assume cs:code,ss:stack,ds:data
delay:
    push bx
    push cx
    mov bx,39
lp:
    mov cx,5882
lp1:
    dec cx
    cmp cx,0
    jg lp1
    
    dec bx
    cmp bx,0
    jg lp
    
    pop cx
    pop bx
    ret
    
start:
    mov ax,data
    mov ds,ax
    
    mov bx,8;
    mov cx,0;
    mov dx,0;
    mov si,0;
    
while: 
    mov ax,[si]
    xchg al,ah
    
    out dx,ax
    inc dx
call delay    
    
    
    out dx,ax
    inc dx 
call delay            
            
    mov ax,dx
    div bl
    mov dl,ah
    mov si,dx
    
    
    
    
    jmp while
                                        .
ends

end start ; 
