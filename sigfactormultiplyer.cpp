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

#include "sigfactormultiplyer.h"

SigFactorMultiplyer::SigFactorMultiplyer(MStreamReader<float> *input, QObject *parent)
    : QThread(parent), input(input), k(1.0)
{
}

SigFactorMultiplyer::~SigFactorMultiplyer()
{
    stop();
    wait(2000);
}

MStreamReader<float> *SigFactorMultiplyer::newReader()
{
    return new MStreamReader<float>(&output);
}

void SigFactorMultiplyer::setFactor(double k)
{
//    qDebug("setFactor(double %f)", k);
    this->k = k;
}

void SigFactorMultiplyer::stop()
{
    stopnext = true;
}

void SigFactorMultiplyer::run()
{
    stopnext = false;

    while (stopnext == false) {
        int size = input->size();

        if (size != 0) {
            float *buffer = new float[size];
            input->read(buffer, size);

            for (int i = 0; i < size; ++i) {
                buffer[i] *= k;
            }

            output.write(buffer, size);
            delete[] buffer;
        } else {
            msleep(4);
        }
    }
}
