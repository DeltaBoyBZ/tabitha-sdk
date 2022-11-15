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

extern std_strLen
extern _exit

; arg order rdi, rsi, rdx, r10, r8, r9

%define SYS_READ 0
%define SYS_WRITE 1
%define SYS_OPEN 2
%define SYS_CLOSE 3
%define SYS_LSEEK 8
%define SYS_STAT 5
%define SYS_FSTAT 6


%define STDIN 0 
%define STDOUT 1
%define STDERR 2

%define O_RDONLY 0
%define O_WRONLY 1
%define O_RDWR 2

%define S_IRWXU 448
%define S_IRWXG 56
%define S_IRWXO 7

%define O_CREAT 64
%define O_APPEND 1024
%define O_TRUNC 512

%define SEEK_SET 0
%define SEEK_CUR 1
%define SEEK_END 2

%define STAT_SIZE 144
%define STAT_FILESIZE_OFFSET 0x30

section .data
newline: db 0x0a

section .text

global std_printLn
global std_readLn
global std_sin
global std_cos
global std_readFile
global std_fileLen
global std_writeFile
global std_appendFile


std_printLn:
; print the main text
push rdi ; rdi has the string ptr 
call std_strLen
mov rdi, rax
mov rdx, rax
mov rax, 1
mov rdi, STDOUT
pop rsi
syscall
; print a newline
mov rax, 1
mov rdi, STDOUT
mov rsi, newline 
mov rdx, 1
syscall 
ret

std_cast:
mov rax, rdi
ret 

std_sin:
push rax
movss dword [rsp], xmm0
fld dword [rsp]
fsin
fstp dword [rsp]
movss xmm0, [rsp]
pop rax
ret 

std_cos: 
push rax
movss dword [rsp], xmm0
fld dword [rsp]
fcos
fstp dword [rsp]
movss xmm0, [rsp]
pop rax 
ret 

; args (filename) 
std_fileLen: 
sub rsp, 0x90
mov rax, SYS_FSTAT
; rdi already as the correct value (filename) 
mov rsi, rsp  
syscall
mov qword rax, [rsp+STAT_FILESIZE_OFFSET]
add rsp, 0x90
ret 

; args (filename, buffer)
std_readFile: 
sub rsp, 0x18 ; buffer, fd, length
mov qword [rsp], rsi 
; open the file
mov rax, SYS_OPEN
; rdi already has the correct value 
mov rsi, O_RDONLY 
mov rdx, 0
syscall
mov qword [rsp+0x08], rax
; get the length of the file
mov rdi, [rsp+0x08]
call std_fileLen
mov qword [rsp+0x10], rax
; get file length
mov rdi, [rsp+0x08]
sub rsp, 0x90
mov rax, SYS_STAT
mov rsi, rsp  
syscall
mov qword rax, [rsp+STAT_FILESIZE_OFFSET]
add rsp, 0x90
mov qword [rsp+0x10], rax
; read from the file
mov rax, SYS_READ
mov qword rdi, [rsp+0x08]
mov qword rsi, [rsp]
mov qword rdx, [rsp+0x10]
syscall
; close the file
mov qword rdi, [rsp+0x08]
mov rax, SYS_CLOSE
syscall
; add a null character
mov qword rax, [rsp+0x00] 
mov qword rbx, [rsp+0x10]
add rax, rbx
inc rax
mov byte [rax], 0
add rsp, 0x18
ret

; args (filename, text)
std_writeFile:
sub rsp, 0x18 ; fd, text, length
mov qword [rsp+0x08], rsi
; open the file
mov rax, SYS_OPEN
; rdi already has the correct value (the filename)
mov rsi, O_WRONLY+O_CREAT+O_TRUNC
mov rdx, S_IRWXG+S_IRWXU+S_IRWXO
syscall
mov qword [rsp], rax ; store the fd
; get the length of the output text 
mov rdi, [rsp+0x08] 
call std_strLen
mov qword [rsp+0x10], rax
; write the text to the file 
mov rax, SYS_WRITE
mov qword rdi, [rsp] 
mov qword rsi, [rsp+0x08]
mov qword rdx, [rsp+0x10]
syscall
; close the file
mov rax, SYS_CLOSE
mov qword rdi, [rsp]
syscall
add rsp, 0x18
ret 

; args (filename, text)
std_appendFile: 
sub rsp, 0x18 ; fd, text, length 
mov qword [rsp+0x08], rsi
; open the file
; open the file
mov rax, SYS_OPEN
; rdi already has the correct value (the filename)
mov rsi, O_RDWR+O_CREAT+O_APPEND
mov rdx, S_IRWXG+S_IRWXU+S_IRWXO
syscall
mov qword [rsp], rax ; store the fd
; get the length of the output text 
mov rdi, [rsp+0x08] 
call std_strLen
mov qword [rsp+0x10], rax
; write the text to the file 
mov rax, SYS_WRITE
mov qword rdi, [rsp] 
mov qword rsi, [rsp+0x08]
mov qword rdx, [rsp+0x10]
syscall
; close the file
mov rax, SYS_CLOSE
mov qword rdi, [rsp]
syscall
add rsp, 0x18
ret 

; args (buffer)
std_readLn:
sub rsp, 0x10 ; index, buffer
mov qword [rsp], 0 
mov qword [rsp+0x08], rdi
.readChar:
mov qword rsi, [rsp+0x08]
mov qword rbx, [rsp]
add rsi, rbx
mov rdi, 0
mov rdx, 1
mov rax, SYS_READ
syscall
.checkChar:
mov qword rax, [rsp+0x08] 
mov qword rbx, [rsp] 
add rax, rbx
mov qword rcx, [rax] 
cmp rcx, 0x0a
je .end
inc rbx
mov qword [rsp], rbx 
jmp .readChar 
.end:  
mov qword rax, [rsp+0x08]
mov qword rbx, [rsp]
add rax, rbx
mov qword [rax], 0
add rsp, 0x10
ret 

