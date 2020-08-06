# MinGW + MSYS - Build environment for Ngspice on Windows
> MSYS provides a Linux like shell and all the utilities required during the configure-make-make install process of Ngspice on Windows.

> MinGW provides the GCC compiler required to build the Ngspice executable.

[Download MinGW (unmodified)](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/seh/x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z)\
[Download MSYS (unmodified)](https://sourceforge.net/projects/mingw-w64/files/External%20binary%20packages%20%28Win64%20hosted%29/MSYS%20%2832-bit%29/MSYS-20111123.zip/download)

**Reduced versions of MinGW and MSYS for NGHDL can be found in this repository under `sources/` folder.**

## Usage of MinGW + MSYS
1. Add `/mingw64/bin/` and `/mingw64/msys/bin` to the path so that all the utilities are able to find each other.
2. Open Windows command prompt at the location you want to use `MinGW + MSYS`.
3. Execute the command to launch `bash` provided by MSYS at this location.
  ```sh
  > pathtoMSYS/bin/bash.exe
  ```
 4. This comand launches bash in the CMD window which can then be used as `BASH shell`, with commands similar to Linux.
 
 
## Reduction of MinGW + MSYS
> Originally MinGW and MSYS are provided for a large variety of development scenarios which are not required by NGHDL, causing size overheads in the final distribution.

1. To remove unrequired parts of MSYS and MinGW, a trial and error based approach is to be followed.

2. To start, both utilities should be reduced to the bare minimum required components which can be noted by studying the Ngspice configure file `(/ngspice/configure)`.
The rest of the utilities are to added by accessing the errors in the logs provided during the configure-make-make install process.
