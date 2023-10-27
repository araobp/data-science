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
import time

### Constants #####

Fs = 80000000.0/64.0/64.0  # Sampling frequency
Nyq = Fs/2.0                # Nyquist frequency
BAUD_RATE = 460800          # UART baud rate
NN = 512                    # The number of samples per frame

INTERVAL = 24

# Command
RAW_WAVE = b'r'
SFFT = b's'
FEATURES = b'f'
FILTERBANK = b'F'
ELAPSED_TIME = b't'
ENABLE_PRE_EMPHASIS = b'p'
DISABLE_PRE_EMPHASIS = b'P'
TX_ON = b'o'
TX_OFF = b'O'

# Internal commands
REC = b'96'
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
        self.num_samples[SPECTROGRAM] = int(NN/2) * INTERVAL
        self.num_samples[FEATURES] = self.filters * 2 * INTERVAL

        # Shapes
        self.shape = {}
        self.shape[SPECTROGRAM] = (INTERVAL, int(NN/2))
        self.shape[FEATURES] = (INTERVAL, self.filters * 2)

        # Capture memory
        self.spec = np.zeros([self.samples * int(NN/2)])
        self.features = np.zeros([2, self.samples * int(NN/2)])

        # Serial port
        self.ser = None

    def is_active(self):
        return self.active

    def tx_on(self):
        if self.ser is None:
            self.ser = serial.Serial(self.port, BAUD_RATE, timeout=3)
            self.ser.write(TX_ON)

    def tx_suspend(self):
        if self.ser is not None:
            self.ser.write(TX_OFF)
            self.ser.reset_input_buffer()
            self.ser = None

    def tx_off(self):
        if self.ser is not None:
            self.ser.write(TX_OFF)
            self.ser.close()
            self.ser = None

    def read(self, cmd, num_repeat = None):
        '''
        As an application processor, send a command
        then receive and process the output.
        '''        

        if self.ser is not None:

            data = []
            
            try:
                if cmd == SPECTROGRAM:
                    self.ser.write(SFFT)
                elif cmd == REC:
                    self.ser.write(RAW_WAVE)
                else:
                    self.ser.write(cmd)
                
                if cmd == REC:  # Record PCM streaming, 16 bit quantization
                    rx = self.ser.read(self.num_samples[RAW_WAVE]*2*num_repeat)
                    rx = zip(rx[0::2], rx[1::2])
                    for msb, lsb in rx:
                        d = b16_to_int(msb, lsb, True)
                        data.append(d)
                    data = np.array(data, dtype=np.int16)
                elif cmd == RAW_WAVE:  # 16bit quantization
                    rx = self.ser.read(self.num_samples[RAW_WAVE]*2)
                    rx = zip(rx[0::2], rx[1::2])
                    for msb, lsb in rx:
                        d = b16_to_int(msb, lsb, True)
                        data.append(d)
                    data = np.array(data, dtype=np.int16)
                elif cmd == SFFT:
                    rx = self.ser.read(self.num_samples[SFFT])
                    data = np.frombuffer(rx, dtype=np.int8)
                elif cmd == SPECTROGRAM:
                    rx = self.ser.read(self.num_samples[SPECTROGRAM])
                    data = np.frombuffer(rx, dtype=np.int8)
                    data = data.reshape(self.shape[cmd])
                elif cmd == FEATURES:
                    rx = self.ser.read(self.num_samples[FEATURES])
                    data = np.frombuffer(rx, dtype=np.int8)
                    data = data.reshape(self.shape[cmd])
                elif cmd == FILTERBANK:
                    filterbank = []
                    k_range = []
                    while True:
                        rx = self.ser.readline().decode('ascii').rstrip('\n,')
                        if rx == 'e':
                            break
                        temp = rx.split(',')
                        k_range.append(np.array(temp[0].split(':'), dtype=int))
                        filterbank.append(np.array(temp[1:], dtype=float))
                    data = (k_range, filterbank)
                elif cmd == ELAPSED_TIME:
                    data = self.ser.readline().decode('ascii').rstrip('\n,')
                    print(data)

            except:
                print('*** serial timeout!')
                traceback.print_exc()
        else:
            data = None

        return data

    def enable_pre_emphasis(self, enable):
        '''
        Enable/disable pre-emphasis.
        '''
        if self.ser is None:
            self.ser = serial.Serial(self.port, BAUD_RATE, timeout=3)
            inactive = True
        else:
            inactive = False

        if enable:
            self.ser.write(ENABLE_PRE_EMPHASIS)
        else:
            self.ser.write(DISABLE_PRE_EMPHASIS)
        
        if inactive:
            self.ser.close()
            self.ser = None

    def right_bit_shift(self, bits):
        '''
        Enable/disable right bit shift on PCM data to avoid overflow.
        '''

        if self.ser is None:
            self.ser = serial.Serial(self.port, BAUD_RATE, timeout=3)
            inactive = True
        else:
            inactive = False

        self.ser.write(str(bits).encode())
        
        if inactive:
            self.ser.close()
            self.ser = None

