# Changes for Ngspice version upgrade

1. Manually add the GHDL server close patch (header files and `close_server` function at the appropriate location) in the `outitf.c` which comes with the new Ngspice version.

> Note: Compare with previous version of NGHDL `outitf.c` file for applying the patch.

2. Call this function `close_server()` in `outitf.c` before the line:
    
```c
    fprintf(stdout, "\nNo. of Data Rows : %d\n", run->pointCount);
```
occurring in following two functions of `outitf.c` (may not be same in the future):
```c
    static void fileEnd(runDesc *run)
```
and
```c
    static void plotEnd(runDesc *run)
```
> Note:
>   1. Do not replace old `outitf.c` with the `outitf.c` in the new Ngspice version.
>   2. Make sure to maintain OS independence of the code, that is same code file for Windows and Linux.

3. Similarly, manually add patch for identification of Mingw and linking of `libws2_32.a` to `makedefs.in` at *`/ngspice/src/xspice/icm`* **after the line `LIBS = -lm`** (important)
```make
    ISMINGW = $(shell uname | grep -c "MINGW64")
    ifeq ($(ISMINGW), 1)
      LIBS = -lm -lws2_32
    endif
     
    ISMINGW = $(shell uname | grep -c "MINGW32")
    ifeq ($(ISMINGW), 1)
      LIBS = -lm -lws2_32
    endif
     
    ISMINGW = $(shell uname | grep -c "MSYS")
    ifeq ($(ISMINGW), 1)
      LIBS = -lm -lws2_32
    endif
```

4. Add patch for linking `libws2_32.a` in Windows OS at `Makefile.in` at *`/ngspice/src/`* by modifying the line:
```make
    @WINGUI_TRUE@am__append_19 = -lpsapi -lShlwapi 
```
To
```make
    @WINGUI_TRUE@am__append_19 = -lpsapi -lShlwapi -lws2_32
```

5. Add GHDL codemodel to the spinit.in file at *`/ngspice/src/`* as an addition to the section:
```make
    * The other codemodels
```
with the line:
```make
    @XSPICEINIT@ codemodel @pkglibdir@/ghdl.cm
```
and in `GNUmakefile.in` at *`/ngspice/src/xspice/icm`* by replacing the line:
```make
    CMDIRS = spice2poly digital analog xtradev xtraevt table
```
with
```make
    CMDIRS = spice2poly digital analog xtradev xtraevt table ghdl
```

6. Create a **`ghdl` folder** with two empty files named `modpath.lst` and `udnpath.lst` at the location *`/ngspice/src/xspice/icm/`* 
