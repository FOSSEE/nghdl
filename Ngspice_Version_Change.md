# Changes for Ngspice version upgrade

1. Download the source code for the new Ngspice version and rename its top-level directory as `ngspice-nghdl`.

2. Manually add the GHDL server close patch (Necessary C headers and `close_server` function at the appropriate location) in the `outitf.c` which comes with the new Ngspice version.

    > Note: Intelligently compare with previous version of NGHDL `outitf.c` file for applying the patch.

3. Call this function `close_server()` in `outitf.c` before the line:
    
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

4. Replace the old `outitf.c` at `src` directory of `master` branch of `nghdl` repository with this newly patched version of `outitf.c` file.

5. Similarly, manually add following patch for identification of Mingw and linking of `libws2_32.a` to `makedefs.in` at *`ngspice-nghdl/src/xspice/icm`* **after the line `LIBS = -lm`** (important)
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

6. Add patch for linking `libws2_32.a` in Windows OS at `Makefile.in` at *`ngspice-nghdl/src/`* by modifying the line:
    ```make
        @WINGUI_TRUE@am__append_19 = -lpsapi -lShlwapi 
    ```
    To
    ```make
        @WINGUI_TRUE@am__append_19 = -lpsapi -lShlwapi -lws2_32
    ```

7. Add GHDL codemodel to the spinit.in file at *`ngspice-nghdl/src/`* as an addition to the section:
    ```make
        * The other codemodels
    ```
    with the line:
    ```make
        @XSPICEINIT@ codemodel @pkglibdir@/ghdl.cm
    ```
    and in `GNUmakefile.in` at *`ngspice-nghdl/src/xspice/icm`* by replacing the line:
    ```make
        CMDIRS = spice2poly digital analog xtradev xtraevt table
    ```
    with
    ```make
        CMDIRS = spice2poly digital analog xtradev xtraevt table ghdl
    ```

8. Create a **`ghdl` directory** with two empty files named `modpath.lst` and `udnpath.lst` at the location *`ngspice-nghdl/src/xspice/icm/`* 

    > Note: For Ubuntu OS, now compress this new version of Ngspice to `ngspice-nghdl.tar.xz` file and replace the existing older tar file at the `master` branch of this repository.


## Following procedures are specific only to Windows OS:

9. Now build and generate Ngspice executable on Windows OS with the following command in a bash shell:

    ```console
        $ mkdir release
        $ mkdir install_dir
        $ cd release
        $ ../configure --with-wingui --enable-xspice --disable-debug --prefix=<absolute_path>/ngspice-nghdl/install_dir --exec-prefix=<absolute_path>/ngspice-nghdl/install_dir
        $ make
        $ make install
    ```
    > Note: MSYS + MinGW environment must be setup before building Ngspice. Refer this [documentation](Windows/MinGW+MSYS.md) for the same.

10. Move the generated ghdl code model from `ngspice-nghdl/release/src/xspice/icm/ghdl.cm` to the `ngspice-nghdl/lib/ngspice/` directory.

11. Download Ngspice which is readily available to be used directly on Windows OS. Now, intelligently merge this downloaded version with the built version (from step 10) except the `ngspice-nghdl/bin` directory.

12. Extract the `ngspice-nghdl.7z` from `Windows/sources/` directory of `installers` branch of this repository. Make the directory structure of the merged version (from step 11) similar to that of this extacted older version.

13. Now, compress this new structured version of Ngspice to a `.7z` format and replace the existing older `ngspice-nghdl.7z` at `Windows/sources/` directory of `installers` branch of this repository.
