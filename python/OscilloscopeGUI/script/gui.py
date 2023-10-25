import numpy as np
import dsp
from scipy.fftpack import dct

# Empty array
EMPTY = np.array([])

# Shadow
SHADOW_SUB = 10

def shadow(pixels, window, shadow_sub):
    if window:
        subtract = lambda x: x - shadow_sub

        a, b, c = window[0], window[1], window[2]
        _pixels = np.copy(pixels)
        _pixels[0:a] = subtract(pixels[0:a])
        _pixels[a:b, c:] = subtract(_pixels[a:b, c:])
        _pixels[b:] = subtract(_pixels[b:])
    else:
        _pixels = pixels
    return _pixels

def spectrum_subtraction(data, ssub=None):
    data_ = np.copy(data)
    if ssub:
        data_ = data_ - ssub
        data_[data_ < 0] = 0
    return data_

# GUI class
class GUI:
    
    def __init__(self, interface, dataset, enable_shadow=True):
        # Serial interface
        self.interface = interface
        self.filters = dataset.filters
        self.samples = dataset.samples
        self.enable_shadow = enable_shadow
        # Time axis and frequency axis
        self.time = {}
        self.freq = {}
        self.time[dsp.RAW_WAVE] = np.linspace(0, self.interface.num_samples[dsp.RAW_WAVE]/dsp.Fs*1000.0, self.interface.num_samples[dsp.RAW_WAVE])
        self.freq[dsp.SFFT] = np.linspace(0, dsp.Fs/2, self.interface.num_samples[dsp.SFFT])
        self.time[dsp.SPECTROGRAM] = np.linspace(0, self.interface.num_samples[dsp.RAW_WAVE]/dsp.Fs*self.samples/2, self.samples)
        self.freq[dsp.SPECTROGRAM] = np.linspace(0, dsp.Nyq, int(dsp.NN/2))
        self.time[dsp.MFSC] = np.linspace(-self.interface.num_samples[dsp.RAW_WAVE]/dsp.Fs*self.samples/2, 0, self.samples)
        self.freq[dsp.MFSC] = np.linspace(0, self.filters-1, self.filters)
        self.time[dsp.MFCC] = np.linspace(-self.interface.num_samples[dsp.RAW_WAVE]/dsp.Fs*self.samples/2, 0, self.samples)
        self.freq[dsp.MFCC] = np.linspace(0, self.filters-1, self.filters)

    def set_labels(self, ax, title, xlabel, ylabel, ylim=None):
        ax.set_title(title)
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        if ylim:
            ax.set_ylim(ylim)

    def tx_on(self):
        self.interface.tx_on()

    def tx_suspend(self):
        self.interface.tx_suspend()

    def tx_off(self):
        self.interface.tx_off()

    # Use matplotlib to plot the output from the device
    def plot(self, ax, cmd, range_=None,
                 cmap=None, ssub=None,
                 window=None, data=EMPTY,
                 grid=False):

        if data is EMPTY:
            if cmd == dsp.MFSC or cmd == dsp.MFCC:
                data = self.interface.read(dsp.FEATURES)
            else:
                data = self.interface.read(cmd)

        ax.clear()
        
        if cmd == dsp.RAW_WAVE:
            ax.plot(self.time[dsp.RAW_WAVE], data)
            self.set_labels(ax, 'Waveform', 'Time [msec]', 'Amplitude', [-range_, range_])

        elif cmd == dsp.SFFT:
            ax.plot(self.freq[dsp.SFFT], data)
            self.set_labels(ax, 'Spectrum', 'Frequency [Hz]', 'Power [dB]', [-70, 90])

        elif cmd == dsp.SPECTROGRAM:
            data_ = data
            data_ = spectrum_subtraction(data, ssub)
            if self.enable_shadow:
                data_ = shadow(data_, window, shadow_sub=SHADOW_SUB)
            ax.pcolormesh(self.time[dsp.SPECTROGRAM],
                        self.freq[dsp.SPECTROGRAM][:range_],
                        data_.T[:range_],
                        cmap=cmap)
            self.set_labels(ax, 'Spectrogram', 'Time [sec]', 'Frequency (Hz)')

        elif cmd == dsp.MFSC:

            data_ = data[:, :self.filters]
            data_ = spectrum_subtraction(data_, ssub)
            if self.enable_shadow:
                data_ = shadow(data_, window, shadow_sub=SHADOW_SUB)
            ax.pcolormesh(self.time[dsp.MFSC],
                          self.freq[dsp.MFSC][:range_+1],
                          data_.T[:range_+1],
                          cmap=cmap)
            self.set_labels(ax, 'Mel-frequency spectrogram', 'Time [sec]', 'MFSC')

        elif cmd == dsp.MFCC:

            data_ = data[:, self.filters:self.filters*2]
            data_ = spectrum_subtraction(data_, ssub)
            if self.enable_shadow:
                data_ = shadow(data, window, shadow_sub=SHADOW_SUB)
            ax.pcolormesh(self.time[dsp.MFCC],
                          self.freq[dsp.MFCC][:range_+1],
                          data_.T[:range_+1],
                          cmap=cmap)
            self.set_labels(ax, 'MFCCs', 'Time [sec]', 'MFCC')

        elif cmd == dsp.FILTERBANK:
            k_range, filterbank = data
            for m in range(1, self.filters+1):
                h = np.zeros(int(dsp.NN/2))
                k_left, len_ = k_range[m]
                h[k_left:k_left+len_] = filterbank[m][:len_]
                ax.plot(h)
            self.set_labels(ax, 'Mel filter bank', 'n', 'Magnitude')
    
        if grid:
            ax.grid(linestyle='--', c='purple', linewidth=1)
    
        return data

    def plot_welch(self, ax, grid=False):
        data = self.interface.read(dsp.SPECTROGRAM)
        ax.clear()

        data = np.sum(data, axis=0)/self.samples
        ax.plot(self.freq[dsp.SFFT], data)
        self.set_labels(ax, "Welch's method", 'Frequency [Hz]', 'Power [dB]', [-70, 90])

        if grid:
            ax.grid(linestyle='--', c='purple', linewidth=1)

