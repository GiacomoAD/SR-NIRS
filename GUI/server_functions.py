# -*- coding: utf-8 -*-

"""

@author: Gio/Gia

This is the code in charge of running a webserver to handle all connections made by our 1st gen. wearable NIRS.
It handles the master ESP connection to gather an log data, as well as our GUI to control the NIRS system over WiFi.

TBD

- Handle reconnects upon unexpected connection issues
- Try to start a local WiFi network (instead of using a smartphone routed network) -> TRYING TO SOLVE THIS USING ANOTHER ESP AS AN 'AP'
- Handle new interface requests, such as pausing, channel gain selection, etc.
- Maybe use 'select' instead of priority queues to make socket data transfer in a more efficient way


"""

#PRECISA PARA IMPORTAR A NOSSA BIBLIOTECA SE ELA NAO ESTA NO ROOT DO PYTHON
import sys
# sys.path.append(r'C:\Users\User\lobnirs\wearablenirs')
# sys.path.append(r'C:\Users\Giovanni\Desktop\coding\FFNIRS\wearablenirs')
sys.path.append(r'C:\Users\GiacomoAD\Documents\Git\wearable\wearablenirs')
#%%
from threading import Thread
import socket
import time
from queue import Queue, PriorityQueue
from operator import itemgetter
import numpy as np
from timeit import default_timer as timer
import linecache


q = Queue(maxsize = 10)
cmd_q = PriorityQueue(maxsize = 5)

global recv_counter
recv_counter = 0
 
global GIOZAO
GIOZAO = 0

global organize_flag
organize_flag = 0

global datavec_1_buffer
global datavec_2_buffer

datavec_1_buffer = []
datavec_2_buffer = []

global last_timestamp
last_timestamp = 0

global lastlineread
lastlineread = 0

