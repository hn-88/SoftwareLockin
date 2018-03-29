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

#include "phasor.h"
#include <math.h>

PhaseShifter::PhaseShifter(MStreamReader<float> *inputplus, MStreamReader<float> *inputminus, MStreamReader<float> *inputperiod, QObject *parent)
    : QThread(parent), inputplus(inputplus), inputminus(inputminus), inputperiod(inputperiod), phi(0.0), period(0.0), phase(0.0), skip(0), interpol(0.0)
{
}

PhaseShifter::~PhaseShifter()
{
    stop();
    wait(2000);
}

MStreamReader<float> *PhaseShifter::newReaderPlus()
{
    return new MStreamReader<float>(&outputplus);
}

MStreamReader<float> *PhaseShifter::newReaderMinus()
{
    return new MStreamReader<float>(&outputminus);
}

void PhaseShifter::setPhi(double phi)
{
    newphi = phi;
}

double PhaseShifter::getPhi() const
{
    return phi;
}

void PhaseShifter::stop()
{
    stopnext = true;
}

void PhaseShifter::run()
{
    stopnext = false;

    while (stopnext == false) {
        if (inputperiod->size() > 0) {
            inputperiod->take(period);
            calculateSkipAndInterpol();
        }

        if (newphi != phi) {
            phi = newphi;
            calculateSkipAndInterpol();
        }

        if (skip == 0) {
            bool work = false;

            int size = inputplus->size();
            if (size >= 4) {
                float *tab = new float[size];
                inputplus->read(tab, size);
                if (interpol > 0.0) {
                    const float lastone = tab[size - 1] + (tab[size - 1] - tab[size - 2]) * interpol;
                    for (int i = 0; i < size - 1; ++i) {
                        tab[i] += (tab[i + 1] - tab[i]) * interpol;
                    }
                    tab[size - 1] = lastone;
                }
                outputplus.write(tab, size);
                delete[] tab;
                work = true;
            }
            size = inputminus->size();
            if (size >= 4) {
                float *tab = new float[size];
                inputminus->read(tab, size);
                if (-interpol > 0.0) {
                    const float lastone = tab[size - 1] + (tab[size - 1] - tab[size - 2]) * (-interpol);
                    for (int i = 0; i < size - 1; ++i) {
                        tab[i] += (tab[i + 1] - tab[i]) * (-interpol);
                    }
                    tab[size - 1] = lastone;
                }
                outputminus.write(tab, size);
                delete[] tab;
                work = true;
            }

            if (work == false) {
                msleep(6);
            }
        } else if (skip > 0) {
            if (inputplus->size() >= skip) {
                inputplus->skip(skip);
                skip = 0;
            } else {
                msleep(6);
            }
        } else if (skip < 0) {
            if (inputminus->size() >= -skip) {
                inputminus->skip(-skip);
                skip = 0;
            } else {
                msleep(6);
            }
        }
    }
}

void PhaseShifter::calculateSkipAndInterpol()
{
    double newphase = phi / (2.0 * M_PI) * period;

    const double tmp = floor(newphase + 0.5);

    skip += tmp - phase;
    phase = tmp;

    interpol = newphase - phase;

//    if (skip != 0) {
//        qDebug("p:%f p:%f", period, phi);
//        qDebug("%f %f", interpol, phase);
//        qDebug("%d", skip);
//    }
}
