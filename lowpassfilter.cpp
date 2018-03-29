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

#include "lowpassfilter.h"
#include <QTime>
#include <math.h>

LowPassFilter::LowPassFilter(MStreamReader<float> *input, int rc, int step, QObject *parent)
    : QThread(parent), input(input), rc(rc), step(step), avgbuffer(0)
{
    avgfilter = true;

    if (rc > 0)
        avgbuffer = new float[rc];
    p = 0;
}

LowPassFilter::~LowPassFilter()
{
    stop();
    wait(2000);

    if (avgbuffer != 0)
        delete[] avgbuffer;
}

void LowPassFilter::setRCStep(int newrc, int newstep, bool newavg)
{
    mutex.lock();

    float *newbuffer = new float[newrc];

    if (avgbuffer != 0) {
        if (newrc > rc) {
            int j = 0;
            for (int i = p; i < rc; ++i, ++j)
                newbuffer[j] = avgbuffer[i];
            for (int i = 0; i < p; ++i, ++j)
                newbuffer[j] = avgbuffer[i];

            p = rc;
            initbuffer = newrc - rc;
        } else {
            int j = newrc - 1;
            for (int i = p - 1; i >= 0 && j >= 0; --i, --j)
                newbuffer[j] = avgbuffer[i];
            for (int i = rc - 1; i >= p && j >= 0; --i, --j)
                newbuffer[j] = avgbuffer[i];

            initbuffer = qMin(initbuffer, newrc);
            p = 0;
        }

        delete[] avgbuffer;
    } else {
        initbuffer = newrc;
    }

    avgbuffer = newbuffer;
    rc = newrc;
    step = newstep;

    if (newavg && !avgfilter)
        initbuffer = rc;

    avgfilter = newavg;

    mutex.unlock();
}

MStreamReader<float> *LowPassFilter::newReader()
{
    return new MStreamReader<float>(&output);
}

void LowPassFilter::stop()
{
    stopnext = true;
}

void LowPassFilter::run()
{
    stopnext = false;

    initbuffer = rc;

    mutex.lock();
    while (stopnext == false) {
        if (rc == 0 || avgbuffer == 0) {
            mutex.unlock();
            msleep(10);
            mutex.lock();
            continue;
        }

        if (avgfilter) {
            if (initbuffer > 0) {
                if (input->size() >= initbuffer) {
                    input->read(avgbuffer + p, initbuffer);
                    if (p >= rc)
                        p = 0;

                    last = avg();
                    output.push(last);
                    initbuffer = 0;
                } else {
                    mutex.unlock();
                    msleep(4);
                    mutex.lock();
                }
            } else {
                if (input->size() >= step) {
                    int cpy = step;

                    if (cpy > rc) {
                        input->skip(cpy - rc);
                        cpy = rc;
                    }

                    int i = rc - p;
                    if (i >= cpy) {
                        input->read(avgbuffer + p, cpy);
                        p += cpy;
                        if (p >= rc)
                            p = 0;
                    } else {
                        input->read(avgbuffer + p, i);
                        p = cpy - i;
                        input->read(avgbuffer, p);
                    }

                    last = avg();
                    output.push(last);
                } else {
                    mutex.unlock();
                    msleep(4);
                    mutex.lock();
                }
            }
        } else {
            if (input->size() >= step) {
                float *b = new float[step];
                input->read(b, step);

                const double a = 1.0 / (rc + 1.0);
                const double c = 1.0 - a;

                if (initbuffer != 0) {
                    last = b[0];
                    initbuffer = 0;
                }

                for (int i = 0; i < step; ++i) {
                    last = a * b[i] + c * last;
                }


                if (isnan(last) || isinf(last))
                    initbuffer = 1;

                output.push(last);
            } else {
                mutex.unlock();
                msleep(4);
                mutex.lock();
            }
        }
    }
    mutex.unlock();
}

float LowPassFilter::avg()
{
    double sum = 0.0;
    for (int i = rc - 1; i >= 0; --i) {
        sum += (double)avgbuffer[i];
    }
    return sum / (double)rc;
}
