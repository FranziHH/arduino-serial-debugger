@rem ------------------------------------------------------------------------------------------------------------------------
@rem Arduino build from command line.
@rem 24/07/2020 Modified for SparkFun Pro Micro
@rem
@rem This uses arduino_debug.exe to compile the sketch, then avrdude to load it to the Pro Micro.  When prompted, press
@rem the Pro Micro's reset button (that you connected).
@rem
@rem Put this file in the Arduino project folder (with MyProg.ino)
@rem 
@rem The board type is correct for the Pro-Micro but you'll need to edit the port, Arduino.exe location, Avrdude location.
@rem
@rem The port is the bootload port which appears in Device Manager only briefly after you reset the Pro-Micro. Connecting 
@rem reset twice quickly makes that port stay around for a few seconds longer.  Even with that, sometimes Windows is not 
@rem quick enough to show it.
@rem
@rem Arduino build: https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
@Rem
@rem Pro micro: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide
@rem
@rem Tested on Windows-XP and Windows-10.
@rem ------------------------------------------------------------------------------------------------------------------------
@rem Arduino-Build von der Befehlszeile aus.
@rem 24.07.2020 Modifiziert für SparkFun Pro Micro
@rem
@rem Dabei wird arduino_debug.exe verwendet, um die Skizze zu kompilieren, und dann avrdude, um sie auf den Pro Micro zu laden. Wenn Sie dazu aufgefordert werden, drücken Sie
@rem die Reset-Taste des Pro Micro (die Sie angeschlossen haben).
@rem
@rem Legen Sie diese Datei in den Arduino-Projektordner (mit MyProg.ino)
@rem
@rem Der Boardtyp ist für den Pro-Micro korrekt, aber Sie müssen den Port, den Arduino.exe-Speicherort und den Avrdude-Speicherort bearbeiten.
@rem
@rem Der Port ist der Bootload-Port, der im Geräte-Manager nur kurz angezeigt wird, nachdem Sie den Pro-Micro zurückgesetzt haben. Wenn Sie
@rem Reset zweimal schnell anschließen, bleibt dieser Port einige Sekunden länger verfügbar. Selbst damit ist Windows manchmal nicht
@rem schnell genug, um ihn anzuzeigen. @rem
@rem Arduino-Build: https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
@Rem
@rem Pro Micro: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide
@rem
@rem Getestet unter Windows XP und Windows 10.
@rem ------------------------------------------------------------------------------------------------------------------------

echo off
setlocal EnableDelayedExpansion
set "startTime=%time: =0%"

@rem ------ edited-settings... --------------------------------------------------------------------

@rem Arduino Board Type
@REM ==================
@rem SparkFun Pro Micro
set brd=SparkFun:avr:promicro:cpu=16MHzatmega32U4

@rem Port name
@rem =========
set prt=COM12

@rem Arduino software executable
@rem
@rem If there are spaces in the directory names, use dir/x in each to get the shortnames so 
@rem the path here doesn't have spaces in it.
@rem
set cmd=C:\Programs\Arduino\arduino_debug.exe

@rem Avrdude location
@rem ================
@rem Windows-XP (use short path to avoid spaces)
:: set avrdude=C:\DOCUME~1\RDL\LOCALS~1\APPLIC~1\ARDUIN~1\packages\arduino\tools\avrdude\6345AA~1.0-A
@rem Windows-10
set avrdude="C:\Programs\Arduino\hardware\tools\avr"
@rem ------- ... edited-settings ------------------------------------------------------------------



@rem ----------------------------------------------------------------------------------------------
@rem Project folder (and file name)
for /F "delims=\" %%A in ("%0%") do (
  set src=%%~nxA
)
set afl=C:%~sp0%src%.ino
echo %afl%
echo %brd% %prt%

@rem ----------------------------------------------------------------------------------------------
@rem Temporary files
set tmp=%temp%\Arduino\%src%
if not exist %tmp% mkdir %tmp%

@rem ----------------------------------------------------------------------------------------------
@rem Compile

@%cmd% --board %brd% --preserve-temp-files --verify --pref build.path=%tmp% %afl%

if %errorlevel% == 0 goto deliver
  echo "FAIL."
pause
exit /b

@rem ----------------------------------------------------------------------------------------------
:deliver
@rem get the hex file from the temporary folder
copy %tmp%\%src%.ino.hex .\
echo .
echo RESET THE PRO MICRO.
echo .
@rem use count to prevent infinite loop
set /a count=0

@rem ----------------------------------------------------------------------------------------------
@rem wait for the port to appear.
:loop
  mode > ports.lis
@rem mode side-effect makes Ard lamp flash! (what about other serial devices?)
findstr %prt% ports.lis
if %errorlevel% == 0 goto do_load
@rem 0 = not found, 1 = found

set /a count=%count%+1
echo Looking for %prt%  %count%

if %count% leq 100 goto loop


echo Failed to find %prt%
pause
goto done_or_not

:do_load

@rem ----------------------------------------------------------------------------------------------
@rem Load the hex file into the Pro Micro.
%avrdude%\bin\avrdude.exe -C%avrdude%\etc\AVRDUD~1.CON -v -patmega32u4 -cavr109 -P%prt% -b57600 -D -Uflash:w:%src%.ino.hex:i

goto postload
:postload       

set "endTime=%time: =0%"
@rem Get elapsed time:
set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"
set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100)"
@rem Convert elapsed time to HH:MM:SS:CC format:
set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"
echo Elapsed:  %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%
@rem
pause done_or_not
exit /b
@rem ------------------------------------------------------------------------------------------------------------------------
