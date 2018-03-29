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

#include "alsainput.h"
#include <QTime>
#include <math.h>

AlsaInput::AlsaInput(const char *device, int rate, QObject *parent)
    : QThread(parent), pcm(0), buf(0)
{
    if (device != 0)
        configAlsa(device, rate);
}

AlsaInput::~AlsaInput()
{
    stop();
    wait(10000);
    kill();
    wait(10000);
    freeAlsa();
}

MStreamReader<float> *AlsaInput::newReaderLeft()
{
    return new MStreamReader<float>(&channels[0]);
}

MStreamReader<float> *AlsaInput::newReaderRight()
{
    return new MStreamReader<float>(&channels[1]);
}

unsigned int AlsaInput::rate() const
{
    return ratevalue;
}

bool AlsaInput::setDevice(const char *device, int rate, QString *error)
{
    freeAlsa();

    return configAlsa(device, rate, error);
}

void AlsaInput::stop()
{
    stopnext = true;
}

void AlsaInput::kill()
{
    if (pcm != 0) {
        snd_pcm_drain(pcm);
        snd_pcm_drop(pcm);
    } else {
//        qDebug("(pcm == 0) !");
    }
}

void AlsaInput::run()
{
    if (pcm == 0 || buf == 0)
        return;

    snd_pcm_state_t state = snd_pcm_state(pcm);
    switch (state) {
    case SND_PCM_STATE_OPEN:
        qDebug("Open");
        break;
    case SND_PCM_STATE_SETUP:
        qDebug("Setup installed");
        break;
    case SND_PCM_STATE_PREPARED:
        qDebug("Ready to start");
        break;
    case SND_PCM_STATE_RUNNING:
        qDebug("Running");
        break;
    case SND_PCM_STATE_XRUN:
        qDebug("Stopped: underrun (playback) or overrun (capture) detected");
        snd_pcm_recover(pcm, -EPIPE, 0);
        break;
    case SND_PCM_STATE_DRAINING:
        qDebug("Draining: running (playback) or stopped (capture)");
        break;
    case SND_PCM_STATE_PAUSED:
        qDebug("Paused");
        break;
    case SND_PCM_STATE_SUSPENDED:
        qDebug("Hardware is suspended (stop thread)");
        return;
    case SND_PCM_STATE_DISCONNECTED:
        qDebug("Hardware is disconnected (stop thread)");
        return;
    }

    int err;
    int size = 2 * period;

    //    int sleep = 500 * period / ratevalue;
//    QTime time;


    stopnext = false;
    int cnt = 12;


    while (stopnext == false) {


        err = snd_pcm_readi(pcm, buf, period);
        //        qDebug("%dms %d", time.restart(), err);


        switch (err) {
        case -EPIPE:
            qDebug("%s", snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EBADFD:
            qDebug("%s", snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -ESTRPIPE:
            qDebug("%s", snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EBUSY:
            qDebug("%s", snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EAGAIN:
            qDebug("%s", snd_strerror(err));
            //            usleep(100);
            continue;
        default:
            break;
        }


        for (int i = 0, j = 0; i < size; i += 2, ++j) {
            buffer0[j] = buf[i];
            buffer1[j] = buf[i + 1];

//            double x = (double)(cnt % 100);
//            double x = (double)((cnt % 200) - 100);
//            x /= 100.0;
//            x *= 2.0 * M_PI;
//            qDebug("%f", x);
//            buffer0[j] = sin(x);
//            buffer1[j] = sin(x);
            //            buffer0[j] = (cnt % 100) < 50 - 0.5;
            //            buffer1[j] = (cnt % 100) < 50 - 0.5;

            cnt++;
        }
        channels[0].write(buffer0, period);
        channels[1].write(buffer1, period);

    }
}

bool AlsaInput::configAlsa(const char *device, int rate, QString *errstr)
{
    qDebug("%s:%s(%s, %d, %ld)", __FILE__, __FUNCTION__, device, rate, (long)errstr);

    int err;

    /* SND_PCM_STATE_OPEN */

    err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_CAPTURE, /*SND_PCM_NONBLOCK*/0);

    if (err < 0) {
        pcm = 0;
        qDebug("snd_pcm_open: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    snd_pcm_hw_params_alloca(&params);

    err = snd_pcm_hw_params_any(pcm, params);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_any: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    err = snd_pcm_hw_params_set_rate_resample(pcm, params, 1);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_rate_resample: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    err = snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_access: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    err = snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_FLOAT);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_format: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    err = snd_pcm_hw_params_set_channels(pcm, params, 2);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_channels: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    ratevalue = rate;
    // ratevalue = 192000;
    // ratevalue = 96000;
    // ratevalue = 44100;
    err = snd_pcm_hw_params_set_rate_near(pcm, params, &ratevalue, 0);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_rate_last: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    } else {
        qDebug("rate: %u", ratevalue);
    }


    period = ratevalue / 10;
    // period = 19200;
    // period = 9600;
    //    period = 4410;
    err = snd_pcm_hw_params_set_period_size_near(pcm, params, &period, 0);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_period_size_near: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    int dir;
    err = snd_pcm_hw_params_get_period_size(params, &period, &dir);
    if (err == 0) {
        qDebug("period: %ld", period);
    }

    snd_pcm_uframes_t buffersize = 4 * period;

    err = snd_pcm_hw_params_set_buffer_size_near(pcm, params, &buffersize);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_buffer_size_near: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }

    err = snd_pcm_hw_params_get_buffer_size(params, &buffersize);
    if (err == 0) {
        qDebug("buffersize: %lu", buffersize);
    }


    err = snd_pcm_hw_params(pcm, params);
    if (err < 0) {
        qDebug("snd_pcm_hw_params: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }


    buf = new float[2 * period];
    buffer0 = new float[period];
    buffer1 = new float[period];

    /* SND_PCM_STATE_SETUP */

    err = snd_pcm_prepare(pcm);
    if (err < 0) {
        qDebug("snd_pcm_prepare: %s", snd_strerror(err));
    }

    /* SND_PCM_STATE_PREPARE */

    err = snd_pcm_start(pcm);
    if (err < 0) {
        qDebug("snd_pcm_prepare: %s", snd_strerror(err));
    }




    snd_pcm_state_t state = snd_pcm_state(pcm);
    switch (state) {
    case SND_PCM_STATE_OPEN:
        qDebug("Open");
        break;
    case SND_PCM_STATE_SETUP:
        qDebug("Setup installed");
        break;
    case SND_PCM_STATE_PREPARED:
        qDebug("Ready to start");
        break;
    case SND_PCM_STATE_RUNNING:
        qDebug("Running");
        break;
    case SND_PCM_STATE_XRUN:
        qDebug("Stopped: underrun (playback) or overrun (capture) detected");
        break;
    case SND_PCM_STATE_DRAINING:
        qDebug("Draining: running (playback) or stopped (capture)");
        break;
    case SND_PCM_STATE_PAUSED:
        qDebug("Paused");
        break;
    case SND_PCM_STATE_SUSPENDED:
        qDebug("Hardware is suspended (stop thread)");
        return false;
    case SND_PCM_STATE_DISCONNECTED:
        qDebug("Hardware is disconnected (stop thread)");
        return false;
    }

    return true;
}

void AlsaInput::freeAlsa()
{
    if (pcm != 0) {
        snd_pcm_drain(pcm);
        snd_pcm_close(pcm);
        pcm = 0;
    }

    if (buf != 0) {
        delete[] buf;
        buf = 0;

        delete[] buffer0;
        delete[] buffer1;
    }
}
