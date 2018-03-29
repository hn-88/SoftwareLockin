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

#include "lockingui.h"
#include "ui_lockingui.h"
#include <QKeyEvent>
#include <QSettings>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QTimer>
#include <QInputDialog>
#include <QTimer>

LockinGui::LockinGui(QString prefix, QWidget *parent)
    :
    QWidget(parent),
    ui(new Ui::LockinGui),
    currx(0.0),
    curry(0.0),
    currnoise(0.0),
    prefix(prefix),
    idTimerGUI(0),
    idTimerSignal(0),
    emitOutputSignal(false)
{
    //qDebug("%s:%d", __FILE__, __LINE__);
    ui->setupUi(this);

    makeDeviceList();


    lockin = new Lockin(this);
    //qDebug("%s:%d", __FILE__, __LINE__);


    loadDefaults(prefix);


    ui->vumeterInput->setLenght(1000);
    //    ui->vumeterInput->setGlobalRange(true);
    //    ui->vumeterInput->fixMinimum(0.0);
    ui->vumeterInput->addStream(lockin->newReaderChopper(), Qt::gray);
    ui->vumeterInput->addStream(lockin->newReaderSignal(), Qt::red);

    ui->vumeterOutput->setLenght(200);
    ui->vumeterOutput->setGlobalRange(true);
    //    ui->vumeterOutput->fixMinimum(0.0);
    ui->vumeterOutput->addStream(lockin->newReaderY(), QColor(Qt::gray));
    ui->vumeterOutput->addStream(lockin->newReaderX(), QColor(Qt::red));

    readerx = lockin->newReaderX();
    readery = lockin->newReaderY();
    readernoise = lockin->newReaderNoise();

    timer = new QTimer(this);
    timer->setSingleShot(true);

    makeConnexion();
}

LockinGui::~LockinGui()
{
    saveDefaults(prefix);
    delete ui;
}

MStreamReader<float> *LockinGui::newReaderX()
{
    return lockin->newReaderX();
}

MStreamReader<float> *LockinGui::newReaderY()
{
    return lockin->newReaderY();
}

float LockinGui::currentX() const
{
    return currx;
}

float LockinGui::currentY() const
{
    return curry;
}

double LockinGui::currentNoise() const
{
    return currnoise;
}

bool LockinGui::isRunning() const
{
    return (idTimerGUI != 0);
}

bool LockinGui::isManualRange() const
{
    return ui->manualrange->isChecked();
}

void LockinGui::autoPhase()
{
    guiAutoPhase();
}

void LockinGui::setIntegrationTime(double time)
{
    ui->integrationtime->setValue(time);
}

void LockinGui::setOutputFrequency(double frequency)
{
    ui->outputfrequency->setValue(frequency);
}

double LockinGui::outputFrequency() const
{
    return ui->outputfrequency->value();
}

void LockinGui::setNbrSampleForNoise(int nbrSample)
{
    ui->noisesamples->setValue(nbrSample);
}

