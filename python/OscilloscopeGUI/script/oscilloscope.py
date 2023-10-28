# << Oscilloscope GUI >>
#
# This implementaion makes use of matplotlib on Tk for agile GUI development.
#
# Reference: https://matplotlib.org/2.1.0/gallery/user_interfaces/embedding_in_tk_sgskip.html
#

import matplotlib
matplotlib.use('TkAgg')

import numpy as np
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as Tk
import tkinter.messagebox as messagebox

from datetime import datetime
import os

import threading

import matplotlib.pyplot as plt

import intf
import plotter
import dataset

import time

VERSION = '2.0.0-alpha'

CMAP_LIST = ('viridis',
             'ocean',
             'hot',
             'gray',
             'magma',
             'cubehelix',
             'cool',
             'BrBG',
             'RdBu',
             'bwr',
             'coolwarm',
             'seismic')

# Command arguments
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-p", "--port", help="serial port identifier", default=None)
parser.add_argument("-D", "--debug",
                    help="debug output",
                    action="store_true")
parser.add_argument("-d", "--dataset_folder",
                    help="Data folder for saving feature data from the device",
                    default='.')
parser.add_argument("-b", "--browser",
                    help="Data browser", action="store_true")
parser.add_argument("-S", "--plot_style",
                    help="plot style", default='dark_background')
parser.add_argument("-o", "--oscilloscope_mode",
                    help="oscilloscope mode", action="store_true")
parser.add_argument("-f", "--fullscreen_mode",
                    help="fullscreen mode", default=None)
parser.add_argument("-c", "--color_map",
                    help="color map", default=','.join(CMAP_LIST))
parser.add_argument("-g", "--show_grid",
                    help="show grid", action="store_true")
parser.add_argument("-W", "--disable_window",
                    help="disable window", action="store_true")
parser.add_argument("-B", "--right_bit_shift", default=2, type=int, help="PCM right bit shift")
parser.add_argument("-r", "--record_time", default=2.0, type=float, help="recording time")
parser.add_argument("-R", "--record_folder", default=".", help="recording data folder")
args = parser.parse_args()

