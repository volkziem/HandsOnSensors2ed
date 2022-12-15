# show_epics_waveform.py, V. Ziemann, 221103
import matplotlib.pyplot as plt  # for plotting
import numpy as np               # for fft
import epics                     # epics access
data=epics.caget("geophone:wf");
plt.subplot(2,1,1)
plt.plot(data);
plt.xlabel("Samples")
plt.ylabel("Amplitude")

plt.subplot(2,1,2)
fftvals=np.fft.fft(data)
plt.plot(abs(fftvals))
plt.ylabel("FFT")

plt.show()