void LockinGui::timerEvent(QTimerEvent *event)
{
    int size;
    bool gui = (event->timerId() == idTimerGUI);

//    qDebug("%d %d", event->timerId(), idTimerGUI);


    // Y value
    size = readery->size();
    if (size > 1)
        readery->skip(size - 1);

    if (size > 0) {
        readery->take(curry);
    }

    if (gui) {
        guiShowValue(curry, ui->labely, ui->progressBarY, ui->labelpowy);
    }


    // X value
    size = readerx->size();
    if (size > 1)
        readerx->skip(size - 1);

    if (size > 0) {
        readerx->take(currx);

        if (emitOutputSignal) {
//            static QTime time;
//            qDebug("%d emit %f %f (gui %d)", time.restart(), currx, curry, gui);
            emit output(currx, curry);
        }
    }

    if (gui) {
        guiShowValue(currx, ui->labelx, ui->progressBarX, ui->labelpowx);

        autophasevalue = 360.0 / 2.0 / M_PI * atan(curry / currx);

        if (currx < 0.0) {
            autophasevalue += 180.0;
        }

        if (isnan(autophasevalue))
            autophasevalue = 0.0;

        while (autophasevalue > 180.0)
            autophasevalue -= 360.0;
        while (autophasevalue <= -180.0)
            autophasevalue += 360.0;

        ui->autophase->setText(((autophasevalue > 0.0)?"+":"") + QString::fromUtf8("%1°").arg(floor(autophasevalue * 100.0 + 0.5) / 100.0));
    }


    // Noise + Chopper Frequency
    if (gui) {
        size = readernoise->size();
        if (size > 1)
            readernoise->skip(size - 1);

        if (size > 0) {
            readernoise->take(currnoise);
            ui->noise->setText(QString::number(floor(currnoise * 100000.0 + 0.5) / 100.0) +QString::fromUtf8(" ‰"));
        }

        ui->chopperfrequency->setText(QString("%1 Hz").arg(lockin->chopperFrequency()));
    }
}

void LockinGui::enableOutputSignal()
{
    emitOutputSignal = true;
}

void LockinGui::guiStartStopOpen()
{

    if (ui->startstop->text() == "stop") {
        if (lockin->stopAlsa()) {
            lockin->closeAlsa();
            ui->startstop->setText("open");
            ui->combodevice->setEnabled(true);
            ui->combooutput->setEnabled(true);;
            ui->rate->setEnabled(true);
            killTimer(idTimerGUI); idTimerGUI = 0;
            killTimer(idTimerSignal); idTimerSignal = 0;
            currx = curry = 0.0;

            emitOutputSignal = false;
            emit started(false);
        } else {
            QMessageBox::critical(this, "Alsa thread error", "Cannot stop the alsa thread.");
        }

        return;
    }

    if (ui->startstop->text() == "start") {
        guiSetOutput();
        guiSetNoise();
        guiSetDynamicRange(ui->comborange->currentIndex());
        guiManualRangeClicked();
        guiSetAngle(ui->phi->value());
        guiSetPll();

        lockin->startAlsa();

        ui->startstop->setText("stop");
        ui->combodevice->setEnabled(false);
        ui->combooutput->setEnabled(false);
        ui->rate->setEnabled(false);

        if (idTimerGUI == 0)
            idTimerGUI = startTimer(100);
        Q_ASSERT(idTimerGUI != 0);
        if (idTimerSignal == 0)
            idTimerSignal = startTimer(1000.0 / ui->outputfrequency->value());
        Q_ASSERT(idTimerSignal != 0);

        emitOutputSignal = true;
        emit started(true);

        return;
    }


    if (ui->startstop->text() == "open") {
        guiSetDevice();
        bool amplificationEnabled = ui->combooutput->currentIndex() != 0;
        ui->tab_ampli->setEnabled(amplificationEnabled);

        return;
    }
}

void LockinGui::guiShowValue(float value, QLabel *label, QProgressBar *bar, QLabel *labelpow)
{
    label->setText(QString::number(value));

    bar->setInvertedAppearance(value < 0.0);

    value = qAbs(value);

    int pow = 0;
    double x = 1.0;

    if (value > x) {
        while (value > x) {
            pow++;
            x *= 10.0;
        }
    } else {
        while (value < x) {
            pow--;
            x /= 10.0;
        }
    }

    labelpow->setText(QString("10<span style=\" vertical-align:super;\">%1</span>").arg(pow));

    int v = value / x * 100.0 + 0.5;
    bar->setMaximum(1000);
    bar->setValue(v);

}

