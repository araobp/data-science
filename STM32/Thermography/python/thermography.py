import matplotlib
matplotlib.use('TkAgg')

import interface

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('port', help='Serial port identifier')
args = parser.parse_args()

if __name__ == '__main__':
    
    itfc = interface.Interface(port = args.port)

    pixels = itfc.read(interface.PIXELS)
    temperature = itfc.read(interface.THERMISTER)

    print(pixels)
    print(temperature)

