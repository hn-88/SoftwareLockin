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

#ifndef __MSTREAM_H__
#define __MSTREAM_H__

#include <QList>
#include <QVector>
#include <QMutex>

/*

 Circular buffer with multiple reader points without security
 if a reader is too slow, the writer might to erase value not still read by the reader.

 */

template <class T> class MStreamReader;


/*
 Writer class
 */

template <class T> class MStream {
public:
    explicit MStream(int bufferSize = 0);
    virtual ~MStream();

    int write(const T *tab, int max);
    void push(T newvalue);


private:
    T *d;
    int size;
    int wptr;
    bool full;

    friend class MStreamReader<T>;
    QVector<MStreamReader<T> *> readers;

    QMutex mutex;
};




/*
 Reader class
 */

template <class T> class MStreamReader {
public:
    explicit MStreamReader(MStream<T> *stream);
    virtual ~MStreamReader();

    int read(T *tab, int max);
    void take(T &value);
    void skip(int i);
    void skipall();
    int size() const;


private:
    friend class MStream<T>;
    MStream<T> *s;

    int rptr;
};












template <class T>
        MStream<T>::MStream(int bufferSize)
            : size(bufferSize), wptr(0), full(false)
{
    if (size == 0)
        size = 1024 * 1024;
    d = new T[size];
}

template <class T>
        MStream<T>::~MStream()
{
    while (readers.size()) {
        delete readers.first();
    }

    delete[] d;
}

template <class T>
        int MStream<T>::write(const T *tab, int max)
{
    if (max >= size) {
        tab += max - size + 1;
        max = size - 1;
    }

    int i;
    i = size - wptr;

    if (i >= max) {
        mutex.lock();
        memcpy(d + wptr, tab, sizeof (T) * max);

//        int s = readers.size();
//        MStreamReader<T> *reader;
//        for (int i = 0; i < s; ++i) {
//            reader = readers[i];
//            if (reader->rptr > wptr && reader->rptr <= wptr + max) {
//                qDebug("overwrite");
//                reader->rptr = wptr + max + 1;
//                if (reader->rptr >= size) {
//                    reader->rptr -= size;
//                }
//            }
//        }

        wptr += max;
        if (wptr >= size) {
            wptr = 0;
            full = true;
//            for (int i = 0; i < s; ++i) {
//                if (readers[i]->rptr == 0) {
//                    readers[i]->rptr = 1;
//                }
//            }
        }
        mutex.unlock();
    } else {
        write(tab, i);
        write(tab + i, max - i);
    }

    return max;
}

template <class T>
        void MStream<T>::push(T newvalue)
{
    mutex.lock();
    d[wptr++] = newvalue;
    if (wptr == size) {
        wptr = 0;
        full = true;
    }

//    register int regsize = readers.size();
//    MStreamReader<T> *regread;
//    for (register int i = 0; i < regsize; ++i) {
//        regread = readers[i];
//        if (regread->rptr == wptr) {
////            qDebug("overwrite");
//            if (++regread->rptr >= size) {
//                regread->rptr = 0;
//            }
//        }
//    }
    mutex.unlock();
}















template <class T>
        MStreamReader<T>::MStreamReader(MStream<T> *stream)
            : s(stream), rptr(0)
{
    s->mutex.lock();

    if (s->full && s->wptr + 1 < s->size) {
        rptr = s->wptr + 1;
    }

    s->readers.append(this);

    s->mutex.unlock();
}

template <class T>
        MStreamReader<T>::~MStreamReader()
{
    s->mutex.lock();
    for (int i = 0; i < s->readers.size(); ++i) {
        if (s->readers[i] == this)
            s->readers.remove(i--);
    }
    s->mutex.unlock();
}

template <class T>
        int MStreamReader<T>::read(T *tab, int max)
{
    int i = size();

    if (max > i)
        max = i;

    s->mutex.lock();
    i = s->size - rptr;
    if (i >= max) {
        memcpy(tab, s->d + rptr, sizeof (T) * max);
        rptr += max;
        if (rptr >= s->size) {
            rptr = 0;
        }
    } else {
        memcpy(tab, s->d + rptr, sizeof (T) * i);
        tab += i;
        rptr = max - i;
        memcpy(tab, s->d, sizeof (T) * rptr);
    }
    s->mutex.unlock();

    return max;
}

template <class T>
        void MStreamReader<T>::take(T &value)
{
    s->mutex.lock();
    value = s->d[rptr++];
    if (rptr >= s->size) {
        rptr = 0;
    }
    s->mutex.unlock();
}

template <class T>
        void MStreamReader<T>::skip(int i)
{
    rptr += i;
    if (rptr >= s->size) {
        rptr -= s->size;
    }
}

template <class T>
void MStreamReader<T>::skipall()
{
    s->mutex.lock();
    rptr = s->wptr;
    s->mutex.unlock();
}

template <class T>
        int MStreamReader<T>::size() const
{
    s->mutex.lock();
    int i = s->wptr - rptr;
    s->mutex.unlock();

    if (i < 0) {
        i += s->size;
    }

    return i;
}

#endif