void LockinGui::guiSetOutput()
{
    double integration = ui->integrationtime->value();
    double period = 1.0 / ui->outputfrequency->value();
    bool avgmode = ui->lowpassmode->currentIndex() == 0;

    if (idTimerSignal != 0)
        killTimer(idTimerSignal);

    int ms = 1000.0 / ui->outputfrequency->value();
    idTimerSignal = startTimer(ms);

    lockin->setOutput(integration, period, avgmode);

    emitOutputSignal = false;
    timer->start(integration * 1000.0);

    // Application asked to unregister timer 0x8086ff4 which is not registered in this thread. Fix application.
    // this bug is solve in Qt 4.7.4
    emit outputFrequencyChanged();
}

void LockinGui::guiSetNoise()
{
    lockin->setNoiseSamples(ui->noisesamples->value());
}

void LockinGui::guiSetPll(int shape)
{
    switch (shape) {
    case 0:
        lockin->setPllShape(PhaseLockedLoop::SINE);
        break;
    case 1:
        lockin->setPllShape(PhaseLockedLoop::SQUARE);
        break;
    case 2:
        lockin->setPllShape(PhaseLockedLoop::TRIANGLE);
        break;
    default:
        guiSetPll(ui->pllshape->currentIndex());
        break;
    }
}

void LockinGui::guiSetDevice()
{
    QString error;
    int realrate = lockin->setDevice(ui->combodevice->itemData(ui->combodevice->currentIndex()).toString(),
                                     ui->combooutput->itemData(ui->combooutput->currentIndex()).toString(), ui->rate->value(), &error);

    if (realrate < 0) {
        QMessageBox::critical(this, "Device error", error);
    } else {
        ui->rate->setValue(realrate);

        ui->startstop->setText("start");
    }
}

void LockinGui::guiDevOptChanged()
{
    ui->startstop->setText("open");
}

void LockinGui::guiSetAngle(double phi)
{
    ui->dial->setValue(phi + 0.5);
    double rad = phi / 360.0 * 2.0 * M_PI;
    lockin->setPhi(rad);
}

void LockinGui::guiDialMoved(int phi)
{
    if (ui->dial->hasFocus())
        ui->phi->setValue(phi);
}

void LockinGui::guiPlusNinety()
{
    double phi = ui->phi->value();
    phi += 90.0;
    if (phi >= 360.0)
        phi -= 360.0;
    ui->phi->setValue(phi);
}

void LockinGui::guiMinusNinety()
{
    double phi = ui->phi->value();
    phi -= 90.0;
    if (phi < 0.0)
        phi += 360.0;
    ui->phi->setValue(phi);
}

void LockinGui::guiAutoPhase()
{
    emitOutputSignal = false;
    timer->start(ui->integrationtime->value() * 1000.0);

    double phi = ui->phi->value();
    phi += autophasevalue;
    if (phi < 0.0)
        phi += 360.0;
    if (phi >= 360.0)
        phi -= 360.0;
    ui->phi->setValue(phi);
}

void LockinGui::guiNewSaturation(float s)
{
    ui->progressBarAmp->setValue(1000.0 * s);
    ui->labelsaturation->setText(QString::number(s));
}

void LockinGui::guiManualRangeClicked()
{
    const bool manual = ui->manualrange->isChecked();
    lockin->setAutoDynamicRange(!manual);
}

void LockinGui::guiSetDynamicRange(int range)
{
    if (ui->manualrange->isEnabled()) {

        const bool manual = ui->manualrange->isChecked();

        if (manual && range != -1) {
            lockin->selectGainRange(range);
        }
    }
}

void LockinGui::guiEditDynamicRanges()
{
    bool ok;
    QString gainranges = lockin->getGainRanges();
    gainranges = QInputDialog::getText(this, "Ranges", "Ranges : <li>gain:frequency;gain:frequency;...</li><br />exemple : <li>1:0;10:400;100:1200</li>", QLineEdit::Normal, gainranges, &ok);

    if (ok) {
        lockin->setGainRanges(gainranges);
        gainranges = lockin->getGainRanges();

        QStringList ranges = gainranges.split(';');
        ui->comborange->clear();
        for (int i = 0; i < ranges.size(); ++i) {
            ui->comborange->addItem("x"+ranges[i]+"Hz");
        }
    }
}

