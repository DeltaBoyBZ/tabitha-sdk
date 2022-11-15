#[===[
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt

]===]

add_library(tabi_core_cross tabi_core.c tabi_core_cross.c) 
add_library(tabi_core_raw tabi_start.asm tabi_core.c tabi_core_raw.asm) 
