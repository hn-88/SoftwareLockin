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

#ifndef ALSAOUTPUTHZ_H
#define ALSAOUTPUTHZ_H

#include <QThread>

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>


/*
 This Class send a square signal with alsa

 */

class AlsaOutputHz : public QThread
{
    Q_OBJECT
public:
    AlsaOutputHz(const char *device = 0, int rate = 44100, QObject *parent = 0);
    ~AlsaOutputHz();

    unsigned int rate() const;

    bool setDevice(const char *device, int rate, QString *error = 0);

    void freeAlsa();

signals:

public slots:
    // change the period of the output signal
    void setOutputPeriod(double newperiod);


    void stop();
    void kill();

private:
    bool configAlsa(const char *device, int rate, QString *errstr = 0);

    void run();

    char *buffer;

    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t period;
    unsigned int ratevalue;

    double outputperiod;
    int squarepos;
    char squareval;

    bool stopnext;
};

#endif // ALSAOUTPUTHZ_H
