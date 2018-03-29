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

#include "vumeter.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <math.h>

Vumeter::Vumeter(int lenght, bool globalrange, QWidget *parent)
    : QGLWidget(parent), lenght(lenght), pause(false), globalrange(globalrange), fixmin(false), fixmax(false)
{
    startTimer(40);
    setFocusPolicy(Qt::ClickFocus);
}

Vumeter::Vumeter(QWidget *parent)
    : QGLWidget(parent), lenght(500), pause(false), globalrange(false), fixmin(false), fixmax(false)
{
    startTimer(40);
    setFocusPolicy(Qt::ClickFocus);
}

Vumeter::~Vumeter()
{
    for (int i = 0; i < channels.size(); ++i) {
        delete[] channels[i].circbuffer;
    }
}

void Vumeter::addStream(MStreamReader<float> *str, QColor color)
{
    struct Channel ch = {str, color, new float[lenght], 0, false, 0.0, 0.0};
    channels.append(ch);
}

void Vumeter::fixMinimum(float min)
{
    fixmin = true;
    gmin = min;
}

void Vumeter::fixMaximum(float max)
{
    fixmax = true;
    gmax = max;
}

void Vumeter::setLenght(int lenght)
{
    mutex.lock();

    for (int i = 0; i < channels.size(); ++i) {
        float *tab = new float[lenght];
        int pos = (channels[i].full) ? qMin(this->lenght, lenght) : qMin(channels[i].pos, lenght);
        for (int j = pos - 1; j >= 0; --j) {
            if (channels[i].pos <= 0)
                channels[i].pos = this->lenght - 1;
            tab[j] = channels[i].circbuffer[--channels[i].pos];
        }
        delete[] channels[i].circbuffer;
        channels[i].circbuffer = tab;

        channels[i].full = (pos == lenght);
        channels[i].pos = channels[i].full ? 0 : pos;
    }

    this->lenght = lenght;
    mutex.unlock();
}

void Vumeter::setGlobalRange(bool global)
{
    this->globalrange = global;
}

void Vumeter::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_LINE_SMOOTH);
}

void Vumeter::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(0, w, h, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

void Vumeter::paintGL()
{
    if (pause == true)
        return;

    mutex.lock();

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (channels.isEmpty())
        return;

    float heightv = height();
    float pmin = heightv * 0.95;
    float pmax = heightv * 0.05;

    if (globalrange) {
        float cmin = gmin;
        float cmax = gmax;

        if (!fixmin) {
            cmin = channels[0].circbuffer[0];
            for (int i = 0; i < channels.size(); ++i) {
                int end = channels[i].full ? lenght : channels[i].pos;
                for (int j = 0; j < end; ++j) {
                    if (channels[i].circbuffer[j] < cmin)
                        cmin = channels[i].circbuffer[j];
                }
            }
        }

        if (!fixmax) {
            cmax = channels[0].circbuffer[0];
            for (int i = 0; i < channels.size(); ++i) {
                int end = channels[i].full ? lenght : channels[i].pos;
                for (int j = 0; j < end; ++j) {
                    if (channels[i].circbuffer[j] > cmax)
                        cmax = channels[i].circbuffer[j];
                }
            }
        }

        glBegin(GL_LINE_STRIP);
        glColor3f(0.3, 0.3, 0.3);
        float yzero = yval(0.0, cmin, cmax, pmin, pmax);
        glVertex2f(0.0, yzero);
        glVertex2f(width(), yzero);
        glEnd();

        for (int i = 0; i < channels.size(); ++i) {
            drawChannel(i, cmin, cmax, pmin, pmax);
        }
    } else {
        for (int i = 0; i < channels.size(); ++i) {
            float cmin = gmin;
            float cmax = gmax;

            if (!fixmin) {
                cmin = channels[i].circbuffer[0];
                for (int j = 0; j < lenght; ++j) {
                    if (channels[i].circbuffer[j] < cmin)
                        cmin = channels[i].circbuffer[j];
                }
            }

            if (!fixmax) {
                cmax = channels[i].circbuffer[0];
                for (int j = 0; j < lenght; ++j) {
                    if (channels[i].circbuffer[j] > cmax)
                        cmax = channels[i].circbuffer[j];
                }
            }

            drawChannel(i, cmin, cmax, pmin, pmax);
        }
    }

    mutex.unlock();

    glFlush();
}

inline float Vumeter::yval(float y, float min, float max, float pixelmin, float pixelmax)
{
    return (y - min) / (max - min) * (pixelmax - pixelmin) + pixelmin;
}

void Vumeter::drawChannel(int i, float cmin, float cmax, float pmin, float pmax)
{
    glBegin(GL_LINE_STRIP);
    glColor3f(channels[i].color.redF(), channels[i].color.greenF(), channels[i].color.blueF());

    float x = 0.0;
    float dx = ((float)width() - 1.0) / ((float)lenght - 1.0);

    if (channels[i].full) {
        for (int j = channels[i].pos; j < lenght; ++j) {
            const float y = channels[i].circbuffer[j];
            glVertex2f(x, yval(y, cmin, cmax, pmin, pmax));
            x += dx;
        }
    }
    for (int j = 0; j < channels[i].pos; ++j) {
        const float y = channels[i].circbuffer[j];
        glVertex2f(x, yval(y, cmin, cmax, pmin, pmax));
        x += dx;
    }

    glEnd();
}

void Vumeter::timerEvent(QTimerEvent *)
{
    mutex.lock();
    int minimum = channels.first().str->size();
    for (int i = 1; i < channels.size(); ++i) {
        int size = channels[i].str->size();
        if (size < minimum)
            minimum = size;
    }

    if (minimum > lenght) {
        for (int i = 0; i < channels.size(); ++i) {
            channels[i].str->skip(minimum - lenght);
            channels[i].pos = 0;
        }
        minimum = lenght;
    }


    for (int i = 0; i < channels.size(); ++i) {
        int max = lenght - channels[i].pos;

        if (max >= minimum) {
            channels[i].str->read(channels[i].circbuffer + channels[i].pos, minimum);
            channels[i].pos += minimum;
            if (channels[i].pos >= lenght) {
                channels[i].pos = 0;
                channels[i].full = true;
            }
        } else {
            channels[i].str->read(channels[i].circbuffer + channels[i].pos, max);
            channels[i].str->read(channels[i].circbuffer, minimum - max);
            channels[i].pos = minimum - max;
            channels[i].full = true;
        }

        for (int j = 0; j < lenght; ++j) {
            if (isnan(channels[i].circbuffer[j]) || isinf(channels[i].circbuffer[j]))
                channels[i].circbuffer[j] = 0.0;
        }
    }

    mutex.unlock();
    updateGL();
}

void Vumeter::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Pause:
    case Qt::Key_P:
        pause = !pause;
        break;
    }
}

void Vumeter::mousePressEvent(QMouseEvent *)
{
    pause = !pause;
}

void Vumeter::mouseDoubleClickEvent(QMouseEvent *)
{
    for (int i = 0; i < channels.size(); ++i) {
        channels[i].pos = 0;
        channels[i].full = false;
    }
    pause = !pause; // double-click make a pause and double cancle it
}

void Vumeter::wheelEvent(QWheelEvent *e)
{
    int newlenght;

    if (e->delta() > 0)
        newlenght = lenght * 1.05 + 0.5;
    else
        newlenght = lenght * 0.95 + 0.5;

    if (newlenght < 10)
        return;

    setLenght(newlenght);
//    qDebug("%d", newlenght);
}