void LockinGui::guiAutoDynamicRangeChanged(int i)
{
    ui->comborange->setCurrentIndex(i);
    emitOutputSignal = false;
    timer->start(qMax<int>(3000, ui->integrationtime->value() * 1000.0));
}

void LockinGui::makeConnexion()
{
    connect(timer, SIGNAL(timeout()), this, SLOT(enableOutputSignal()));

    connect(ui->integrationtime, SIGNAL(valueChanged(double)), this, SLOT(guiSetOutput()));
    connect(ui->outputfrequency, SIGNAL(valueChanged(double)), this, SLOT(guiSetOutput()));
    connect(ui->lowpassmode, SIGNAL(currentIndexChanged(int)), this, SLOT(guiSetOutput()));

    connect(ui->phi, SIGNAL(valueChanged(double)), this, SLOT(guiSetAngle(double)));
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(guiDialMoved(int)));

    connect(ui->combodevice, SIGNAL(activated(int)), this, SLOT(guiDevOptChanged()));
    connect(ui->combooutput, SIGNAL(activated(int)), this, SLOT(guiDevOptChanged()));
    connect(ui->rate, SIGNAL(valueChanged(int)), this, SLOT(guiDevOptChanged()));
    connect(ui->startstop, SIGNAL(clicked()), this, SLOT(guiStartStopOpen()));

    connect(ui->plusninety, SIGNAL(clicked()), this, SLOT(guiPlusNinety()));
    connect(ui->minusninety, SIGNAL(clicked()), this, SLOT(guiMinusNinety()));
    connect(ui->autophase, SIGNAL(clicked()), this, SLOT(guiAutoPhase()));

    connect(ui->pllshape, SIGNAL(currentIndexChanged(int)), this, SLOT(guiSetPll(int)));

    connect(lockin, SIGNAL(saturationValueChanged(float)), this, SLOT(guiNewSaturation(float)));
    connect(ui->manualrange, SIGNAL(clicked()), this, SLOT(guiManualRangeClicked()));
    connect(ui->comborange, SIGNAL(currentIndexChanged(int)), this, SLOT(guiSetDynamicRange(int)));

    connect(ui->editranges, SIGNAL(clicked()), this, SLOT(guiEditDynamicRanges()));

    connect(lockin, SIGNAL(dynamicRangeChanged(int)), this, SLOT(guiAutoDynamicRangeChanged(int)));

    connect(ui->noisesamples, SIGNAL(valueChanged(int)), this, SLOT(guiSetNoise()));
}

void LockinGui::loadDefaults(QString prefix)
{
    QSettings settings("leso-pb_lockin", "li_"+prefix);
    ui->combodevice->setCurrentIndex(settings.value("device", 0).toInt());
    ui->combooutput->setCurrentIndex(settings.value("output", 0).toInt());
    ui->rate->setValue(settings.value("rate", 44100).toInt());
    ui->integrationtime->setValue(settings.value("integrationtime", 0.5).toDouble());
    ui->outputfrequency->setValue(settings.value("outputfrequency", 10).toDouble());
    ui->lowpassmode->setCurrentIndex(settings.value("lowpassmode", 0).toInt());
    ui->phi->setValue(settings.value("angle", 0.0).toDouble());
    ui->dial->setValue(ui->phi->value() + 0.5);
    ui->pllshape->setCurrentIndex(settings.value("pllshape", 0).toInt());
    ui->manualrange->setChecked(settings.value("manualrange", false).toBool());
    ui->noisesamples->setValue(settings.value("noisesamples", 20).toInt());
    loadGainRangeList();
}

