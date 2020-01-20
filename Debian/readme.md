Debian Installer Documentation 
====


It contains all the documenation for installers on Debian (Ubuntu 14.04 and above).


## Separation of Ubuntu 14.04 and Ubuntu 16.04+ installers:
1. Dependency of gnat : 
	a. Ubuntu 14.04 : gnat (v4.6)
	b. Ubuntu 16.04+: gnat-5 (v5.4)
2. Dependency of llvm :
	- Available version on Ubuntu 14.04 : llvm (v3.4)
	- Available version on Ubuntu 16.04+ : llvm (v3.4)
	- Requires llvm version between 3.5 and 3.9 for compilation of GHDL with llvm config


##GHDL Compilation:
1. Following steps are OS specific and GHDL executables should be generated on that system for distribution on 
same OS configuration.
2. For distribution of GHDL on Ubuntu 14.04, directly use the llvm code provided on GHDL's github page.
3. For distribution of GHDL on Ubuntu 16.04 and above, follow the steps:
	a. Download source code from GHDL's github page.
	b. Extract the tar file, open a terminal and change directory one-level inside.
	c. First install gnat dependency as:
	
		$ sudo apt install gnat-5

	d. Install llvm dependency and verify that version of llvm is between 3.5 and 3.9

		$ sudo apt install llvm

	e. To compile GHDL, type following commands:

		$ mkdir -p install_dir
		$ mkdir -p release
		$ cd release/
		$ ../configure --with-llvm-config --prefix=<absolute_path_to_install_dir>
		$ make
		$ make install

	f. GHDL executables will be made in install_dir folder which can be used for distribution.
	g. Note that target machine should have the same version of gnat as that of the source machine on which 
	   GHDL was compiled.

##How to package GHDL executable with NGHDL?
1. Rename `install_dir` directory to the `ghdl-<version>` and compress it to `.tar.xz` format.
2. Place this compressed file along with the installer file (`install-nghdl.sh`) in the `nghdl` folder.
