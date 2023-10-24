# << Digial signal processing for oscilloscope GUI >>
#
# Interface to the edge device (STM32L4 w/ my original MEMS mic Arduino shield)
#
# Follow the definition in the include files below:
# https://github.com/araobp/acoustic-event-detection/tree/master/stm32/Inc

import serial
import numpy as np
import traceback
import threading

### Constants #####

Fs = 80000000.0/64.0/64.0  # Sampling frequency
Nyq = Fs/2.0                # Nyquist frequency
BAUD_RATE = 460800          # UART baud rate
NN = 512                    # The number of samples per frame

# Command
RAW_WAVE = b'r'
SFFT = b's'
FEATURES = b'f'
FILTERBANK = b'F'
ELAPSED_TIME = b't'
ENABLE_PRE_EMPHASIS = b'p'
DISABLE_PRE_EMPHASIS = b'P'
PCM_RIGHT_BIT_SHIFT = b'8'
DISABLE_PCM_RIGHT_BIT_SHIFT = b'0'
TX_ON = b'o'
TX_OFF = b'O'

# Internal commands
SPECTROGRAM = b'97'
MFSC = b'98'
MFCC = b'99'

###################

b16_to_int = lambda msb, lsb, signed: int.from_bytes([msb, lsb], byteorder='big', signed=signed)
#b8_to_int = lambda d, signed: int.from_bytes([d], byteorder='big', signed=signed)

# Interface class
class Interface:
    
    def __init__(self, port, dataset):
        # Serial interface
        self.port = port
        self.filters = dataset.filters
        self.samples = dataset.samples
        self.lock = threading.Lock()
        self.active = False
        if self.port:
            try:
                ser = serial.Serial(self.port, BAUD_RATE)
                ser.close()
                self.active = True
            except:
                print('*** Cannot open {}!'.format(port))

        # main.c
        self.num_samples = {}            # The number of samples to receive from the device
        self.num_samples[RAW_WAVE] = NN
        self.num_samples[SFFT] = int(NN/2)
        self.num_samples[SPECTROGRAM] = int(NN/2) * self.samples
        self.num_samples[FEATURES] = self.filters * 2 * self.samples

        # Shapes
        self.shape = {}
        self.shape[SPECTROGRAM] = (self.samples, int(NN/2))
        self.shape[FEATURES] = (self.samples, self.filters * 2)

        # Capture memory
        self.spec = np.zeros([self.samples * int(NN/2)])
        self.features = np.zeros([2, self.samples * int(NN/2)])

    def is_active(self):
        return self.active

    def serial_port(self):
        return serial.Serial(self.port, BAUD_RATE, timeout=3)
    
    def set_continouse(self, cont):
        self.cont = cont

    def read(self, cmd):
        '''
        As an application processor, send a command
        then receive and process the output.
        '''        
        data = []
        try:
            ser = self.serial_port()
            if cmd == SPECTROGRAM:
                ser.write(SFFT)
            else:
                ser.write(cmd)

            ser.write(TX_ON)

            if cmd == RAW_WAVE:  # 16bit quantization
                rx = ser.read(self.num_samples[RAW_WAVE]*2)
                rx = zip(rx[0::2], rx[1::2])
                for msb, lsb in rx:
                    d = b16_to_int(msb, lsb, True)
                    data.append(d)
                data = np.array(data, dtype=np.int16)
            elif cmd == SFFT:
                rx = ser.read(self.num_samples[SFFT])
                data = np.frombuffer(rx, dtype=np.int8)
            elif cmd == SPECTROGRAM:
                rx = ser.read(self.num_samples[SPECTROGRAM])
                data = np.frombuffer(rx, dtype=np.int8)
                data = data.reshape(self.shape[cmd])
            elif cmd == FEATURES:
                rx = ser.read(self.num_samples[FEATURES])
                data = np.frombuffer(rx, dtype=np.int8)
                data = data.reshape(self.shape[cmd])
            elif cmd == FILTERBANK:
                filterbank = []
                k_range = []
                while True:
                    rx = ser.readline().decode('ascii').rstrip('\n,')
                    if rx == 'e':
                        break
                    temp = rx.split(',')
                    k_range.append(np.array(temp[0].split(':'), dtype=int))
                    filterbank.append(np.array(temp[1:], dtype=float))
                data = (k_range, filterbank)
            elif cmd == ELAPSED_TIME:
                data = ser.readline().decode('ascii').rstrip('\n,')
                print(data)

            ser.write(TX_OFF)
            ser.close()
        except:
            print('*** serial timeout!')
            traceback.print_exc()

        return data

    def enable_pre_emphasis(self, enable):
        '''
        Enable/disable pre-emphasis.
        '''
        ser = self.serial_port()
        if enable:
            ser.write(ENABLE_PRE_EMPHASIS)
        else:
            ser.write(DISABLE_PRE_EMPHASIS)
        ser.close()
    
    def enable_eightbit_shift(self, enable):
        '''
        Enable/disable eight bit shift on PCM data to avoid overflow.
        '''
        ser = self.serial_port()
        if enable:
            ser.write(PCM_RIGHT_BIT_SHIFT)
        else:
            ser.write(DISABLE_PCM_RIGHT_BIT_SHIFT)
        ser.close()

