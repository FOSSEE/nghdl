!include "CPUFeatures.nsh" 
!include "zipdll.nsh"

;-----------------------------------------------------------------------------------------
; String replacement function
!define StrRep "!insertmacro StrRep"
!macro StrRep output string old new
    Push `${string}`
    Push `${old}`
    Push `${new}`
    !ifdef __UNINSTALL__
        Call un.StrRep
    !else
        Call StrRep
    !endif
    Pop ${output}
!macroend
 
!macro Func_StrRep un
    Function ${un}StrRep
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9
 
        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
            Goto loop
        done:
 
        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRep ""
!insertmacro Func_StrRep "un."
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
; NGHDL installation macro 
Section "nghdl-loadsource"
    SetOutPath $EXEDIR
    
    File "ghdl.7z"
    File "verilator.7z"
    File "mingw64.7z"
    File "MSYS.7z"
    File "nghdl-src.7z"
    File "ngspice-nghdl.7z"
SectionEnd

Section "nghdl-src"
    SetOutPath $INSTDIR
    Nsis7z::ExtractWithDetails "$EXEDIR\nghdl-src.7z" "Extracting NGHDL %s..."
    EnVar::SetHKLM
    DetailPrint "EnVar::SetHKLM"
    EnVar::AddValue "Path" "$INSTDIR\eSim\nghdl\src"
    Pop $0
    DetailPrint "EnVar::AddValue returned=|$0|"
    Delete "$EXEDIR\nghdl-src.7z"
SectionEnd

Section "msys2"
    SetOutPath $INSTDIR
    Nsis7z::ExtractWithDetails "$EXEDIR\MSYS.7z" "Extracting MSYS2 %s..."
    EnVar::SetHKLM
    EnVar::AddValue "Path" "$INSTDIR\MSYS\usr\bin"
    Pop $0
    DetailPrint "EnVar::AddValue returned=|$0|"
    Delete "$EXEDIR\MSYS.7z"
SectionEnd

Section "mingw64"
    SetOutPath $INSTDIR\MSYS
    Nsis7z::ExtractWithDetails "$EXEDIR\mingw64.7z" "Extracting MinGW %s..."
    EnVar::SetHKLM
    EnVar::AddValue "Path" "$INSTDIR\MSYS\mingw64\bin"
    Pop $0
    DetailPrint "EnVar::AddValue returned=|$0|"
    Delete "$EXEDIR\mingw64.7z"
SectionEnd

Section "ghdl"
    SetOutPath $INSTDIR
    Nsis7z::ExtractWithDetails "$EXEDIR\ghdl.7z" "Extracting GHDL %s..."
    ;EnVar::SetHKLM
    ;EnVar::AddValue "Path" "$INSTDIR\mingw64\GHDL\bin"
    ;Pop $0
    ;DetailPrint "EnVar::AddValue returned=|$0|"
    CopyFiles "$INSTDIR\GHDL\bin\*" "$INSTDIR\MSYS\mingw64\bin\"
    CopyFiles "$INSTDIR\GHDL\include\*" "$INSTDIR\MSYS\mingw64\include\"
    CopyFiles "$INSTDIR\GHDL\lib\*" "$INSTDIR\MSYS\mingw64\lib\"
    RMDir /r "$INSTDIR\GHDL"
    Delete "$EXEDIR\ghdl.7z"
SectionEnd

Section "verilator"
    SetOutPath $INSTDIR
    Nsis7z::ExtractWithDetails "$EXEDIR\verilator.7z" "Extracting Verilator %s..."
    CopyFiles "$INSTDIR\verilator\bin\*" "$INSTDIR\MSYS\mingw64\bin\"
    CopyFiles "$INSTDIR\verilator\share\verilator\bin\*" "$INSTDIR\MSYS\mingw64\bin\"
    CopyFiles "$INSTDIR\verilator\share\verilator\include\*" "$INSTDIR\MSYS\mingw64\include\"
    CopyFiles "$INSTDIR\verilator\share\verilator\examples\*" "$INSTDIR\MSYS\mingw64\examples\"
    CopyFiles "$INSTDIR\verilator\share\verilator\verilator-config.cmake" "$INSTDIR\MSYS\mingw64\"
    CopyFiles "$INSTDIR\verilator\share\verilator\verilator-config-version.cmake" "$INSTDIR\MSYS\mingw64\"
    CopyFiles "$INSTDIR\verilator\share\pkgconfig" "$INSTDIR\MSYS\mingw64\"
    RMDir /r "$INSTDIR\verilator"
    Delete "$EXEDIR\verilator.7z"
