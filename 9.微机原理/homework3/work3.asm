data segment
    arr db 21H,20H,22H,14H,0CH,04H            
ends            
stack segment
    dw   1000H  dup(0)
ends
                      

code segment       
    assume cs:code,ss:stack,ds:data

;��cx�������ӳ�ʱ�䡣��λΪms��
;mov cx,1000����ʱ1��
delay:
    push bx
lp:
    mov bx,220
lp1:
    dec bx
    cmp bx,0
    jg lp1
    
    dec cx
    cmp cx,0
    jg lp
    
    pop bx
    ret

;��cx������˸����    
flash:

lp2:
    push cx
    
    inc dx
    out dx,ax 
    mov cx,500 
call delay               
    
    
    dec dx            
    out dx,ax   
    mov cx,500 
call delay    


    pop cx
    dec cx
    cmp cx,0
    jg lp2
    
    ret 
    
    

    
    
start:
    mov ax,data
    mov ds,ax
        
while: 

;�����̵�5��
    mov dx,0;
    mov si,0;    
    mov ax,[si]
    xchg al,ah
    out dx,ax
    mov cx,5000
call delay    

;������˸���
    mov cx,5
call flash;
    
;������Ƶ�    
    mov si,2
    mov dx,2
    mov ax,[si]
    xchg al,ah 
    
    out dx,ax
    mov cx,2000
call delay

;�ϱ��̵�5��
    mov si,4
    mov dx,4
    mov ax,[si]
    xchg al,ah
    out dx,ax
    mov cx,5000
call delay
 
;��˸5��    
    mov cx,5
call flash    

;�ϱ���Ƶ�
    mov si,2
    mov dx,3
    mov ax,[si]
    xchg al,ah 
    
    out dx,ax
    mov cx,2000
call delay
                
    jmp while
                                        .
ends

end start ; 
