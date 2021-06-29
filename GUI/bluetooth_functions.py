import serial
import serial.tools.list_ports
import time
from threading import Thread

logs_path = r'C:\Users\GiacomoAD\Desktop\PythonEnv\GUI\Logs' + "\\"

string_T = time.ctime()                     #   Creating file name
string_T = string_T.replace(" ", "_")       #based on server starting 
string_T = string_T.replace(":", "-")       #date and current time

file_path = logs_path + string_T + r'.csv'


class BTFunctions():
    def __init__(self, console_signal, start_signal, vetor):
        self.signal = console_signal
        self.signal2 = start_signal
        self.devicePort = "NULL"
        self.ser = serial.Serial()
        self.threadAlive = False
        self.isLooking = False
        self.ser.baudrate = 115200
        self.bufferIn = ''
        self.vet = vetor

        with open(file_path, "a+") as f:
            f.write('')
        pass

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
            self.signal.emit('Could not find device. Check BT connection')
            self.signal2.emit(False)
            return 0
        self.signal.emit('\n'+ str(port) + " is connected to Serial Line.\n Waiting for START SIGNAL")
        print(port + " is connected to Serial Line")
        
        while self.ser.in_waiting:  # Or: while ser.inWaiting():
            print('waiting')
            print(self.ser.readline())
        
        return 1

    def startBTacquisiton(self):
        print('Sending > byte')
        self.signal.emit('\n'+ 'Sent START SIGNAL to main device')
        self.ser.write(b'>')
        time.sleep(0.5)
        self.threadAlive = True
        self.readingThread = Thread(target=self.keepReading, args=(self.signal,))
        self.readingThread.daemon = True
        self.readingThread.start()
    

    def disconnect(self):
        if(self.threadAlive):
            self.threadAlive = False
            self.readingThread.join()
        self.ser.close()

        if(self.devicePort == 'NULL'):
            pass

        else:
            self.signal.emit("Device was DISCONNECTED from " + str(self.devicePort))
            self.signal2.emit(False)
            self.devicePort = 'NULL'
        


    def keepReading(self, console_signal):
        line_counter = 0
        while True:
            if(self.threadAlive == False):
                break

            try:
                self.bufferIn = ser.readline()
                line_counter += 1
                
                if(line_counter == 100):
                    self.vet.append(self.bufferIn)
                    
                    line_counter = 0
                    with open(file_path, "a+") as f:
                        f.write(self.bufferIn.decode('utf-8'))
                    self.bufferIn = ''

                # with open(file_path, "a+") as f:
                #     f.write((self.ser.readline()).decode('utf-8'))
                #     pass
                #print(self.ser.readline())

            except:
                pass

    def threadedFindDevice(self):
        first_flag = False
        while True:
            ports = list(serial.tools.list_ports.comports())
            for p in ports:
                if("COM5" in p[0]):
                    self.devicePort = p[0]
                    return(self.connectDevice(self.devicePort))
                else:
                    self.signal.emit("Looking for device on BT ports...")
                    time.sleep(5)
            
            if(first_flag == False):
                self.signal.emit("Looking for device on BT ports...")
                self.signal2.emit(False)
                first_flag = True
            else:
                self.signal.emit(".")
            time.sleep(5)

            if(self.isLooking == False):
                break