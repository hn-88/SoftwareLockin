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

#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include <QThread>
#include <QMutex>

#include "mstream.h"

class LowPassFilter : public QThread
{
    Q_OBJECT
public:
    /*
     rc : the number of samples used to make the average
     step : the number of samples between two averages

     **********************************>
     |    1  | |  2    |
     +----+--+-+       |
          |  +----+----+
          v       |
                  v
          RC
     <--------->
        step
     <------->
     */
    LowPassFilter(MStreamReader<float> *input, int rc = 0, int step = 0, QObject *parent = 0);
    ~LowPassFilter();

    void setRCStep(int newrc, int newstep, bool newavg = true);

    MStreamReader<float> *newReader();

signals:

public slots:
    void stop();

private:
    void run();
    float avg();

    MStreamReader<float> *input;
    MStream<float> output;

    bool stopnext;

    int rc;
    int step;

    int initbuffer;

    bool avgfilter;

    float *avgbuffer;
    int p;

    float last;

    QMutex mutex;
};

#endif // LOWPASSFILTER_H
