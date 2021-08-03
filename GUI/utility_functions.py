CH_NUMBER = 3

import os
from PySide2 import QtWidgets

###################################################
    ######## CONNECTION PAGE FUNCTIONS ########
###################################################
def chooseFolder(diretorio, lineEd_chooseFolder):
        diretorio[0] = QtWidgets.QFileDialog.getExistingDirectory(None, 'Select a folder:', 'C:\\', QtWidgets.QFileDialog.ShowDirsOnly)
        
        if diretorio[0] == '':
            diretorio[0] = os.getcwd()
            diretorio[0] = diretorio[0].replace('\\','/')

        lineEd_chooseFolder.setText(diretorio[0])
###################################################
        ############ END OF ############
    ######## CONNECTION PAGE FUNCTIONS ########
###################################################


###################################################
    ######## MONITORING PAGE FUNCTIONS ########
###################################################
def chConfigDisp(combo_chSelect, plainTextEdit_ChSettings, chGains):
    currentCh = combo_chSelect.currentText()
    plainTextEdit_ChSettings.setPlainText('Current Selected Channel:\t' + currentCh + '\n')
    plainTextEdit_ChSettings.appendPlainText('Auto-Gain Setting:\t' + chGains[int(currentCh)-1])



def saveTrigger(time, diretorio):
        tempo = time.toString('hh:mm:ss')
        tempo = tempo.split(':')

        trigger_path = diretorio[0] + '/' + 'Triggers.txt'

        with open(trigger_path, "a+") as f:
            f.write(str(60*int(tempo[1]) + int(tempo[2])) + '\n')
        pass

        print(60*int(tempo[1]) + int(tempo[2]))

def chPlotDisp(lineEdit_2, activeChannels):
        text = lineEdit_2.text()
        
        if text == '':
            for ch in range(len(activeChannels)):
                activeChannels[ch] = 1

            print(activeChannels)
        
        else:
            if '-' in text:
                text = text.split('-')

                try:
                    [int(item) for item in text]
                    startingCh = int(text[0]) - 1 
                    endingCh = int(text[-1])
                    activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                    for ch in range(startingCh, endingCh):
                        activeChannels[ch] = 1
                    
                    print(activeChannels)
                
                except:
                    activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                    activeChannels[0] = 1
                    print('Invalid Input!')
                
            
            elif ';' in text:
                text = text.split(';')

                try:
                    [int(item) for item in text]

                    activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                    for ch in range(len(text)):
                        activeChannels[int(text[ch])-1] = 1

                    print(activeChannels)

                except:
                    activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                    activeChannels[0] = 1
                    print('Invalid Input!')
            
            else:
                try:
                    text = int(text)
                    
                    if text <= CH_NUMBER and text != 0:
                        activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                        activeChannels[text-1] = 1

                        print(activeChannels)
                        

                    else:
                        activeChannels = [0 for ch in range(1,CH_NUMBER+1)]
                        activeChannels[0] = 1
                        print('Invalid channel!')

                except:
                    print('Invalid Input!')
###################################################
        ############ END OF ############
    ######## CONNECTION PAGE FUNCTIONS ########
###################################################



# CHANGE THIS TO ANOTHER .py FILE TO HANDLE ONLY CHANNEL CONFIG
