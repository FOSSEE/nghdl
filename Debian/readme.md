Debian Installer Documentation 
====


It contains all the documenation for installers on Debian (Ubuntu 14.04 and above).
Separate installers are required to resolve dependencies only of GHDL and are mentioned below. 


## Differences in Ubuntu 14.04, Ubuntu 16.04 and Ubuntu 18.04 installers:
1. Dependency on gnat :
	- Ubuntu 14.04 : gnat (v4.6)
	- Ubuntu 16.04 : gnat-5 (v5.4)
	- Ubuntu 18.04 : gnat-5 (v5.5)
2. Dependency on llvm :
	- Available version on Ubuntu 14.04 : llvm (v3.4)
	- Available version on Ubuntu 16.04 : llvm (v3.5)
	- Available version on Ubuntu 18.04 : llvm (v6.0)
	- Requires llvm version between 3.5 and 3.9 for compilation of GHDL with llvm config
	- Ubuntu 18.04 requires installation of LLVM on target machine as it is used during runtime of GHDL as shared library.
3. Dependency on gcc : 
	- Ubuntu 14.04 and 16.04 : gcc (v5.4.0) is available without pie (i.e. position-independent executable)
	- Ubuntu 18.04 : gcc (v7.4.0) is available with pie enabled as default option.
	- Refer [this](https://github.com/ghdl/ghdl/issues/213) Github issue for dependency on gcc for Ubuntu 18.04 and above.


## GHDL Compilation:
1. Compilation of GHDL is OS specific and GHDL executables should be generated on that system for distribution on same OS configuration.
2. For distribution of GHDL (v0.36) on Ubuntu 14.04, directly use the llvm executable provided on GHDL's Github page.
3. For distribution of GHDL (v0.36) on Ubuntu 16.04 and above, follow the steps:
	a. Download source code (v0.36) from GHDL's Github page.
	b. Extract the tar file, open a terminal and change directory one-level inside.
	c. First install gnat dependency as:
	
		$ sudo apt install gnat-5

	d. Install llvm dependency and verify that version of llvm is between 3.5 and 3.9

		- For Ubuntu 16.04 :

			$ sudo apt install llvm

		- For Ubuntu 18.04 :

			$ sudo apt install llvm-3.9

	e. Install clang dependency:

		$ sudo apt install clang


	f. To compile GHDL, type following commands:

		$ mkdir -p install_dir
		$ mkdir -p release
		$ cd release/

		- For Ubuntu 16.04 :

			$ ../configure --with-llvm-config --prefix=<absolute_path_to_install_dir>

		- For Ubuntu 18.04 :

			$ ../configure --with-llvm-config=/usr/bin/llvm-config-3.9 --prefix=<absolute_path_to_install_dir>

		$ make
		$ make install

	f. GHDL executables will be made in install_dir folder which can be used for distribution.
	g. Note that target machine should have the same version of gnat and OS as that of the source machine on which GHDL was compiled.


## How to package GHDL executable with NGHDL?
1. Rename `install_dir` directory to the `ghdl-<version>` and compress it to `.tar.xz` format.
2. Place this compressed file along with the installer file (`install-nghdl.sh`) in the `nghdl` folder.
