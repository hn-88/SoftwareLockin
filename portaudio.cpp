#include <QtGlobal>
#include <portaudio.h>

void configure(int device, PaStream **stream)
{
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        qDebug("Pa_Initialize: %s", Pa_GetErrorText(err));
        return;
    }

    const PaDeviceInfo *deviceinfo = Pa_GetDeviceInfo(device);

    PaStreamParameters input = {0, 2, paFloat32, deviceinfo->defaultHighInputLatency, NULL};

    err = Pa_OpenStream(stream, &input, NULL, deviceinfo->defaultSampleRate, 0, paNoFlag, NULL, NULL);
    if (err != paNoError) {
        qDebug("Pa_OpenStream: %s", Pa_GetErrorText(err));
        return;
    }

    const PaStreamInfo *streaminfo = Pa_GetStreamInfo(*stream);
    qDebug("inputLatency : %f", streaminfo->inputLatency);

}

void read(PaStream *stream)
{
    PaError err;

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        qDebug("Pa_StartStream: %s", Pa_GetErrorText(err));
        return;
    }

    int bufferSize = 16;
    float *buffer = new float[2 * bufferSize];

    for (int i = 0; i < 1; ++i) {
        err = Pa_ReadStream(stream, buffer, bufferSize);
        if (err != paNoError) {
            qDebug("Pa_ReadStream: %s", Pa_GetErrorText(err));
            return;
        }
        for (int j = 0; j < bufferSize; ++j) {
            qDebug("%f", buffer[j]);
        }
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        qDebug("Pa_StopStream: %s", Pa_GetErrorText(err));
        return;
    }
}

void freeaudio(PaStream *stream)
{
    PaError err;

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        qDebug("Pa_CloseStream: %s", Pa_GetErrorText(err));
        return;
    }

    err = Pa_Terminate();
    if (err != paNoError) {
        qDebug("Pa_Terminate: %s", Pa_GetErrorText(err));
        return;
    }
}

int main(int , char *[])
{
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        qDebug("Pa_Initialize: %s", Pa_GetErrorText(err));
        return 1;
    }
    qDebug("%s\n", Pa_GetVersionText());

    PaDeviceIndex count = Pa_GetDeviceCount();

    for (int i = 0; i < count; ++i) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
        if (info->maxInputChannels >= 2)
            continue;
        qDebug("name                     : %s", info->name);
        qDebug("defaultHighInputLatency  : %f", info->defaultHighInputLatency);
        qDebug("defaultHighOutputLatency : %f", info->defaultHighOutputLatency);
        qDebug("defaultLowInputLatency   : %f", info->defaultLowInputLatency);
        qDebug("defaultLowOutputLatency  : %f", info->defaultLowOutputLatency);
        qDebug("defaultSampleRate        : %f", info->defaultSampleRate);
        qDebug("hostApi                  : %d", info->hostApi);
        qDebug("maxInputChannels         : %d", info->maxInputChannels);
        qDebug("maxOutputChannels        : %d", info->maxOutputChannels);
        qDebug("structVersion            : %d\n", info->structVersion);
    }
    err = Pa_Terminate();
    if (err != paNoError) {
        qDebug("Pa_Terminate: %s", Pa_GetErrorText(err));
        return 1;
    }


    PaStream *s1;
    PaStream *s2;


    configure(0, &s1);
    configure(0, &s2);

    read(s1);
    read(s2);

    freeaudio(s1);
    freeaudio(s2);
    return 0;
}
