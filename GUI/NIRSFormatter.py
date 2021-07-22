# -*- coding: utf-8 -*-

import glob
import os
import numpy as np
import pickle 
import scipy.io

#    SourceDet (SD) - This is a structured variable containing the source/detector geometry. It has the following fields:
class SourceDet:
    """
         Lambda - Wavelengths used for data acquisition; dimensions <number of wavelengths> by 1
         nSrcs - Number of lasers; scalar variable
         nDets - Number of detectors; scalar variable
         
         SrcPos - Array of probe coordinates of the lasers; dimensions <number of lasers> by 3
         
         DetPos - Array of probe coordinates of the detectors; dimensions <number of detectors> by 3
         
         MeasList - List of source/detector/wavelength measurement channels. It’s an array with
         dimensions, <number of channels> by 4. The meaning of the 4 columns are as follows:

             Column 1 index of the source from the SD.SrcPos list.
             Column 2 index of the detector from the SD.DetPos list.
             Column 3 is unused right now and contains all ones.
             Column 4 index of the wavelength from SD.Lambda
         
    """
    def __init__(self, lbda=(735,850), nSrcs=1, nDets=3):
        
        self.Lambda = np.array(lbda, dtype=int)
        self.nSrcs = nSrcs
        self.nDets = nDets
        self.SrcPos = np.zeros((nSrcs,3))
        self.DetPos = np.zeros((nDets,3))
        
    def createMeas(self, col1, col2, col3, col4):
        self.MeasList = np.column_stack((col1,col2,col3,col4))
        
    def setSrcPos(self, srcpos):
        self.SrcPos = srcpos
        
    def setDetPos(self, detpos):
        self.DetPos = detpos
    
    def setMeas(self, meas):
        self.MeasList = meas
        
        

class NIRSFormatter:
    
    def __init__(self, filePath, sd=SourceDet()):
        
        list_of_files = glob.glob(filePath + r'\*.csv')
        file = max(list_of_files, key=os.path.getmtime)
        
        self.convertFile(file)
        self.sd = sd
        self.name = (file.split('\\'))[-1][0:-4]

    def setName(self, name):
        self.name = name

    def setSD(self, sd):
        self.SD = sd
        
    def setS(self, s):
        self.s = s
        
    def setMl(self, ml):
        self.ml = ml
        
    def setAux(self, aux):
        self.aux = aux
    
    def convertFile(self, file):
        
        t = []
        R = []
        IR = []

        with open(file, "r") as f:
            data = f.read()

        # with open(configFile, "r") as f:
        #     configData = f.read()

        data        =   data.split('\n')
        dataRead    =   [data[x].split(';') for x in range(len(data))]
        
        # dataRead now contains reading structured as:
        # dataRead[i][j] -> i = data point in time, j(j+1) = Infrared(Red) reading by channel j

        for x in range(int((len(dataRead[1])-1)/2)):
            R.append([])
            IR.append([])
        
        for x in range(len(dataRead)-1):
            t.append(dataRead[x][0])

            for j in range(int((len(dataRead[1])-1)/2)):
               R[j].append(dataRead[x][1+(2*j)])
               IR[j].append(dataRead[x][1+(2*j+1)])
               
        
        self.t = np.array(t, dtype=float)
        
        for x in range(len(R)):
            R[x] = np.array(R[x], dtype=float)
            IR[x] = np.array(IR[x], dtype=float)
        
        for x in range(len(R)):
            if x==0:
                d = (R[x])
                d = np.vstack((d,IR[x]))
                
            else:
               d = np.vstack((d,R[x]))
               d = np.vstack((d,IR[x]))
            
        self.d = np.transpose(d)

    def saveNIRS(self, filePath):
        fileName = self.name + ".nirs"        
        scipy.io.savemat(filePath + '\\' + fileName, mdict={'Obj1': self})

        
    
