// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <iostream>
#include <complex>
#include <cmath>
#include <queue>
#include <map>

//these bits represent on and off keying from remote captures
template <typename CodesType>
void loadCodes(CodesType &codes)
{
    codes[true] ["A"]   = "1111110000000000000011010011011010010011001001101001101101001001001001101001101001101101001001";
    codes[false]["A"]   = "1111110000000000000011010011011011010011001001001101001101101001101001101001101101101101001001";
    codes[true] ["B"]   = "1111110000000000000011010011011011011111001001101001101001001101101001101101101101001101001101";
    codes[false]["B"]   = "1111110000000000000011010011011010011011001001001101001001101101001101101101001001001101001101";
    codes[true] ["C"]   = "1111110000000000000011010011011011011111101101101101001001101001001101101001001001101101101001";
    codes[false]["C"]   = "1111110000000000000011010011011010011011101101001001001001001101101101001001001101101101101001";
    codes[true] ["D"]   = "1111110000000000000011010011011010011011001001001101001001101101001101101101001001001101101101";
    codes[false]["D"]   = "1111110000000000000011010011011010011011001101101101001101001001001101001001101101001101101101";
    codes[true] ["ALL"] = "1111110000000000000011010011011011011011101001001101001101101001101001101001101101001001101001";
    codes[false]["ALL"] = "1111110000000000000011010011011010011011101101001001001001001101101101001001001101001001101001";
}

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
 * |param group Which group to control A, B, C, D, or ALL.
 * |default "A"
 * |option [A] "A"
 * |option [B] "B"
 * |option [C] "C"
 * |option [D] "D"
 * |option [All] "ALL"
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
 * |param startLabel[Start Label] An optional start of burst label.
 * |widget StringEntry()
 * |default ""
 * |preview valid
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
 * |setter setStartLabel(startLabel)
 * |setter setEndLabel(endLabel)
 **********************************************************************/
class Brennenstuhl3600 : public Pothos::Block
{
public:
    Brennenstuhl3600(void):
        _mode(false),
        _group("A"),
        _rate(1.0),
        _gain(1.0),
        _repeat(1)
    {
        this->setupOutput(0, typeid(std::complex<float>));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setMode));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setGroup));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setRate));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setGain));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setRepeat));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setStartLabel));
        this->registerCall(this, POTHOS_FCN_TUPLE(Brennenstuhl3600, setEndLabel));
        loadCodes(_codeMap);
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

    void setStartLabel(const std::string &label)
    {
        _startLabel = label;
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

        if (_doStartBurst and not _startLabel.empty())
        {
            const size_t elemsInBurst = _toTransmit.size()*_sampsPerSym;
            outPort->postLabel(Pothos::Label(_startLabel, elemsInBurst, 0));
            _doStartBurst = false;
        }

        auto out = outPort->buffer().as<std::complex<float> *>();
        for (size_t i = 0; i < N; i++)
        {
            //std::cout << "_sampsPerSym " << _sampsPerSym << std::endl;
            //std::cout << "_samplesCount " << _samplesCount << std::endl;
            //std::cout << "_toTransmit.size() " << _toTransmit.size() << std::endl;
            //std::cout << "_toTransmit.front() " << _toTransmit.front() << std::endl;
            out[i] = _toTransmit.front();
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
        _toTransmit = std::queue<float>(); //clear
        _sampsPerSym = size_t(_rate*500e-6); //0.5 ms
        _samplesCount = 0;
        _doStartBurst = true;
        const auto &code = _codeMap[_mode][_group];
        for (size_t i = 0; i < _repeat; i++)
        {
            for (const auto &ch : code)
            {
                _toTransmit.push((ch=='0')?0.0:_gain);
            }
        }
    }

    //current state
    std::queue<float> _toTransmit;
    size_t _sampsPerSym;
    size_t _samplesCount;
    bool _doStartBurst;

    //config
    bool _mode;
    std::string _group;
    double _rate;
    float _gain;
    size_t _repeat;
    std::string _startLabel;
    std::string _endLabel;

    //map enable to group to list of codes
    std::map<bool, std::map<std::string, std::string>> _codeMap;
};

static Pothos::BlockRegistry registerBrennenstuhl3600(
    "/btle/brennenstuhl_3600", &Brennenstuhl3600::make);
