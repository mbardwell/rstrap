# Data recording code for rStrap
#
# Written by M. Bardwell, 2018

def recordValues(length):
    """
    opens 
    :type length: int.
    :rtype values_array: int array
    """
    import serial
    
    ser = serial.Serial(
        port='COM11',\
        baudrate=115200,\
        parity=serial.PARITY_NONE,\
        stopbits=serial.STOPBITS_ONE,\
        bytesize=serial.EIGHTBITS,\
            timeout=1)
    
    values = ""
    values_array = []
    no_failures = 0
    filename = 'arthur'
    with open(filename + ".txt", "w") as file:
        for i in range(length * 38): # each line is roughly 38 char
            for line in ser.read():
                if chr(line) == '\n':
                    values = values.strip('<info> app: ADC measuremement ')
                    if values.find('dout error') == -1:
                        file.write(values)
                        values_array.append(int(values))
                    else:
                        no_failures += 1
                    values = ""
                else:
                    values += chr(line)
        file.close()
    ser.close()
    print(filename, '\n',
          "number of dout errors: {}.\nValues: {}".format(no_failures, 
                                                          values_array))
    return no_failures, values_array

def analysis(array):
    import numpy as np
    array = np.array(array)
    mean = array.mean()
    std = array.std()
    print("mean: {} std: {}".format(mean, std))
    
no_failures, values = recordValues(50)
analysis(values)