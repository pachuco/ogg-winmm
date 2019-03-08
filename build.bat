@echo off

::Edit gccbase from build.bat or leave it empty if already in %PATH%.
set gccbase=G:\p_files\rtdk\mingw32-gcc5\bin
set GCC=gcc.exe
set AR=ar.exe
set WINDRES=windres.exe

if [%1] equ [force] (set FORCECOMPILE=1)
if [%gccbase%] neq [] set PATH=%PATH%;%gccbase%

set bin=%CD%\bin
set tmp=%CD%\tmp
set err=%CD%
set l_ogg=libogg-1.3.3
set l_vorb=libvorbis-1.3.6
set l_winmm=winmm
set l_rc=wav-winmm

call :del err_*.log 0
call :del %bin%\%l_winmm%.dll 0

set name=%l_ogg%
set opts=-m32 -std=gnu99 -O2 -shared -s
set files=framing.c bitwise.c
set incl=-I.
call :compile_ar %name% "%files% %opts% %incl% %links%"

set name=%l_vorb%
set opts=-m32 -std=gnu99 -O2 -s -static-libgcc
::psytune.c tone.c barkmel.c
set files=analysis.c bitrate.c block.c codebook.c envelope.c floor0.c floor1.c info.c lookup.c lpc.c lsp.c mapping0.c mdct.c psy.c registry.c res0.c sharedbook.c smallft.c synthesis.c vorbisenc.c vorbisfile.c window.c
set files=%files% %bin%\%l_ogg%.a
set incl=-I. -I..\%l_ogg%
set links=
call :compile_ar %name% "%files% %opts% %incl% %links%"

%WINDRES% src\%l_winmm%\%l_rc%.rc.in -O coff -o %bin%\%l_rc%.o

set name=%l_winmm%
set opts=-m32 -std=gnu99 -Wl,--enable-stdcall-fixup -O2 -shared -s
set files=player.c wav-winmm.c stubs.c wav-winmm.def
set files=%files% %bin%\%l_vorb%.a %bin%\%l_ogg%.a %bin%\%l_rc%.o
set incl=-I. -I..\%l_vorb% -I..\%l_ogg%
set links=-lwinmm -lkernel32
call :compile_dll %name% "%files% %opts% %incl% %links%"
set includes=-I

echo .
echo all done!
pause
exit




:errcheck
::objname
if %errorlevel% neq 0 (
    echo oops %~1!
    pause
    call :del %err%\err_%~1.log 1
    call :del %tmp%\* 0
    exit
)
exit /B 0

:compile_dll
::objname params
if not defined FORCECOMPILE ( if exist %bin%\%~1.dll (
    echo skipped %~1.dll
    exit /B 0
))
pushd src\%~1
%GCC% -o %bin%\%~1.dll %~2 2> %err%\err_%~1.log
call :errcheck %~1
call :del %err%\err_%~1.log 1
call :del *.o 0

echo compiled %~1
popd
exit /B 0

:compile_ar
::objname params
if not defined FORCECOMPILE ( if exist %bin%\%~1.a (
    echo skipped %~1.a
    exit /B 0
))
pushd src\%~1
%GCC% -c %~2 2> %err%\err_%~1.log
call :errcheck %~1

%AR% cr %bin%\%~1.a *.o 2> %err%\err_%~1.log
call :errcheck %~1
call :del %err%\err_%~1.log 1
call :del *.o 0

echo compiled %~1
popd
exit /B 0

:del
::file delIfEmpty
if exist %~1 (
    if %~2 equ 1 (
        for %%R in (%~1) do if not %%~zR lss 1 exit /B 0
    )
    del %~1 /Q
)
exit /B 0