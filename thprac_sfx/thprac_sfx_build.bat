@echo off

rem Really wanted to use #embed for this, so it can ONLY be compiled with Clang, at least for now

python thprac_sfx_pack.py ..\Release\thprac.exe ..\Release\thprac32.dll ..\Release\thprac64.dll
llvm-rc /FO .\thprac_sfx.res ..\thprac\thprac_launcher.rc

clang -m32 -std=c23 -Os -ffreestanding -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -c thprac_sfx_lzma.c -o thprac_sfx_lzma.o
lld-link /machine:x86 /subsystem:windows /entry:start /nodefaultlib /fixed /merge:.rdata=.text /opt:ref /safeseh:no /out:thprac.exe thprac_sfx_lzma.o thprac_launcher.res kernel32.lib