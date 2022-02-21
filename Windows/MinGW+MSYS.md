# MinGW + MSYS - Build environment for Ngspice on Windows
> MSYS provides a Linux like shell and all the utilities required during the configure-make-make install process of Ngspice on Windows.

> MinGW provides the GCC compiler required to build the Ngspice executable.

> [Download MSYS2 (unmodified)](https://www.msys2.org/). Following packages are found to be required for building Ngspice and Verilator:

    - $ pacman -S --needed base-devel mingw-w64-x86_64-toolchain

      When prompted for the options, Select the one corresponding to ``mingw-w64-x86_64-gcc``

    - $ pacman -S --needed mingw-w64-x86_64-python mingw-w64-x86_64-make
    - $ pacman -S --needed mingw-w64-x86_64-dlfcn autoconf2.71


**Reduced versions of MinGW and MSYS for NGHDL can be found in this repository under `sources/` folder.**

## Usage of MinGW + MSYS
1. Add `/MSYS/usr/bin/` and `/MSYS/mingw64/bin` to the path so that all the utilities are able to find each other.
2. Open Windows command prompt at the location you want to use `MinGW + MSYS`.
3. Execute the command to launch `bash` provided by MSYS at this location.
  ```sh
  > pathtoMSYS/usr/bin/bash.exe
  ```
4. This comand launches bash in the CMD window which can then be used as `BASH shell`, with commands similar to Linux.
5. Note that this reduced version is not sufficient for building Ngspice and Verilator from their source codes.


## Reduction of MinGW + MSYS
> Originally MinGW and MSYS are provided for a large variety of development scenarios which are not required by NGHDL, causing size overheads in the final distribution.

1. To remove unrequired parts of MSYS and MinGW, a trial and error based approach is to be followed.

2. To start with, the packages listed above can work as the base for the reduction.

3. Both utilities should be reduced to the bare minimum required components which can be noted by studying the Ngspice configure file `(/ngspice/configure)`.

4. The rest of the utilities are to added by accessing the errors in the logs provided during the ``configure``-``make``-``make install`` process. Similar can be done for Verilator as well.