SectionEnd

Section "envar-refresh"	
	ReadEnvStr $R0 "PATH"
	StrCpy $R0 "$R0;$INSTDIR\eSim\nghdl\src;$INSTDIR\MSYS\mingw64\bin;$INSTDIR\MSYS\usr\bin;"
	System::Call 'Kernel32::SetEnvironmentVariable(t, t) i("PATH", R0).r0'
SectionEnd


Section "nghdl-installNgspice"
    SetOutPath $INSTDIR
    Nsis7z::ExtractWithDetails "$EXEDIR\ngspice-nghdl.7z" "Extracting Ngspice %s..."

    ;CopyFiles $INSTDIR\eSim\nghdl\src\outitf.c $INSTDIR\ngspice-nghdl\src\frontend

    CopyFiles "$INSTDIR\MSYS\mingw64\x86_64-w64-mingw32\lib\libws2_32.a" "$INSTDIR\eSim\nghdl\src\ghdlserver"

    SetOutPath $INSTDIR\ngspice-nghdl
    CreateDirectory $INSTDIR\ngspice-nghdl\release
    SetOutPath $INSTDIR\ngspice-nghdl\release

    Var /GLOBAL shellpath
    Var /GLOBAL shellpath1
    Var /GLOBAL cpucores

    ${CPUFeatures.GetCount} $cpucores

    StrCpy $shellpath $INSTDIR

    ${StrRep} '$shellpath1' '$shellpath' '\' '/'

    ;FileOpen $0  "$INSTDIR\ngspice-nghdl\release\installngspice.sh" w
    ;FileWrite $0 `../configure --with-wingui --enable-xspice --disable-debug --prefix=$shellpath1/ngspice-nghdl/install_dir --exec-prefix=$shellpath1/ngspice-nghdl/install_dir &&$\n`
    ;FileWrite $0 `make -j$cpucores &&$\n`
    ;FileWrite $0 `make install$\n`
    ;FileClose $0

    ;nsExec::ExecToLog "$INSTDIR\mingw64\msys\bin\bash.exe installngspice.sh"
    ;Delete $INSTDIR\ngspice-nghdl\release\installngspice.sh
    Delete "$EXEDIR\ngspice-nghdl.7z"

    SetOutPath $INSTDIR

    EnVar::SetHKLM
    EnVar::AddValue "Path" "$INSTDIR\ngspice-nghdl\bin"
    Pop $0
    DetailPrint "EnVar::AddValue returned=|$0|"

SectionEnd
  
Section "nghdl-config.ini" 
    CreateDirectory $PROFILE\.nghdl
    FileOpen $0  "$PROFILE\.nghdl\config.ini" w
    FileWrite $0 `[NGSPICE]$\n`
    FileWrite $0 `NGSPICE_HOME = $INSTDIR\ngspice-nghdl$\n`
    FileWrite $0 `DIGITAL_MODEL = %(NGSPICE_HOME)s\src\xspice\icm\ghdl$\n`
    FileWrite $0 `RELEASE = %(NGSPICE_HOME)s\release$\n`
    FileWrite $0 `[SRC]$\n`
    FileWrite $0 `SRC_HOME = $INSTDIR\eSim\nghdl$\n`
    FileWrite $0 `LICENSE = %(SRC_HOME)s\LICENSE$\n`
    FileWrite $0 `[COMPILER]$\n`
    FileWrite $0 `MSYS_HOME = $INSTDIR\mingw64\msys\bin$\n`
    FileClose $0
SectionEnd

;-------------------------------------------------------------------------------------
