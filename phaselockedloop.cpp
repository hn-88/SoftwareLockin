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

#include "phaselockedloop.h"
#include <math.h>

PhaseLockedLoop::PhaseLockedLoop(MStreamReader<float> *input, MStreamReader<float> *inputmedian, QObject *parent)
    : QThread(parent), input(input), inputmedian(inputmedian), compareValue(0.0)
{
}

PhaseLockedLoop::~PhaseLockedLoop()
{
    stop();
    wait(2000);
}

MStreamReader<float> *PhaseLockedLoop::newReaderOutput()
{
    return new MStreamReader<float>(&output);
}

MStreamReader<float> *PhaseLockedLoop::newReaderPeriod()
{
    return new MStreamReader<float>(&outputperiod);
}

void PhaseLockedLoop::setShape(enum PLLShape shape)
{
    this->shape = shape;
}

void PhaseLockedLoop::stop()
{
    stopnext = true;
}

void PhaseLockedLoop::run()
{
    bool isBigger = false;
    bool wasBigger = false;
    float periodCounter = 0.0;

    stopnext = false;
    while (stopnext == false) {

        // update the compare value
        if (inputmedian != 0 && inputmedian->size() > 0) {
            inputmedian->take(compareValue);
        }

        int size = input->size();
        if (size > 0) {

            // read in the mstream
            float *inputbuffer = new float[size];
            input->read(inputbuffer, size);

            // for eatch values readed
            for (int i = 0; i < size; ++i) {
                isBigger = inputbuffer[i] > compareValue;

                // on the rising edge
                if (wasBigger == false && isBigger == true) {
                    int t = periodCounter;

                    float *outputbuffer = new float[t];

                    double k = 0.5;

                    // create output values PLL signal
                    for (int j = 0; j < t && stopnext == false; ++j) {
                        const double f = k / periodCounter;

                        switch (shape) {

                        case SINE:
                            outputbuffer[j] = sin(2.0 * M_PI * f);
                            break;

                        case COSINE:
                            outputbuffer[j] = cos(2.0 * M_PI * f);
                            break;

                        case SQUARE:
                            outputbuffer[j] = 2.0 * (f < 0.5) - 1.0;
                            if (f == 0.5)
                                outputbuffer[j] = 0.0;
                            break;

                        case TRIANGLE:
                            outputbuffer[j] = (f < 0.25) ? (4.0 * f) : ((f < 0.75) ? (1.0 - 4.0 * (f - 0.25)) : (4.0 * (f - 0.75) - 1.0));
                            break;
                        }

                        k += 1.0;
                    }
                    // and write on output
                    output.write(outputbuffer, t);

                    delete[] outputbuffer;

                    outputperiod.push(periodCounter);
                    periodCounter = 0;
                }

                // incrase the period size
                periodCounter++;
                wasBigger = isBigger;
            }

            delete[] inputbuffer;
        } else {
            msleep(10);
        }
    }
}
