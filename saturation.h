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

#ifndef SATURATION_H
#define SATURATION_H

#include <QThread>
#include <QMutex>
#include "mstream.h"
#include <QTimer>

/*

 read the saturation of the signals and emit some signals to change the ranges.

 */

class Saturation : public QThread
{
    Q_OBJECT
public:
    struct Range {
        double g;
        double f;
    };

    Saturation(MStreamReader<float> *input, int bufferSize, QObject *parent = 0);
    ~Saturation();

    void setBufferSize(int newBufferSize);
    void setAutoRange(bool enable);

    void setRanges(QList<struct Range> list);
    QList<struct Range> getRanges() const;
    void setRange(int id);

    void setEnabled(bool);

signals:
    void rangeChanged(int);

    void ampliFactorChanged(double);
    void perdiodOutputChanged(double);
    void saturationValueChanged(float);

public slots:
    void stop();

private slots:
    void sendFactor();

private:
    void run();

    bool enabled;

    MStreamReader<float> *input;

    bool stopnext;

    float *buffer;
    int bufferSize;

    int currentRange;
    bool autogain;

    QMutex mutex;

    QList<struct Range> ranges;

    QTimer *timer;
    double ampfactorasync;
};

#endif // SATURATION_H
