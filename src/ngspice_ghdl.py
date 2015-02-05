#!/usr/bin/env python


"""#!/usr/bin/python"""

#This file create the gui to install code model in the ngspice.

import os
import sys
import shutil
import subprocess
from PyQt4 import QtGui
from PyQt4 import QtCore
from ConfigParser import SafeConfigParser


class Mainwindow(QtGui.QWidget):
    def __init__(self):
        super(Mainwindow, self).__init__()
        print "Initializing.........."
        self.home = os.path.expanduser("~")
        self.parser = SafeConfigParser()
        self.parser.read(self.home+'/.FreeEDA/config.ini')
        self.initUI()

    def initUI(self):
        self.uploadbtn = QtGui.QPushButton('Upload')
        self.uploadbtn.clicked.connect(self.uploadModle)
        self.exitbtn = QtGui.QPushButton('Exit')
        self.exitbtn.clicked.connect(self.closeWindow)
        self.browsebtn = QtGui.QPushButton('Browse')
        self.browsebtn.clicked.connect(self.browseFile)
        self.ledit = QtGui.QLineEdit(self)
        self.tedit = QtGui.QTextEdit(self)
	
	 	

        #Creating gridlayout
        grid = QtGui.QGridLayout()
        grid.setSpacing(5)
        grid.addWidget(self.ledit, 1, 0)
        grid.addWidget(self.browsebtn, 1, 1)
        grid.addWidget(self.uploadbtn,2,0)
        grid.addWidget(self.exitbtn,2,1)
        grid.addWidget(self.tedit,3,0)

        self.setLayout(grid)
        self.setGeometry(300, 300, 600,600)
        self.setWindowTitle("Ngspice Digital Model Creator")
        #self.setWindowIcon(QtGui.QIcon('logo.png'))
        self.tedit.setReadOnly(True)
        self.show()
	


    def closeWindow(self):
        print "Close button clicked"
        quit()

    def browseFile(self):
        print "Browse button clicked"
        self.filename = QtGui.QFileDialog.getOpenFileName(self, 'Open File', '.')
        print "Path file :", self.filename
        self.ledit.setText(self.filename)

    def createModelDirectory(self):
        print "Create Model Directory Called"
        self.digital_home=self.parser.get('NGSPICE','DIGITAL_MODEL')
        print "Digital Home",self.digital_home
        os.chdir(self.digital_home)
        print "Current Working Directory Changed to",os.getcwd()
        self.modelname = os.path.basename(str(self.filename)).split('.')[0]
        print "Model name is :",self.modelname
        # Looking if model directory is present or not
        if os.path.isdir(self.modelname):
            print "Model Already present"
            ret = QtGui.QMessageBox.critical(self, "Critical",'''<b>The Model already exist.Do you want to overwrite it?</b><br/>
                    <b>If yes press ok else cancel it and change the name of you vhdl file</b>''', QtGui.QMessageBox.Ok, QtGui.QMessageBox.Cancel)
            if ret == QtGui.QMessageBox.Ok:
                print "Overwriting existing model"
                cmd="rm -rf "+self.modelname
                #process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
                subprocess.call(cmd, shell=True)
                os.mkdir(self.modelname)
            else:
                print "Exiting application"
                quit()
                


        else:
            print "Creating new model directory"
            os.mkdir(self.modelname)
    
    def addingModelInModpath(self):
        print "Adding Model in Modpath file",self.modelname,self.digital_home
        #Adding name of model in the modpath file
        #Check if the string is already in the file
        with open(self.digital_home+"/modpath.lst",'a+') as f:
            flag = 0
            for line in f:
                if line.strip() == self.modelname:
                    print "Found model"
                    flag = 1
                    break
                else:
                    pass

            if flag == 0:
                print "Adding model name into modpath.lst"
                f.write(self.modelname+"\n")
            else:
                print "Model name is already into modpath.lst"


    def createModelFiles(self):
        print "Create Model Files Called"
        os.chdir(self.cur_dir)
        print "Current Working directory changed to ",self.cur_dir
        cmd = "python ~/.FreeEDA/model_generation.py "+str(self.ledit.text())
        stdouterr = os.popen4(cmd)[1].read()
        self.tedit.append(stdouterr)
        #Moving file to model directory
        path=self.digital_home+"/"+self.modelname
        shutil.move("cfunc.mod",path)
        shutil.move("ifspec.ifs",path)

        #Creating directory inside model directoy
        os.mkdir(path+"/DUTghdl/")
        shutil.move("connection_info.txt",path+"/DUTghdl/")
        shutil.move("start_server.sh",path+"/DUTghdl/")
        shutil.move(self.modelname+"_tb.vhdl",path+"/DUTghdl/")
        
        shutil.copy(str(self.filename),path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/compile.sh",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/uthash.h",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/ghdlserver.c",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/ghdlserver.h",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/Utility_Package.vhdl",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/Vhpi_Package.vhdl",path+"/DUTghdl/")
        
        os.chdir(path+"/DUTghdl")
        subprocess.call("bash "+path+"/DUTghdl/compile.sh", shell=True)
        subprocess.call("chmod a+x start_server.sh",shell=True)
        os.remove("compile.sh")
        os.remove("ghdlserver.c")
        #os.remove("ghdlserver.h")
        #os.remove("Utility_Package.vhdl")
        #os.remove("Vhpi_Package.vhdl")
        
        
        

        


    def runMake(self):
        print "run Make Called"
        self.release_home=self.parser.get('NGSPICE','RELEASE')
        os.chdir(self.release_home)
        try:
            cmd = "make"
            self.tedit.append("Running Make")
            subprocess.call(cmd,shell=True)
            #stdouterr = os.popen4(cmd)[1].read()
            #self.tedit.append(stdouterr)
        except:
            print "There is error in 'make' "
            quit()

    def runMakeInstall(self):
        print "run Make Install Called"
        try:
            cmd = "make install"
            self.tedit.append("Running Make Install")
            subprocess.call(cmd,shell=True)
            #stdouterr = os.popen4(cmd)[1].read()
            #self.tedit.append(stdouterr)
        except:
            print "There is error during in 'make install' "
            quit()
    
    def uploadModle(self):
        print "Upload button clicked"
        self.file_extension = os.path.splitext(str(self.filename))[1]
        print "File extension",self.file_extension
        print "Parser Content:",self.parser.get('NGSPICE', 'NGSPICE_HOME')
        self.cur_dir = os.getcwd()
        print "My Current Working Directory",self.cur_dir
        if self.file_extension == ".vhdl":
            self.createModelDirectory()
            self.addingModelInModpath()
            self.createModelFiles()
            self.runMake()
            self.runMakeInstall()
        else:
            QtGui.QMessageBox.about(self,'Message','''<b>Important Message.</b><br/><br/>This accepts only <b>.vhdl</b> file ''')


    	

def main():
    app = QtGui.QApplication(sys.argv)
    w = Mainwindow()
    sys.exit(app.exec_())

if __name__=='__main__':
    main()
