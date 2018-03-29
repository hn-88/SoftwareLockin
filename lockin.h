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

#ifndef LOCKIN_H
#define LOCKIN_H

#include <QObject>
#include "alsainput.h"
#include "alsaoutputhz.h"
#include "lowpassfilter.h"
#include "phaselockedloop.h"
#include "signalmultiplyer.h"
#include "coutwriter.h"
#include "signalmiddle.h"
#include "phasor.h"
#include "sigfactormultiplyer.h"
#include "saturation.h"
#include "noise.h"

class Lockin : public QObject
{
    Q_OBJECT
public:
    Lockin(const QString &devicein = "default", const QString &deviceout = "default", int rate = 44100, QObject *parent = 0);
    Lockin(QObject *parent);
    ~Lockin();

    void startAlsa();
    bool stopAlsa();
    void closeAlsa();
    bool isAlsaRunning();

    // signal and chopper after Phase shifter X
    MStreamReader<float> *newReaderSignal();
    MStreamReader<float> *newReaderChopper();

    // output x & y
    MStreamReader<float> *newReaderX();
    MStreamReader<float> *newReaderY();

    // noise
    MStreamReader<double> *newReaderNoise();


    // set the phase angle in radian
    void setPhi(double phi);

    void setPllShape(PhaseLockedLoop::PLLShape shape);

    void setOutput(double integrationtime, double outputPeriod, bool avgmode = true);
    void setNoiseSamples(int samples);
    int setDevice(const QString &devicein, const QString &deviceout, int rate, QString *error = 0);

    void setAutoDynamicRange(bool enable);
    void selectGainRange(int id);

    // format "gain1:frequency1;gain2:freuqency2"
    QString getGainRanges() const;
    void setGainRanges(const QString &gainranges);

    double chopperFrequency();

signals:
    void saturationValueChanged(float);
    void dynamicRangeChanged(int);

public slots:
    void setAmplificationFactor(double k);
    void setOutputFrequency(double hz);

private:
    // create objects
    void mkLockin(const QString &devicein, const QString &deviceout, int rate);

    // delete objects
    void rmLockin();

    void timerEvent(QTimerEvent *);
    int timerid;

    double integrationtime;
    double outputPeriod;

    int noisesamples;

    float chopperPeriod;





    // see the block schematic
    AlsaInput *soundcardinput;

    AlsaOutputHz *soundcardoutput;
    SigFactorMultiplyer *multiplication;
    Saturation *signalsaturation;

    SignalMiddle *median;

    PhaseLockedLoop *pll;

    LowPassFilter *avg;

    PhaseShifter *phaseshifterx;
    PhaseShifter *phaseshiftery;

    SignalMultiplyer *multiplicationx;
    SignalMultiplyer *multiplicationy;

    LowPassFilter *avgoutputx;
    LowPassFilter *avgoutputy;

    Noise *noise;




    // use to give the chopper frequency
    MStreamReader<float> *readerPeriod;
};

#endif // LOCKIN_H
