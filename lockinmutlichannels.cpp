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

#include "lockinmutlichannels.h"
#include "lockingui.h"
#include <QMenuBar>
#include <QDockWidget>
#include <QMdiSubWindow>
#include <QInputDialog>
#include <QSettings>
#include <QTextStream>

QTextStream cout(stdout);

LockinMutliChannels::LockinMutliChannels(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("PCSC-Lockin");
    this->setWindowIcon(QIcon(":/icons/samovar-icon.png"));

    mdi = new QMdiArea(this);
    setCentralWidget(mdi);
    setMenuBar(new QMenuBar(this));

    QMenu *menu = menuBar()->addMenu("Menu");
    menu->addAction("add lockin...", this, SLOT(addLockin()), QKeySequence(QKeySequence::New));
    menu->addAction("quit", this, SLOT(close()));

    output = new DockOutput(this);
    QDockWidget *dock = new QDockWidget("Output", this);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setWidget(output);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(output, SIGNAL(cleared()), this, SLOT(outputCleared()));


    time.start();
}

void LockinMutliChannels::addLockin()
{
    QSettings settings("leso-pb_lockin", "global");
    bool ok;
    QString name = QInputDialog::getText(this, "Lockin name", "Lockin name :", QLineEdit::Normal, settings.value("lastlockinname", "").toString(), &ok);


    if (ok && !name.isEmpty()) {
        settings.setValue("lastlockinname", name);

        LockinGui *li = new LockinGui(name, this);
        li->setWindowTitle(name);
        li->setAttribute(Qt::WA_DeleteOnClose);
        mdi->addSubWindow(li);
        li->show();
        connect(li, SIGNAL(output(float,float)), this, SLOT(newValues(float,float)));
        connect(li, SIGNAL(outputFrequencyChanged()), this, SLOT(outputFr()));
        connect(li, SIGNAL(started(bool)), this, SLOT(outputFr()));
    }
}

void LockinMutliChannels::outputFr()
{
    QList<QMdiSubWindow *> subWindows = mdi->subWindowList();

    double outputFrequencyMax = -1;

    int len = subWindows.size();
    for (int i = 0; i < len; ++i) {
        LockinGui *li = qobject_cast<LockinGui *>(subWindows[i]->widget());

        if (li != 0 && li->isRunning()) {
            if (li->outputFrequency() > outputFrequencyMax)
                outputFrequencyMax = li->outputFrequency();
        }
    }

    if (timerId)
        killTimer(timerId);
    timerId = 0;

    if (outputFrequencyMax != -1) {
        timerId = startTimer(1000.0 / outputFrequencyMax);

//        qDebug("start : %d", (int)(1000.0 / outputFrequencyMax));
    }
}

void LockinMutliChannels::newValues(float x, float )
{
    QWidget *li = qobject_cast<QWidget *>(sender());

    QList<QMdiSubWindow *> sub = mdi->subWindowList();
    int index = -1;
    for (int i = 0; i < sub.size(); ++i) {
        if (sub[i]->widget() == li)
            index = i;
    }

    if (index != -1) {
        datas.prepend(QPair<int, QPointF>(index, QPointF(time.elapsed() / 1000.0, x)));
    }
}

void LockinMutliChannels::timerEvent(QTimerEvent *)
{
    double seconds = time.elapsed() / 1000.0;
    QString line;

    int size = mdi->subWindowList().size();

    for (int i = 0; i < size; ++i) {
        QPointF last, lastlast;

        int j;

        // search last point
        for (j = 0; j < datas.size(); ++j) {
            if (datas[j].first == i) {
                last = datas[j].second;
                break;
            }
        }

        // search last last point
        for (++j; j < datas.size(); ++j) {
            if (datas[j].first == i) {
                lastlast = datas[j].second;
                break;
            }
        }

        // remove others
        for (++j; j < datas.size(); ++j) {
            if (datas[j].first == i)
                datas.removeAt(j--);
        }

        line.append(" " + QString::number(last.y() + (lastlast.y() - last.y())
                                          * (seconds - last.x()) / (lastlast.x() - last.x())));
    }

    if (!line.isEmpty()) {
        line.prepend(QString::number(time.elapsed() / 1000.0));
        output->append(line);
        cout << line << endl;
    }
}

void LockinMutliChannels::outputCleared()
{
    time.start();
    cout << "clear" << endl;
}
