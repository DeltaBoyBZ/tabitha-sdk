#[===[
Copyright 2022 Matthew Peter Smith

This file is provided under the terms of the Mozilla Public License - Version 2.0. 
A copy of the licence can be found in the root of The Tabitha SDK GitHub repository,

https://github.com/DeltaBoyBZ/tabitha-sdk 

or alternatively a copy can be found at,

https://www.mozilla.org/media/MPL/2.0/index.815ca599c9df.txt


]===]

add_library(tabi_std_cross tabi_std_cross.c tabi_std.c)
add_library(tabi_std_raw tabi_std_raw.asm tabi_std.c)
