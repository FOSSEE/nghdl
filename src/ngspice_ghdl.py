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
        #super(Mainwindow, self).__init__()
        QtGui.QMainWindow.__init__(self)
        print "Initializing.........."
        self.home = os.path.expanduser("~")
        licensefile = os.path.join(self.home+'/.FreeEDA/', "LICENSE")
        fileopen = open(licensefile, 'r')
        print fileopen.read()
        self.parser = SafeConfigParser()
        self.parser.read(self.home+'/.FreeEDA/config.ini')
        self.file_list = []             #to keep the supporting files
        self.initUI()

    def initUI(self):
        self.uploadbtn = QtGui.QPushButton('Upload')
        self.uploadbtn.clicked.connect(self.uploadModle)
        self.exitbtn = QtGui.QPushButton('Exit')
        self.exitbtn.clicked.connect(self.closeWindow)
        self.browsebtn = QtGui.QPushButton('Browse')
        self.browsebtn.clicked.connect(self.browseFile)
        self.addbtn = QtGui.QPushButton('Add Files')
        self.addbtn.clicked.connect(self.addFiles)
        self.removebtn = QtGui.QPushButton('Remove Files')
        self.removebtn.clicked.connect(self.removeFiles)
        self.ledit = QtGui.QLineEdit(self)
        self.sedit = QtGui.QTextEdit(self)
        self.process = QtCore.QProcess(self)
        self.terminal = QtGui.QWidget(self)

        self.process.start('xterm',['-into', str(self.terminal.winId())])

        #Creating gridlayout
        grid = QtGui.QGridLayout()
        grid.setSpacing(5)
        grid.addWidget(self.ledit, 1, 0)
        grid.addWidget(self.browsebtn, 1, 1)
        grid.addWidget(self.sedit, 2, 0, 4, 1)
        grid.addWidget(self.addbtn, 2, 1)
        grid.addWidget(self.removebtn, 3, 1)
        grid.addWidget(self.terminal, 6, 0,10,1)
        grid.addWidget(self.uploadbtn, 17, 0)
        grid.addWidget(self.exitbtn,17, 1)

        self.setLayout(grid)
        self.setGeometry(300, 300, 600,600)
        self.setWindowTitle("Ngspice Digital Model Creator")
        #self.setWindowIcon(QtGui.QIcon('logo.png'))
        self.show()
	


    def closeWindow(self):
        try:
            self.process.close()
        except:
                pass
        print "Close button clicked"
        quit()

    def browseFile(self):
        print "Browse button clicked"
        self.filename = QtGui.QFileDialog.getOpenFileName(self, 'Open File', '.')
        print "Path file :", self.filename
        self.ledit.setText(self.filename)

    def addFiles(self):
        print "Add Files button clicked"
        title = self.addbtn.text()
        for file in QtGui.QFileDialog.getOpenFileNames(self, title):
                print "Supporting file :", file
                self.sedit.append(str(file))
                self.file_list.append(file)


    def removeFiles(self):
            self.fileRemover = FileRemover(self)


    #check extensions of all supporting files
    def checkSupportFiles(self):
        nonvhdl_count = 0
        for file in self.file_list:
            extension = os.path.splitext(str(file))[1]
            if extension != ".vhdl":
                nonvhdl_count += 1
                self.file_list.remove(file)

        if nonvhdl_count > 0:
             QtGui.QMessageBox.about(self,'Message','''<b>Important Message.</b><br/><br/>This accepts only <b>.vhdl</b> file ''')


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
        print stdouterr
        #Moving file to model directory
        path=self.digital_home+"/"+self.modelname
        shutil.move("cfunc.mod",path)
        shutil.move("ifspec.ifs",path)

        #Creating directory inside model directoy
        os.mkdir(path+"/DUTghdl/")
        shutil.move("connection_info.txt",path+"/DUTghdl/")
        shutil.move("start_server.sh",path+"/DUTghdl/")
        shutil.move("sock_pkg_create.sh",path+"/DUTghdl/")
        shutil.move(self.modelname+"_tb.vhdl",path+"/DUTghdl/")
        
        shutil.copy(str(self.filename),path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/compile.sh",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/uthash.h",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/ghdlserver.c",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/ghdlserver.h",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/Utility_Package.vhdl",path+"/DUTghdl/")
        shutil.copy(self.home+"/.FreeEDA/ghdlserver/Vhpi_Package.vhdl",path+"/DUTghdl/")

        for file in self.file_list:
                shutil.copy(str(file), path+"/DUTghdl/")
        
        os.chdir(path+"/DUTghdl")
        subprocess.call("bash "+path+"/DUTghdl/compile.sh", shell=True)
        subprocess.call("chmod a+x start_server.sh",shell=True)
        subprocess.call("chmod a+x sock_pkg_create.sh",shell=True)
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
            cmd = " make"
            print "Running Make"
            path = os.getcwd()
            #subprocess.call(cmd,shell=True)
            command = "cd "+path +";"+cmd +";"+"make install"
            #command = "cd "+path +";"+cmd 
            self.args = ['-into', str(self.terminal.winId()),'-hold','+s','-e', command]
            self.process.start('xterm', self.args)

            print "pid ------ >",self.process.pid()
            
            #stdouterr = os.popen4(cmd)[1].read()
            #self.tedit.append(stdouterr)
        except:
            print "There is error in 'make' "
            quit()

    def runMakeInstall(self):
        print "run Make Install Called"
        try:
            cmd = " make install"
            print "Running Make Install"
            path = os.getcwd()
            print "cwd------------>", path
            #subprocess.call(cmd,shell=True)
            command = "cd "+path+ ";"+cmd
            self.args = ['-into', str(self.terminal.winId()),'-hold','-e', command]
            #self.process.start('xterm', self.args)
            #self.process.waitForFinished(-1)
            #stdouterr = os.popen4(cmd)[1].read()
            #self.tedit.append(stdouterr)
        except:
            print "There is error during in 'make install' "
            quit()

    
    def uploadModle(self):
        print "Upload button clicked"
        try:
            self.process.close()
        except:
                pass
        try:
            self.file_extension = os.path.splitext(str(self.filename))[1]
            print "File extension",self.file_extension
            print "Parser Content:",self.parser.get('NGSPICE', 'NGSPICE_HOME')
            self.cur_dir = os.getcwd()
            print "My Current Working Directory",self.cur_dir
            self.checkSupportFiles()
            if self.file_extension == ".vhdl":
                self.createModelDirectory()
                self.addingModelInModpath()
                self.createModelFiles()
                self.runMake()
                #self.runMakeInstall()
            else:
                QtGui.QMessageBox.about(self,'Message','''<b>Important Message.</b><br/><br/>This accepts only <b>.vhdl</b> file ''')
        except:
            QtGui.QMessageBox.about(self, 'Message','''<b>Error</b><br/><br/> select a <b>.vhdl</b> file ''')

class FileRemover(QtGui.QWidget):

        def __init__(self, main_obj):
                super(FileRemover, self).__init__()
                self.row = 0
                self.col = 0
                self.cb_dict = {}
                self.marked_list = []
                self.files = main_obj.file_list
                self.sedit = main_obj.sedit

                print self.files

                self.grid = QtGui.QGridLayout()
                removebtn = QtGui.QPushButton('Remove', self)
                removebtn.clicked.connect(self.removeFiles)

                self.grid.addWidget(self.createCheckBox(), 0, 0)
                self.grid.addWidget(removebtn, 1, 1)

                self.setLayout(self.grid)
                self.show()

        def createCheckBox(self):

                self.checkbox = QtGui.QGroupBox()
                self.checkbox.setTitle('Remove Files')
                self.checkgrid = QtGui.QGridLayout()

                self.checkgroupbtn = QtGui.QButtonGroup()

                for path in self.files:

                        print path

                        self.cb_dict[path] = QtGui.QCheckBox(path)
                        self.checkgroupbtn.addButton(self.cb_dict[path])
                        self.checkgrid.addWidget(self.cb_dict[path], self.row, self.col)
                        self.row += 1

                self.checkgroupbtn.setExclusive(False)
                self.checkgroupbtn.buttonClicked.connect(self.mark_file)
                self.checkbox.setLayout(self.checkgrid)

                return self.checkbox

        def mark_file(self):

                for path in self.cb_dict:
                        if self.cb_dict[path].isChecked():
                                if path not in self.marked_list:
                                        self.marked_list.append(path)

                        else:
                                if path in self.marked_list:
                                        self.marked_list.remove(path)

        def removeFiles(self):

                for path in self.marked_list:
                        print path, "is removed"
                        self.sedit.append(path + " removed")
                        self.files.remove(path)

                self.sedit.clear()
                for path in self.files:
                        self.sedit.append(path)

                self.marked_list[:] = []
                self.files[:] = []
                self.close()



def main():
    app = QtGui.QApplication(sys.argv)
    w = Mainwindow()
    sys.exit(app.exec_())

if __name__=='__main__':
    main()
