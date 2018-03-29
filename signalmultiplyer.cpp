/* --------------------------------------------------------------------------
PCSC-Lockin Copyright (c) 2011, EPFL LESO-PB, research group nanotechnology
for solar energy conversion, Mario Geiger André Kostro Andreas Schüler
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by Mario Geiger, André Kostro
   and Andreas Schüler in LESO-PB lab, research group nanotechnology for
   solar energy conversion, at EPFL.
4. Neither the name of the EPFL LESO-PB nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY EPFL LESO-PB ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL EPFL LESO-PB BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

DESCRIPTION:

---------------------------------------------------------------------------- */

#include "signalmultiplyer.h"

SignalMultiplyer::SignalMultiplyer(MStreamReader<float> *input0, MStreamReader<float> *input1, QObject *parent)
    : QThread(parent), input0(input0), input1(input1)
{
}

SignalMultiplyer::~SignalMultiplyer()
{
    stop();
    wait(2000);
}

MStreamReader<float> *SignalMultiplyer::newReader()
{
    return new MStreamReader<float>(&output);
}

void SignalMultiplyer::stop()
{
    stopnext = true;
}

void SignalMultiplyer::run()
{
    float f0, f1;

    stopnext = false;
    while (stopnext == false) {
        int size = qMin(input0->size(), input1->size());
        if (size == 1) {
            input0->take(f0);
            input1->take(f1);

            output.push(f0 * f1);
        } else if (size > 1) {
            float *buffer0 = new float[size];
            float *buffer1 = new float[size];

            input0->read(buffer0, size);
            input1->read(buffer1, size);

            for (int i = 0; i < size; ++i) {
                buffer0[i] *= buffer1[i];
            }

            output.write(buffer0, size);

            delete[] buffer0;
            delete[] buffer1;
        } else {
            msleep(10);
        }
    }
}
