NGHDL Installer (Ubuntu OS)
====

It contains the documentation to package NGHDL for Ubuntu OS.

> Note: If planning to freeze the NGHDL source code for a target platform (Ubuntu OS), then refer this [documentation](executable.md). Remember to update the installer script to work with this executable!

## How to package NGHDL for Ubuntu OS?

1. Take the `master` branch containing the source code. Rename the folder to `nghdl`.

2. Add GHDL source code `ghdl-<version>.tar.xz` in the `nghdl` folder. 

3. Add the installer script `install-nghdl.sh` in the `nghdl` folder.

4. Add the NGHDL executable (if available) in `nghdl/src` folder. Also, remove following files from this folder:
	- `.git` folder
	- all python files (Applicable only if eSim executable is used)
	- GHDLside.md
	- Workflow.md
	- .gitignore

2. Compress it to `.zip` format.

3. Place this compressed file in `eSim-<version>` folder.
