import serial
import serial.tools.list_ports
import time
from threading import Thread

from datetime import datetime
from pathlib import Path

class USBFunctions():
    def __init__(self, folder_path, console_signal, start_signal, plot_signal, vetor):
       self.pathFolder = folder_path
       self.signal = console_signal
       self.signal2 = start_signal
       self.plotSignal = plot_signal
       self.devicePort = "NULL"
       self.ser = serial.Serial()
       self.threadAlive = False
       self.isLooking = False
       self.ser.baudrate = 115200
       self.ser.timeout = 0.5
       self.bufferIn = ''
       self.vet = vetor
       self.configFlag = False
       self.configBuffer = []

    def findDevices(self):
        self.isLooking = True
        self.lookingThread = Thread(target=self.threadedFindDevice, args=())
        self.lookingThread.daemon = True
        self.lookingThread.start()
 

    def connectDevice(self, port):
        self.ser.port = port
        try:
            self.ser.open()
            self.signal2.emit(True)
        except:
            self.signal.emit('Could not find device. Check USB connection')
            self.signal2.emit(False)
            return 0
        self.signal.emit('\n'+ str(port) + " is connected to Serial Line.\n Waiting for START SIGNAL")
        print(port + " is connected to Serial Line")
        
        while self.ser.in_waiting:  # Or: while ser.inWaiting():
            print('waiting')
            print(self.ser.readline())
        
        return 1

    def startUSBacquisiton(self):
        print('Sending > byte')
        self.signal.emit('\n'+ '\nSent START SIGNAL to main device')
        self.ser.write(b'>')

        if(not self.checkStart()):
            self.signal.emit('\n'+ 'Unable to connect to device. Please, check connections and try again.')

        else:
            self.signal2.emit(False)
            self.signal.emit('#C')
            self.signal.emit('CONNECTED!\n' + 'Self calibration routine started.')

            timestamp = str(datetime.today())
            timestamp = timestamp[:-10].replace(' ', '_')
            timestamp = timestamp.replace(':','-')
            
            self.pathFolder[0] = self.pathFolder[0] + "/" + timestamp
            Path(self.pathFolder[0]).mkdir(parents=True, exist_ok=True)
            
            self.signal.emit('\n'+ 'Data will be logged and saved at: ' + str(self.pathFolder[0]))

            #Path(file_path + "/my/directory").mkdir(parents=True, exist_ok=True)

            time.sleep(0.25)
            self.ser.timeout = None
            self.threadAlive = True
            self.readingThread = Thread(target=self.keepReading, args=(self.signal,))
            self.readingThread.daemon = True
            self.readingThread.start()
    

    def checkStart(self):
        try:
            incBuffer = self.ser.readline()
            incBuffer = incBuffer.decode('utf-8')

            if(incBuffer == 'BTSTART\n'):
                return True
        
            else:
                return False

        except Exception as e:
            print(e)
            return False

    def disconnect(self):
        if(self.threadAlive):
            self.threadAlive = False
            self.readingThread.join()
            self.writingThread.join()

        self.ser.close()

        if(self.devicePort == 'NULL'):
            pass

        else:
            self.signal.emit("Device was DISCONNECTED from " + str(self.devicePort))
            self.signal2.emit(False)
            self.devicePort = 'NULL'
        


    def keepReading(self, console_signal):
        start_write_thread = 1
        lines_read = 0

        while True:
            if(self.threadAlive == False):
                break

            try:

                self.bufferIn = self.ser.readlines(150)
                
                for line in self.bufferIn:
                    line = line.decode('utf-8')

                    # If incoming message is a configuration message
                    if line[0:2] == '#C':
                        self.configFlag = True
                        self.configBuffer.append(line)
                    else:
                        self.vet.append(line)

                self.bufferIn = ''

                self.plotSignal.emit(True)

                if(start_write_thread == 1 and len(self.vet) >= 500):
                    start_write_thread = 0
                    self.writingThread = Thread(target=self.threadedWriteFile)
                    self.writingThread.daemon = True
                    self.writingThread.start()


            except Exception as e:
                print(e)
                pass

    def threadedWriteFile(self):
        last_line = 0
        string_T = time.ctime()                     # Creating file name
        string_T = string_T.replace(" ", "_")       #based on server starting 
        string_T = string_T.replace(":", "-")       #date and current time

        file_path = self.pathFolder[0] + '/' + string_T + r'.csv'
        config_file_path = self.pathFolder[0] + '/' + string_T + r'_config.csv'

        with open(file_path, "a+") as f:
            while True:
                if(self.threadAlive == False):
                    break

                for i, linha in enumerate(self.vet):
                    if(i >= last_line):
                        f.write(linha)
                        last_line += 1

                f.flush()

                if self.configFlag == True:
                    self.configFlag = False

                    with open(config_file_path, "a+") as fConfig:
                        for linha in self.configBuffer:
                            fConfig.write(linha[2:])

                    
                time.sleep(2)

        
            
                    
 

    def threadedFindDevice(self):
        first_flag = False
        while True:
            ports = list(serial.tools.list_ports.comports())
            for p in ports:
                if("Silicon Labs" in p[1]):
                    self.devicePort = p[0]
                    return(self.connectDevice(self.devicePort))
                else:
                    self.signal.emit("Looking for device on USB ports...")
                    time.sleep(5)
            
            if(first_flag == False):
                self.signal.emit("Looking for device on USB ports...")
                self.signal2.emit(False)
                first_flag = True
            else:
                self.signal.emit(".")
            time.sleep(5)

            if(self.isLooking == False):
                break