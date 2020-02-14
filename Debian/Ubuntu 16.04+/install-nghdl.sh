#!/bin/bash 
#===============================================================================
#          FILE: install-nghdl.sh
# 
#         USAGE: ./install-nghdl.sh --install
#                 or
#                ./install-nghdl.sh --uninstall
# 
#   DESCRIPTION: It is installation script for ngspice and ghdl work (nghdl). 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Fahim Khan, Rahul Paknikar
#  ORGANIZATION: eSim, FOSSEE group at IIT Bombay
#       CREATED: Friday 14 February 2020 16:30
#      REVISION:  ---
#===============================================================================

ngspice="ngspice-nghdl"
ghdl="ghdl-0.36"
config_dir="$HOME/.nghdl"
config_file="config.ini"
src_dir=`pwd`

#Will be used to take backup of any file
sysdate="$(date)"
timestamp=`echo $sysdate|awk '{print $3"_"$2"_"$6"_"$4 }'`


#All functions goes here
function installDependency
{

    echo "Installing dependencies for ghdl-0.36 LLVM................"

    echo "Installing Make..........................................."
    sudo apt install -y make
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"Make\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi
    
    echo "Installing GNAT-5........................................."
    sudo apt install -y gnat-5
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"GNAT-5\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi

	echo "Installing LLVM-3.9......................................."
    sudo apt install -y llvm-3.9
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"LLVM-3.9\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi    

    echo "Installing Clang................................."
    sudo apt-get install -y clang
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"Clang\" dependency couldn't be installed.\nKindly resolve above \"apt-get\" errors and try again."
        exit 1
    fi

    echo "Installing Zlib1g-dev....................................."
    sudo apt install -y zlib1g-dev
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"Zlib1g-dev\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi
  
    # Specific dependency for canberra-gtk modules
    echo "Installing Gtk Canberra modules..........................."
    sudo apt install -y libcanberra-gtk-module libcanberra-gtk3-module
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"canberra-gtk-module\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi  

    # Specific dependency for nvidia graphic cards
    echo "Installing graphics dependency for ngspice souce build"
    echo "Installing libxaw7........................................"
    sudo apt install -y libxaw7
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"libxaw7\" dependency couldn't be installed.\nKindly resolve above errors and try again."
        exit 1
    fi

    echo "Installing libxaw7-dev...................................."
    sudo apt install -y libxaw7-dev
    if [ $? -ne 0 ]; then
        echo -e "\n\n\"libxaw7-dev\" dependency couldn't be installed.\nKindly resolve above \"apt-get\" errors and try again."
        exit 1
    fi


    echo "Installing GHDL-0.36 LLVM................................."
    grep -h "ghdl" /usr/local/bin/ghdl > /dev/null
    if [ $? -ne 0 ]; then
        tar -xJf ghdl-0.36.tar.xz
        if [ "$?" == 0 ]; then
            echo "ghdl-0.36 successfully extracted..."
            echo "Changing directory to ghdl-0.36 installation..."
            cd ghdl-0.36/
            echo "Configuring ghdl-0.36 build as per requirements..."
            #Other configure flags can be found at - https://github.com/ghdl/ghdl/blob/master/configure
            sudo ./configure --with-llvm-config=/usr/bin/llvm-config-3.9
            echo "Building the install file for ghdl-0.36 LLVM..."
            sudo make -j$(nproc)
            echo "Installing ghdl-0.36 LLVM..."
            sudo make install
            echo "Removing unused part of ghdl-0.36 LLVM....."
    		sudo rm -rf ../ghdl-0.36
        else
            echo "Unable to extract ghdl-0.36 LLVM"
            echo "Exiting installation"
            exit 1
        fi
    else
        echo "GHDL already exists....."
        echo "Leaving ghdl-0.36 LLVM installation"
    fi
    
}


