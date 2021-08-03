################################################################################
##
## BY: GIACOMO A. DOLLEVEDO
## PROJECT MADE WITH: Qt Designer and PySide2
## V: 1.0.0
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

from datetime import datetime
from pathlib import Path

# GUI FILE
from app_modules import *

#CH_NUMBER = 3

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

        self.diretorio = [os.getcwd()]

        self.chGains = ['Not Calibrated' for i in range(1,CH_NUMBER+1)]

        self.signal = MySignal()
        self.signal.sig_with_str.connect(self.print_console_log)
        self.signal.sig_start_acq.connect(self.set_startButton)
        self.signal.sig_dataPlot.connect(self.plotData)
        self.signal.sig_getCfg.connect(self.getChConfig)

        app.aboutToQuit.connect(self.closeEvent)

        self.x = ["0.0;0;0;0;0;0;0\n"]

        self.timer0 = QTimer(self)
        self.time = QTime(0,0,0)
        self.timer0.timeout.connect(self.showTime)
        self.timer0.start(1000)
        self.showTime()
        self.timerFlag = 0
        
        
        self.USBhandler = USBFunctions(self.diretorio, self.signal.sig_with_str, self.signal.sig_start_acq, self.signal.sig_dataPlot, self.signal.sig_getCfg, self.x)
        #self.BThandler = BTFunctions(self.signal.sig_with_str, self.signal.sig_start_acq, self.x)

        
        self.xnp = np.asarray([])
        self.ynp = [np.asarray([]) for i in range(6)]

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
        UIFunctions.userIcon(self, "GD", "", True)
        ## ==> END ##

        self.configOn = 0

        ## CONFIG PANEL BUTTONS CALLBACKS
        self.ui.button_config.clicked.connect(lambda: UIFunctions.configMenu(self, self.configOn))
        self.ui.horizontalSlider_2.valueChanged[int].connect(lambda: UIFunctions.sliderChange(self, self.ui.horizontalSlider_2.value(),1))
        self.ui.horizontalSlider_3.valueChanged[int].connect(lambda: UIFunctions.sliderChange(self, self.ui.horizontalSlider_3.value(),2))

        self.ui.lineEdit_2.returnPressed.connect(lambda: chPlotDisp(self.ui.lineEdit_2, self.activeChannels)) # Current Channel Display

        ##CONNECTION PAGE BUTTONS CALLBACKS
        #self.ui.button_start_server.clicked.connect(lambda: ServerFunctions.startServer(ServerFunctions, self.signal.sig_with_str))
        self.ui.radioButton_3.toggled.connect(lambda: self.radioCheck())
        
        self.ui.button_start_acq.clicked.connect(lambda: self.USBhandler.startUSBacquisiton())
        #self.ui.button_start_acq.clicked.connect(lambda: self.BThandler.startBTacquisiton())
        
        self.ui.button_chooseFolder.clicked.connect(lambda: chooseFolder(self.diretorio, self.ui.lineEd_chooseFolder))

        ##CONNECTING TRIGGER BUTTON CALLBACK
        self.ui.button_trigger.clicked.connect(lambda: saveTrigger(self.time, self.diretorio))

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

        ## CANVAS 1 - MAIN PLOT
        self.canvas1 = MplCanvas(self, width=5, height=4, dpi=100)
        self.canvas1.ax1.set_xlabel('Time [s]')
        self.canvas1.ax1.set_ylabel('Digitalized Voltage')
        self.canvas1.ax1.set_title('Channel Output')
        self.canvas1.ax1.set_facecolor('none')
        self.canvas1.fig1.set_facecolor('none')
        self.canvas1.fig1.subplots_adjust(left=0.05, right=0.97, bottom=0.15, top=0.9)

        self.ch_number = CH_NUMBER
        self.ch_key=[n for n in range(1,self.ch_number+1)] # Channels that are chosen at the moment.
        self.plt_colors = ['r','b','g','y','c','m','w', 'r--','b--','g--','y--','c--','m--','w--','r','b','g','y','c','m','w', 'r--','b--','g--','y--','c--','m--','w--']                      #Colors can also be hexstring code -- THIS IS CURRENTLY A PLACEHOLDER
        self.ln = [self.canvas1.ax1.plot([], [], self.plt_colors[i]) for i in range(2*self.ch_number)]
        
        self.activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
        self.activeChannels[0] = 1

        self.threadFlag = []
        self.threadFlag.append(True)

        self.ui.gridLayout_4.addWidget(self.canvas1, 0, 0, 1, 1)

        self.animation = startPlot(self)

        ## CANVAS 2 - FFT
        self.canvas2 = MplCanvas(self, width=5, height=4, dpi=100)
        self.canvas2.ax1.set_xlabel('Frequency [Hz]')
        self.canvas2.ax1.set_facecolor('none')
        self.canvas2.fig1.set_facecolor('none')      

        self.ui.gridLayout_5.addWidget(self.canvas2, 0, 1, 1, 1)
   

        ## CHANNEL CONFIG COMBO BOX
        for i in range(1,self.ch_number+1):
            self.ui.combo_chSelect.addItem(str(i))

        self.ui.combo_chSelect.currentIndexChanged.connect(lambda: chConfigDisp(self.ui.combo_chSelect, self.ui.plainTextEdit_ChSettings, self.chGains))
        chConfigDisp(self.ui.combo_chSelect, self.ui.plainTextEdit_ChSettings, self.chGains)

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

            #TURNING OFF WIFI BUTTONS
            self.ui.button_start_server.setEnabled(False)
            self.ui.button_connect_server.setEnabled(False)
            
        else:
            self.USBhandler.isLooking = False

            self.ui.button_start_server.setEnabled(True)
            self.ui.button_connect_server.setEnabled(True)
            self.ui.plainTextEdit_2.clear()
            try:
                self.USBhandler.disconnect()
            except:
                pass

  
    def set_startButton(self, flag):
        self.ui.button_start_acq.setEnabled(flag)

    def plotData(self, flag):
        #print('SINAL RECEBIDO')
        self.threadFlag[0] = flag
        readFromFile(self.xnp, self.y, self.threadFlag, self)

    def Button(self):
        # GET BT CLICKED
        btnWidget = self.sender()

        # PAGE HOME
        if btnWidget.objectName() == "btn_home":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_home)
            UIFunctions.resetStyle(self, "btn_home")
            UIFunctions.labelPage(self, "Home")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE CONNECTIONS
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

    ## ==> END ##

    ########################################################################
    ## START ==> UTILITY FUNCTIONS
    ########################################################################

    def print_console_log(self, str):

        if(str == '#C'):
            self.ui.plainTextEdit_2.clear()
        else:
            self.ui.plainTextEdit_2.setPlainText( self.ui.plainTextEdit_2.toPlainText() + str)

    def showTime(self):
        currentTime = QTime.currentTime()
        self.ui.timeC_label.setText(currentTime.toString('hh:mm:ss'))
        if(len(self.x) >= 150):
            self.time = self.time.addSecs(1)
            self.ui.timeL_label.setText(self.time.toString('hh:mm:ss'))

    def getChConfig(self, flag):
        if(flag):
            #print('Getting Config')
            try:
                cfgFile = glob.glob(self.diretorio[0] + r'/*_config*')
                with open(cfgFile[0], 'r') as f:
                    firstLine = f.readline()

                self.chGains = firstLine.split(';')
                self.chConfigDisp()

            except:
                print('Config File not found!')
        else:
            pass


    ########################################################################
    ## END ==> UTILITY FUNCTIONS
    ############################## ---/--/--- ##############################

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
    ## ==> END ##

    def closeEvent(self):
        print('Saving .nirs file')
        try:
            self.USBhandler.disconnect()
            saveFile = NIRSFormatter(filePath=self.diretorio[0])
            saveFile.saveNIRS(self.diretorio[0])

        except Exception as e:
            print('FAILED TO SAVE .nirs FILE!')
            print(e)
            
        sys.exit(0)

    ########################################################################
    ## END ==> APP EVENTS
    ############################## ---/--/--- ##############################

    

class MySignal(QtCore.QObject):
    sig_start_acq = QtCore.Signal(bool)
    sig_with_str = QtCore.Signal(str)
    sig_dataPlot = QtCore.Signal(bool)
    sig_getCfg = QtCore.Signal(bool)

########################################################################
## PLOTTING FUNCTIONS
############################## ---/--/--- ##############################





if __name__ == "__main__":
    app = QApplication(sys.argv)
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeui.ttf')
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeuib.ttf')
    window = MainWindow()
    sys.exit(app.exec_())
