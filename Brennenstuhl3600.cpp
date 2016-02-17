// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <iostream>
#include <complex>
#include <cmath>
#include <queue>

/***********************************************************************
 * |PothosDoc Brennenstuhl 3600
 *
 * Generate wireless remote control packets for the Brennenstuhl 3600.
 * This block outputs multiple wireless control packets in a burst
 * when a change to the on/off mode is made (or on activation).
 *
 * |category /Control
 * |keywords brennenstuhl power control
 *
 * |param mode The mode is either on (true) or off (false)
 * |default false
 * |option [On] true
 * |option [Off] false
 *
 * |param group Which group to control A, B, C, or D.
 * |default "A"
 *
 * |param rate The sample rate of the output stream
 * |units Sps
 * |default 250e3
 *
 * |param gain The level of the output samples.
 * |default 0.7
 *
 * |param repeat The number of times to repeat a control packet.
 * |default 10
 *
 * |param endLabel[End Label] An optional end of burst label.
 * |widget StringEntry()
 * |default "txEnd"
 * |preview valid
 *
 * |factory /btle/brennenstuhl_3600()
 * |setter setMode(mode)
 * |setter setGroup(group)
 * |setter setRate(rate)
 * |setter setGain(gain)
 * |setter setRepeat(repeat)
 * |setter setEndLabel(endLabel)
 **********************************************************************/
class Brennenstuhl3600 : public Pothos::Block
{
public:
    Brennenstuhl3600(void)
    {
        this->setupOutput(0, typeid(std::complex<float>));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setMode));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setGroup));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setRate));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setGain));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setRepeat));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setEndLabel));
    }

    static Block *make(void)
    {
        return new Brennenstuhl3600();
    }

    void setMode(const bool mode)
    {
        _mode = mode;
        this->trigger();
    }

    void setGroup(const std::string &group)
    {
        _group = group;
    }

    void setRate(const double rate)
    {
        _rate = rate;
    }

    void setGain(const float gain)
    {
        _gain = gain;
    }

    void setRepeat(const size_t repeat)
    {
        _repeat = repeat;
    }

    void setEndLabel(const std::string &label)
    {
        _endLabel = label;
    }

    void activate(void)
    {
        this->trigger();
    }

    void work(void)
    {
        auto outPort = this->output(0);
        size_t N = outPort->elements();
        if (N == 0) return;
        if (_toTransmit.empty()) return;

        auto out = outPort->buffer().as<std::complex<float> *>();
        for (size_t i = 0; i < N; i++)
        {
            out[i] = _toTransmit.front()?_gain:0;
            _samplesCount++;
            if (_samplesCount == _sampsPerSym)
            {
                _toTransmit.pop();
                _samplesCount = 0;
            }
            if (_toTransmit.empty())
            {
                if (not _endLabel.empty())
                    outPort->postLabel(Pothos::Label(_endLabel, Pothos::Object(), i));
                N = i+1;
                break;
            }
        }

        outPort->produce(N);
    }

private:

    void trigger(void)
    {
        _toTransmit = std::queue<bool>(); //clear
        _sampsPerSym = size_t(_rate/500e-6); //0.5 ms
        _samplesCount = 0;
        //TODO fill with codes
    }

    //current state
    std::queue<bool> _toTransmit;
    size_t _sampsPerSym;
    size_t _samplesCount;

    //config
    bool _mode;
    std::string _group;
    double _rate;
    float _gain;
    size_t _repeat;
    std::string _endLabel;
};

static Pothos::BlockRegistry registerBrennenstuhl3600(
    "/btle/brennenstuhl_3600", &Brennenstuhl3600::make);
