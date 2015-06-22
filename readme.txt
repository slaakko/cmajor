*********************
Cmajor version 1.0.0
*********************

o	Building the System library in Windows:

cd %APPDATA%\Cmajor\system
build.bat

o	Building the System library in Linux:

make sys

========================
Troubleshooting in Windows
==========================

o	library reference 'system.cml' not found.

You have to build the System Library for the used configuration (debug/release) and backend (LLVM/C) first.

o	Cannot start llc.exe because libgcc_s_sjlj-1.dll is missing (or something like that).

The bin directory of mingw_w64
(in my machine C:\Program Files\mingw-w64\x86_64-5.1.0-posix-sjlj-rt_v4-rev0\mingw64\bin)
must be in the PATH environment variable.

o	Build seems to succeed but program does not work, or other mysterious error.

Try rebuild command (-R option) or clean and then build.

o	IDE messes up things.

Try using the command line compiler (cmc.exe).

========================
Troubleshooting in Linux
========================

o   library reference 'system.cml' not found

You have to build the System Library for the used configuration (debug/release) and backend (LLVM/C) first.

o   sh: 1: llc: not found

You have probably not installed LLVM tools.

o   Build seems to succeed but program does not work, or other mysterious error.

Try rebuild command (-R option) or clean and then build.

o   Ubuntu Document Viewer does not show PDF documents included in Cmajor.

At least okular shows them beautifully.
