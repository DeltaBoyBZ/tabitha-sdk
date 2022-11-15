; 
; Copyright 2022 Matthew Peter Smith
; 
; This file is provided under the terms of the Mozilla Public License - Version 2.0. 
; A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,
; 
; https://github.com/DeltaBoyBZ/tabitha-sdk 
; 
; or alternatively a copy can be found at,
; 
; https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt
; 

extern main 
extern _tabi_init
extern _tabi_destroy

section .text

global _exit

global core_alloc
global core_dealloc
global core_memcpy

; arg order: rdi, rsi, rdx, r10, r8, r9

%define SYSCALL_MMAP 9
%define SYSCALL_MUNMAP 11

%define PROT_READ 1
%define PROT_WRITE 2

%define MAP_ANONYMOUS 32
%define MAP_SHARED 1

; args (numbytes) 
core_alloc: 
push rdi
mov rsi, rdi                  ; length
add rsi, 8                    ; need extra space to store the length itself 
mov rax, SYSCALL_MMAP
xor rdi, rdi                  ; address (null because we want the system to provide this) 
mov rdx, PROT_READ+PROT_WRITE ; memory protection (can read and write)
mov r10, MAP_SHARED+MAP_ANONYMOUS        ; memory flags - memory is not associated with a file
mov r8, -1                    ; file descriptor (probably ignored due to flags)
mov r9, 0                     ; offset
syscall 
pop rsi 
mov qword [rax], rsi          ; store the length of the allocation 
add rax, 8                    
ret 

; args (ptr)
core_dealloc:
mov rax, SYSCALL_MUNMAP
sub rdi, 8
mov rsi, [rdi] 
syscall
ret

; arg (dest, src, numBytes)
core_memcpy:
mov rbx, -1
.copy_byte: 
mov rax, [rsi]
mov [rdi], rax  
inc rbx 
inc rsi
inc rdi 
cmp rbx, rdx
jl .copy_byte 
ret  


_exit:
mov rax, 60
syscall


   
