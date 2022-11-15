/*
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt

*/

section .text
global _start
extern main 
extern _tabi_init
extern _tabi_destroy

_start:
call _tabi_init
call main
push rax
call _tabi_destroy
pop rdi
mov rax, 60
syscall