function installNgspice
{

    echo "Installing Ngspice........................................"
    #Checking if ngspice-nghdl directory is already present in Home directory
    if [ -d $HOME/$ngspice ];then
        echo "$ngspice directory already exists at $HOME"
        echo "Leaving Ngspice installation..."
    else
        #Extracting Ngspice to Home Directory
        cd $src_dir
        tar -xJf $ngspice.tar.xz -C $HOME 
        if [ "$?" == 0 ];then 
            echo "Ngspice extracted sucessfuly to $HOME"
            #change to ngspice-nghdl directory
            cd $HOME/$ngspice
            #Make local install directory
            mkdir -p install_dir
            #Make release directory for build
            mkdir -p release
            #Change to release directory
            cd release
            echo "Configuring Ngspice..."
            sleep 2
            ../configure --enable-xspice --disable-debug  --prefix=$HOME/$ngspice/install_dir/ --exec-prefix=$HOME/$ngspice/install_dir/
            
            #dirty fix for adding patch to ngspice base code
            cp $src_dir/src/outitf.c $HOME/$ngspice/src/frontend

            make -j$(nproc)
            make install
            if [ "$?" == 0 ];then
                echo "Removing previously installed Ngspice (if any)..."
                sudo apt-get purge -y ngspice

                echo "Ngspice installed sucessfully................."
                echo "Adding softlink for the installed Ngspice....."

                sudo rm /usr/bin/ngspice
                sudo ln -s $HOME/$ngspice/install_dir/bin/ngspice /usr/bin/ngspice
                if [ $? -ne 0 ];then
                    echo "Failed to add Ngspice softlink............"
                    echo "Remove earlier installations at /usr/bin/ngspice and try again..."
                    exit 1
                else
                	#Make it executable
    				sudo chmod 755 /usr/bin/ngspice
                    echo "Added softlink for Ngspice"
                fi
            else 
                echo "There was some error while installing Ngspice"
            fi
        else 
            echo "Unable to extract Ngspice tar file"
            exit 1;
        fi
    fi

}


function createConfigFile
{
    
    #Creating config.ini file and adding configuration information
    #Check if config file is present
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
    
    ## Creating softlink 
    cd /usr/local/bin
    if [[ -L nghdl ]];then
        echo "Symlink was already present"
        sudo unlink nghdl
        sudo ln -sf $src_dir/src/ngspice_ghdl.py nghdl
    else
        echo "Creating symlink"
        sudo ln -sf $src_dir/src/ngspice_ghdl.py nghdl
        if [ $? -ne 0 ];then
            echo "Failed to add NGHDL softlink"
            echo "Remove earlier installations at /usr/local/bin/nghdl and try again..."
            exit 1
        else
        	#Make it executable
    		sudo chmod 755 nghdl
            echo "Added softlink for NGHDL..............................."
        fi
    fi
    cd $pwd

}


#####################################################################
#       Script start from here                                     #
#####################################################################

###Checking if file is passsed as argument to script

if [ "$#" -eq 1 ];then
    option=$1
else
    echo "USAGE : "
    echo "./install-nghdl.sh --install"
    exit 1;
fi

##Checking flags
if [ $option == "--install" ];then
    
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
    echo "Removing NGHDL....................."
    sudo rm -rf $HOME/ngspice-nghdl $HOME/.nghdl /usr/share/kicad/library/eSim_Nghdl.lib /usr/local/bin/nghdl /usr/bin/ngspice
    echo "Removing libxaw7-dev..............."
    sudo apt purge -y libxaw7-dev
    echo "Removing GHDL......................"
    sudo rm -rf /usr/local/bin/ghdl /usr/local/bin/ghdl1-llvm /usr/local/lib/ghdl /usr/local/lib/libghdlvpi.so /usr/local/include/vpi_user.h
    echo "Removing LLVM......................"
    sudo apt-get purge -y llvm-3.9
    echo "Removing GNAT-5...................."
    sudo apt purge -y gnat-5
else 
    echo "Please select the proper operation."
    echo "--install"
    echo "--uninstall"
fi