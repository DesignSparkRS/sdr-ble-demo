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
THRESH = 0.01

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

    bits = np.array([0]*envelope.size, np.uint8)
    for i, value in enumerate(envelope):
        if value > THRESH: bits[i] = 1
        else: bits[i] = 0

    startCorrMask = np.concatenate((
        np.array([1]*int(3e-3*INRATE)),
        np.array([0]*int(7e-3*INRATE)),
    ))

    codes = set()
    codeCounts = dict()

    print('correlate for frame starts')
    matches = np.correlate(bits, startCorrMask)/sum(startCorrMask)

    print('extract codes')
    i = 0;
    while i < matches.size:
        match = matches[i]
        if match > 0.9:
            samplePt = i + int(250e-6*INRATE)
            code = ""
            for j in range(47*8):
                x = bits[samplePt]
                if x > 0.5: code += "1"
                else: code += "0"
                samplePt += int(500e-6*INRATE)
            codes.add(code)
            try: codeCounts[code] += 1
            except: codeCounts[code] = 1
            i += int(10e-3*INRATE) #skip start frame
        else: i += 1

    for code, count in codeCounts.iteritems(): print("%d: %s"%(count, code))

    #plotHelper(
    #    bits[i-1000:i+int(50e-3*INRATE)],
    #)
