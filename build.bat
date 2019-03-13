@echo off

::Edit gccbase from build.bat or leave it empty if already in %PATH%.
set gccbase=G:\p_files\rtdk\mingw32-gcc5\bin
set GCC=gcc.exe
set AR=ar.exe
set WINDRES=windres.exe

if [%1] equ [M_FORCE] (set M_FORCE=1)
for %%a in (%*) DO (
    if [%%a] equ [force] (set M_FORCE=1)
    if [%%a] equ [debug] (set M_DEBUG=1)
)

if [%gccbase%] neq [] set PATH=%PATH%;%gccbase%
set bin=%CD%\bin
set tmp=%CD%\tmp
set err=%CD%

set l_ogg=libogg-1.3.3
set l_vorb=libvorbis-1.3.6
set l_winmm=winmm
set l_testmci=testmci

call :del err_*.log 0
call :del %bin%\%l_winmm%.dll 0
call :del %bin%\%l_testmci%.exe 0

set name=%l_ogg%
set files=framing.c bitwise.c
set inclinks=-I.
call :compile_ar %name% "%files% %inclinks% -m32 -std=gnu99 -O2 -shared -s"


set name=%l_vorb%
::psytune.c tone.c barkmel.c
set files=analysis.c bitrate.c block.c codebook.c envelope.c floor0.c floor1.c info.c lookup.c lpc.c lsp.c mapping0.c mdct.c psy.c registry.c res0.c sharedbook.c smallft.c synthesis.c vorbisenc.c vorbisfile.c window.c
set files=%files% %bin%\%l_ogg%.a
set inclinks=-I. -I..\%l_ogg%
call :compile_ar %name% "%files% %inclinks% -m32 -std=gnu99 -O2 -shared -s"

set name=%l_testmci%
set files=testmci.c
set inclinks=-lwinmm -I.
call :compile_bin %name% "%files% %inclinks% -mconsole -m32 -std=gnu99 -Wl,--enable-stdcall-fixup -O2 -s" exe

%WINDRES% src\%l_winmm%\wav-winmm.rc.in -O coff -o %bin%\wav-winmm-rc.o
set name=%l_winmm%
if defined M_DEBUG set o_dbg=%opts% -D _DEBUG
set files=player.c wav-winmm.c stubs.c cdplayer.c wav-winmm.def
set files=%files% sup\winmm_out.c sup\winfile.c sup\util.c
set files=%files% %bin%\%l_vorb%.a %bin%\%l_ogg%.a %bin%\wav-winmm-rc.o
set inclinks=-lwinmm -I. -I..\%l_vorb% -I..\%l_ogg%
call :compile_bin %name% "%files% %inclinks% -m32 -std=gnu99 -Wl,--enable-stdcall-fixup -O2 -shared -s %o_dbg%" dll 

echo .
echo all done!
exit /B 0




:compile_bin
::objname params extension
if not defined M_FORCE ( if exist %bin%\%~1.%~3 (
    echo skipped %~1.%~3
    exit /B 0
))
pushd src\%~1
%GCC% -o %bin%\%~1.%~3 %~2 %p% 2> %err%\err_%~1.log
call :errcheck %~1
call :del %err%\err_%~1.log 1
call :del *.o 0

echo compiled %~1
popd
exit /B 0

:compile_ar
::objname params
if not defined M_FORCE ( if exist %bin%\%~1.a (
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

:errcheck
::objname
if %errorlevel% neq 0 (
    echo oops %~1!
    pause
    call :del %err%\err_%~1.log 1
    call :del ..\%~1\*.o 0
    call :del %tmp%\* 0
    exit
)
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