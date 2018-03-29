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

#include "saturation.h"

Saturation::Saturation(MStreamReader<float> *input, int bufferSize, QObject *parent)
    : QThread(parent), enabled(true), input(input), bufferSize(bufferSize)
{
    currentRange = -1;
    autogain = false;

    if (bufferSize != 0)
        buffer = new float[bufferSize];
    else
        buffer = 0;

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendFactor()));
}

Saturation::~Saturation()
{
    stop();
    wait(2000);
}

void Saturation::setBufferSize(int newBufferSize)
{
    mutex.lock();
    if (bufferSize != 0 && newBufferSize != 0) {
        delete[] buffer;
        buffer = 0;
        bufferSize = 0;
    }

    if (newBufferSize != 0) {
        buffer = new float[newBufferSize];
        bufferSize = newBufferSize;
    }
    mutex.unlock();
}

void Saturation::setAutoRange(bool enable)
{
    autogain = enable;
}

void Saturation::setRanges(QList<struct Range> list)
{
    mutex.lock();
    ranges = list;
    currentRange = -1;
    mutex.unlock();
}

QList<struct Saturation::Range> Saturation::getRanges() const
{
    return ranges;
}

void Saturation::setRange(int id)
{
    if (id < ranges.size()) {
        currentRange = id;
        emit perdiodOutputChanged(1.0 / ranges[currentRange].f);
        emit rangeChanged(currentRange);

        ampfactorasync = 1.0 / ranges[currentRange].g;
        timer->start(100);
    }
}

void Saturation::setEnabled(bool yes)
{
    enabled = yes;
}

void Saturation::sendFactor()
{
    emit ampliFactorChanged(ampfactorasync);
}

void Saturation::stop()
{
    stopnext = true;
}

void Saturation::run()
{
    stopnext = false;

    int upLastTime = 1;

    mutex.lock();
    while (stopnext == false) {
        int size = input->size();

        if (bufferSize != 0 && size >= bufferSize) {
            input->read(buffer, bufferSize);

            float peak = 0.0;
            for (int i = 0; i < bufferSize; ++i) {
                const float va = qAbs(buffer[i]);
                if (va > peak)
                    peak = va;
            }

            emit saturationValueChanged(peak);


            if (enabled && autogain) {
//                qDebug("%d", __LINE__);
                if (upLastTime > 0) {
//                    qDebug("%d", __LINE__);
                    upLastTime--;
                } else if (!ranges.isEmpty()) {
//                    qDebug("%d", __LINE__);
                    if (peak >= 0.99) {
                        if (currentRange > 0) {
                            currentRange--;
                            setRange(currentRange);
                            upLastTime = 1;
                        }
                    }

                    if (peak <= 0.05) {
                        if (currentRange < ranges.size() - 1) {
                            currentRange++;
                            setRange(currentRange);
                            upLastTime = 1;
                        }
                    }

                    if (currentRange == -1) {
                        currentRange = 0;
                        setRange(currentRange);
                    }
                }
            }
        } else {
            mutex.unlock();
            msleep(50);
            mutex.lock();
        }
    }
    mutex.unlock();
}
