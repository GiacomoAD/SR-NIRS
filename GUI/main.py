################################################################################
##
## BY: GIACOMO A. DOLLEVEDO
## PROJECT MADE WITH: Qt Designer and PySide2
## V: 1.0.0
##
## This project can be used freely for all uses, as long as they maintain the
## respective credits only in the Python scripts, any information in the visual
## interface (GUI) can be modified without any implication.
##
## There are limitations on Qt licenses if you want to use your products
## commercially, I recommend reading them on the official website:
## https://doc.qt.io/qtforpython/licenses.html
##
## Base GUI layout https://github.com/Wanderson-Magalhaes/Simple_PySide_Base
##
################################################################################

import sys
import platform
from PySide2 import QtCore, QtGui, QtWidgets
from PySide2.QtCore import (QCoreApplication, QPropertyAnimation, QDate, QDateTime, QMetaObject, QObject, QPoint, QRect, QSize, QTime, QUrl, Qt, QEvent, QTimer, QTime)
from PySide2.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont, QFontDatabase, QIcon, QKeySequence, QLinearGradient, QPalette, QPainter, QPixmap, QRadialGradient)
from PySide2.QtWidgets import *


import glob
import os

import matplotlib
matplotlib.use("Qt5Agg") #backend of matplotlib
import matplotlib.animation as FuncAnimation
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT

from matplotlib import style
import matplotlib.pyplot as plt

style.use("dark_background")

import numpy as np


# GUI FILE
from app_modules import *

class MplCanvas(FigureCanvasQTAgg):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig1, self.ax1 = plt.subplots()
        super(MplCanvas, self).__init__(self.fig1)
    

