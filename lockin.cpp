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

#include "lockin.h"
#include <math.h>
#include <QStringList>

Lockin::Lockin(const QString &devicein, const QString &deviceout, int rate, QObject *parent)
    : QObject(parent), timerid(0), integrationtime(0.3), outputPeriod(0.1), noisesamples(0)
{
    mkLockin(devicein, deviceout, rate);
}

Lockin::Lockin(QObject *parent)
    : QObject(parent), timerid(0), integrationtime(0.3), outputPeriod(0.1), noisesamples(0)
{
    mkLockin("", "", 0);
}

Lockin::~Lockin()
{
    rmLockin();
}

void Lockin::startAlsa()
{
    soundcardinput->start(QThread::NormalPriority);
    soundcardoutput->start(QThread::NormalPriority);
}

bool Lockin::stopAlsa()
{
    soundcardinput->stop();
    soundcardoutput->stop();
    if (soundcardinput->wait(300) && soundcardoutput->wait(300))
        return true;

    soundcardinput->terminate();
    soundcardoutput->terminate();
    if (soundcardinput->wait(300) && soundcardoutput->wait(300))
        return true;

    soundcardinput->kill();
    soundcardoutput->kill();
    if (soundcardinput->wait(300) && soundcardoutput->wait(300))
        return true;

    return false;
}

void Lockin::closeAlsa()
{
    soundcardinput->freeAlsa();
    soundcardoutput->freeAlsa();
}

bool Lockin::isAlsaRunning()
{
    return (soundcardinput->isRunning() && soundcardoutput->isRunning());
}

MStreamReader<float> *Lockin::newReaderSignal()
{
    return phaseshifterx->newReaderMinus();
}

MStreamReader<float> *Lockin::newReaderChopper()
{
    return phaseshifterx->newReaderPlus();
}

MStreamReader<float> *Lockin::newReaderX()
{
    return avgoutputx->newReader();
}

MStreamReader<float> *Lockin::newReaderY()
{
    return avgoutputy->newReader();
}

MStreamReader<double> *Lockin::newReaderNoise()
{
    return noise->newReader();
}

void Lockin::setPhi(double phi)
{
    phaseshifterx->setPhi(phi);
    phaseshiftery->setPhi(phi + M_PI / 2.0);
}

void Lockin::setPllShape(PhaseLockedLoop::PLLShape shape)
{
    pll->setShape(shape);
}

void Lockin::setAmplificationFactor(double k)
{
    multiplication->setFactor(1.0 / k);
}

void Lockin::setOutputFrequency(double hz)
{
    soundcardoutput->setOutputPeriod(1.0 / hz);
}

void Lockin::setOutput(double integrationtime, double outputPeriod, bool avgmode)
{
    this->integrationtime = integrationtime;
    this->outputPeriod = outputPeriod;
    avgoutputx->setRCStep(soundcardinput->rate() * integrationtime, soundcardinput->rate() * outputPeriod, avgmode);
    avgoutputy->setRCStep(soundcardinput->rate() * integrationtime, soundcardinput->rate() * outputPeriod, avgmode);
}

void Lockin::setNoiseSamples(int samples)
{
    this->noisesamples = samples;
    noise->setSize(samples);
}

int Lockin::setDevice(const QString &devicein, const QString &deviceout, int rate, QString *error)
{
    if (!stopAlsa()) {
        *error = "Cannot stop alsa thread.";
        return -2;
    }

    if (!soundcardinput->setDevice(devicein.toLatin1().data(), rate, error))
        return -1;

    if (deviceout.isEmpty()) {
        signalsaturation->setEnabled(false);
        multiplication->setFactor(1.0);
    } else {
        signalsaturation->setEnabled(true);

        if (!soundcardoutput->setDevice(deviceout.toLatin1().data(), rate, error))
            return -1;
    }

    setOutput(integrationtime, outputPeriod);
    signalsaturation->setBufferSize(0.5 * rate);

    return soundcardinput->rate();
}

void Lockin::setAutoDynamicRange(bool enable)
{
//    qDebug("%s(%d)", __FUNCTION__, enable);
    signalsaturation->setAutoRange(enable);
}

void Lockin::selectGainRange(int id)
{
//    qDebug("%s(%d)", __FUNCTION__, id);
    signalsaturation->setRange(id);
}

QString Lockin::getGainRanges() const
{
//    qDebug("%s()", __FUNCTION__);
    QList<struct Saturation::Range> ranges = signalsaturation->getRanges();
    QString str;
    for (int i = 0; i < ranges.size(); ++i) {
        if (!str.isEmpty())
            str.append(';');
        str.append(QString::number(ranges[i].g)+':'+QString::number(ranges[i].f));
    }
    return str;
}

