########################################################################
## Extract wireless remote codes for the brennenstuhl 3600 power switch
##
## The input is a baseband capture file, complex float 32,
## rate 250 ksps, center frequency 433.92 MHz, full-scale 1.0.
########################################################################

import sys
import numpy as np
import matplotlib.pyplot as plt

INRATE = 250e3

def runningMeanFast(x, N):
    return np.convolve(x, np.ones((N,))/N)[(N-1):]

def plotHelper(*samps):
    timeScale = (1/INRATE)*1e3
    t = np.arange(0.0, timeScale*samps[0].size, timeScale)
    for s in samps: plt.plot(t, s)
    plt.xlabel('time (ms)')
    plt.ylabel('Amplitude')
    plt.ylim(-0.1, 1.1)
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    rawSamples = np.fromfile(sys.argv[1], np.complex64)
    print("Found %d samples"%rawSamples.size)

    absSamples = np.abs(rawSamples)
    envelope = runningMeanFast(absSamples, 20)

    selection = envelope[envelope.size/2:envelope.size/2+30000]
    selection /= max(selection)

    bits = np.array([0]*selection.size, np.uint8)
    for i, value in enumerate(selection):
        if value > 0.5: bits[i] = 1
        else: bits[i] = 0

    #find transitions of 3 ms of 1s to 7ms of zeros
    startCorrMask = np.concatenate((
        np.array([1]*int(3e-3*INRATE)),
        np.array([0]*int(7e-3*INRATE)),
    ))

    print('correlate for frame starts')
    matches = np.correlate(bits, startCorrMask)/float(sum(startCorrMask))
    for i, value in enumerate(matches):
        if value > 0.9:
            print value, i
            bits = bits[i:]
            break

    while bits[0] == 0:
        bits = bits[1:]
    bits = bits[:int(47e-3*INRATE)]

    code = bits[::50]
    codeStr = ""
    for ch in code:
        if ch == 0: codeStr += '0'
        else: codeStr += '1'
    print codeStr

    dumpSamples = bits
    plotHelper(dumpSamples)

    dumpSamples = dumpSamples.astype(np.complex64)
    print("Dumping %d samples"%dumpSamples.size)
    dumpSamples.tofile(sys.argv[2])