class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        ## PRINT ==> SYSTEM
        print('System: ' + platform.system())
        print('Version: ' + platform.release())

        self.signal = MySignal()
        self.signal.sig_with_str.connect(self.print_console_log)
        self.signal.sig_start_acq.connect(self.set_startButton)
        self.signal.sig_dataPlot.connect(self.plotData)

        self.x = ["0.0;0;0;0;0\n"]

        self.timer0 = QTimer(self)
        self.time = QTime(0,0,0)
        self.timer0.timeout.connect(self.showTime)
        self.timer0.start(1000)
        self.showTime()
        self.timerFlag = 0
        
        

        self.USBhandler = USBFunctions(self.signal.sig_with_str, self.signal.sig_start_acq, self.signal.sig_dataPlot, self.x)
        self.BThandler = BTFunctions(self.signal.sig_with_str, self.signal.sig_start_acq, self.x)

        
        self.xnp = np.asarray([])
        self.ynp = [np.asarray([]) for i in range(4)]

        self.y = []
        self.y.append([[],[]])
        ########################################################################
        ## START - WINDOW ATTRIBUTES
        ########################################################################

        ## REMOVE ==> STANDARD TITLE BAR
        UIFunctions.removeTitleBar(True)
        ## ==> END ##

        ## SET ==> WINDOW TITLE
        self.setWindowTitle('ffNIRS GUI v0.0.1')
        UIFunctions.labelTitle(self, 'ffNIRS GUI')
        UIFunctions.labelDescription(self, 'New GUI layout - Test')
        ## ==> END ##

        ## WINDOW SIZE ==> DEFAULT SIZE
        startSize = QSize(1680, 1050)
        self.resize(startSize)
        self.setMinimumSize(startSize)
        # UIFunctions.enableMaximumSize(self, 500, 720)
        ## ==> END ##

        ## ==> CREATE MENUS
        ########################################################################

        ## ==> TOGGLE MENU SIZE
        self.ui.btn_toggle_menu.clicked.connect(lambda: UIFunctions.toggleMenu(self, 220, True))
        ## ==> END ##

        ## ==> ADD CUSTOM MENUS
        self.ui.stackedWidget.setMinimumWidth(20)
        UIFunctions.addNewMenu(self, "Home Screen", "btn_home", "url(:/16x16/icons/16x16/cil-home.png)", True)
        UIFunctions.addNewMenu(self, "Connections", "btn_conn_screen", "url(:/16x16/icons/16x16/cil-rss.png)", True)
        UIFunctions.addNewMenu(self, "Monitor Screen", "btn_graph_screen", "url(:/16x16/icons/16x16/cil-screen-desktop.png)", True)

        #UIFunctions.addNewMenu(self, "Config", "btn_widgets", "url(:/16x16/icons/16x16/cil-equalizer.png)", False)
        ## ==> END ##

        # START MENU => SELECTION
        UIFunctions.selectStandardMenu(self, "btn_home")
        ## ==> END ##

        ## ==> START PAGE
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_home)
        ## ==> END ##

        ## USER ICON ==> SHOW HIDE
        UIFunctions.userIcon(self, "GH", "", True)
        ## ==> END ##

        self.configOn = 0

        ## CONFIG PANEL BUTTONS CALLBACKS
        self.ui.button_config.clicked.connect(lambda: UIFunctions.configMenu(self, self.configOn))
        self.ui.horizontalSlider_2.valueChanged[int].connect(lambda: UIFunctions.sliderChange(self, self.ui.horizontalSlider_2.value(),1))
        self.ui.horizontalSlider_3.valueChanged[int].connect(lambda: UIFunctions.sliderChange(self, self.ui.horizontalSlider_3.value(),2))

        ##CONNECTION PAGE BUTTONS CALLBACKS
        self.ui.button_start_server.clicked.connect(lambda: ServerFunctions.startServer(ServerFunctions, self.signal.sig_with_str))
        self.ui.radioButton_3.toggled.connect(lambda: self.radioCheck())
        
        self.ui.button_start_acq.clicked.connect(lambda: self.USBhandler.startUSBacquisiton())
        #self.ui.button_start_acq.clicked.connect(lambda: self.BThandler.startBTacquisiton())
        

        ##CONNECTING TRIGGER BUTTON CALLBACK
        self.ui.button_trigger.clicked.connect(lambda: self.saveTrigger())

        ## ==> MOVE WINDOW / MAXIMIZE / RESTORE
        ########################################################################
        def moveWindow(event):
            # IF MAXIMIZED CHANGE TO NORMAL
            if UIFunctions.returStatus() == 1:
                UIFunctions.maximize_restore(self)

            # MOVE WINDOW
            if event.buttons() == Qt.LeftButton:
                self.move(self.pos() + event.globalPos() - self.dragPos)
                self.dragPos = event.globalPos()
                event.accept()

        # WIDGET TO MOVE
        self.ui.frame_label_top_btns.mouseMoveEvent = moveWindow
        ## ==> END ##

        ## ==> LOAD DEFINITIONS
        ########################################################################
        UIFunctions.uiDefinitions(self)
        ## ==> END ##

        ########################################################################
        ## END - WINDOW ATTRIBUTES
        ############################## ---/--/--- ##############################




        ########################################################################
        #                                                                      #
        ## START -------------- WIDGETS FUNCTIONS/PARAMETERS ---------------- ##
        #                                                                      #
        ## ==> USER CODES BELLOW                                              ##
        ########################################################################
        self.ui.button_start_acq.setEnabled(False)


        ## ==> QTableWidget RARAMETERS
        ########################################################################
        #self.ui.tableWidget.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        ## ==> END ##

        ## CANVAS 1 - MAIN PLOT
        self.canvas1 = MplCanvas(self, width=5, height=4, dpi=100)
        self.canvas1.ax1.set_xlabel('Time [s]')
        self.canvas1.ax1.set_ylabel('Digitalized Voltage')
        self.canvas1.ax1.set_title('Channel Output')
        self.canvas1.ax1.set_facecolor('none')
        self.canvas1.fig1.set_facecolor('none')
        self.canvas1.fig1.subplots_adjust(left=0.05, right=0.97, bottom=0.15, top=0.9)

        self.ch_number = 1
        self.ch_key=[n for n in range(1,self.ch_number+1)] # Channels that are chosen at the moment.
        self.plt_colors = ['r','b','g','y','c','m','w', 'r--','b--','g--','y--','c--','m--','w--','r','b','g','y','c','m','w', 'r--','b--','g--','y--','c--','m--','w--']                      #Colors can also be hexstring code -- THIS IS CURRENTLY A PLACEHOLDER
        self.ln = [self.canvas1.ax1.plot([], [], self.plt_colors[i]) for i in range(2*self.ch_number)]
        
        list_of_files = glob.glob(r'C:\Users\GiacomoAD\Desktop\PythonEnv\GUI\Logs\*.csv') # * means all if need specific format then *.csv
        file_path = max(list_of_files, key=os.path.getctime) # finding last created file to save stuff
        #file_path = r'C:\Users\GiacomoAD\Desktop\PythonEnv\GUI\Logs\Sat_Apr_17_22-57-43_2021.csv'


        self.threadFlag = []
        self.threadFlag.append(True)

        self.fileThread = Thread(target=readFromFile, args=(file_path,self.xnp,self.y,self.threadFlag,self,))
        self.fileThread.daemon = True
        self.fileThread.start()

        self.ui.gridLayout_4.addWidget(self.canvas1, 0, 0, 1, 1)

        self.animation = startPlot(self, file_path)



        ## CANVAS 2 - FFT
        self.canvas2 = MplCanvas(self, width=5, height=4, dpi=100)
        self.canvas2.ax1.set_xlabel('Frequency [Hz]')
        self.canvas2.ax1.set_facecolor('none')
        self.canvas2.fig1.set_facecolor('none')      

        self.ui.gridLayout_5.addWidget(self.canvas2, 0, 1, 1, 1)
   
        ########################################################################
        #                                                                      #
        ## END --------------- WIDGETS FUNCTIONS/PARAMETERS ----------------- ##
        #                                                                      #
        ############################## ---/--/--- ##############################





        ## SHOW ==> MAIN WINDOW
        ########################################################################
        self.show()
        ## ==> END ##

    ########################################################################
    ## MENUS ==> DYNAMIC MENUS FUNCTIONS
    ########################################################################
    
    def radioCheck(self):
        if(self.ui.radioButton_3.isChecked()):
            self.ui.plainTextEdit_2.clear()
            self.USBhandler.isLooking = True
            self.USBhandler.findDevices()

            #self.BThandler.isLooking = True
            #self.BThandler.findDevices()

            
            # if(self.USBhandler.listDevices()):
            #     self.ui.button_start_acq.setEnabled(True)
            # else:
            #     self.ui.button_start_acq.setEnabled(False)
            #TURNING OFF WIFI BUTTONS
            self.ui.button_start_server.setEnabled(False)
            self.ui.button_connect_server.setEnabled(False)
            
        else:
            self.USBhandler.isLooking = False

            #self.BThandler.isLooking = False
            self.ui.button_start_server.setEnabled(True)
            self.ui.button_connect_server.setEnabled(True)
            self.ui.plainTextEdit_2.clear()
            try:
                self.USBhandler.disconnect()
                #self.BThandler.disconnect()
            except:
                pass


    def print_console_log(self, str):
        self.ui.plainTextEdit_2.setPlainText( self.ui.plainTextEdit_2.toPlainText() + str)
        
    def set_startButton(self, flag):
        self.ui.button_start_acq.setEnabled(flag)

    def plotData(self, flag):
        #print('SINAL RECEBIDO')
        self.threadFlag[0] = flag
        readFromFile('', self.xnp, self.y, self.threadFlag, self)

    def showTime(self):
        currentTime = QTime.currentTime()
        self.ui.timeC_label.setText(currentTime.toString('hh:mm:ss'))
        if(len(self.x) >= 150):
            self.time = self.time.addSecs(1)
            self.ui.timeL_label.setText(self.time.toString('hh:mm:ss'))

    def saveTrigger(self):
        tempo = self.time.toString('hh:mm:ss')
        tempo = tempo.split(':')
        print(60*int(tempo[1]) + int(tempo[2]))

    def Button(self):
        # GET BT CLICKED
        btnWidget = self.sender()
        print(self.configOn)

        # PAGE HOME
        if btnWidget.objectName() == "btn_home":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_home)
            UIFunctions.resetStyle(self, "btn_home")
            UIFunctions.labelPage(self, "Home")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE NEW USER
        if btnWidget.objectName() == "btn_conn_screen":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_conn)
            UIFunctions.resetStyle(self, "btn_conn_screen")
            UIFunctions.labelPage(self, "Connections")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))
      
        # PAGE GRAPHS
        if btnWidget.objectName() == "btn_graph_screen":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_graph)
            UIFunctions.resetStyle(self, "btn_graph_screen")
            UIFunctions.labelPage(self, "Monitor")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE WIDGETS
        if btnWidget.objectName() == "btn_widgets":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_widgets)
            UIFunctions.resetStyle(self, "btn_widgets")
            UIFunctions.labelPage(self, "Custom Widgets")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

    ## ==> END ##

    ########################################################################
    ## START ==> APP EVENTS
    ########################################################################

    ## EVENT ==> MOUSE DOUBLE CLICK
    ########################################################################
    def eventFilter(self, watched, event):
        if watched == self.le and event.type() == QtCore.QEvent.MouseButtonDblClick:
            print("pos: ", event.pos())
    ## ==> END ##

    ## EVENT ==> MOUSE CLICK
    ########################################################################
    def mousePressEvent(self, event):
        self.dragPos = event.globalPos()
        if event.buttons() == Qt.LeftButton:
            print('Mouse click: LEFT CLICK')
        if event.buttons() == Qt.RightButton:
            print('Mouse click: RIGHT CLICK')
        if event.buttons() == Qt.MidButton:
            print('Mouse click: MIDDLE BUTTON')
    ## ==> END ##

    ## EVENT ==> KEY PRESSED
    ########################################################################
    def keyPressEvent(self, event):
        print('Key: ' + str(event.key()) + ' | Text Press: ' + str(event.text()))
    ## ==> END ##

    ## EVENT ==> RESIZE EVENT
    ########################################################################
    def resizeEvent(self, event):
        self.resizeFunction()
        return super(MainWindow, self).resizeEvent(event)

    def resizeFunction(self):
        print('Height: ' + str(self.height()) + ' | Width: ' + str(self.width()))
    ## ==> END ##

    ########################################################################
    ## END ==> APP EVENTS
    ############################## ---/--/--- ##############################

    

class MySignal(QtCore.QObject):
    sig_start_acq = QtCore.Signal(bool)
    sig_with_str = QtCore.Signal(str)
    sig_dataPlot = QtCore.Signal(bool)

########################################################################
## PLOTTING FUNCTIONS
############################## ---/--/--- ##############################





if __name__ == "__main__":
    app = QApplication(sys.argv)
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeui.ttf')
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeuib.ttf')
    window = MainWindow()
    sys.exit(app.exec_())
