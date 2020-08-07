#!/bin/bash 
#===============================================================================
#          FILE: install-nghdl.sh
# 
#         USAGE: ./install-nghdl.sh --install
#                 			OR
#                ./install-nghdl.sh --uninstall
# 
#   DESCRIPTION: Installation script for Ngspice and GHDL simulators (NGHDL)
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Fahim Khan, Rahul Paknikar
#  ORGANIZATION: eSim, FOSSEE group at IIT Bombay
#       CREATED: Tuesday 02 December 2014 17:01
#      REVISION: Sunday 02 August 2020 01:35
#===============================================================================

ngspice="ngspice-nghdl"
ghdl="ghdl-0.37"
config_dir="$HOME/.nghdl"
config_file="config.ini"
src_dir=`pwd`

# Will be used to take backup of any file
sysdate="$(date)"
timestamp=`echo $sysdate|awk '{print $3"_"$2"_"$6"_"$4 }'`


# All functions goes here

error_exit() {
    echo -e "\n\nError! Kindly resolve above error(s) and try again."
    echo -e "\nAborting Installation...\n"
}


function installDependency
{

    echo "Installing dependencies for $ghdl LLVM................"

    echo "Installing Make..........................................."
    sudo apt install -y make
    
    echo "Installing GNAT..........................................."
    sudo apt install -y gnat

	echo "Installing LLVM-8........................................."
    sudo apt install -y llvm-8

    echo "Installing Clang.........................................."
    sudo apt-get install -y clang

    echo "Installing Zlib1g-dev....................................."
    sudo apt install -y zlib1g-dev
  
    # Specific dependency for canberra-gtk modules
    echo "Installing Gtk Canberra modules..........................."
    sudo apt install -y libcanberra-gtk-module libcanberra-gtk3-module

    # Specific dependency for nvidia graphic cards
    echo "Installing graphics dependency for ngspice souce build"
    echo "Installing libxaw7........................................"
    sudo apt install -y libxaw7

    echo "Installing libxaw7-dev...................................."
    sudo apt install -y libxaw7-dev


    echo "Installing $ghdl LLVM................................."
    tar -xJf $ghdl.tar.xz
    echo "$ghdl successfully extracted"
    echo "Changing directory to $ghdl installation"
    cd $ghdl/
    echo "Configuring $ghdl build as per requirements"
    chmod +x configure
    # Other configure flags can be found at - https://github.com/ghdl/ghdl/blob/master/configure
    sudo ./configure --with-llvm-config=/usr/bin/llvm-config-8
    echo "Building the install file for $ghdl LLVM"
    sudo make
    sudo make install

    set +e 		# Temporary disable exit on error
    trap "" ERR # Do not trap on error of any command
    
    echo "Removing unused part of $ghdl LLVM"
    sudo rm -rf ../$ghdl
    
    set -e 		# Re-enable exit on error
    trap error_exit ERR

    echo "GHDL installed successfully"
    
}


function installNgspice
{

    echo "Installing Ngspice........................................"

    # Extracting Ngspice to Home Directory
    cd $src_dir
    tar -xJf $ngspice.tar.xz -C $HOME 
    
    echo "Ngspice extracted sucessfully to $HOME"
    # Change to ngspice-nghdl directory
    cd $HOME/$ngspice
    # Make local install directory
    mkdir -p install_dir
    # Make release directory for build
    mkdir -p release
    # Change to release directory
    cd release
    echo "Configuring Ngspice........."
    sleep 2
    
    chmod +x ../configure
    ../configure --enable-xspice --disable-debug  --prefix=$HOME/$ngspice/install_dir/ --exec-prefix=$HOME/$ngspice/install_dir/
            
    # Adding patch to Ngspice base code
    cp $src_dir/src/outitf.c $HOME/$ngspice/src/frontend

    make -j$(nproc)
    make install

    # Make it executable
    sudo chmod 755 $HOME/$ngspice/install_dir/bin/ngspice
    
    set +e 		# Temporary disable exit on error
    trap "" ERR # Do not trap on error of any command

    echo "Removing previously installed Ngspice (if any)"    
    sudo apt-get purge -y ngspice

    echo "Ngspice installed sucessfully"
    echo "Adding softlink for the installed Ngspice"

    # Add symlink to the path
    sudo rm /usr/bin/ngspice

    set -e 		# Re-enable exit on error
    trap error_exit ERR

    sudo ln -sf $HOME/$ngspice/install_dir/bin/ngspice /usr/bin/ngspice
    echo "Added softlink for Ngspice..."

}


function createConfigFile
{
    
    # Creating config.ini file and adding configuration information
    # Check if config file is present
    if [ -d $config_dir ];then
        rm $config_dir/$config_file && touch $config_dir/$config_file
    else
        mkdir $config_dir && touch $config_dir/$config_file
    fi
    
    echo "[NGSPICE]" >> $config_dir/$config_file
    echo "NGSPICE_HOME = $HOME/$ngspice" >> $config_dir/$config_file
    echo "DIGITAL_MODEL = %(NGSPICE_HOME)s/src/xspice/icm/ghdl" >> $config_dir/$config_file
    echo "RELEASE = %(NGSPICE_HOME)s/release" >> $config_dir/$config_file
    echo "[SRC]" >> $config_dir/$config_file
    echo "SRC_HOME = $src_dir" >> $config_dir/$config_file
    echo "LICENSE = %(SRC_HOME)s/LICENSE" >> $config_dir/$config_file

}


function createSoftLink
{
    # Make it executable
    sudo chmod 755 $src_dir/src/ngspice_ghdl.py

    # Creating softlink
    cd /usr/local/bin
    if [[ -L nghdl ]];then
        echo "Symlink was already present"
        sudo unlink nghdl
    fi
    
    sudo ln -sf $src_dir/src/ngspice_ghdl.py nghdl
    echo "Added softlink for NGHDL..."

    cd $pwd

}


#####################################################################
#       Script start from here                                     #
#####################################################################

### Checking if file is passsed as argument to script

if [ "$#" -eq 1 ];then
    option=$1
else
    echo "USAGE : "
    echo "./install-nghdl.sh --install"
    exit 1;
fi

## Checking flags
if [ $option == "--install" ];then
    
    set -e  # Set exit option immediately on error
    set -E  # inherit ERR trap by shell functions

    # Trap on function error_exit before exiting on error
    trap error_exit ERR
    
    #Calling functions
    installDependency
    if [ $? -ne 0 ];then
        echo -e "\n\n\nERROR: Unable to install required packages. Please check your internet connection.\n\n"
        exit 0
    fi
    installNgspice
    createConfigFile
    createSoftLink

elif [ $option == "--uninstall" ];then
    sudo rm -rf $HOME/ngspice-nghdl $HOME/.nghdl /usr/share/kicad/library/eSim_Nghdl.lib /usr/local/bin/nghdl /usr/bin/ngspice
    echo "Removing libxaw7-dev..............."
    sudo apt purge -y libxaw7-dev
    echo "Removing GHDL......................"
    sudo rm -rf /usr/local/bin/ghdl /usr/local/bin/ghdl1-llvm /usr/local/lib/ghdl /usr/local/lib/libghdlvpi.so /usr/local/include/vpi_user.h
    echo "Removing LLVM......................"
    sudo apt-get purge -y llvm-8
    echo "Removing GNAT......................"
    sudo apt purge -y gnat
else 
    echo "Please select the proper operation."
    echo "--install"
    echo "--uninstall"
fi
