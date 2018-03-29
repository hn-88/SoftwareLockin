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

#ifndef LOCKINGUI_H
#define LOCKINGUI_H

#include <QWidget>
#include <QTimer>
#include "lockin.h"

class QProgressBar;
class QLabel;

namespace Ui {
    class LockinGui;
}

class LockinGui : public QWidget
{
    Q_OBJECT

public:
    LockinGui(QString prefix = "def", QWidget *parent = 0);
    ~LockinGui();

    MStreamReader<float> *newReaderX();
    MStreamReader<float> *newReaderY();

    float currentX() const;
    float currentY() const;
    double currentNoise() const;

    bool isRunning() const;
    bool isManualRange() const;

    void autoPhase();

    void setIntegrationTime(double time);
    void setOutputFrequency(double frequency);
    double outputFrequency() const;
    void setNbrSampleForNoise(int nbrSample);


signals:
    void output(float x, float y);
    void outputFrequencyChanged();
    void started(bool);

private:
    void makeConnexion();
    void loadDefaults(QString prefix);
    void saveDefaults(QString prefix);
    void makeDeviceList();
    void loadGainRangeList();
    void saveGainRangeList();

    void timerEvent(QTimerEvent *);

    void guiShowValue(float value, QLabel *label, QProgressBar *bar, QLabel *labelpow);

    Ui::LockinGui *ui;

    Lockin *lockin;
    MStreamReader<float> *readerx;
    MStreamReader<float> *readery;
    MStreamReader<double> *readernoise;

    float currx;
    float curry;
    double currnoise;

    QString prefix;

    int idTimerGUI;
    int idTimerSignal;

    bool emitOutputSignal;

    double autophasevalue;

    QTimer *timer;

private slots:
    void enableOutputSignal();
    void guiSetNoise();
    void guiSetOutput();
    void guiSetPll(int = -1);
    void guiSetDevice();
    void guiDevOptChanged();
    void guiStartStopOpen();
    void guiSetAngle(double phi);
    void guiDialMoved(int value);
    void guiPlusNinety();
    void guiMinusNinety();
    void guiAutoPhase();
    void guiNewSaturation(float);
    void guiManualRangeClicked();
    void guiSetDynamicRange(int);
    void guiEditDynamicRanges();
    void guiAutoDynamicRangeChanged(int);
};

#endif // LOCKINGUI_H