class ServerFunctions():

    def __init__(self):
        print('class started')

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: choose_handler                                                                                         #
    #                                                                                                                       #
    #   Description: This method authenticates the connected client and returns a flag that indicates the correct handler   #
    #                                                                                                                       #
    #   Input parameters:   connection  -> Socket object from the recent connection                                         #
    #                       address     -> Address from the recent connection                                               #
    #                                                                                                                       #
    #   Output parameters:  Returns a flag to each assigned handler                                                         #
    #                                                                                                                       #
    #########################################################################################################################  
    def choose_handler(connection, address):
        
        teste = 'authcode'
        sendD(connection, teste)
        print('Authentication key has been sent')
        
        key = receive(connection).decode()
        print('Client has been authenticated')
        
        if key == 'ESPONLINE':
            return 1
        
        elif key == 'INTERFACEONLINE':
            return 2
        
        else:
            return 0
    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: dif                                                                                                    #
    #                                                                                                                       #
    #   Description: Calculates difference between nubmers that are strings                                                 #
    #                                                                                                                       #
    #   Input parameters:   str1  -> string 1                                                                               #
    #                       str2  ->                                                                                        #
    #                                                                                                                       #
    #                                                                                                                       #
    #                                                                                                                       #
    #########################################################################################################################  
    def diff(str1,str2):
        valor = eval(str1 + "-" + str2)
        return abs(valor)

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: dif                                                                                                    #
    #                                                                                                                       #
    #   Description: Calculates difference between nubmers that are strings                                                 #
    #                                                                                                                       #
    #   Input parameters:   str1  -> string 1                                                                               #
    #                       str2  ->                                                                                        #
    #                                                                                                                       #
    #                                                                                                                       #
    #                                                                                                                       #
    ######################################################################################################################### 
    def diffcont(value,vector):
        #checks if there is any compatible timestamps left
        a = 0
        vector_t = [item[0] for item in vector]
        for i in range(len(vector_t)):
            if diff(value, vector_t[i]) <= 0.2:
                a=1;
        return a

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: averagevec                                                                                             #
    #                                                                                                                       #
    #   Description: Calculates the average from two string inputs                                                          #
    #                                                                                                                       #
    #   Input parameters:   vec1  -> string 1                                                                               #
    #                       vec2  ->                                                                                        #
    #                                                                                                                       #
    #                                                                                                                       #
    #                                                                                                                       #
    ######################################################################################################################### 
    def averagevec(vec1,vec2):
        vec1_aux = [float(a) for a in vec1[0:8]]
        vec2_aux = [float(a) for a in vec2[0:8]]
        average_aux = list(np.asarray(vec1_aux)/2 + np.asarray(vec2_aux)/2)
        average = [str(a) for a in average_aux]
        return average

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: organizedata2                                                                                          #
    #                                                                                                                       #
    #   Description:                                                                                                        #
    #                                                                                                                       #
    #   Input parameters:   prevec  -> string 1                                                                             #
    #                                                                                                                       #
    #                                                                                                                       #
    #                                                                                                                       #
    ######################################################################################################################### 
    def organizedata2(prevec,last_timestamp):
        # Organize data coming from saved buffer file

        prevec_aux2 = [prevec[j].split(";") for j in range(len(prevec))]
        datavec = prevec_aux2

        # sweep vector to find coincidences, if there are any. If there are, average them out 
        # (CHANGE THIS FOR THE CASE THEY ARE ZERO) and then make new data vector.
        for i in range(len(prevec_aux2)):
            for j in range(len(prevec_aux2)):
                if prevec_aux2[i][0] == prevec_aux2[j][0] and i != j and datavec[i] != [[]] and datavec[j]!=[[]]:
                    datavec[i] = averagevec(prevec_aux2[i],prevec_aux2[j])
                    datavec[j] = [[]]

        datavec_filt=list(filter(lambda a: a != [[]], datavec))

        dvec_tosort = datavec_filt.copy()
        # Now we have to go back to strings!
        for k in range(len(dvec_tosort)):
            dvec_tosort[k][0] = float(dvec_tosort[k][0])

        datavec_sorted = sorted(dvec_tosort, key=itemgetter(0))


        for p in range(len(datavec_sorted)):
            datavec_sorted[p][0] = str(datavec_sorted[p][0])

        dvec_tocut = datavec_sorted.copy()
        timestamps = [tempo[0] for tempo in datavec_sorted]
        
        try:    
            corte = timestamps.index(last_timestamp)
            dvec_cut = dvec_tocut[corte:-1]
            
        except:
            dvec_cut = dvec_tocut
        # last_timestamp_index = timestamps.index(timestamps[-1])

        joint_data1 = [";".join(dvec_cut[i]) for i in range(len(dvec_cut[i]))]
        joint_data2 = "\n".join(joint_data1)

        organizedata2.last_timestamp = timestamps[-1]

        return joint_data2

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: file_len                                                                                           #
    #                                                                                                                       #
    #   Description: function returns file length                                                                                               #
    #                                                                                                                       #
    #   Input parameters:   filename     ->                                                                                   #
    #                                                                                                                       #
    #   Output parameters: interger number                                                                                               #
    #                                                                                                                       #
    #########################################################################################################################

    def file_len(fname):
        with open(fname) as f:
            for i, l in enumerate(f):
                pass
        return i + 1

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: organizedata                                                                                           #
    #                                                                                                                       #
    #   Description:                                                                                                        #
    #                                                                                                                       #
    #   Input parameters:   prevec     ->                                                                                   #
    #                                                                                                                       #
    #   Output parameters:                                                                                                  #
    #                                                                                                                       #
    #########################################################################################################################  




    def organizedata(rawfile):
        
        global sync_buffer_1
        global sync_buffer_2
        global last_timestamp
        global datavec_1_buffer
        global datavec_2_buffer
        global lastlineread
        start = timer()
        
        m=1
        buffdata = []
        # reads raw file and creates a buffer variable
        linecache.checkcache(rawfile)
        buffdata = linecache.getlines(rawfile)[lastlineread:]
        lastlineread = len(buffdata)+lastlineread
        print(lastlineread)
            
        prevec_aux = [buffdata[i].rstrip() for i in range(len(buffdata))]
        
        prevec_aux2 = [prevec_aux[j].split(";") for j in range(len(prevec_aux))]

        #separate data from each client and put wl's togheter
        # FORMAT: TIME;CH1WL1;CH2WL1;CH3WL1;CH4WL1;CH1WL2;
        datavec_1_aux = [['0','0','0','0','0']]
        datavec_2_aux = [['0','0','0','0','0']]
        datavec = []
        
        for k in range(len(prevec_aux2)):
            if prevec_aux2[k][0] == "#01": 
                datavec_1_aux.append(prevec_aux2[k][1:])              
            elif prevec_aux2[k][0] == "#02":
                datavec_2_aux.append(prevec_aux2[k][1:]) #
        datavec_1_aux.pop(0); # Removes [0,0,0,0,0]
        datavec_2_aux.pop(0);
        
        datavec_1 = []
        datavec_1.extend(datavec_1_buffer)
        datavec_2 = []
        datavec_2.extend(datavec_2_buffer)
        l=len(datavec_1)
        
        for k in range(0,len(datavec_1_aux)-1,2):
            datavec_1.append(datavec_1_aux[k][:5])
            datavec_1[l].extend(datavec_1_aux[k+1][:5])
            datavec_1[l][0] = str(round(eval(datavec_1[l][0] + "+" + datavec_1[l][5])/2,2))
            datavec_1[l].pop(5)
            l+=1
        
        l=len(datavec_2)
        for k in range(0,len(datavec_2_aux)-1,2):
            datavec_2.append(datavec_2_aux[k][:5])
            datavec_2[l].extend(datavec_2_aux[k+1][:5])
            datavec_2[l][0] = str(round(eval(datavec_2[l][0] + "+" + datavec_2[l][5])/2,2))
            datavec_2[l].pop(5)
            l+=1
        l=0
        
        #create copies of datavecs for next loop
        datavec_1_buffer = datavec_1.copy()
        datavec_2_buffer = datavec_2.copy()
            
        for i in range(len(datavec_1)):
            for j in range(len(datavec_2)):
                # Also, we can't append the initialized vectors (0,0,0,0,0)
                # Also, we can't append strange symbols!
                flag = 0
                dif = diff(datavec_1[i][0],datavec_2[j][0])
                # If there is a time coincidence, append it to datavec. Check if the current time is not already appended and if there are no strange stuff in the vector.
                if flag !=1 and dif <= 0.2 and len(datavec_1[i])==9 and len(datavec_2[j])==9 and datavec_2[j][0] not in [item[0][0] for item in datavec] and datavec_1[i][0] not in [item[0][0] for item in datavec] and float(datavec_1[i][0]) > float(last_timestamp) and float(datavec_2[j][0]) > float(last_timestamp): # and [dig.isascii() for dig in datavec_1[i]] and [dig2.isascii() for dig2 in datavec_2[j]] :
                    foo = []
                    foo = [datavec_1[i].copy()]
                    foo[0].extend(datavec_2[j][1:].copy())
                    # Remove the points that were appended from the buffered copies
                    datavec_1_buffer[i] = []
                    datavec_2_buffer[j] = []
                    
                    datavec.append(foo)
                    # If there is no match for the time given in client #01, append it!
                    
                #     flag = 1
                # if flag != 1 and datavec_1[i][0] not in [item[0][0] for item in datavec] and datavec_1[i][0] != '0' and len(datavec_1[i]) == 9 and diffcont(datavec_1[i][0],datavec_2) != 1 : # and [dig3.isascii() for dig3 in datavec_1[i]]:
                #     foo=[]
                #     foo = [datavec_1[i].copy()]
                #     foo[0].extend(['nan','nan','nan','nan','nan','nan','nan','nan'])
                #     datavec.append(foo)
                #     flag = 1
                #     # If there is no match for the time given in client #02, append it!
                # if flag != 1 and datavec_2[j][0] not in [item[0] for item in datavec_1] and datavec_2[j][0] not in [item[0][0] for item in datavec] and datavec_2[j][0] != '0' and len(datavec_2[j]) == 9 and datavec_1[i][0] != '0' and diffcont(datavec_2[j][0],datavec_1) != 1: # and [dig4.isascii() for dig4 in datavec_2[j]]:
                #     foo = [[datavec_2[j][0]]]
                #     foo[0].extend(['nan','nan','nan','nan','nan','nan','nan','nan'])
                #     foo[0].extend(datavec_2[j][1:])
                #     print(j)
                #     datavec.append(foo)
                #     flag=1

                    
        # Now we have to go back to strings!
        print(datavec)
        datavec_sorted = sorted(datavec, key=itemgetter(0))
        try:
            last_timestamp = datavec_sorted[-1][0][0]
        except:
            pass
                
        #pruning buffer
        datavec_1_buffer = [x for x in datavec_1_buffer if x != [] and float(x[0]) >= float(last_timestamp)]
        datavec_2_buffer = [x for x in datavec_2_buffer if x != [] and float(x[0]) >= float(last_timestamp)]
        
        joint_data1 = [";".join(datavec_sorted[i][0]) for i in range(len(datavec_sorted))]
        joint_data2 = "\n".join(joint_data1)
        joint_data2 = joint_data2 + "\n"
        end = timer()
        print(start-end)
    # else:
    #     joint_data2 = []
        return joint_data2


    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: command_loop                                                                                           #
    #                                                                                                                       #
    #   Description:                                                                                                        #
    #                                                                                                                       #
    #   Input parameters:   command     ->                                                                                  #
    #                       connection  -> Socket object that data will be received from                                    #
    #   Output parameters:  Returns the message in byte format                                                              #
    #                                                                                                                       #
    #########################################################################################################################  

    """
    probably unsused
    """
    def command_loop(command, connection):
        
        while True:
            sendD(connection, command)
            print('enviei um pause')
            time.sleep(0.5)
            
            if(q.empty()):
                pass
            else:
                print(q.get())
                break
        
        return False
            
    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: receive                                                                                                #
    #                                                                                                                       #
    #   Description: This method is used to receive text data from a socket connection, without decoding from bytes to text #
    #                                                                                                                       #
    #   Input parameters:   conn  -> Socket object that data will be received from                                          #
    #                                                                                                                       #
    #   Output parameters:  Returns the message in byte format                                                              #
    #                                                                                                                       #
    ######################################################################################################################### 
    def receive(conn):
        try:
            # '-xxxxx-
            
            message_len = int(conn.recv(5).decode())   #Receiving first 5 bytes (incoming message length)
            return conn.recv(message_len)              #Returning the incoming message

        except socket.error as e:
            print(e)
            pass


    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: sendD                                                                                                  #
    #                                                                                                                       #
    #   Description: This method is used to send text data over a socket connection, enconding it into bytes and            #
    #                   marking the first 5 bytes with the message length.                                                  #
    #                                                                                                                       #
    #   Input parameters:   conn  -> Socket object that data will be sent to                                                #
    #                       data  -> String that will be sent over the connection                                           #
    #                                                                                                                       #
    #   Output parameters:  n/a                                                                                             #
    #                                                                                                                       #
    #########################################################################################################################   
    def sendD(conn, data):
        try:            
            data_b = str.encode(data)
            data_len = str(len(data_b))
            
            code = str.encode(data_len.rjust(5, '0'))   #Making a 5 byte sized string with '0's
            message = code + data_b                     #Concatenating length and message
            conn.sendall(message)                       #Sending resulting message
            
        except socket.error as e:
            print(e)
            pass
        
        
    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: threaded_client_handle                                                                                 #
    #                                                                                                                       #
    #   Description: Threaded function to handle the Master ESP <-> server connection. This method runs a loop that keeps   #
    #                   receiving the ESP data and logging it into a .csv file. Also handle pauses and other commands.      #
    #                                                                                                                       #
    #   Input parameters:   connection  -> Master ESP connection socket object                                              #
    #                       address     -> Connection address object                                                        #
    #                       file_path   -> Logging file directory path                                                      #
    #                       conn_dict   -> Socket connections dictionary                                                    #
    #                                                                                                                       #
    #   Output parameters:  n/a                                                                                             #
    #                                                                                                                       #
    #########################################################################################################################   
    def threaded_client_handle(connection, address, file_path, file_path_raw, conn_dict):
        global GIOZAO
        with connection:
                print('Connected by', address)
                print('ESP is waiting for a start signal\n')
                start = False
                global recv_counter
                buff = 0
                
                while start == False:
                    # Receving incoming data from client
                    inc_data = receive(connection)
                    print(inc_data.decode())
                    
                    if inc_data.decode() == 'ready2go':
                        start  = True
                        
                    else:
                        pass
                
                while True:
                    # Receving incoming data from client
                    try:
                        inc_data = receive(connection)
                        # print(inc_data)
                        # print(inc_data.decode('utf-8'))
                    
                    except:
                    # print('perda de pacote')
                        inc_data = 'perda'
                        pass
                    
                    recv_counter = recv_counter + 1
                    
                    """
                    Esse loop tem que estar sempre lendo o buffer
                    
                    Recebe dado     ->    log no arquivo
                    Recebe pause    ->    sinalizar que foi pausado
                    
                    """
                    
                    # If nothing is being sent by the client, break the connection
                    if not inc_data:
                        break
                    
                    
                    if inc_data == 'perda':
                        pass
                    
                    # If paused status is being sent, do nothing                 
                    # elif inc_data.decode('utf-8') == 'paused':
                    #     print('pausado')
                        
                    
                    
                    else:
                        # try:
                        #     prevec = inc_data.decode('utf-8','ignore') #NN;float;int;...
                        #     GIOZAO += 1
                        #     print(GIOZAO)

                        # except:
                        #     pass



                        # if buff<1:
                        #     buff+=1
                        #     with open(file_path_buffer, "a+") as f:
                        #         data_b = organizedata(prevec)
                        #         f.write(data_b)


                        # else:
                        #     m=1
                        #     buff = 0
                            
                        #     with open(file_path_buffer, "a+") as f:
                        #         data_b = organizedata(prevec)
                        #         f.write(data_b)
                            # m=1
                            # readbuffer = open(file_path_buffer,'r')
                            # buffdata = []
                            
                            # while m == 1:
                            #     bufaux1 = readbuffer.readline()
                            #     if bufaux1 == '':
                            #         m=0
                            #     else:
                            #        buffdata.append(bufaux1[0:-1])
                                    
                        #     if not hasattr(organizedata2, "last_timestamp_index"):
                        #         organizedata2.last_timestamp = 0
                                
                        #     data = organizedata2(buffdata,organizedata2.last_timestamp)
                            
                        #     with open(file_path_buffer, "r+") as f:
                        #         lines = f.readlines()
                        #         lines = lines[round(len(lines)/2):-1]
                        #         f.seek(0)
                        #         f.truncate(0)
                        #         for line in lines:
                        #             f.write(line)

                        #     with open(file_path, "a+") as b:
                        #         b.write(data)   # Cliente;Tempo;Dado;Dado;Dado;Dado
                        # #                                           # Queriamos que fosse Tempo;Dado.... , mas
                        # #                                           # pra isso precisa estar na ordem de canais.
                                
                                
                        # try:
                        prevec = inc_data.decode('utf-8','ignore') #NN;float;int;...                        
                    
                        with open(file_path_raw, "a+") as b:
                            b.write(prevec)   # This saves a raw version of the data.
                        global organize_flag
                        if organize_flag == 0:
                            if file_len(file_path_raw) > 100:
                                organize_flag = 1
                        
                        
                        if organize_flag == 1:
                            print('ANTES\n' + str(prevec))
                            data = organizedata(file_path_raw)
                            print("DEPOIS\n" + str(data))
                            with open(file_path, "a+") as f:
                                f.write(data)   # Cliente;Tempo;Dado;Dado;Dado;Dado
                    #                                           # Queriamos que fosse Tempo;Dado.... , mas
                    #                                           # pra isso precisa estar na ordem de canais.

                        # except:
                        #     print("erro no try")
                        #     pass
                        
                        
        
                        
                        
                            
                            
                conn_dict.pop("ESP")
                        

    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: threaded_interface_handle                                                                              #
    #                                                                                                                       #
    #   Description: Threaded function to handle the interface <-> server <-> ESP connection. This method runs a loop that  #
    #                   keeps receiving the GUI status and user commands, sending those to the Master ESP                   #
    #                                                                                                                       #
    #   Input parameters:   connection1 -> GUI connection socket object                                                     #
    #                       connection2 -> Master ESP connection socket object                                              #
    #                       address     -> Connection address objection                                                     #
    #                       conn_dict   -> Socket connections dictionary                                                    #
    #                                                                                                                       #
    #   Output parameters:  n/a                                                                                             #
    #                                                                                                                       #
    #########################################################################################################################
    def threaded_interface_handle(connection1, connection2, address, conn_dict):
        with connection1:
            print('Connected by ', address)
            global recv_counter
            
            conn_flag   = True
            acq_flag    = False
            
            while True:
                try:
                    
                    data    = receive(connection1)     #Receiving interface request status
                    data_s  = data.decode()
                    if data_s != "keepgoing":
                        print(data_s[0:4])
                        BOSTA = data_s[0:4]
                        if data_s[0:4] == "gain":
                            print("JESUS QUE LIXO FODEU")
                        if BOSTA == "gain":
                            print("UFA MENOS 1 ERRO")
                    
                except:
                    pass
                
                if data_s == 'check':                   #Check ESP Connection
                    try:
                        if 'ESP' in conn_dict:
                            sendD(connection1, 'ok')
                        else:
                            sendD(connection1, 'notok')
                    except:
                        pass
                
                #%% TEST THIS
                while conn_flag == False:               #Waiting for ESP reconnection
                    if 'ESP' in conn_dict:
                        conn_flag = True
                        connection2 = conn_dict["ESP"]
                        recv_counter = 0
                        data_s = 'start'             
                    else:
                        pass
                #%%    
                
                if data_s == 'start':                #Command to start aquisition
                    try:
                        cmd_q.put((1, 'gogogo'))
                        acq_flag = True
                    except:
                        pass
                                    
                elif data_s == 'stop':                 #Command to pause aquisition              
                    try:
                        cmd_q.put((1, 'pause')) 
                        print('foi pausado')
                    except:
                        pass

                elif data_s[0:4] == 'gain':
                    try:
                        cmd_q.put((1, data_s))
                        print('Mudanca de ganho')
                    except:
                        pass
                    
                else:                               #Anything else -> keep doing the same  
                    if(cmd_q.qsize() == 0):         #If the command queue is empty, put the 
                        try:
                            cmd_q.put((3, 'keepgoing'))
                        except:
                            pass
                    else:
                        pass
                    
                if 'ESP' in conn_dict:                               
                    if (recv_counter == 10) or (acq_flag == True):
                        try:
                            sendD(connection2, cmd_q.get()[1])   
                            recv_counter = 0
                            acq_flag = False
                            # print('sent something')
                        except:
                            pass
                else:
                    print('ESP disconnected. Waiting for reconnection')
                    conn_flag = False
                    pass
                
                
                
    #########################################################################################################################
    #                                                                                                                       #
    #   Method Name: main                                                                                                   #
    #                                                                                                                       #
    #   Description: This function will start a socket server to handle connections between Master ESP Client and           #
    #                   our Tkinter GUI, logging all the received data. It runs on an infinite loop.                        #
    #                                                                                                                       #
    #   Input parameters:   n/a                                                                                             #
    #                                                                                                                       #
    #   Output parameters:  n/a                                                                                             #
    #                                                                                                                       #
    #########################################################################################################################
    #%% Main function starts here
        
    def test(console_signal):
            
        # HOST = '127.0.0.1'  # Standard LocalHost Address
        # HOST = '192.168.4.2'  # Routed WiFi (ESP)
        
        # HOST = '192.168.43.177'  # Routed WiFi (Giacomo/Gio)
        HOST = '192.168.0.13'
        # HOST = socket.gethostbyname(socket.gethostname())   #this might be a general solution to any WiFi network

        
        PORT = 25565        # Port to listen on (non-privileged ports are > 1023)
        
        string_T = time.ctime()                     #   Creating file name
        string_T = string_T.replace(" ", "_")       #based on server starting 
        string_T = string_T.replace(":", "-")       #date and current time
        
        path = r'C:\Users\GiacomoAD\Desktop\PythonEnv\GUI\Logs' + "\\"
        file_path = path + string_T + r'.csv'
        # file_path_buffer = path + string_T + "buffer" + r'.csv'
        file_path_raw = path + string_T + r'raw.csv'
        threads_alive = 0
        
        with open(file_path_raw, "a+") as f:
            f.write('0.0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0\n')
            pass
        
        with open(file_path, "a+") as f:        #Creating a new file
            f.write('0.0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0\n')            #Inserting a starting point for each channel/wavelength (prevents interface error)
            pass
        
    ##%% SERVER CREATION AND CONNECTION SECTION

        #   Statement to ensure this will run only once and will not create infinite
        # instances recursively (Python error)
        #if __name__ == 'test':

        # Creating a socket object
        # AF_INET     -> IPv4 address
        # SOCK_STREAM -> TCP Connection
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        connections = {}    #Connections made to the server
        
        # Binding socket object to a specific host (IP) and network port
        try:
            s.bind((HOST, PORT))
        except socket.error as e:
            print(e)
        
        
        # Starting the server by listening to incoming connections
        s.listen()
        console_signal.emit('Server started on: ' + HOST + ' ' + str(PORT))
        #print('------SERVER STARTED------')
    
        
        
        while True:
            # When connection is found, store its socket object (conn) and address
            #window.ui.plainTextEdit_2.setPlainText(window.ui.plainTextEdit_2.toPlainText() + '\n' + "Listening to connections...")
            console_signal.emit('Listening to Connections...')
            #print('Listening to connections...\n')
            try:
                conn, addr = s.accept()
            except (KeyboardInterrupt, SystemExit, socket.error):
                s.close()
                window.ui.plainTextEdit_2.setPlainText("SERVER CLOSED")
                #print('Server Closed')
            
            #Trying to find suitable connection handler based on client authentication
            try:
                handler_type = choose_handler(conn, addr) 

            except:
                print('Could not find any suitable handler')     


                #   Statement to ensure this will run only once and will not create infinite
                # instances recursively (Python error)
                #if __name__ == '__main__':
                    
                if handler_type == 1:       #Handler == 1 -> ESP HANDLER
                    
                    esp_connection = conn
                    
                    try:
                    #Trying to start the main client handling thread
                        client_thread = Thread(target=threaded_client_handle, args=(conn, addr, file_path, file_path_raw, connections,))
                        client_thread.daemon = True
                        client_thread.start()
                        threads_alive += 1
                        
                        connections["ESP"] = conn    #Adding ESP key and socket to connections dictionary
                        
                        print('-----------> Main Client Thread Started')
                    
                    except:
                        print('ESP Thread did not start.')
                        pass
                    
                elif handler_type == 2:     #Handler == 2 -> INTERFACE HANDLER
                    
                    interface_connection = conn
                                    
                    #Checking if ESP key is in dictionary, meaning ESP Client is connected to the server
                    if "ESP" in connections:
                    
                        #If so, tells interface everything is ok and starts interface handler 
                        sendD(conn, "OK!")
                        
                        try:
                            interface_thread = Thread(target=threaded_interface_handle, args=(interface_connection, esp_connection, addr, connections,))
                            interface_thread.daemon = True
                            interface_thread.start()
                            threads_alive += 1
                            connections["INTERFACE"] = conn     #Adding ESP key and socket to connections dictionary
                            print('-----------> Interface Client Thread Started')
                        
                    
                        except:
                            print('INTERFACE Thread did not start.')
                            pass
                    
                    #If ESP is not connected, sends back error info back to the interface and closes connection
                    else:
                        sendD(conn, "ERROR!")
                        print('Interface Connection failed. Closing Connection.\n')
                        conn.close()
                    
                    
                else:
                    conn.close()
                    
                
            s.close()
            print('Server closed')
        
            return

    def startServer(self, console_signal):
        server_thread = Thread(target=self.test, args=(console_signal,))
        server_thread.daemon = True
        server_thread.start()
    #main()