void LockinGui::saveDefaults(QString prefix)
{
    QSettings settings("leso-pb_lockin", "li_"+prefix);
    settings.setValue("device", ui->combodevice->currentIndex());
    settings.setValue("output", ui->combooutput->currentIndex());
    settings.setValue("rate", ui->rate->value());
    settings.setValue("integrationtime", ui->integrationtime->value());
    settings.setValue("outputfrequency", ui->outputfrequency->value());
    settings.setValue("lowpassmode", ui->lowpassmode->currentIndex());
    settings.setValue("angle", ui->phi->value());
    settings.setValue("pllshape", ui->pllshape->currentIndex());
    settings.setValue("manualrange", ui->manualrange->isChecked());
    settings.setValue("currentrange", ui->comborange->currentIndex());
    settings.setValue("noisesamples", ui->noisesamples->value());
    saveGainRangeList();
}

void LockinGui::makeDeviceList()
{
    const QString alsadir = "/proc/asound/";

    QDir dir(alsadir);
    if (!dir.exists()) {
        QMessageBox::critical(this, alsadir, QString("the directory <i>%1</i> cannot be open.").arg(alsadir));
        return;
    }
    QStringList cards = dir.entryList(QStringList() << "card*", QDir::Dirs);
    for (int i = 0; i < cards.size(); ++i) {
        int card = cards[i].mid(4).toInt();
        dir.cd(cards[i]);
        QStringList pcms = dir.entryList(QStringList() << "pcm*", QDir::Dirs);
        for (int j = 0; j < pcms.size(); ++j) {
            int pcm = pcms[j].mid(3, pcms[j].size() - 4).toInt();

            // simplify
            if (pcm != 0)
                continue;


            bool inputDev = pcms[j].endsWith('c');
            bool outputDev = pcms[j].endsWith('p');
            dir.cd(pcms[j]);
            //QStringList subs = dir.entryList(QStringList() << "sub*", QDir::Dirs);
            //for (int k = 0; k < subs.size(); ++k) {
                //int sub = subs[k].mid(3).toInt();
                //dir.cd(subs[k]);
                QFile info(dir.filePath("info"));
                if (!info.open(QIODevice::ReadOnly)) {
                    QMessageBox::critical(this, info.fileName(), QString("the info file <i>%1</i> cannot be open.<br /><i>%2</i>").arg(info.fileName()).arg(info.errorString()));
                } else {
                    const QString data = QString(info.readAll());
                    const QString name = data.section("name: ", 1).section("\n", 0, 0);

                    const QString plug = QString("plughw:%1,%2").arg(card).arg(pcm)/*.arg(sub)*/;

                    if (inputDev) {
                        ui->combodevice->addItem(QIcon(":icons/mic-icon.png"), QString("%1 [%2,%3]").arg(name).arg(card).arg(pcm)/*.arg(sub)*/, plug);
                    }
                    if (outputDev) {
                        ui->combooutput->addItem(QIcon(":icons/sound-icon.png"), QString("%1 [%2,%3]").arg(name).arg(card).arg(pcm)/*.arg(sub)*/, plug);
                    }
                    info.close();
                }
                //dir.cdUp();
            //}
            dir.cdUp();
        }
        dir.cdUp();
    }
}

void LockinGui::loadGainRangeList()
{
    QSettings localsettings("leso-pb_lockin", "li_"+prefix);
    QString gainranges = localsettings.value("gainrange", "").toString();

    if (gainranges.isEmpty()) {
        QSettings globalsettings("leso-pb_lockin", "global");
        gainranges = globalsettings.value("gainrange", "1:0").toString();
    }

    lockin->setGainRanges(gainranges);

    QStringList ranges = gainranges.split(';');
    for (int i = 0; i < ranges.size(); ++i) {
        ui->comborange->addItem("x"+ranges[i]+"Hz");
    }

    ui->comborange->setCurrentIndex(localsettings.value("currentrange", 0).toInt());
}

void LockinGui::saveGainRangeList()
{
    QSettings localsettings("leso-pb_lockin", "li_"+prefix);
    localsettings.setValue("gainrange", lockin->getGainRanges());
    QSettings globalsettings("leso-pb_lockin", "global");
    globalsettings.setValue("gainrange", lockin->getGainRanges());
}
