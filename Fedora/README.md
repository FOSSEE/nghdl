NGHDL Installer (Ubuntu OS)
====

It contains the documentation to package NGHDL for Ubuntu OS.

> Note: If planning to freeze the NGHDL source code for a target platform (Ubuntu OS), then refer this [documentation](executable.md). Remember to update the installer script to work with this executable!

## How to package NGHDL for Ubuntu OS?

1. Take the `master` branch containing the source code. Rename the folder to `nghdl`.

2. Add GHDL source code `ghdl-<version>.tar.xz` from the `installers` branch in the `nghdl` folder.

3. Add Verilator source code `verilator-<version>.tar.xz` from the `installers` branch in the `nghdl` folder.

4. Add the installer script `install-nghdl.sh` from the `installers` branch in the `nghdl` folder.

5. Add the NGHDL executable (if available) in `nghdl/src` folder. Also, remove following files from this folder:
	- `.git` folder
	- all python files (Applicable only if eSim executable is used)
	- GHDLside.md
	- Workflow.md
	- .gitignore

6. Compress it directly to the `.zip` format.

7. Place this compressed file in `eSim-<version>` folder.
