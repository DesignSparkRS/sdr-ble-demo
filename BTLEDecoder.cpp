// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include "BTLEUtils.hpp"
#include <iostream>
#include <cmath>

/***********************************************************************
 * |PothosDoc BTLE Decoder
 *
 * Decode bluetooth low energy packets.
 * The decoder block accepts a stream of real-valued samples on input port 0,
 * and produces a message containing keyword value pairs on output port 0.
 *
 * <h2>Input format</h2>
 *
 * The input port expects either signed integers that have been frequency demodulated
 * or alternatively, frequency demodulated floating point samples between -pi and +pi.
 * The scaling of the input samples does not matter, and the input sample rate should be 2 Msps.
 * A typical upstream flow involves raw complex baseband samples and the "Freq Demod" block.
 *
 * <h2>Output format</h2>
 *
 * Each decoded BTLE packet results in a dictionary message of type Pothos::ObjectKwargs.
 * The keyword and value pairs correspond with the fields in the BTLE packet.
 *
 * |category /Decode
 * |keywords bluetooth low energy
 *
 * |factory /btle/btle_decoder()
 **********************************************************************/
class BTLEDecoder : public Pothos::Block
{
public:
    BTLEDecoder(void)
    {
        this->setupInput(0); //unspecified type, handles conversion
        this->setupOutput(0);
    }

    static Block *make(void)
    {
        return new BTLEDecoder();
    }

    void work(void)
    {
        auto inPort = this->input(0);
        auto inBuff = inPort->buffer();
        auto N = inBuff.elements();
        if (N == 0) return; //nothing available

        //floating point support
        if (inBuff.dtype.isFloat())
        {
            auto float32Buff = inBuff.convert(typeid(float));
            auto in = float32Buff.as<const float *>();
            const float gain = (1 << 15)/M_PI;
            for (size_t i = 0; i < N; i++)
            {
                if (_decoder.feedOne(uint16_t(in[i]*gain)))
                {
                    this->output(0)->postMessage(_decoder.packetData);
                    //std::cout << Pothos::Object(_decoder.packetData).toString() << std::endl;
                }
            }
        }

        //fixed point support
        else
        {
            auto int16Buff = inBuff.convert(typeid(int16_t));
            auto in = int16Buff.as<const uint16_t *>();
            for (size_t i = 0; i < N; i++)
            {
                if (_decoder.feedOne(in[i]))
                {
                    this->output(0)->postMessage(_decoder.packetData);
                    //std::cout << Pothos::Object(_decoder.packetData).toString() << std::endl;
                }
            }
        }

        //consume all input elements
        inPort->consume(inPort->elements());
    }

private:
    BTLEUtilsDecoder _decoder;
};

static Pothos::BlockRegistry registerBTLEDecoder(
    "/btle/btle_decoder", &BTLEDecoder::make);
