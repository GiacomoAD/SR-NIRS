
import time
import numpy as np
import matplotlib.animation as FuncAnimation

global last_line
last_line = 0


NUMBER_OF_CHANNELS = 3

def resetFlag(flag):
        flag[0] = True
        pass

def readFromFile(x, y, threadFlag, window):
    global last_line
    #while True:
    while threadFlag[0]:


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
                if len(splitData) != 7:
                    splitData = [np.nan, np.nan, np.nan, np.nan, np.nan, np.nan, np.nan]

                try:
                    splitData = [float(j) for j in splitData]
                except:
                    splitData = [np.nan for j in splitData]
                    continue

                #x.append(splitData[0])
                window.xnp = np.append(window.xnp, splitData[0])        #Time 
                window.ynp[0] = np.append(window.ynp[0], splitData[1])  #Ch1 - IR
                window.ynp[1] = np.append(window.ynp[1], splitData[2])  #Ch1 - R
                window.ynp[2] = np.append(window.ynp[2], splitData[3])  #Ch2 - IR
                window.ynp[3] = np.append(window.ynp[3], splitData[4])  #Ch2 - R
                window.ynp[4] = np.append(window.ynp[4], splitData[5])  #Ch3 - IR
                window.ynp[5] = np.append(window.ynp[5], splitData[6])  #Ch3 - R

        else:
            #print('Agora eh FALSA')
            threadFlag[0] = False

########################################################################
## PLOTTING ANIMATION FUNCTIONS
############################## ---/--/--- ##############################


def gen_animate(window):
    i = 0
    j = 0


    cte = 100   # Number of data points per iteration
    n = 1;

    plot_time_buffer = np.asarray([])
    plot_data_buffer = [np.asarray([]) for i in range(2*NUMBER_OF_CHANNELS)]

    autoscale_x_axis = False
    t_min = 0
    t_max = 5
    window.canvas1.ax1.set_xlim([t_min, t_max])

    autoscale_mult = []
    for i in range(2*NUMBER_OF_CHANNELS):
        autoscale_mult.append([0])

    currMax = 0
    currMin = 0

    maxAtt = 0
    minAtt = 0

    autoscale_y_axis = False
    new_points_flag = False

    while True:
        
        #Every iteration 
        try:
            if len(window.xnp) > cte*n:
                n += 1

                plot_time_buffer = np.append(plot_time_buffer, window.xnp[j:j+cte])

                for i in range(NUMBER_OF_CHANNELS):
                    plot_data_buffer[2*i] = np.append(plot_data_buffer[2*i], window.ynp[2*i][j:j+cte])
                    plot_data_buffer[2*i+1] = np.append(plot_data_buffer[2*i+1], window.ynp[2*i+1][j:j+cte])
                    

                j = len(plot_time_buffer)
                new_points_flag = True

                if len(autoscale_mult[0]) < 100:
                    autoscale_y_axis = True

                    for ch in range(2*NUMBER_OF_CHANNELS):
                        autoscale_mult[ch].extend(plot_data_buffer[ch][-10:])
                    


            else:
                pass

        except Exception as e:
            print('Falhou em ler os dados')
            #print(e)
            new_points_flag = False
            autoscale_x_axis = False
            yield [plot_time_buffer, plot_data_buffer]
            pass
        
        try:
            if autoscale_x_axis == False and plot_time_buffer[-1] >= 5:
                autoscale_x_axis = True
        except Exception as e:
            #print(e)
            pass

        #print(plot_time_buffer + plot_data_buffer)
        yield [plot_time_buffer, plot_data_buffer]

        # AUTOSCALING Y-AXIS
        try:
            if autoscale_y_axis == True and new_points_flag == True:
                new_points_flag = False

                firstChFlag = True

                for ch in range(2*NUMBER_OF_CHANNELS):
                    del autoscale_mult[ch][0:25]
                    autoscale_mult[ch].extend(plot_data_buffer[ch][-25:])

                for i in range(NUMBER_OF_CHANNELS):
                    if window.activeChannels[i] == 1:
                        if firstChFlag:
                            currMax = np.amax(autoscale_mult[2*i])
                            currMin = np.amin(autoscale_mult[2*i+1])
                            maxAtt = currMax
                            minAtt = currMin
                            firstChFlag = False

                        else:
                            currMax = np.amax(autoscale_mult[2*i])
                            currMin = np.amin(autoscale_mult[2*i+1])

                        if currMax >= maxAtt:
                            maxAtt = currMax

                        if currMin <= minAtt:
                            minAtt = currMin
                
                window.canvas1.ax1.set_ylim([minAtt, maxAtt])

        except Exception as e:
            #print(e)
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

        except Exception as e:
            #print(e)
            pass


# NUMERO DE PONTOS NA TELA
# T = 1/f = 0.05s
# Eixo X tem 5s => 5/0.05 = 100 ptos na tela

#    Updating plot function
def animate_plot(value, ln, window):
    # This function takes the value yieldded by gen_animate and appends it to the lines being plotted.
    # value is the structured (time, Internsity) data.

    r_gain = float(window.ui.label_14.text())
    ir_gain = float(window.ui.label_13.text())

    #ln[0][0].set_data(value[0], ch1IR*ir_gain)
    #ln[1][0].set_data(value[0], ch1R*r_gain)

    for i in range(len(window.activeChannels)):
        if window.activeChannels[i] == 1:
            ln[2*i][0].set_data(value[0], value[1][2*i])
            ln[2*i+1][0].set_data(value[0], value[1][2*i+1])
            
        else:
            pass

    return ln,



def startPlot(window):
    ani_interval = 200 # Animation frames interval (in milliseconds)
    ani = FuncAnimation.FuncAnimation(window.canvas1.fig1, animate_plot, frames=gen_animate(window), fargs=(window.ln, window,), blit=False, interval = ani_interval)
    return ani

    