if __name__ == '__main__':

    dataset = dataset.DataSet(args.dataset_folder)
    class_file = args.dataset_folder + '/class_labels.yaml'

    plt.style.use(args.plot_style)

    itfc_instance = intf.Interface(port=args.port, dataset=dataset)
        
    ### Default settings to Interface ###
    # Note : pre-emphasis (HPF) is for data(FFT etc) other than PCM wave form.
    if itfc_instance.is_active():
        itfc_instance.enable_pre_emphasis(True)  # Pre emphasis enabled
        itfc_instance.right_bit_shift(args.right_bit_shift)
    ###############################

    PADX = 6
    PADX_GRID = 2
    PADY_GRID = 2
    WIDTH = 7
    BG = 'darkturquoise'
    
    cmap_list = args.color_map.split(',')

    cnt = 0

    filename = None
    data = None
    cnn_model = None
    last_operation = None

    async_active = False

    EMPTY = np.array([])

    if args.oscilloscope_mode or args.fullscreen_mode:
        show_capture_area = False
    else:
        show_capture_area = True

    plotter = plotter.Plotter(intf_instance=itfc_instance, dataset=dataset, show_capture_area=show_capture_area)

    if dataset.model and not args.browser:
        import inference
        cnn_model = inference.Model(dataset)

    root = Tk.Tk()
    if args.fullscreen_mode:
        root.wm_title("")
    else:
        root.wm_title("Oscilloscope {}".format(VERSION))

    if args.browser:
        fig, ax = plt.subplots(1, 1, figsize=(10, 4))
    else:
        fig, ax = plt.subplots(1, 1, figsize=(11, 4))        
    fig.subplots_adjust(bottom=0.15)
    
    frame = Tk.Frame(master=root)
    frame_row0 = Tk.Frame(master=frame)
    frame_row0a = Tk.Frame(master=frame_row0)
    frame_row0b = Tk.Frame(master=frame_row0, padx=PADX)
    frame_row1 = Tk.Frame(master=frame)
    frame_row2 = Tk.Frame(master=frame)
    frame_row3 = Tk.Frame(master=frame)
    frame_row4 = Tk.Frame(master=frame)
    
    canvas = FigureCanvasTkAgg(fig, master=frame_row0a)
    canvas.draw()

    # Save training data for deep learning
    def save():
        global cnt, filename
        class_label = entry_class_label.get()
        func, data, window, pos = last_operation
        dt = datetime.today().strftime('%Y%m%d%H%M%S')
        if args.dataset_folder:
            dataset_folder = args.dataset_folder
        else:
            dataset_folder = './data'

        if class_label == '':
            filename = dataset_folder+'/data/{}-{}'.format(dt, func.__name__)
        else:
            if func == mfsc or func == mfcc:  # f both data at a time
                filename = dataset_folder+'/data/{}-features-{}-{}'.format(dt, class_label, pos)
            else:
                filename = dataset_folder+'/data/{}-{}-{}'.format(dt, class_label, func.__name__)
            data = data.flatten()
            with open(filename+'.csv', "w") as f:
                f.write(','.join(data.astype(str)))

            cnt += 1
            counter.configure(text='({})'.format(str(cnt)))

    def infer(data):
        class_label, p = cnn_model.infer(data)
        label_inference.configure(text='This is {} ({} %)'.format(class_label, int(p)))

    def exec_func(func, interval = None):
        global async_active
        async_active = True
        def _exec_func():
            global async_active
            plotter.start_streaming()
            while async_active:
                func()
                if interval is not None:
                    plotter.suspend_streaming()
                    time.sleep(interval)
                    plotter.start_streaming()
            plotter.stop_streaming()

        threading.Thread(target=_exec_func).start()

    def exec_func2(func, button, label):
        global async_active
        async_active = True
        def _exec_func():
            global async_active
            plotter.start_streaming()
            func()
            plotter.stop_streaming()
            button.configure(bg=button_colors[label])
            async_active = False

        threading.Thread(target=_exec_func).start()

    def exec_async(button, label, func, interval=None):
        global async_active
        if button.cget('bg') == button_colors[label]:
            if not async_active:
                button.configure(bg='red')
                if ((interval is not None) and (interval < 0)):
                    exec_func2(func, button, label)
                else:
                    exec_func(func, interval)
        else:       
            button.configure(bg=button_colors[label])
            async_active = False
    
    def raw_wave():
        global async_active
        def _raw_wave():
            global last_operation
            range_ = int(range_amplitude.get())
            data = plotter.plot(ax, intf.RAW_WAVE, range_=range_, grid=args.show_grid)
            last_operation = (raw_wave, data, None, None)
            fig.tight_layout()
            canvas.draw()

        exec_async(button_waveform, 'waveform', _raw_wave, 0.4)

    def fft():
        global async_active
        def _fft():
            global last_operation
            ssub = int(spectrum_subtraction.get())
            data = plotter.plot(ax, intf.SFFT, grid=args.show_grid)
            last_operation = (fft, data, None, None)
            fig.tight_layout()
            canvas.draw()

        exec_async(button_psd, 'psd', _fft, 0.4)

    def spectrogram(data=EMPTY, pos=0):
        global async_active
        def _spectrogram(data=data, pos=pos):
            global last_operation, dataset
            ssub = int(spectrum_subtraction.get())    
            range_ = int(range_spectrogram.get())
            cmap_ = var_cmap.get()
            if data is EMPTY:
                window = dataset.windows[int(range_window.get())]
                data = plotter.plot(ax, intf.SPECTROGRAM, range_, cmap_, ssub, grid=args.show_grid)
            else:
                window = dataset.windows[pos]
                plotter.plot(ax, intf.SPECTROGRAM, range_, cmap_, ssub, data=data,
                            window=None)
            last_operation = (spectrogram, data, window, pos)
            fig.tight_layout()
            canvas.draw()

        exec_async(button_spectrogram, 'spectrogram', _spectrogram)

    def mfsc(data=EMPTY, pos=None):
        global async_active
        def _mfsc(data=data, pos=pos):
            global last_operation, dataset
            ssub = int(spectrum_subtraction.get())
            range_ = int(range_mfsc.get())
            cmap_ = var_cmap.get()
            if data is EMPTY:
                window = dataset.windows[int(range_window.get())]
                data = plotter.plot(ax, intf.MFSC, range_, cmap_, ssub,
                                window=window, grid=args.show_grid)
            else:
                if pos is not None and not args.disable_window:
                    window = dataset.windows[pos]
                else:
                    window = None
                plotter.plot(ax, intf.MFSC, range_, cmap_, ssub, data=data,
                            window=window)
            if cnn_model:
                print(window)
                a, b, c = window[0], window[1], window[2]
                infer(data[a:b,:c])
            last_operation = (mfsc, data, window, pos)
            fig.tight_layout()
            canvas.draw()
        
        if data is EMPTY:
            exec_async(button_mfsc, 'mfsc', _mfsc)
        else:
            _mfsc(data=data, pos=pos)


    def mfcc(data=EMPTY, pos=None):
        global async_active
        def _mfcc(data=data, pos=pos):
            global last_operation, dataset
            ssub = int(spectrum_subtraction.get())    
            range_ = int(range_mfcc.get())
            cmap_ = var_cmap.get()
            if data is EMPTY:
                window = dataset.windows[int(range_window.get())]
                data = plotter.plot(ax, intf.MFCC, range_, cmap_, ssub,
                                window=window, grid=args.show_grid)
            else:
                if pos is not None and not args.disable_window:
                    window = dataset.windows[pos]
                else:
                    window = None
                plotter.plot(ax, intf.MFCC, range_, cmap_, ssub, data=data,
                            window=window)
            # TODO: inference for MFCCs
            #if cnn_model:
            #    infer(data, pos)
            last_operation = (mfcc, data, window, pos)
            fig.tight_layout()
            canvas.draw()

        if data is EMPTY:
            exec_async(button_mfcc, 'mfcc', _mfcc)
        else:
            _mfcc(data=data, pos=pos)

    def welch():
        global async_active
        def _welch():
            plotter.plot_welch(ax, grid=args.show_grid)
            fig.tight_layout()
            canvas.draw()

        exec_async(button_welch, 'welch', _welch)

    def rec():
        global async_active
        def _rec():
            range_ = int(range_amplitude.get())
            plotter.plot_rec(args.record_folder, args.record_time, ax, range_=range_, grid=args.show_grid)
            fig.tight_layout()
            canvas.draw()

        exec_async(button_rec, 'rec', _rec, -1.0)

    def pre_emphasis_toggle():
        if button_pre_emphasis.cget('bg') == BG:
            button_pre_emphasis.configure(bg='red')
            itfc_instance.enable_pre_emphasis(True)
        else:       
            button_pre_emphasis.configure(bg=BG)
            itfc_instance.enable_pre_emphasis(False)
            
    def savefig():
        fig.savefig('screen_shot.png')

    def remove():
        global filename, cnt
        if filename:
            os.remove(filename+'.csv')
            cnt -= 1
            counter.configure(text='({})'.format(str(cnt)))

    def quit():
        global async_active
        if not async_active:
            root.quit()
            root.destroy()
        else:
            messagebox.showinfo("Warning", "Turn off the active capture button!")

    def confirm():
        global cnt
        class_label = entry_class_label.get()
        labels = dataset.count_class_labels()
        if class_label in labels.keys():
            cnt = labels[class_label]
        else:
            cnt = 0
        print(labels)
        counter.configure(text='({})'.format(str(cnt)))
        canvas._tkcanvas.focus_set()

    def capture_area(pos):
        last_operation[0](data=last_operation[1], pos=int(pos))

    def filterbank():
        plotter.plot(ax, intf.FILTERBANK)
        canvas.draw()

    def elapsed_time():
        plotter.plot(ax, intf.ELAPSED_TIME)

    ### Key press event ###

    def on_key_event(event):
        if not async_active:
            c = event.key
            pos = range_window.get()
            if c == 'right':
                if pos < len(dataset.windows) - 1:
                    pos += 1
                    range_window.set(pos)
                    capture_area(pos)
            elif c == 'left':
                if pos > 0:
                    pos -= 1
                    range_window.set(pos)
                    capture_area(pos)            
            elif c == 'up':
                if last_operation is None:
                    print('Up key becomes effective after executing an operation.')
                else:
                    func = last_operation[0]
                    if func in (mfsc, mfcc):
                        func(pos=int(range_window.get()))
                    else:
                        func()
            elif c == 'down':
                save()
            
    if not args.browser:
        canvas.mpl_connect('key_press_event', on_key_event)

    ### File select event ###
    def on_select(event):
        widget = event.widget
        index = int(widget. curselection()[0])
        filename = widget.get(index)
        params = filename.split('-')
        func = globals()[dataset.feature]

        with open(args.dataset_folder + '/data/' + filename) as f:
            data = np.array(f.read().split(','), dtype='float')
        
        if func == mfsc or func == mfcc:
            data = data.reshape(dataset.samples*2, dataset.filters)
            if func == mfsc:
                data = data[0:dataset.samples, :]
            elif func == mfcc:
                data = data[dataset.samples:dataset.samples*2, :]

            pos = params[3]
            if pos == 'a':
                func(data=data, pos=None)
            else:
                func(data=data, pos=int(pos))                
        else:
            data = data.reshape(dataset.samples, dataset.filters)
            func(data=data)
        
    ### Row 0b ####
    if args.browser:
        list_files = Tk.Listbox(master=frame_row0b, width=30, height=24)
        files = [f for f in os.listdir(args.dataset_folder+'/data')]
        for f in files:
            list_files.insert(Tk.END, f)
        list_files.bind('<<ListboxSelect>>', on_select)
    
        scrollbar = Tk.Scrollbar(master=frame_row0b, orient="vertical")
        scrollbar.config(command=list_files.yview)
        list_files.config(yscrollcommand=scrollbar.set)
    
    ### Row 1 ####
    entry_class_label = Tk.Entry(master=frame_row1, width=14)
    var_cmap = Tk.StringVar()
    var_cmap.set(cmap_list[0])
    cmap = Tk.OptionMenu(frame_row1, var_cmap, *cmap_list)
    cmap.config(bg=BG, activebackground='paleturquoise')
    counter = Tk.Label(master=frame_row1)
    counter.configure(text='({})'.format(str(0)))
    range_amplitude = Tk.Spinbox(master=frame_row1, width=6,
                                 values=[2**7, 2**9, 2**11, 2**13, 2**15])
    range_mfsc = Tk.Spinbox(master=frame_row1, width=3,
                                       values=[dataset.filters, int(dataset.filters * 0.8), int(dataset.filters * 0.6)])
    range_spectrogram = Tk.Spinbox(master=frame_row1, width=4,
                                   values=[int(intf.NN/2), int(intf.NN/2 * 0.7), int(intf.NN/2 * 0.4)])
    range_spectrogram.selection_to(2)
    range_mfcc = Tk.Spinbox(master=frame_row1, width=3,
                            values=[14, int(dataset.filters * 0.5), dataset.filters])
    spectrum_subtraction = Tk.Spinbox(master=frame_row1, width=3,
                                      values=[0, 5, 10, 15, 20, 25, 30])
    label_class = Tk.Label(master=frame_row1, text='Class label:')
    label_image = Tk.Label(master=frame_row1, text='Mask:')
    label_color = Tk.Label(master=frame_row1, text='Color:')

    button_rec = Tk.Button(master=frame_row1, text='Rec', command=rec,
                                bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_waveform = Tk.Button(master=frame_row1, text='Wave', command=raw_wave,
                                bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_psd = Tk.Button(master=frame_row1, text='FFT', command=fft,
                           bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_spectrogram = Tk.Button(master=frame_row1, text='Spec', command=spectrogram,
                                   bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_welch = Tk.Button(master=frame_row1, text='Welch', command=welch,
                            bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_mfsc = Tk.Button(master=frame_row1, text='MFSCs', command=mfsc,
                                       bg='pink', activebackground='grey', padx=PADX, width=WIDTH)
    button_mfcc = Tk.Button(master=frame_row1, text='MFCCs', command=mfcc,
                            bg='yellowgreen', activebackground='grey', padx=PADX, width=WIDTH)

    button_colors = {
        'rec': BG,
        'waveform': BG,
        'psd': BG,
        'spectrogram': BG,
        'welch': BG,
        'mfsc': 'pink',
        'mfcc': 'yellowgreen',
    }

    ### Row 2 ####
    button_pre_emphasis = Tk.Button(master=frame_row2, text='Emphasis', command=pre_emphasis_toggle,
                                    bg='red', activebackground='grey', padx=PADX, width=WIDTH)
    button_savefig = Tk.Button(master=frame_row2, text='Savefig', command=savefig,
                               bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_remove = Tk.Button(master=frame_row2, text='Remove', command=remove,
                              bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_save = Tk.Button(master=frame_row2, text='Save', command=save,
                              bg=BG, activebackground='grey', padx=PADX, width=WIDTH)
    button_quit = Tk.Button(master=frame_row2, text='Quit', command=quit,
                            bg='yellow', activebackground='grey', padx=PADX, width=WIDTH)
    button_confirm = Tk.Button(master=frame_row2, text='Confirm', command=confirm,
                            bg='khaki1', activebackground='grey', padx=PADX, width=WIDTH)

    ### Row 3 ####
    button_filterbank = Tk.Button(master=frame_row3, text='Filterbank', command=filterbank,
                                  bg=BG, activebackground='grey', padx=PADX)
    button_elapsed_time = Tk.Button(master=frame_row3, text='Elapsed time', command=elapsed_time,
                                    bg=BG, activebackground='grey', padx=PADX)

    ### Row 4 ####
    label_window = Tk.Label(master=frame_row4, text='Window:')
    range_window = Tk.Scale(master=frame_row4, orient=Tk.HORIZONTAL, length=180,
                            from_=0, to=len(dataset.windows)-1, tickinterval=1)
    if cnn_model:
        label_inference = Tk.Label(master=frame_row3, width=40, fg='DeepSkyBlue4', padx=PADX)
        label_inference.config(font=("Arial", 20))
    
    ##### Place the parts on Tk #####

    frame.pack(expand=True, fill=Tk.BOTH)

    ### Row 0: main canvas
    if args.browser:
        frame_row0a.grid(row=0, column=0)
        frame_row0b.grid(row=0, column=1)
        list_files.grid(row=0, column=0, padx=PADX_GRID)
        list_files.pack(side="left", expand=True, fill=Tk.BOTH)
        scrollbar.pack(side="right", expand=True, fill=Tk.BOTH)
    else:
        frame_row0a.pack(expand=True, fill=Tk.BOTH)
    frame_row0.pack(expand=True, fill=Tk.BOTH)
    canvas._tkcanvas.pack(expand=True, fill=Tk.BOTH)

    if args.fullscreen_mode:
        func = globals()[args.fullscreen_mode]
        if func in (raw_wave, fft, spectrogram, mfsc, mfcc):
            func()

    else:

        ### Row 1: operation ####

        frame_row1.pack(pady=PADY_GRID)

        if not cnn_model:

            if not args.oscilloscope_mode:
                # Class label entry
                label_class.grid(row=0, column=0, padx=PADX_GRID)
                entry_class_label.grid(row=0, column=1, padx=PADX_GRID)
                counter.grid(row=0, column=2, padx=PADX_GRID)
            # Rec
            button_rec.grid(row=0, column=3, padx=PADX_GRID)

            # Waveform
            range_amplitude.grid(row=0, column=4, padx=PADX_GRID)
            button_waveform.grid(row=0, column=5, padx=PADX_GRID)

            # FFT (PSD)
            button_psd.grid(row=0, column=6, padx=PADX_GRID)

            # Linear-scale Spectrogram (PSD)
            range_spectrogram.grid(row=0, column=7, padx=PADX_GRID)
            button_spectrogram.grid(row=0, column=8, padx=PADX_GRID)

            # Welch's method
            button_welch.grid(row=0, column=9, padx=PADX_GRID)

        if not cnn_model or (cnn_model and dataset.feature == 'mfsc'):
            # Mel-scale Spectrogram (PSD)
            range_mfsc.grid(row=0, column=10, padx=PADX_GRID)
            button_mfsc.grid(row=0, column=11, padx=PADX_GRID)

        # MFCC
        if not cnn_model or (cnn_model and dataset.feature == 'mfcc'):
            range_mfcc.grid(row=0, column=12, padx=PADX_GRID)
            button_mfcc.grid(row=0, column=13, padx=PADX_GRID)

        # CMAP
        label_image.grid(row=0, column=14, padx=PADX_GRID)
        spectrum_subtraction.grid(row=0, column=15, padx=PADX_GRID)
        cmap.grid(row=0, column=16, padx=PADX_GRID)

        ### Row 2 ####

        frame_row2.pack(pady=PADY_GRID)

        # Repeat, pre_emphasis, save fig and delete
        button_pre_emphasis.grid(row=0, column=5, padx=PADX_GRID)
        if not cnn_model:
            if not args.oscilloscope_mode:
                button_confirm.grid(row=0, column=7, padx=PADX_GRID)
                button_save.grid(row=0, column=8, padx=PADX_GRID)
                button_remove.grid(row=0, column=9, padx=PADX_GRID)
        button_savefig.grid(row=0, column=10, padx=PADX_GRID)

        # Quit
        button_quit.grid(row=0, column=11, padx=PADX_GRID)

        ### Row 3 ####

        # DEBUG
        if args.debug:
            frame_row3.pack(pady=PADY_GRID)
            button_filterbank.grid(row=0, column=0, padx=PADX_GRID)
            button_elapsed_time.grid(row=0, column=1, padx=PADX_GRID)

        elif not args.oscilloscope_mode and cnn_model:
            frame_row3.pack(pady=PADY_GRID)
            label_inference.grid(row=0, column=0, padx=PADX_GRID)
            label_inference.configure(text='...')

        ### Row 4 ####
        if not args.oscilloscope_mode:
            frame_row4.pack(pady=PADY_GRID)
            label_window.grid(row=0, column=0, padx=PADX_GRID)
            range_window.grid(row=0, column=1, padx=PADX_GRID)

    ##### loop forever #####
    Tk.mainloop()
