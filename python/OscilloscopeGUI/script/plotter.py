import numpy as np
import intf
from scipy.fftpack import dct
from scipy.io import wavfile
import datetime

def spectrum_subtraction(data, ssub=None):
    data_ = np.copy(data)
    if ssub:
        data_ = data_ - ssub
        data_[data_ < 0] = 0
    return data_

# GUI class
class Plotter:
    
    def __init__(self, intf_instance, dataset, show_capture_area=True):
        # Serial interface
        self.intf = intf_instance
        self.filters = dataset.filters
        self.samples = dataset.samples
        self.show_capture_area = show_capture_area
        # Time axis and frequency axis
        self.time = {}
        self.freq = {}
        sampling_period = self.intf.num_samples[intf.RAW_WAVE]/intf.Fs*self.samples/2
        self.time[intf.RAW_WAVE] = np.linspace(0, self.intf.num_samples[intf.RAW_WAVE]/intf.Fs*1000.0, self.intf.num_samples[intf.RAW_WAVE])
        self.freq[intf.SFFT] = np.linspace(0, intf.Fs/2, self.intf.num_samples[intf.SFFT])
        self.time[intf.SPECTROGRAM] = np.linspace(-sampling_period, 0, self.samples)
        self.freq[intf.SPECTROGRAM] = np.linspace(0, intf.Nyq, int(intf.NN/2))
        self.time[intf.MFSC] = np.linspace(-sampling_period, 0, self.samples)
        self.freq[intf.MFSC] = np.linspace(0, self.filters-1, self.filters)
        self.time[intf.MFCC] = np.linspace(-sampling_period, 0, self.samples)
        self.freq[intf.MFCC] = np.linspace(0, self.filters-1, self.filters)
        self.df_spec = np.zeros([self.samples, int(intf.NN/2)])
        self.df_mfsc = np.zeros([self.samples, self.filters])
        self.df_mfcc = np.zeros([self.samples, self.filters])

    def draw_capture_area(self, ax, cmd, window, range):
        x0 = self.time[cmd][window[0]]
        x1 = self.time[cmd][window[1]]

        if cmd == intf.MFSC:
            y0 = -0.5
        elif cmd == intf.MFCC:
            y0 = 0.5
        y1 = range - 1 + 0.5

        ax.vlines([x0, x1], y0, y1, lw=2)
        ax.hlines([y0, y1], x0, x1, lw=2)

    def set_labels(self, ax, title, xlabel, ylabel, ylim=None):
        ax.set_title(title)
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        if ylim:
            ax.set_ylim(ylim)

    def start_streaming(self):        
        self.df_spec = np.zeros([self.samples, int(intf.NN/2)])
        self.df_mfsc = np.zeros([self.samples, self.filters])
        self.df_mfcc = np.zeros([self.samples, self.filters])
        self.intf.tx_on()

    def suspend_streaming(self):
        self.intf.tx_suspend()

    def stop_streaming(self):
        self.intf.tx_off()

    # Use matplotlib to plot the output from the device
    def plot(self, ax, cmd, range_=None,
                 cmap=None, ssub=None,
                 window=None, data=None,
                 grid=False):

        if data is None:
            if cmd == intf.MFSC or cmd == intf.MFCC:
                data = self.intf.read(intf.FEATURES)
                
                self.df_mfsc[0:self.samples-intf.INTERVAL,:] = self.df_mfsc[intf.INTERVAL:self.samples,:]
                self.df_mfsc[self.samples-intf.INTERVAL:self.samples,:] = data[:, :self.filters]

                self.df_mfcc[0:self.samples-intf.INTERVAL,:] = self.df_mfcc[intf.INTERVAL:self.samples,:]
                self.df_mfcc[self.samples-intf.INTERVAL:self.samples,:] = data[:, self.filters:self.filters*2]

                data = np.append(self.df_mfsc, self.df_mfcc)
            
            elif cmd == intf.SPECTROGRAM:
                data = self.intf.read(intf.SPECTROGRAM)
                self.df_spec[0:self.samples-intf.INTERVAL,:] = self.df_spec[intf.INTERVAL:self.samples,:]
                self.df_spec[self.samples-intf.INTERVAL:self.samples,:] = data[:, :int(intf.NN/2)]

            elif cmd == intf.FILTERBANK or cmd == intf.ELAPSED_TIME:
                data = self.intf.debug_read(cmd)
            
            else:
                data = self.intf.read(cmd)
            
        else:
                if cmd == intf.MFSC:
                    self.df_mfsc = data[:self.filters * self.samples]
                elif cmd == intf.MFCC:
                    self.df_mfcc = data[self.filters * self.samples: self.filters * self.samples * 2]

        ax.clear()
            
        if cmd == intf.RAW_WAVE:
            ax.plot(self.time[intf.RAW_WAVE], data)
            self.set_labels(ax, 'Waveform', 'Time [msec]', 'Amplitude', [-range_, range_])

        elif cmd == intf.SFFT:
            markerline, stemline, baseline, = ax.stem(self.freq[intf.SFFT], data)
            markerline.set_markerfacecolor('none')
            self.set_labels(ax, 'Spectrum', 'Frequency [Hz]', 'Power [dB]', [-70, 90])

        elif cmd == intf.SPECTROGRAM:
            data_ = spectrum_subtraction(self.df_spec, ssub)
            self.pc_spec = ax.pcolormesh(self.time[intf.SPECTROGRAM],
                        self.freq[intf.SPECTROGRAM][:range_],
                        data_.T[:range_],
                        cmap=cmap)
            self.set_labels(ax, 'Spectrogram', 'Time [sec]', 'Frequency (Hz)')

        elif cmd == intf.MFSC:
            data_ = spectrum_subtraction(self.df_mfsc, ssub)
            ax.pcolormesh(self.time[intf.MFSC],
                          self.freq[intf.MFSC][:range_+1],
                          data_.T[:range_+1],
                          cmap=cmap)
            if self.show_capture_area:
                self.draw_capture_area(ax, cmd, window, range_)
            self.set_labels(ax, 'Mel-frequency spectrogram', 'Time [sec]', 'MFSC')

        elif cmd == intf.MFCC:
            data_ = spectrum_subtraction(self.df_mfcc, ssub)
            ax.pcolormesh(self.time[intf.MFCC],
                          self.freq[intf.MFCC][:range_+1],
                          data_.T[:range_+1],
                          cmap=cmap)
            if self.show_capture_area:
                self.draw_capture_area(ax, cmd, window, range_)
            self.set_labels(ax, 'MFCCs', 'Time [sec]', 'MFCC')

        elif cmd == intf.FILTERBANK:
            k_range, filterbank = data
            for m in range(1, self.filters+1):
                h = np.zeros(int(intf.NN/2))
                k_left, len_ = k_range[m]
                h[k_left:k_left+len_] = filterbank[m][:len_]
                ax.plot(h)
            self.set_labels(ax, 'Mel filter bank', 'n', 'Magnitude')
    
        if grid:
            ax.grid(linestyle='--', c='purple', linewidth=1)
    
        return data

    def plot_welch(self, ax, grid=False):
        data = self.intf.read(intf.SPECTROGRAM)
        self.df_spec[0:self.samples-intf.INTERVAL,:] = self.df_spec[intf.INTERVAL:self.samples,:]
        self.df_spec[self.samples-intf.INTERVAL:self.samples,:] = data[:, :int(intf.NN/2)]
        data_ = self.df_spec.copy()
        ax.clear()

        data_ = np.sum(data_, axis=0)/self.samples
        markerline, stemline, baseline, = ax.stem(self.freq[intf.SFFT], data_)
        markerline.set_markerfacecolor('none')
        self.set_labels(ax, "Welch's method", 'Frequency [Hz]', 'Power [dB]', [-70, 90])

        if grid:
            ax.grid(linestyle='--', c='purple', linewidth=1)

    # Record PCM streaming
    def plot_rec(self, folder, record_time, ax, range_, grid):
        ax.clear()
        filename = '{}.wav'.format(datetime.datetime.now().strftime('%Y%m%d%H%M%S'))
        num_repeat = int(record_time * intf.Fs / self.intf.num_samples[intf.RAW_WAVE])
        data = self.intf.read(intf.REC, num_repeat)
        wavfile.write('{}/{}'.format(folder, filename), int(intf.Fs), data)
        print(data[:64])
        print(data.shape)
        ax.plot(self.time[intf.RAW_WAVE], data[:self.intf.num_samples[intf.RAW_WAVE]])
        self.set_labels(ax, 'Waveform', 'Time [msec]', 'Amplitude', [-range_, range_])
        if grid:
            ax.grid(linestyle='--', c='purple', linewidth=1)



