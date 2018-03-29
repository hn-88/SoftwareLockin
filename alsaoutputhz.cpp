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

#include "alsaoutputhz.h"
#include <QTime>
#include <math.h>

AlsaOutputHz::AlsaOutputHz(const char *device, int rate, QObject *parent)
    : QThread(parent), buffer(0), pcm(0)
{
    squareval = 127;

    if (device != 0)
        configAlsa(device, rate);
}

AlsaOutputHz::~AlsaOutputHz()
{
    //qDebug("%s:%d", __FILE__, __LINE__);
    stop();
    wait(2000);
    freeAlsa();
}

unsigned int AlsaOutputHz::rate() const
{
    return ratevalue;
}

bool AlsaOutputHz::setDevice(const char *device, int rate, QString *error)
{
    freeAlsa();

    return configAlsa(device, rate, error);
}

void AlsaOutputHz::freeAlsa()
{
    if (pcm != 0) {
        snd_pcm_drain(pcm);
        snd_pcm_close(pcm);
        pcm = 0;
    }

    if (buffer != 0) {
        delete[] buffer;
        buffer = 0;
    }
}

void AlsaOutputHz::setOutputPeriod(double newperiod)
{
    if (isinf(newperiod))
        outputperiod = 0.0;
    else
        outputperiod = newperiod / 2.0;
//    qDebug("outputperiod == %f", outputperiod);
}

void AlsaOutputHz::stop()
{
    stopnext = true;
}

void AlsaOutputHz::kill()
{
    if (pcm != 0) {
        snd_pcm_drain(pcm);
        snd_pcm_drop(pcm);
    } else {
//        qDebug("%s:%d %s (pcm == 0) !", __FILE__, __LINE__, __FUNCTION__);
    }
}

bool AlsaOutputHz::configAlsa(const char *device, int rate, QString *errstr)
{
    qDebug("%s:%s(%s, %d, %ld)", __FILE__, __FUNCTION__, device, rate, (long)errstr);
    int err;

    /* SND_PCM_STATE_OPEN */

    err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, /*SND_PCM_NONBLOCK*/0);

    if (err < 0) {
        pcm = 0;
        qDebug("snd_pcm_open: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

    snd_pcm_hw_params_alloca(&params);

    err = snd_pcm_hw_params_any(pcm, params);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_any: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

    err = snd_pcm_hw_params_set_rate_resample(pcm, params, 1);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_rate_resample: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

    err = snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_access: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

    err = snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S8);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_format: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

    err = snd_pcm_hw_params_set_channels(pcm, params, 1);
    if (err < 0) {
        qDebug("snd_pcm_hw_params_set_channels: %s", snd_strerror(err));
        if (errstr != 0)
            *errstr = QString::fromLocal8Bit(snd_strerror(err));
        return false;
    }
    //qDebug("%s:%d", __FILE__, __LINE__);

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
    //qDebug("%s:%d", __FILE__, __LINE__);


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

    //    snd_pcm_hw_params_free(params);

    buffer = new char[1 * period];

    /* SND_PCM_STATE_SETUP */

    err = snd_pcm_prepare(pcm);
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

void AlsaOutputHz::run()
{
    if (pcm == 0 || buffer == 0)
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
    int size = 1 * period;

    stopnext = false;
    QTime time;
    time.start();

    while (stopnext == false) {
        int t = (outputperiod * ratevalue) + 0.5;
//        qDebug("%f * %u == %d", outputperiod, ratevalue, t);
        for (int i = 0; i < size; ++i) {

            buffer[i] = squareval;

            if (t != 0 && ++squarepos >= t) {
                squarepos = 0;
                squareval = -squareval;
            }
        }


        err = snd_pcm_writei(pcm, buffer, period);
//                qDebug("%d ms", time.restart());

        switch (err) {
        case -EPIPE:
            qDebug("%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EBADFD:
            qDebug("%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -ESTRPIPE:
            qDebug("%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EBUSY:
            qDebug("%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, snd_strerror(err));
            snd_pcm_recover(pcm, err, 0);
            continue;
        case -EAGAIN:
            qDebug("%s:%d %s %s", __FILE__, __LINE__, __FUNCTION__, snd_strerror(err));
            continue;
        default:
            break;
        }
    }
}
