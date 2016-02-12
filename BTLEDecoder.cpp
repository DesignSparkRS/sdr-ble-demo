// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include "BTLEUtils.hpp"
#include <iostream>

/***********************************************************************
 * |PothosDoc BTLE Decoder
 *
 * Decode bluetooth low energy packets.
 * The decoder block accepts a stream of 16-bit integer samples on input port 0,
 * and produces a message containing keyword value pairs on output port 0.
 *
 * <h2>Input format</h2>
 *
 * The input port expects signed 16-bit integers that have been frequency demodulated.
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
        this->setupInput(0, typeid(uint16_t));
        this->setupOutput(0);
    }

    static Block *make(void)
    {
        return new BTLEDecoder();
    }

    void work(void)
    {
        auto inPort = this->input(0);
        auto N = inPort->elements();
        auto in = inPort->buffer().as<const uint16_t *>();
        for (size_t i = 0; i < N; i++)
        {
            if (_decoder.feedOne(in[i]))
            {
                this->output(0)->postMessage(_decoder.packetData);
                //std::cout << Pothos::Object(_decoder.packetData).toString() << std::endl;
            }
        }
        inPort->consume(N);
    }

private:
    BTLEUtilsDecoder _decoder;
};

static Pothos::BlockRegistry registerBTLEDecoder(
    "/btle/btle_decoder", &BTLEDecoder::make);