void Lockin::setGainRanges(const QString &gainranges)
{
//    qDebug("%s(%s)", __FUNCTION__, gainranges.toLatin1().data());
    QList<struct Saturation::Range> list;

    QStringList ranges = gainranges.split(';');
    for (int i = 0; i < ranges.size(); ++i) {
        QStringList values = ranges[i].split(':');
        if (values.size() == 2) {
            struct Saturation::Range r = {values[0].toDouble(), values[1].toDouble()};
            list.append(r);
        }
    }

    signalsaturation->setRanges(list);
}

double Lockin::chopperFrequency()
{
    int size = readerPeriod->size();
    if (size > 1)
        readerPeriod->skip(size - 1);

    if (size > 0)
        readerPeriod->take(chopperPeriod);

    double fr = (double)soundcardinput->rate() / chopperPeriod;

    return fr;
}

void Lockin::timerEvent(QTimerEvent *)
{
}

void Lockin::mkLockin(const QString &devicein, const QString &deviceout, int rate)
{
//    qDebug("%s()", __FUNCTION__);
    if (devicein.isEmpty()) {
        soundcardinput = new AlsaInput();
        rate = 0;
    } else {
        soundcardinput = new AlsaInput(devicein.toLatin1().data(), rate);
        rate = soundcardinput->rate();
    }

    if (devicein.isEmpty()) {
        soundcardoutput = new AlsaOutputHz();
    } else {
        soundcardoutput = new AlsaOutputHz(deviceout.toLatin1().data(), rate);
    }

    multiplication = new SigFactorMultiplyer(soundcardinput->newReaderLeft());
    multiplication->start(QThread::NormalPriority);

    signalsaturation = new Saturation(soundcardinput->newReaderLeft(), rate);
    connect(signalsaturation, SIGNAL(perdiodOutputChanged(double)), soundcardoutput, SLOT(setOutputPeriod(double)));
    connect(signalsaturation, SIGNAL(ampliFactorChanged(double)), multiplication, SLOT(setFactor(double)));
    connect(signalsaturation, SIGNAL(saturationValueChanged(float)), this, SIGNAL(saturationValueChanged(float)));
    connect(signalsaturation, SIGNAL(rangeChanged(int)), this, SIGNAL(dynamicRangeChanged(int)));
    signalsaturation->start(QThread::LowPriority);

    ////qDebug("%s:%d", __FILE__, __LINE__);

    median = new SignalMiddle(soundcardinput->newReaderRight(), 5000);
    median->start(QThread::LowPriority);

    ////qDebug("%s:%d", __FILE__, __LINE__);

    pll = new PhaseLockedLoop(soundcardinput->newReaderRight(), median->newReader());
    pll->start(QThread::NormalPriority);

    avg = new LowPassFilter(pll->newReaderPeriod(), 150, 100);
    avg->start(QThread::LowPriority);

    readerPeriod = avg->newReader();

    ////qDebug("%s:%d", __FILE__, __LINE__);

    phaseshifterx = new PhaseShifter(pll->newReaderOutput(), multiplication->newReader(), avg->newReader());
    phaseshifterx->setPhi(0.0);
    phaseshifterx->start(QThread::NormalPriority);

    phaseshiftery = new PhaseShifter(pll->newReaderOutput(), multiplication->newReader(), avg->newReader());
    phaseshiftery->setPhi(M_PI / 2.0);
    phaseshiftery->start(QThread::NormalPriority);

    ////qDebug("%s:%d", __FILE__, __LINE__);

    multiplicationx = new SignalMultiplyer(phaseshifterx->newReaderPlus(), phaseshifterx->newReaderMinus());
    multiplicationx->start(QThread::LowPriority);

    multiplicationy = new SignalMultiplyer(phaseshiftery->newReaderPlus(), phaseshiftery->newReaderMinus());
    multiplicationy->start(QThread::LowPriority);


    avgoutputx = new LowPassFilter(multiplicationx->newReader(), integrationtime * rate, rate * outputPeriod);
    avgoutputx->start(QThread::NormalPriority);
    ////qDebug("%s:%d", __FILE__, __LINE__);

    avgoutputy = new LowPassFilter(multiplicationy->newReader(), integrationtime * rate, rate * outputPeriod);
    avgoutputy->start(QThread::NormalPriority);

    noise = new Noise(avgoutputx->newReader(), noisesamples);
    noise->start(QThread::LowPriority);
    ////qDebug("%s:%d", __FILE__, __LINE__);
}

void Lockin::rmLockin()
{
    //    delete out;
    delete noise;
    delete avgoutputy;
    delete avgoutputx;
    delete multiplicationy;
    delete multiplicationx;
    delete phaseshiftery;
    delete phaseshifterx;
    delete avg;
    delete pll;
    delete median;
    delete signalsaturation;
    delete multiplication;
    delete soundcardoutput;
    delete soundcardinput;
}
