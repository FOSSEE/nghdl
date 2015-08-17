#!/bin/bash 
#===============================================================================
#
#          FILE: install.sh
# 
#         USAGE: ./install.sh 
# 
#   DESCRIPTION: It is installation script for ngspice and ghdl work (nghdl). 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Fahim Khan , fahim.elex@gmail.com
#  ORGANIZATION: FreeEDA, FOSSEE group at IIT Bombay
#       CREATED: Tuesday 02 December 2014 17:01
#      REVISION:  ---
#===============================================================================

ngspice="ngspice-26"
src_loc=".FreeEDA"
config_file="config.ini"
cur_dir=`pwd`

#Will be used to take backup of any file
sysdate="$(date)"
timestamp=`echo $sysdate|awk '{print $3"_"$2"_"$6"_"$4 }'`



#Checking if ngspice-26 directory is already present in Home directory
if [ -d $HOME/$ngspice ];then
        echo "$ngspice directory already exist taking its backup"
        mv $HOME/$ngspice $HOME/$ngspice.$timestamp
fi


#Extracting Ngspice to Home Directory
tar -xzvf $ngspice.tar.gz -C $HOME

if [ "$?" == 0 ];then 
    echo "Ngspice extracted sucessfuly to $HOME "
    #change to ngspice-26 directory
    cd $HOME/$ngspice
    #Make local install directory
    mkdir -p install_dir
    #Make release directory for build
    mkdir -p release
    #Change to release directory
    cd release
    echo "Installing Ngspice....."
    echo "------------------------------------"  
    sleep 5
    ../configure --enable-xspice --disable-debug  --prefix=$HOME/$ngspice/install_dir/ --exec-prefix=$HOME/$ngspice/install_dir/    
    make
    make install
    if [ "$?" == 0 ];then
        echo "Ngspice Installed sucessfully"
    else 
        echo "There was some error in installing ngspice"
    fi

else 
    echo "Unable to extract ngspice tar file"
    exit 1;
fi




#Creating directory to put source 
if [ -d "$HOME/$src_loc" ];then
    echo "(.)FreeEDA directory already in $HOME,removing it and copying new code"
    rm -rf ~/$src_loc
    mkdir -p ~/$src_loc
else
    mkdir -p ~/$src_loc
fi

#Change to current directory
cd $cur_dir
cp -rv src/* ~/$src_loc/

#Copying LICENSE file
cp LICENSE ~/$src_loc/

#Creating config.ini file and adding configuration information

cd ~/$src_loc/
touch $config_file

echo "[NGSPICE]" >> $config_file
echo "NGSPICE_HOME = $HOME/$ngspice" >> $config_file
echo "DIGITAL_MODEL = %(NGSPICE_HOME)s/src/xspice/icm/ghdl" >> $config_file
echo "RELEASE = %(NGSPICE_HOME)s/release" >> $config_file


## Creating softlink 
cd /usr/local/bin
if [[ -L nghdl ]];then
    echo "Symlink was already present"
    sudo unlink nghdl
    sudo ln -sf ~/$src_loc/ngspice_ghdl.py nghdl

else
    echo "Creating synmlink"
    sudo ln -sf ~/$src_loc/ngspice_ghdl.py nghdl
fi


