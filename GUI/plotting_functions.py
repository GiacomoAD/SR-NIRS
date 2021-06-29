
import time
import numpy as np
import matplotlib.animation as FuncAnimation

global last_line
last_line = 0

def resetFlag(flag):
        flag[0] = True
        pass

def readFromFile(file_path, x, y, threadFlag, window):
    global last_line
    #while True:
    while threadFlag[0]:
        #threadFlag[0] = False
        #print('Openning File')

        if last_line < len(window.x):

            data = window.x[last_line]
            last_line += 1

            if data == '':
                threadFlag[0] = False
                if threadFlag[0] == False:
                    #print('Stopping Thread')
                    break

            else:
                threadFlag[0] = True
                splitData = data.split(';')
                splitData[-1] = splitData[-1].replace('\n','')
                #print(splitData)
                if len(splitData) != 5:
                    splitData = [np.nan, np.nan, np.nan, np.nan, np.nan]

                try:
                    splitData = [float(j) for j in splitData]
                except:
                    splitData = [np.nan for j in splitData]
                    continue

                #x.append(splitData[0])
                window.xnp = np.append(window.xnp, splitData[0])
                window.ynp[0] = np.append(window.ynp[0], splitData[1])
                window.ynp[1] = np.append(window.ynp[1], splitData[2])
                window.ynp[2] = np.append(window.ynp[2], splitData[3])
                window.ynp[3] = np.append(window.ynp[3], splitData[4])

        else:
            #print('Agora eh FALSA')
            threadFlag[0] = False
        
        # with open(file_path, "r") as f:
        #     for i, line in enumerate (f):
        #         if i >= last_line:
        #             data = line
        #             last_line = i+1
        #             # print("Ultima linha" + str(last_line) + '\t' + str(data))
        #             # print(data)
        #             ## NOTHING TO READ
        #             if data == '':
        #                 threadFlag[0] = False
        #                 if threadFlag[0] == False:
        #                     #print('Stopping Thread')
        #                     break
                    
        #             ## READ A VALUE
        #             else:
        #                 threadFlag[0] = True
        #                 splitData = data.split(';')
                        
        #                 if len(splitData) != 3:
        #                     splitData = [np.nan, np.nan, np.nan]

        #                 try:
        #                     splitData = [float(j) for j in splitData]
        #                 except:
        #                     splitData = [np.nan for j in splitData]
        #                     continue

        #                 #x.append(splitData[0])
        #                 window.xnp = np.append(window.xnp, splitData[0])
        #                 window.ynp[0] = np.append(window.ynp[0], splitData[1])
        #                 window.ynp[1] = np.append(window.ynp[1], splitData[2])
        #                 #y[0][0].append(splitData[1])
        #                 #y[0][1].append(splitData[2])
                        
            
        # while threadFlag[0] == False:
        #     resetFlag(threadFlag)
        #     #print('Pausing 1s')
    
                    

########################################################################
## PLOTTING ANIMATION FUNCTIONS
############################## ---/--/--- ##############################

#plot_time_buffer = np.zeros(1)
#plot_data_buffer = [[],[]]

def gen_animate(window):
    i = 0
    j = 0


    cte = 100
    n = 1;

    plot_time_buffer = np.asarray([])
    plot_data_buffer = [np.asarray([]) for i in range(2)]

    autoscale_x_axis = False
    t_min = 0
    t_max = 5
    window.canvas1.ax1.set_xlim([t_min, t_max])

    autoscale_points = []
    autoscale_y_axis = False
    new_points_flag = False

    while True:
        
        #Every iteration 
        try:
            if len(window.xnp) > cte*n:
                n += 1
                #plot_time_buffer.extend(window.x[j:j+10])
                plot_time_buffer = np.append(plot_time_buffer, window.xnp[j:j+cte])
                #plot_data_buffer[0].extend(window.y[0][0][j:j+10])
                #plot_data_buffer[1].extend(window.y[0][1][j:j+10])
                plot_data_buffer[0] = np.append(plot_data_buffer[0], window.ynp[0][j:j+cte])
                plot_data_buffer[1] = np.append(plot_data_buffer[1], window.ynp[1][j:j+cte])


                j = len(plot_time_buffer)
                new_points_flag = True
                #print(plot_time_buffer)

                if len(autoscale_points) < 100:
                    autoscale_points.extend(plot_data_buffer[0][-10:])
                    autoscale_y_axis = True
                    #window.canvas1.ax1.set_ylim([np.amin(autoscale_points), np.amax(autoscale_points)])

            else:
                pass

        except:
            print('Falhou em ler os dados')
            new_points_flag = False
            autoscale_x_axis = False
            yield [plot_time_buffer, plot_data_buffer]
            pass
        
        try:
            if autoscale_x_axis == False and plot_time_buffer[-1] >= 5:
                autoscale_x_axis = True
        except:
            pass

        #print(plot_time_buffer + plot_data_buffer)
        yield [plot_time_buffer, plot_data_buffer]

        # AUTOSCALING Y-AXIS
        try:
            if autoscale_y_axis == True and new_points_flag == True:
                new_points_flag = False
                del autoscale_points[0:10]
                autoscale_points.extend(plot_data_buffer[0][-10:])
                window.canvas1.ax1.set_ylim([np.amin(autoscale_points), np.amax(autoscale_points)])
        except:
            pass

        # AUTOSCALING X-AXIS
        try:
            if autoscale_x_axis == True:
                if plot_time_buffer[-1] <= t_max:
                    pass
                else:
                    t_min += 0.5
                    t_max += 0.5
                    window.canvas1.ax1.set_xlim([t_min, t_max])
        except:
            pass


# NUMERO DE PONTOS NA TELA
# T = 1/f = 0.05s
# Eixo X tem 5s => 5/0.05 = 100 ptos na tela

#    Updating plot function
def animate_plot(value, ln, window):
    # This function takes the value yieldded by gen_animate and appends it to the lines being plotted.
    # value is the structured (time, Internsity) data.
    # print(value)

    #tempo = np.asarray(value[0])
    canal1_r = (value[1][0])
    canal1_ir = (value[1][1])

    r_gain = float(window.ui.label_14.text())
    ir_gain = float(window.ui.label_13.text())

    ln[0][0].set_data(value[0], canal1_r*r_gain)
    ln[1][0].set_data(value[0], canal1_ir*ir_gain)


    return ln,



def startPlot(window, file_path):
    ani_interval = 200 # Animation frames interval (in milliseconds)
    ani = FuncAnimation.FuncAnimation(window.canvas1.fig1, animate_plot, frames=gen_animate(window), fargs=(window.ln, window,), blit=False, interval = ani_interval)
    return ani

    