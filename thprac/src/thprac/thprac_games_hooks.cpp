
#include "thprac_games_hooks.h"
#include <vector>
#include "../3rdParties/d3d8/include/dsound.h"
#include "thprac_hook.h"
#include <string>
#include "thprac_utils.h"

namespace THPrac {
bool g_is_d3d_hooked = false;
extern DWORD* g_gameGuiDevice;
extern DWORD* g_gameGuiHwnd;
extern std::unordered_map<KeyDefine, KeyDefine, KeyDefineHashFunction> g_keybind;// launcher_cfg.cpp


#pragma region Sound
struct SoundOpt {
    HRESULT(STDMETHODCALLTYPE* g_realCreateSoundBuffer)
    (IDirectSound8* thiz, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer, LPUNKNOWN pUnkOuter);
    HRESULT(STDMETHODCALLTYPE* g_realDuplicateSoundBuffer)
    (IDirectSound8* thiz, LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER* ppDSBufferDuplicate);
    bool enable_fasterBGM;
    bool no_change_pitch;
    float pitch_ofs = 1.0f;
    struct SB_Struct {
        LPDIRECTSOUNDBUFFER p_sb;
        WAVEFORMATEX orig_format;
        DWORD* vtbl_orig;
        DWORD* vtbl_changed;
    };
    float bgm_speed = 1.0f;
    std::vector<SB_Struct> soundbuffers;
} g_sound_opt;

HRESULT STDMETHODCALLTYPE SetFormat_Changed(LPDIRECTSOUNDBUFFER thiz, LPCWAVEFORMATEX pcfxFormat)
{
    for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end(); i++) {
        if (i->p_sb == thiz) {
            i->orig_format = *pcfxFormat;
            HRESULT(STDMETHODCALLTYPE * realSetFormat)
            (LPDIRECTSOUNDBUFFER thiz, LPCWAVEFORMATEX pcfxFormat);
            realSetFormat = (decltype(realSetFormat))(i->vtbl_orig[14]);
            HRESULT res = realSetFormat(thiz, pcfxFormat);
            i = g_sound_opt.soundbuffers.erase(i);
            return res;
        }
    }
    return -1;
}

HRESULT(STDMETHODCALLTYPE SoundBuffer_Release_Changed)
(LPDIRECTSOUNDBUFFER thiz)
{
    for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end();) {
        if (i->p_sb == thiz) {
            HRESULT(STDMETHODCALLTYPE * realSoundBuffer_Release)
            (LPDIRECTSOUNDBUFFER thiz);
            realSoundBuffer_Release = (decltype(realSoundBuffer_Release))(i->vtbl_orig[2]);
            HRESULT res = realSoundBuffer_Release(thiz);
            delete i->vtbl_changed;
            delete i->vtbl_orig;
            i = g_sound_opt.soundbuffers.erase(i);
            return res;
        } else {
            i++;
        }
    }
    return -1;
}

HRESULT STDMETHODCALLTYPE SoundBuffer_Initialize_Changed(LPDIRECTSOUNDBUFFER thiz, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc)
{
    for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end(); i++) {
        if (i->p_sb == thiz) {

            DSBUFFERDESC desc = *pcDSBufferDesc;
            desc.dwFlags |= DSBCAPS_CTRLFREQUENCY;

            HRESULT(STDMETHODCALLTYPE * realSoundBuffer_Initialize)
            (LPDIRECTSOUNDBUFFER thiz, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pcDSBufferDesc);
            realSoundBuffer_Initialize = (decltype(realSoundBuffer_Initialize))(i->vtbl_orig[10]);
            HRESULT res = realSoundBuffer_Initialize(thiz, pDirectSound, &desc);
            return res;
        }
    }
    return -1;
}

HRESULT STDMETHODCALLTYPE Play_Changed(LPDIRECTSOUNDBUFFER thiz, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)
{
    for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end(); i++) {
        if (i->p_sb == thiz) {
            HRESULT(STDMETHODCALLTYPE * realPlay)
            (LPDIRECTSOUNDBUFFER thiz, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
            realPlay = (decltype(realPlay))(i->vtbl_orig[12]);


            if (i->orig_format.nChannels != 0) {
                WAVEFORMATEX fmt = i->orig_format;
                i->p_sb->SetFrequency((DWORD)floorf(fmt.nSamplesPerSec * g_sound_opt.bgm_speed));
            }

            HRESULT res = realPlay(thiz, dwReserved1, dwPriority, dwFlags);
            return res;
        }
    }
    return -1;
}

/****************************************************************************
 *
 * NAME: smbPitchShift.cpp
 * VERSION: 1.2
 * HOME URL: http://blogs.zynaptiq.com/bernsee
 * KNOWN BUGS: none
 *
 * SYNOPSIS: Routine for doing pitch shifting while maintaining
 * duration using the Short Time Fourier Transform.
 *
 * DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
 * (one octave down) and 2. (one octave up). A value of exactly 1 does not change
 * the pitch. numSampsToProcess tells the routine how many samples in indata[0...
 * numSampsToProcess-1] should be pitch shifted and moved to outdata[0 ...
 * numSampsToProcess-1]. The two buffers can be identical (ie. it can process the
 * data in-place). fftFrameSize defines the FFT frame size used for the
 * processing. Typical values are 1024, 2048 and 4096. It may be any value <=
 * MAX_FRAME_LENGTH but it MUST be a power of 2. osamp is the STFT
 * oversampling factor which also determines the overlap between adjacent STFT
 * frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
 * recommended for best quality. sampleRate takes the sample rate for the signal
 * in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in
 * indata[] should be in the range [-1.0, 1.0), which is also the output range
 * for the data, make sure you scale the data accordingly (for 16bit signed integers
 * you would have to divide (and multiply) by 32768).
 *
 * COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 *
 * 						The Wide Open License (WOL)
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice and this license appear in all source copies.
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND. See http://www.dspguru.com/wol.htm for more information.
 *
 *****************************************************************************/


#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 8192

void smbFft(float* fftBuffer, long fftFrameSize, long sign)
{
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long i, bitm, j, le, le2, k;
    for (i = 2; i < 2 * fftFrameSize - 2; i += 2) {
        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1) {
            if (i & bitm)
                j++;
            j <<= 1;
        }
        if (i < j) {
            p1 = fftBuffer + i;
            p2 = fftBuffer + j;
            temp = *p1;
            *(p1++) = *p2;
            *(p2++) = temp;
            temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }
    for (k = 0, le = 2; k < (long)(log(fftFrameSize) / log(2.) + .5); k++) {
        le <<= 1;
        le2 = le >> 1;
        ur = 1.0;
        ui = 0.0;
        arg = static_cast<float>(M_PI / (le2 >> 1));
        wr = cos(arg);
        wi = sign * sin(arg);
        for (j = 0; j < le2; j += 2) {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;
            for (i = j; i < 2 * fftFrameSize; i += le) {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float* indata, float* outdata)
{
    static float gInFIFO[MAX_FRAME_LENGTH] = { 0 };
    static float gOutFIFO[MAX_FRAME_LENGTH] = { 0 };
    static float gFFTworksp[2 * MAX_FRAME_LENGTH] = { 0 };
    static float gLastPhase[MAX_FRAME_LENGTH / 2 + 1] = { 0 };
    static float gSumPhase[MAX_FRAME_LENGTH / 2 + 1] = { 0 };
    static float gOutputAccum[2 * MAX_FRAME_LENGTH] = { 0 };
    static float gAnaFreq[MAX_FRAME_LENGTH] = { 0 };
    static float gAnaMagn[MAX_FRAME_LENGTH] = { 0 };
    static float gSynFreq[MAX_FRAME_LENGTH] = { 0 };
    static float gSynMagn[MAX_FRAME_LENGTH] = { 0 };
    static long gRover = false, gInit = false;
    double magn, phase, tmp, window, real, imag;
    double freqPerBin, expct;
    long i, k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

    /* set up some handy variables */
    fftFrameSize2 = fftFrameSize / 2;
    stepSize = fftFrameSize / osamp;
    freqPerBin = sampleRate / (double)fftFrameSize;
    expct = 2. * M_PI * (double)stepSize / (double)fftFrameSize;
    inFifoLatency = fftFrameSize - stepSize;
    if (gRover == false)
        gRover = inFifoLatency;

    /* initialize our static arrays */
    if (gInit == false) {
        memset(gInFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gOutFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gFFTworksp, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
        memset(gLastPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
        memset(gSumPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
        memset(gOutputAccum, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
        memset(gAnaFreq, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gAnaMagn, 0, MAX_FRAME_LENGTH * sizeof(float));
        gInit = true;
    }

    /* main processing loop */
    for (i = 0; i < numSampsToProcess; i++) {

        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover - inFifoLatency];
        gRover++;

        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;

            /* do windowing and re,im interleave */
            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
                gFFTworksp[2 * k] = static_cast<float>(gInFIFO[k] * window);
                gFFTworksp[2 * k + 1] = 0.;
            }

            /* ***************** ANALYSIS ******************* */
            /* do transform */
            smbFft(gFFTworksp, fftFrameSize, -1);

            /* this is the analysis step */
            for (k = 0; k <= fftFrameSize2; k++) {

                /* de-interlace FFT buffer */
                real = gFFTworksp[2 * k];
                imag = gFFTworksp[2 * k + 1];

                /* compute magnitude and phase */
                magn = 2. * sqrt(real * real + imag * imag);
                phase = atan2(imag, real);

                /* compute phase difference */
                tmp = phase - gLastPhase[k];
                gLastPhase[k] = static_cast<float>(phase);

                /* subtract expected phase difference */
                tmp -= (double)k * expct;

                /* map delta phase into +/- Pi interval */
                qpd = static_cast<long>(tmp / M_PI);
                if (qpd >= 0)
                    qpd += qpd & 1;
                else
                    qpd -= qpd & 1;
                tmp -= M_PI * (double)qpd;

                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = osamp * tmp / (2. * M_PI);

                /* compute the k-th partials' true frequency */
                tmp = (double)k * freqPerBin + tmp * freqPerBin;

                /* store magnitude and true frequency in analysis arrays */
                gAnaMagn[k] = static_cast<float>(magn);
                gAnaFreq[k] = static_cast<float>(tmp);
            }

            /* ***************** PROCESSING ******************* */
            /* this does the actual pitch shifting */
            memset(gSynMagn, 0, fftFrameSize * sizeof(float));
            memset(gSynFreq, 0, fftFrameSize * sizeof(float));
            for (k = 0; k <= fftFrameSize2; k++) {
                index = static_cast<long>(k * pitchShift);
                if (index <= fftFrameSize2) {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index] = gAnaFreq[k] * pitchShift;
                }
            }

            /* ***************** SYNTHESIS ******************* */
            /* this is the synthesis step */
            for (k = 0; k <= fftFrameSize2; k++) {

                /* get magnitude and true frequency from synthesis arrays */
                magn = gSynMagn[k];
                tmp = gSynFreq[k];

                /* subtract bin mid frequency */
                tmp -= (double)k * freqPerBin;

                /* get bin deviation from freq deviation */
                tmp /= freqPerBin;

                /* take osamp into account */
                tmp = 2. * M_PI * tmp / osamp;

                /* add the overlap phase advance back in */
                tmp += (double)k * expct;

                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += static_cast<float>(tmp);
                phase = gSumPhase[k];

                /* get real and imag part and re-interleave */
                gFFTworksp[2 * k] = static_cast<float>(magn) * cosf(static_cast<float>(phase));
                gFFTworksp[2 * k + 1] = static_cast<float>(magn) * sinf(static_cast<float>(phase));
            }

            /* zero negative frequencies */
            for (k = fftFrameSize + 2; k < 2 * fftFrameSize; k++)
                gFFTworksp[k] = 0.;

            /* do inverse transform */
            smbFft(gFFTworksp, fftFrameSize, 1);

            /* do windowing and add to output accumulator */
            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
                gOutputAccum[k] += static_cast<float>(2. * window * gFFTworksp[2 * k] / (fftFrameSize2 * osamp));
            }
            for (k = 0; k < stepSize; k++)
                gOutFIFO[k] = gOutputAccum[k];

            /* shift accumulator */
            memmove(gOutputAccum, gOutputAccum + stepSize, fftFrameSize * sizeof(float));

            /* move input FIFO */
            for (k = 0; k < inFifoLatency; k++)
                gInFIFO[k] = gInFIFO[k + stepSize];
        }
    }
}


void smbPitchShift2(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float* indata, float* outdata)
{
    static float gInFIFO[MAX_FRAME_LENGTH] = { 0 };
    static float gOutFIFO[MAX_FRAME_LENGTH] = { 0 };
    static float gFFTworksp[2 * MAX_FRAME_LENGTH] = { 0 };
    static float gLastPhase[MAX_FRAME_LENGTH / 2 + 1] = { 0 };
    static float gSumPhase[MAX_FRAME_LENGTH / 2 + 1] = { 0 };
    static float gOutputAccum[2 * MAX_FRAME_LENGTH] = { 0 };
    static float gAnaFreq[MAX_FRAME_LENGTH]= { 0 };
    static float gAnaMagn[MAX_FRAME_LENGTH]= { 0 };
    static float gSynFreq[MAX_FRAME_LENGTH]= { 0 };
    static float gSynMagn[MAX_FRAME_LENGTH]= { 0 };
    static long gRover = false, gInit = false;
    double magn, phase, tmp, window, real, imag;
    double freqPerBin, expct;
    long i, k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

    /* set up some handy variables */
    fftFrameSize2 = fftFrameSize / 2;
    stepSize = fftFrameSize / osamp;
    freqPerBin = sampleRate / (double)fftFrameSize;
    expct = 2. * M_PI * (double)stepSize / (double)fftFrameSize;
    inFifoLatency = fftFrameSize - stepSize;
    if (gRover == false)
        gRover = inFifoLatency;

    /* initialize our static arrays */
    if (gInit == false) {
        memset(gInFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gOutFIFO, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gFFTworksp, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
        memset(gLastPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
        memset(gSumPhase, 0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
        memset(gOutputAccum, 0, 2 * MAX_FRAME_LENGTH * sizeof(float));
        memset(gAnaFreq, 0, MAX_FRAME_LENGTH * sizeof(float));
        memset(gAnaMagn, 0, MAX_FRAME_LENGTH * sizeof(float));
        gInit = true;
    }

    /* main processing loop */
    for (i = 0; i < numSampsToProcess; i++) {

        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover - inFifoLatency];
        gRover++;

        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;

            /* do windowing and re,im interleave */
            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
                gFFTworksp[2 * k] = static_cast<float>(gInFIFO[k] * window);
                gFFTworksp[2 * k + 1] = 0.;
            }

            /* ***************** ANALYSIS ******************* */
            /* do transform */
            smbFft(gFFTworksp, fftFrameSize, -1);

            /* this is the analysis step */
            for (k = 0; k <= fftFrameSize2; k++) {

                /* de-interlace FFT buffer */
                real = gFFTworksp[2 * k];
                imag = gFFTworksp[2 * k + 1];

                /* compute magnitude and phase */
                magn = 2. * sqrt(real * real + imag * imag);
                phase = atan2(imag, real);

                /* compute phase difference */
                tmp = phase - gLastPhase[k];
                gLastPhase[k] = static_cast<float>(phase);

                /* subtract expected phase difference */
                tmp -= (double)k * expct;

                /* map delta phase into +/- Pi interval */
                qpd = static_cast<long>(tmp / M_PI);
                if (qpd >= 0)
                    qpd += qpd & 1;
                else
                    qpd -= qpd & 1;
                tmp -= M_PI * (double)qpd;

                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = osamp * tmp / (2. * M_PI);

                /* compute the k-th partials' true frequency */
                tmp = (double)k * freqPerBin + tmp * freqPerBin;

                /* store magnitude and true frequency in analysis arrays */
                gAnaMagn[k] = static_cast<float> (magn);
                gAnaFreq[k] = static_cast<float> (tmp);
            }

            /* ***************** PROCESSING ******************* */
            /* this does the actual pitch shifting */
            memset(gSynMagn, 0, fftFrameSize * sizeof(float));
            memset(gSynFreq, 0, fftFrameSize * sizeof(float));
            for (k = 0; k <= fftFrameSize2; k++) {
                index = static_cast<long> (k * pitchShift);
                if (index <= fftFrameSize2) {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index] = gAnaFreq[k] * pitchShift;
                }
            }

            /* ***************** SYNTHESIS ******************* */
            /* this is the synthesis step */
            for (k = 0; k <= fftFrameSize2; k++) {

                /* get magnitude and true frequency from synthesis arrays */
                magn = gSynMagn[k];
                tmp = gSynFreq[k];

                /* subtract bin mid frequency */
                tmp -= (double)k * freqPerBin;

                /* get bin deviation from freq deviation */
                tmp /= freqPerBin;

                /* take osamp into account */
                tmp = 2. * M_PI * tmp / osamp;

                /* add the overlap phase advance back in */
                tmp += (double)k * expct;

                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += static_cast<float>(tmp);
                phase = gSumPhase[k];

                /* get real and imag part and re-interleave */
                gFFTworksp[2 * k] = static_cast<float> (magn) * cosf(static_cast<float>(phase));
                gFFTworksp[2 * k + 1] = static_cast<float> (magn) * sinf(static_cast<float>(phase));
            }

            /* zero negative frequencies */
            for (k = fftFrameSize + 2; k < 2 * fftFrameSize; k++)
                gFFTworksp[k] = 0.;

            /* do inverse transform */
            smbFft(gFFTworksp, fftFrameSize, 1);

            /* do windowing and add to output accumulator */
            for (k = 0; k < fftFrameSize; k++) {
                window = -.5 * cos(2. * M_PI * (double)k / (double)fftFrameSize) + .5;
                gOutputAccum[k] +=  static_cast<float>(2. * window * gFFTworksp[2 * k] / (fftFrameSize2 * osamp));
            }
            for (k = 0; k < stepSize; k++)
                gOutFIFO[k] = gOutputAccum[k];

            /* shift accumulator */
            memmove(gOutputAccum, gOutputAccum + stepSize, fftFrameSize * sizeof(float));

            /* move input FIFO */
            for (k = 0; k < inFifoLatency; k++)
                gInFIFO[k] = gInFIFO[k + stepSize];
        }
    }
}

void ChangePitch(char* buffer_in, char* buffer_out, long data_len, float pitch_change, float sample_rate)
{
    // assume it's 16-bit
    long num_samples = data_len / 2 / 2;
    static std::vector<float> float_inL;
    static std::vector<float> float_outL;
    static std::vector<float> float_inR;
    static std::vector<float> float_outR;
    float_inL.resize(num_samples);
    float_outL.resize(num_samples);
    float_inR.resize(num_samples);
    float_outR.resize(num_samples);
    short* pcm_src = (short*)buffer_in;
    for (long i = 0; i < num_samples; i++) {
        float_inL[i] = (float)pcm_src[i*2] / 32768.0f;
        float_inR[i] = (float)pcm_src[i*2+1] / 32768.0f;
    }
    smbPitchShift(pitch_change, num_samples, 2048, 4, sample_rate, float_inL.data(), float_outL.data());
    smbPitchShift2(pitch_change, num_samples, 2048, 4, sample_rate, float_inR.data(), float_outR.data());
    short* pcm_dst = (short*)buffer_out;
    for (long i = 0; i < num_samples; i++) {
        float valL = float_outL[i] * 32768.0f;
        valL = std::clamp(valL, -32768.0f, 32767.0f);

        float valR = float_outR[i] * 32768.0f;
        valR = std::clamp(valR, -32768.0f, 32767.0f);
        pcm_dst[i*2] = (short)valL;
        pcm_dst[i*2+1] = (short)valR;
    }
}


HRESULT STDMETHODCALLTYPE Unlock_Changed(LPDIRECTSOUNDBUFFER thiz, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)
{
    WAVEFORMATEX wavf;
    thiz->GetFormat(&wavf, sizeof(wavf), nullptr);
    for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end(); i++) {
        if (i->p_sb == thiz) {
            if (g_sound_opt.no_change_pitch){
                if (g_sound_opt.pitch_ofs / g_sound_opt.bgm_speed != 1.0f) {
                    if (pvAudioPtr1)
                        ChangePitch((char*)pvAudioPtr1, (char*)pvAudioPtr1, dwAudioBytes1, g_sound_opt.pitch_ofs / g_sound_opt.bgm_speed, static_cast<float>(wavf.nSamplesPerSec));
                    if (pvAudioPtr2)
                        ChangePitch((char*)pvAudioPtr2, (char*)pvAudioPtr2, dwAudioBytes2, g_sound_opt.pitch_ofs / g_sound_opt.bgm_speed, static_cast<float>(wavf.nSamplesPerSec));
                }
            }
            HRESULT(STDMETHODCALLTYPE * realUnlock)
            (LPDIRECTSOUNDBUFFER thiz, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
            realUnlock = (decltype(realUnlock))(i->vtbl_orig[19]);
            HRESULT res = realUnlock(thiz, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
            return res;
        }
    }
    return -1;
}



HRESULT STDMETHODCALLTYPE DuplicateSoundBuffer_Changed(IDirectSound8* thiz, LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER* ppDSBufferDuplicate)
{
    auto res = g_sound_opt.g_realDuplicateSoundBuffer(thiz, pDSBufferOriginal, ppDSBufferDuplicate);
    if (SUCCEEDED(res)) {
        int idx = -1;
        for (int i = 0; i < std::ssize(g_sound_opt.soundbuffers); i++) {
            if (g_sound_opt.soundbuffers[i].p_sb == pDSBufferOriginal) {
                idx = i;
                break;
            }
        }
        if (idx != -1) {
            SoundOpt::SB_Struct sb;
            memset(&sb, 0, sizeof(sb));
            sb.vtbl_orig = new DWORD[21];
            sb.vtbl_changed = new DWORD[21];
            DWORD* address = (DWORD*)(*(DWORD*)*ppDSBufferDuplicate);
            for (int i = 0; i < 21; i++) {
                sb.vtbl_orig[i] = (DWORD)address[i];
                sb.vtbl_changed[i] = (DWORD)address[i];
            }
            sb.vtbl_changed[14] = (DWORD)SetFormat_Changed;
            sb.vtbl_changed[2] = (DWORD)SoundBuffer_Release_Changed;
            sb.vtbl_changed[10] = (DWORD)SoundBuffer_Initialize_Changed;
            sb.vtbl_changed[12] = (DWORD)Play_Changed;
            sb.vtbl_changed[19] = (DWORD)Unlock_Changed;
            (*(DWORD*)*ppDSBufferDuplicate) = (DWORD)sb.vtbl_changed;
            g_sound_opt.soundbuffers.push_back(sb);
        }
    }
    return res;
}

HRESULT STDMETHODCALLTYPE CreateSoundBuffer_Changed(IDirectSound8* thiz, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER* ppDSBuffer, LPUNKNOWN pUnkOuter)
{
    HRESULT res;
    DSBUFFERDESC desc = *pcDSBufferDesc;
    if (desc.dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY) { // only BGM
        desc.dwFlags |= DSBCAPS_CTRLFREQUENCY;
    }

    SoundOpt::SB_Struct sb;
    memset(&sb, 0, sizeof(sb));
    if (desc.lpwfxFormat)
        sb.orig_format = *desc.lpwfxFormat;
    res = g_sound_opt.g_realCreateSoundBuffer(thiz, &desc, ppDSBuffer, pUnkOuter);
    if (SUCCEEDED(res)) {
        sb.p_sb = *ppDSBuffer;
        if (desc.dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY) { // only BGM
            sb.vtbl_orig = new DWORD[21];
            sb.vtbl_changed = new DWORD[21];
            DWORD* address = (DWORD*)(*(DWORD*)*ppDSBuffer);
            for (int i = 0; i < 21; i++) {
                sb.vtbl_orig[i] = (DWORD)address[i];
                sb.vtbl_changed[i] = (DWORD)address[i];
            }
            sb.vtbl_changed[14] = (DWORD)SetFormat_Changed;
            sb.vtbl_changed[2] = (DWORD)SoundBuffer_Release_Changed;
            sb.vtbl_changed[10] = (DWORD)SoundBuffer_Initialize_Changed;
            sb.vtbl_changed[12] = (DWORD)Play_Changed;
            sb.vtbl_changed[19] = (DWORD)Unlock_Changed;
            (*(DWORD*)*ppDSBuffer) = (DWORD)sb.vtbl_changed;
            g_sound_opt.soundbuffers.push_back(sb);
        }
    }
    return res;
}
void HookBGMSpeed(bool is_hook, bool no_change_pitch)
{
    g_sound_opt.enable_fasterBGM = is_hook;
    g_sound_opt.no_change_pitch = no_change_pitch;
    if (is_hook) {
        IDirectSound8* pdirectsound;
        HRESULT hr = DirectSoundCreate8(NULL, &pdirectsound, NULL);
        if (SUCCEEDED(hr)) {
            HookVTable(pdirectsound, 3, CreateSoundBuffer_Changed, (void**)&g_sound_opt.g_realCreateSoundBuffer);
            // HookVTable(pdirectsound, 5, DuplicateSoundBuffer_Changed, (void**)&g_realDuplicateSoundBuffer);
            pdirectsound->Release();
        }
    }
}


void ChangeBGMSpeed(float speed,float pitch_ofs)
{
    if (g_sound_opt.enable_fasterBGM) {
        float orig_speed = g_sound_opt.bgm_speed;
        float orig_pitch = g_sound_opt.pitch_ofs;

        float new_speed = speed == -1.0f ? g_sound_opt.bgm_speed : speed;
        float new_pitch = pitch_ofs == -1.0f ? g_sound_opt.pitch_ofs : pitch_ofs;

        for (auto i = g_sound_opt.soundbuffers.begin(); i != g_sound_opt.soundbuffers.end(); i++) {
            if (i->orig_format.nChannels != 0) {
                WAVEFORMATEX fmt = i->orig_format;
                DWORD status;
                i->p_sb->GetStatus(&status);
                if (status & DSBSTATUS_PLAYING) {
                    LPVOID p1, p2;
                    DWORD n1, n2;
                    i->p_sb->Stop();
                    i->p_sb->Lock(0, 0, &p1, &n1, &p2, &n2, DSBLOCK_ENTIREBUFFER);

                    g_sound_opt.bgm_speed = new_speed / orig_speed;
                    g_sound_opt.pitch_ofs = new_pitch / orig_pitch;

                    i->p_sb->Unlock(p1, n1, p2, n2);

                    g_sound_opt.bgm_speed = new_speed;
                    g_sound_opt.pitch_ofs = new_pitch;
                    i->p_sb->SetFrequency(static_cast<DWORD>(fmt.nSamplesPerSec * g_sound_opt.bgm_speed));
                    i->p_sb->Play(0, 0, DSBPLAY_LOOPING);
                }
            }
        }
    }
}
bool BGMPitchChanger()
{
    static float pitch_2powed = 0.0f;
    static float speed = 1.0f;
    float pitch_cur = g_sound_opt.pitch_ofs;
    float speed_cur = g_sound_opt.bgm_speed;

    if (g_sound_opt.enable_fasterBGM && g_sound_opt.no_change_pitch){
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat("BGM Pitch", &pitch_2powed, 0.01f, -2.0f, 2.0f);
        pitch_2powed = std::clamp(pitch_2powed, -2.0f, 2.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150.0f);
        ImGui::DragFloat("BGM SPD", &speed, 0.01f, 0.25f, 5.0f);
        speed = std::clamp(speed, 0.25f, 5.0f);

        float pitch_new = powf(2.0f, pitch_2powed);
        float pitch_2powed_old = log2f(pitch_cur);
        if (fabsf(pitch_2powed_old - pitch_2powed) > 0.0001f || fabsf(speed_cur - speed)>0.001f)
        {
            ImGui::SameLine();
            if (ImGui::Button("OK")){
                ChangeBGMSpeed(speed, pitch_new);
                return true;
            }
        }

    }
    return false;
}

#pragma endregion

#pragma region Font
struct FontOpt {
    bool g_useCustomFont = false;
    std::string g_customFont = "MS Gothic";
    HFONT(WINAPI* g_realCreateFontA)
    (int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName);
    HFONT(WINAPI* g_realCreateFontW)
    (int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName);
} g_font_opt;


HFONT WINAPI CreateFontA_Changed(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline,
    DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName)
{
    static std::vector<std::string> fonts = EnumAllFonts();
    unsigned char font_Gothic[] = { 0x82, 0x6C, 0x82, 0x72, 0x20, 0x83, 0x53, 0x83, 0x56, 0x83, 0x62, 0x83, 0x4E, 0x00 }; // 俵俽 僑僔僢僋, MS Gothic
    unsigned char font_Mincho[] = { 0x82, 0x6C, 0x82, 0x72, 0x20, 0x96, 0xBE, 0x92, 0xA9, 0x00 }; // 俵俽 柧挬, MS Mincho
    if (g_font_opt.g_useCustomFont)
        return g_font_opt.g_realCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, g_font_opt.g_customFont.c_str());
    if (strcmp((char*)font_Gothic, pszFaceName) == 0) {
        return g_font_opt.g_realCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, "MS Gothic");
    } else if (strcmp((char*)font_Mincho, pszFaceName) == 0) {
        // some computer might not have mincho font
        if (std::find(fonts.begin(), fonts.end(), "MS Mincho") == fonts.end())
            return g_font_opt.g_realCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, "MS Gothic");
        return g_font_opt.g_realCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, "MS Mincho");
    }
    return g_font_opt.g_realCreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}

HFONT WINAPI CreateFontW_Changed(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline,
    DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName)
{
    static std::vector<std::string> fonts = EnumAllFonts();
    wchar_t font_YuGothic[] = { 0x6E38, 0x30B4, 0x30B7, 0x30C3, 0x30AF, 0 }; // Yu Gothic...
    wchar_t font_YuMincho[] = L"Yu Mincho";
    wchar_t font_YuMeiryo[] = { 0x30E1, 0x30A4, 0x30EA, 0x30AA, 0 }; // Meiryo
    wchar_t font_MsGothic[] = { 0xFF2D, 0xFF33, 0x0020, 0x30B4, 0x30B7, 0x30C3, 0x30AF, 0x0 }; // MS Gothic
    if (g_font_opt.g_useCustomFont) {
        // fall back to CreateFontA
        return CreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, g_font_opt.g_customFont.c_str());
    }

    // Yu Gothic
    if (wcscmp(font_YuGothic, pszFaceName) == 0) {
        if (std::find(fonts.begin(), fonts.end(), "Yu Gothic") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"Yu Gothic");
        if (std::find(fonts.begin(), fonts.end(), "MS Gothic") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"MS Gothic");
        if (std::find(fonts.begin(), fonts.end(), "MS Mincho") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"MS Mincho");
    }
    // Yu Mincho
    if (wcscmp(font_YuMincho, pszFaceName) == 0) {
        if (std::find(fonts.begin(), fonts.end(), "Yu Mincho") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"Yu Mincho");
        if (std::find(fonts.begin(), fonts.end(), "MS Mincho") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"MS Mincho");
    }
    // Meiryo
    if (wcscmp(font_YuMeiryo, pszFaceName) == 0) {
        if (std::find(fonts.begin(), fonts.end(), "Meiryo") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"Meiryo");
    }
    // MS Gothic
    if (wcscmp(font_MsGothic, pszFaceName) == 0) {
        if (std::find(fonts.begin(), fonts.end(), "MS Gothic") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"MS Gothic");
        if (std::find(fonts.begin(), fonts.end(), "MS Mincho") != fonts.end())
            return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, L"MS Mincho");
    }
    return g_font_opt.g_realCreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
}

void HookCreateFont(bool useCorrectJaFont, bool useCustomFont, int fontidx)
{
    g_font_opt.g_useCustomFont = useCustomFont;
    if (useCustomFont || useCorrectJaFont) {
        HookIAT(GetModuleHandle(NULL), "GDI32.dll", "CreateFontA", CreateFontA_Changed, (void**)&g_font_opt.g_realCreateFontA);
        HookIAT(GetModuleHandle(NULL), "GDI32.dll", "CreateFontW", CreateFontW_Changed, (void**)&g_font_opt.g_realCreateFontW);
    }
    if (useCustomFont) {
        auto& all_fonts = EnumAllFonts();
        if (fontidx >= 0 && fontidx < std::ssize(all_fonts))
            g_font_opt.g_customFont = all_fonts[fontidx];
    }
}

#pragma endregion

#pragma region InputLatencyTest

InputLatencyTestOpt g_input_latency_test;

void InputLatencyTestOpt::Init()
{
    clockid = SetUpClock();
}
void InputLatencyTestOpt::Input()
{
    if (test_input_latency) {
        ResetClock(clockid);
    }
}
void InputLatencyTestOpt::Present()
{
    if (test_input_latency) {
        last_input_latency = ResetClock(clockid);
    }
}
double InputLatencyTestOpt::GetLatency()
{
    return last_input_latency;
}

void EnableInputLatencyTest(bool is_enable)
{
    g_input_latency_test.test_input_latency = is_enable;
    if (is_enable)
        g_input_latency_test.Init();
}

bool GetLatency(double* latency)
{
    if (latency && g_input_latency_test.test_input_latency)
    {
        *latency = g_input_latency_test.GetLatency();
        return true;
    }
    return false;
 }

#pragma endregion

#pragma region Input_Joy
extern FastRetryOpt g_fast_re_opt;
extern InputOpt g_input_opt;


template <typename T>
struct Ranges {
    T x_min, x_max;
    T y_min, y_max;
};
#define DEG_TO_RAD(x) ((x) * 0.0174532925199432957692369076848f)

template <typename T>
bool pov_to_xy(T& x, T& y, Ranges<T>& ranges, DWORD pov)
{
    // According to MSDN, some DirectInput drivers report the centered
    // position of the POV indicator as 65,535. This matches both that
    // behavior and JOY_POVCENTERED for WinMM.
    if (LOWORD(pov) == 0xFFFF) {
        return false;
    }
    T x_center = (ranges.x_max - ranges.x_min) / 2;
    T y_center = (ranges.y_max - ranges.y_min) / 2;

    float angle_deg = pov / 100.0f;
    float angle_rad = DEG_TO_RAD(angle_deg);
    // POV values ≠ unit circle angles, so...
    float angle_sin = 1.0f - cosf(angle_rad);
    float angle_cos = sinf(angle_rad) + 1.0f;
    x = (T)(ranges.x_min + (angle_cos * x_center));
    y = (T)(ranges.y_min + (angle_sin * y_center));
    return true;
}

struct winmm_joy_caps_t {
    // joyGetPosEx() will return bogus values on joysticks without a POV, so
    // we must check if we even have one.
    bool initialized = false;
    bool has_pov;
    Ranges<DWORD> range;
};
std::vector<winmm_joy_caps_t> joy_info;



MMRESULT WINAPI joyGetDevCapsA_Changed(UINT uJoyID, LPJOYCAPSA pjc, UINT cbjc)
{
    if (g_input_opt.g_disable_joy)
        return MMSYSERR_NODRIVER;
    return g_input_opt.g_realJoyGetDevCapsA(uJoyID, pjc, cbjc);
}
MMRESULT WINAPI joyGetPosEx_Changed(UINT uJoyID, LPJOYINFOEX pji)
{
    if (g_input_opt.g_disable_joy)
        return MMSYSERR_NODRIVER;

    pji->dwFlags |= JOY_RETURNPOV;
    auto ret_pos = g_input_opt.g_realJoyGetPosEx(uJoyID, pji);
    if (ret_pos != JOYERR_NOERROR) {
        return ret_pos;
    }

    if (uJoyID >= joy_info.size()) {
        joy_info.resize(uJoyID + 1);
    }

    auto& jc = joy_info[uJoyID];

    if (!jc.initialized) {
        JOYCAPSW caps;
        auto ret_caps = joyGetDevCapsW(uJoyID, &caps, sizeof(caps));
        (void)ret_caps; // suppress "unused variable" warning
        assert(ret_caps == JOYERR_NOERROR);

        jc.initialized = true;
        jc.has_pov = (caps.wCaps & JOYCAPS_HASPOV) != 0;
        jc.range.x_min = caps.wXmin;
        jc.range.x_max = caps.wXmax;
        jc.range.y_min = caps.wYmin;
        jc.range.y_max = caps.wYmax;
    } else if (!jc.has_pov) {
        return ret_pos;
    }
    pov_to_xy(pji->dwXpos, pji->dwYpos, jc.range, pji->dwPOV);
    return ret_pos;
}

DWORD WINAPI XInputGetState_Changed3(DWORD dwUserIndex, void* pState)
{
    if (g_input_opt.g_disable_joy)
        return ERROR_DEVICE_NOT_CONNECTED;
    return g_input_opt.g_realXInputGetState3(dwUserIndex, pState);
}

DWORD WINAPI XInputGetState_Changed4(DWORD dwUserIndex, void* pState)
{
    if (g_input_opt.g_disable_joy)
        return ERROR_DEVICE_NOT_CONNECTED;
    return g_input_opt.g_realXInputGetState4(dwUserIndex, pState);
}
void HookJoyInput()
{
    HookIAT(GetModuleHandle(NULL), "winmm.dll", "joyGetPosEx", joyGetPosEx_Changed, (void**)&g_input_opt.g_realJoyGetPosEx);
    HookIAT(GetModuleHandle(NULL), "winmm.dll", "joyGetDevCapsA", joyGetDevCapsA_Changed, (void**)&g_input_opt.g_realJoyGetDevCapsA);
    HookIAT(GetModuleHandle(NULL), "xinput1_3.dll", "XInputGetState", XInputGetState_Changed3, (void**)&g_input_opt.g_realXInputGetState3); // th18
    HookIAT(GetModuleHandle(NULL), "xinput1_4.dll", "XInputGetState", XInputGetState_Changed4, (void**)&g_input_opt.g_realXInputGetState4); // th19+
}

#pragma endregion

#define IS_KEY_DOWN(x) (((x) & 0x80) == 0x80)
enum Key {
    K_LEFT,
    K_RIGHT,
    K_UP,
    K_DOWN
};

void ClearInputData(bool inactivate)
{
    memset(g_input_opt.fake_di_State, 0, 256);
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI && g_input_opt.ddevice) {
        while (true) {
            DWORD dwItems = 32;
            DIDEVICEOBJECTDATA rgdod[32];
            auto res = g_input_opt.ddevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
            if (FAILED(res) || dwItems == 0) {
                break;
            }
        }
        if (inactivate)
            g_input_opt.ddevice->Unacquire();
        else
            g_input_opt.ddevice->Acquire();
    }
}

#pragma region rawInput
void GetRawInput(DWORD hraw)
{
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        static BYTE lpb[1024];
        UINT dwSize = 1024;
        if (GetRawInputData((HRAWINPUT)hraw, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != -1) {
            RAWINPUT* raw = (RAWINPUT*)lpb;
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                UINT scanCode = raw->data.keyboard.MakeCode;
                bool isUp = (raw->data.keyboard.Flags & RI_KEY_BREAK);

                if (raw->data.keyboard.Flags & RI_KEY_E0) {
                    scanCode |= 0x80;
                }
                if (scanCode < 256) {
                    g_input_opt.fake_di_State[scanCode] = isUp ? 0x00 : 0x80;
                }
            }
        }
    }
}

void ProcessRawInput(BYTE state[256])
{
    static char alignas(8) buffer[2048];
    UINT bufferSize = sizeof(buffer);
    UINT count = GetRawInputBuffer((PRAWINPUT)buffer, &bufferSize, sizeof(RAWINPUTHEADER));
    if (count == 0 || count == (UINT)-1) {
        memcpy(state, g_input_opt.fake_di_State, 256);
        return;
    }
    BYTE kb_is_down[256] = { 0 };
    BYTE kb_is_down_last[256] = { 0 };
    while (count > 0) {
        RAWINPUT* pRaw = (RAWINPUT*)buffer;
        memcpy(kb_is_down_last, g_input_opt.fake_di_State, 256);
        for (int i = 0; i < (int)count; ++i) {
            if (pRaw->header.dwType == RIM_TYPEKEYBOARD) {
                RAWKEYBOARD* pKb = &pRaw->data.keyboard;
                if (pKb->MakeCode == 0 && pKb->Flags == 0 && pRaw->header.dwSize > 32) {
                    pKb = (RAWKEYBOARD*)((LPBYTE)pRaw + 24); // WoW64 fix
                }
                UINT scanCode = pKb->MakeCode;
                bool isUp = (pKb->Flags & RI_KEY_BREAK);
                if (pKb->Flags & RI_KEY_E0) {
                    scanCode |= 0x80;
                }
                if (scanCode < 256) {
                    BYTE curstate = isUp ? 0x00 : 0x80;
                    g_input_opt.fake_di_State[scanCode] = curstate;
                    if (kb_is_down_last[scanCode] == 0x0 && !isUp)
                        kb_is_down[scanCode] = 0x80;
                }
            }
            pRaw = NEXTRAWINPUTBLOCK((PRAWINPUT)pRaw);
        }
        bufferSize = sizeof(buffer);
        count = GetRawInputBuffer((PRAWINPUT)buffer, &bufferSize, sizeof(RAWINPUTHEADER));
        if (count == 0 || count == (UINT)-1) {
            break;
        }
    }
    for (int i = 0; i < 256; i++) {
        state[i] = kb_is_down[i] | g_input_opt.fake_di_State[i];
    }
}
#pragma endregion

#pragma region dinput
HRESULT STDMETHODCALLTYPE GetDeviceState_Changed(LPDIRECTINPUTDEVICE8 thiz, DWORD num, LPVOID state);
HRESULT STDMETHODCALLTYPE SetProperty_Changed(LPDIRECTINPUTDEVICE8 thiz, REFGUID guid, LPCDIPROPHEADER header);
HRESULT STDMETHODCALLTYPE SetCooperativeLevel_Changed(LPDIRECTINPUTDEVICE8 thiz, HWND hwnd, DWORD flag)
{
    if (g_input_opt.ddevice == thiz) {
        if (g_input_opt.disable_win_key) {
            flag = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY;
        } else {
            flag = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
        }
        auto res = g_input_opt.realSetCooperativeLevel(thiz, hwnd, flag);
        bool re_acquire = false;
        if (res == DIERR_ACQUIRED) {
            thiz->Unacquire();
            re_acquire = true;
            res = g_input_opt.realSetCooperativeLevel(thiz, hwnd, flag);
        }
        if (g_input_opt.use_get_device_data) {
            DIPROPDWORD dipdw;
            dipdw.diph.dwSize = sizeof(DIPROPDWORD);
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            dipdw.diph.dwObj = 0;
            dipdw.diph.dwHow = DIPH_DEVICE;
            dipdw.dwData = 64;
            SetProperty_Changed(thiz, DIPROP_BUFFERSIZE, &dipdw.diph);
        }
        if (re_acquire) {
            thiz->Acquire();
        }
        return res;
    }
    return g_input_opt.realSetCooperativeLevel(thiz, hwnd, flag);
}

HRESULT WINAPI EnumDevices_Changed(LPDIRECTINPUT8 thiz, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags)
{
    if (g_input_opt.g_disable_joy && dwDevType == DI8DEVCLASS_GAMECTRL)
        return DI_OK;
    return g_input_opt.realEnumDevices(thiz, dwDevType, lpCallback, pvRef, dwFlags);
}
HRESULT WINAPI DDevice_Release_Changed(LPDIRECTINPUTDEVICE8 thiz)
{
    if (g_input_opt.ddevice == thiz) {
        return DI_OK;
    }
    return g_input_opt.realDDRelease(thiz);
}
HRESULT WINAPI Dinput_Release_Changed(LPDIRECTINPUT8 thiz)
{
    if (g_input_opt.dinput == thiz) {
        return DI_OK;
    }
    return g_input_opt.realDIRelease(thiz);
}

HRESULT STDMETHODCALLTYPE SetDataFormat_Changed(LPDIRECTINPUTDEVICE8 thiz, LPCDIDATAFORMAT fmt)
{
    if (g_input_opt.ddevice == thiz) {
        auto res = g_input_opt.realSetDataFormat(thiz, fmt);
        if (res == DIERR_ACQUIRED) {
            thiz->Unacquire();
            res = g_input_opt.realSetDataFormat(thiz, fmt);
        }
        return res;
    }
    return g_input_opt.realSetDataFormat(thiz, fmt);
}
HRESULT WINAPI Poll_Changed(LPDIRECTINPUTDEVICE8 thiz)
{
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        return DI_OK;
    }
    auto res = g_input_opt.realPoll(thiz);
    return res;
}
HRESULT WINAPI Acquire_Changed(LPDIRECTINPUTDEVICE8 thiz)
{
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        return DI_OK;
    }
    auto res = g_input_opt.realAcquire(thiz);
    return res;
}
HRESULT STDMETHODCALLTYPE SetProperty_Changed(LPDIRECTINPUTDEVICE8 thiz, REFGUID guid, LPCDIPROPHEADER header)
{
    if (g_input_opt.ddevice == thiz && g_input_opt.use_get_device_data) {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = 64;
        auto res = g_input_opt.realSetProperty(thiz, DIPROP_BUFFERSIZE, &dipdw.diph);
        if (res == DIERR_ACQUIRED) {
            thiz->Unacquire();
            res = g_input_opt.realSetProperty(thiz, DIPROP_BUFFERSIZE, &dipdw.diph);
        }
        return res;
    }
    return g_input_opt.realSetProperty(thiz, guid, header);
}

HRESULT STDMETHODCALLTYPE CreateDevice_Changed(LPDIRECTINPUT8 thiz, const GUID& guid, LPDIRECTINPUTDEVICE8W* lpddevice, LPUNKNOWN lpunk)
{
    if (g_input_opt.dinput == thiz && guid == GUID_SysKeyboard) {
        if (g_input_opt.ddevice != nullptr) {
            *lpddevice = g_input_opt.ddevice;
            (*lpddevice)->Unacquire();
            return DI_OK;
        }
        auto res = g_input_opt.realCreateDevice(thiz, guid, lpddevice, lpunk);
        if (res == DI_OK) {
            HookVTable(*lpddevice, 2, DDevice_Release_Changed, (void**)&(g_input_opt.realDDRelease));
            HookVTable(*lpddevice, 7, Acquire_Changed, (void**)&(g_input_opt.realAcquire));
            HookVTable(*lpddevice, 9, GetDeviceState_Changed, (void**)&(g_input_opt.realGetDeviceState));
            HookVTable(*lpddevice, 11, SetDataFormat_Changed, (void**)&(g_input_opt.realSetDataFormat));
            HookVTable(*lpddevice, 13, SetCooperativeLevel_Changed, (void**)&(g_input_opt.realSetCooperativeLevel));
            HookVTable(*lpddevice, 6, SetProperty_Changed, (void**)&(g_input_opt.realSetProperty));
            HookVTable(*lpddevice, 25, Poll_Changed, (void**)&(g_input_opt.realPoll));
            g_input_opt.ddevice = *lpddevice;
            g_input_opt.ddevice = *lpddevice;
        }
        return res;
    }
    return g_input_opt.realCreateDevice(thiz, guid, lpddevice, lpunk);
}

HRESULT WINAPI DirectInput8Create_Changed(HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    if (g_input_opt.dinput != nullptr) {
        *ppvOut = g_input_opt.dinput;
        return DI_OK;
    }
    auto res = g_input_opt.g_realDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    if (res == DI_OK) {
        HookVTable(*ppvOut, 2, Dinput_Release_Changed, (void**)&(g_input_opt.realDIRelease));
        HookVTable(*ppvOut, 3, CreateDevice_Changed, (void**)&(g_input_opt.realCreateDevice));
        HookVTable(*ppvOut, 4, EnumDevices_Changed, (void**)&(g_input_opt.realEnumDevices));
        g_input_opt.dinput = *(LPDIRECTINPUT8*)ppvOut;
    }
    return res;
}

HRESULT STDMETHODCALLTYPE GetDeviceState_Changed(LPDIRECTINPUTDEVICE8 thiz, DWORD num, LPVOID state)
{
    if (num != 256) { // no keyboard
        if (g_input_opt.g_disable_joy)
            return DIERR_INPUTLOST; // 8007001E

        HRESULT res = -1;
        res = g_input_opt.realGetDeviceState(thiz, num, state);

        if (num == sizeof(DIJOYSTATE) || num == sizeof(DIJOYSTATE2)) {
            auto* js = (DIJOYSTATE*)state;
            Ranges<long> di_range = { -1000, 1000, -1000, 1000 };
            bool ret_map = false;
            for (int i = 0; i < elementsof(js->rgdwPOV) && !ret_map; i++) {
                ret_map = pov_to_xy(js->lX, js->lY, di_range, js->rgdwPOV[i]);
            }
        }
        return res;
    }
    HRESULT res = DI_OK;
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_win32KeyAPI) {
        memset(state, 0, 256);
        for (auto keydef : keyBindDefine) {
            ((BYTE*)state)[keydef.dik] = (GetAsyncKeyState(keydef.vk) & 0x8000) ? 0x80 : 0;
            // though a bit slower ,,,
        }
    } else if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        memset(state, 0, 256);
        ProcessRawInput((BYTE*)state);
        if (g_input_opt.is_ri_inited) {
            res = DI_OK;
        } else {
            if (thiz == nullptr) {
                for (auto keydef : keyBindDefine) {
                    ((BYTE*)state)[keydef.dik] = (GetAsyncKeyState(keydef.vk) & 0x8000) ? 0x80 : 0;
                }
            } else {
                res = g_input_opt.realGetDeviceState(thiz, num, state);
            }
        }
    } else {
        if (!g_input_opt.use_get_device_data) {
            memset(state, 0, 256);
            res = g_input_opt.realGetDeviceState(thiz, num, state);
        } else {
            BYTE last_state[256];
            memcpy(last_state, g_input_opt.fake_di_State, 256);

            res = g_input_opt.realGetDeviceState(thiz, num, state);

            DIDEVICEOBJECTDATA rgdod[32];
            while (true) {
                DWORD dwItems = 32;
                res = thiz->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
                if (FAILED(res) || dwItems == 0) {
                    if (res == DIERR_INPUTLOST) {
                        res = thiz->Acquire();
                        if (FAILED(res))
                            break;
                    } else {
                        break;
                    }
                }
                for (DWORD i = 0; i < dwItems; i++) {
                    DWORD scanCode = rgdod[i].dwOfs;
                    if (rgdod[i].dwData & 0x80) {
                        g_input_opt.fake_di_State[scanCode] = 0x80;
                        if (last_state[scanCode] == 0) {
                            ((BYTE*)state)[scanCode] = 0x80;
                            // is_down[scanCode] = 0x80;
                        }
                    } else {
                        if (last_state[scanCode] == 0x80) {
                            ((BYTE*)state)[scanCode] = 0;
                        }
                        g_input_opt.fake_di_State[scanCode] = 0x00;
                    }
                }
            }
        }
    }

    if (g_keybind.size() != 0) {
        static BYTE new_keyBoardState[256] = { 0 };
        bool new_keyBoardState_changed[256] = { 0 };
        memcpy_s(new_keyBoardState, num, ((BYTE*)state), num);
        for (auto& bind : g_keybind) {
            if (IS_KEY_DOWN(((BYTE*)state)[bind.second.dik])) {
                new_keyBoardState[bind.first.dik] |= 0x80;
                if (!new_keyBoardState_changed[bind.first.dik])
                    new_keyBoardState[bind.second.dik] &= (~0x80);
                new_keyBoardState_changed[bind.first.dik] = true;
            }
        }
        memcpy_s(((BYTE*)state), num, new_keyBoardState, num);
    }
    if (g_input_opt.enable_auto_shoot) {
        bool cur_isdown = IS_KEY_DOWN(((BYTE*)state)[g_input_opt.shoot_key_DIK]);
        bool last_isdown = g_input_opt.last_is_auto_shoot_key_down;
        g_input_opt.last_is_auto_shoot_key_down = cur_isdown;
        if (cur_isdown && !last_isdown) {
            g_input_opt.is_auto_shooting = !g_input_opt.is_auto_shooting;
        }
        if (g_input_opt.is_auto_shooting) {
            bool is_other_down = false;
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_Z]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_X]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_C]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_D]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_ESCAPE]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_R]);
            is_other_down |= IS_KEY_DOWN(((BYTE*)state)[DIK_Q]);
            if (is_other_down) {
                g_input_opt.is_auto_shooting = false;
            } else {
                if (g_input_opt.is_th128) {
                    ((BYTE*)state)[DIK_C] = 0x80;
                } else {
                    ((BYTE*)state)[DIK_Z] = 0x80;
                }
            }
        }
    }

    if (g_input_opt.disable_xkey) {
        ((BYTE*)state)[DIK_X] = 0x0;
    }
    if (g_input_opt.disable_xkey && g_input_opt.disable_Ckey_at_same_time) {
        ((BYTE*)state)[DIK_C] = 0x0;
    }
    if (g_input_opt.disable_shiftkey) {
        ((BYTE*)state)[DIK_LSHIFT] = 0x0;
        ((BYTE*)state)[DIK_RSHIFT] = 0x0;
    }
    if (g_input_opt.disable_zkey) {
        ((BYTE*)state)[DIK_Z] = 0x0;
    }
    if (g_input_opt.disable_f10_11_13) {
        ((BYTE*)state)[DIK_F10] = 0x0;
    }
    if (g_input_opt.g_socd_setting != InputOpt::SOCD_Setting::SOCD_Default) {
        static BYTE last_keyBoardState[256] = { 0 };
        static uint32_t cur_time = 0;
        static uint32_t keyBoard_press_time[4] = { 0 };

        BYTE* keyBoardState = (BYTE*)state;
        cur_time++;

        if (IS_KEY_DOWN(keyBoardState[DIK_LEFTARROW]) && !IS_KEY_DOWN(last_keyBoardState[DIK_LEFTARROW]))
            keyBoard_press_time[K_LEFT] = cur_time;
        if (IS_KEY_DOWN(keyBoardState[DIK_RIGHTARROW]) && !IS_KEY_DOWN(last_keyBoardState[DIK_RIGHTARROW]))
            keyBoard_press_time[K_RIGHT] = cur_time;
        if (IS_KEY_DOWN(keyBoardState[DIK_UPARROW]) && !IS_KEY_DOWN(last_keyBoardState[DIK_UPARROW]))
            keyBoard_press_time[K_UP] = cur_time;
        if (IS_KEY_DOWN(keyBoardState[DIK_DOWNARROW]) && !IS_KEY_DOWN(last_keyBoardState[DIK_DOWNARROW]))
            keyBoard_press_time[K_DOWN] = cur_time;

        memcpy_s(last_keyBoardState, num, keyBoardState, num);

        if (IS_KEY_DOWN(keyBoardState[DIK_LEFTARROW]) && IS_KEY_DOWN(keyBoardState[DIK_RIGHTARROW])) {
            if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_2) {
                if (keyBoard_press_time[K_LEFT] > keyBoard_press_time[K_RIGHT]) {
                    keyBoardState[DIK_RIGHTARROW] = 0;
                } else {
                    keyBoardState[DIK_LEFTARROW] = 0;
                }
            } else if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_N) {
                keyBoardState[DIK_RIGHTARROW] = 0;
                keyBoardState[DIK_LEFTARROW] = 0;
            }
        }
        if (IS_KEY_DOWN(keyBoardState[DIK_DOWNARROW]) && IS_KEY_DOWN(keyBoardState[DIK_UPARROW])) {
            if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_2) {
                if (keyBoard_press_time[K_UP] > keyBoard_press_time[K_DOWN]) {
                    keyBoardState[DIK_DOWNARROW] = 0;
                } else {
                    keyBoardState[DIK_UPARROW] = 0;
                }
            } else if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_N) {
                keyBoardState[DIK_DOWNARROW] = 0;
                keyBoardState[DIK_UPARROW] = 0;
            }
        }
    }
    if (g_fast_re_opt.fast_retry_count_down) {
        BYTE* keyBoardState = (BYTE*)state;
        if (g_fast_re_opt.fast_retry_count_down <= g_fast_re_opt.fast_retry_cout_down_max)
            keyBoardState[DIK_ESCAPE] = 0x80;
        if (g_fast_re_opt.fast_retry_count_down <= 1)
            keyBoardState[DIK_R] = 0x80;
    }

    g_input_latency_test.Input();
    return res;
}

#pragma endregion

#pragma region win32Keyboard


HRESULT STDMETHODCALLTYPE GetDeviceState_Changed(LPDIRECTINPUTDEVICE8 thiz, DWORD num, LPVOID state);
BOOL WINAPI GetKeyboardState_Changed(PBYTE keyBoardState)
{
    HRESULT res;
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI || g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        HRESULT res_dinpu8getState = DI_OK;
        BYTE keyboardState_dinput8[256];
        BYTE* keyboardState = keyboardState_dinput8;
        if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI) {
            if (g_input_opt.ddevice) {
                auto pres = g_input_opt.ddevice->Poll();
                if (FAILED(pres)) {
                    if (FAILED(g_input_opt.ddevice->Acquire())) {
                        goto LB_FINAL;
                    }
                    if (FAILED(g_input_opt.ddevice->Poll())) {
                        goto LB_FINAL;
                    }
                }
                res_dinpu8getState = g_input_opt.ddevice->GetDeviceState(256, keyboardState_dinput8);
            } else
                goto LB_FINAL;
        } else {
            if (g_input_opt.is_ri_inited) {
                GetDeviceState_Changed(nullptr, 256, keyboardState);
            } else
                goto LB_FINAL;
        }
        if (res_dinpu8getState == DI_OK) {
            memset(keyBoardState, 0, 256);
            for (auto keydef : keyBindDefine) {
                keyBoardState[keydef.vk] = keyboardState[keydef.dik];
            }
            if ((keyboardState[DIK_LSHIFT] & 0x80) || (keyboardState[DIK_RSHIFT] & 0x80))
                keyBoardState[VK_SHIFT] = 0x80;
            if ((keyboardState[DIK_LCONTROL] & 0x80) || (keyboardState[DIK_RCONTROL] & 0x80))
                keyBoardState[VK_CONTROL] = 0x80;
            if ((keyboardState[DIK_LMENU] & 0x80) || (keyboardState[DIK_RMENU] & 0x80))
                keyBoardState[VK_MENU] = 0x80;
            return TRUE;
        }
    }
LB_FINAL:
    res = g_input_opt.g_realGetKeyboardState(keyBoardState);
    if (g_keybind.size() != 0) {
        static BYTE new_keyBoardState[256] = { 0 };
        bool new_keyBoardState_changed[256] = { 0 };
        memcpy_s(new_keyBoardState, 256, keyBoardState, 256);
        for (auto& bind : g_keybind) {
            if (IS_KEY_DOWN(keyBoardState[bind.second.vk])) {
                new_keyBoardState[bind.first.vk] |= 0x80;
                if (!new_keyBoardState_changed[bind.first.vk])
                    new_keyBoardState[bind.second.vk] &= (~0x80);
                switch (bind.second.vk) {
                default:
                    break;
                case VK_LSHIFT:
                case VK_RSHIFT:
                    if (!new_keyBoardState_changed[VK_LSHIFT])
                        new_keyBoardState[VK_SHIFT] &= (~0x80);
                    break;
                case VK_LCONTROL:
                case VK_RCONTROL:
                    if (!new_keyBoardState_changed[VK_CONTROL])
                        new_keyBoardState[VK_CONTROL] &= (~0x80);
                    break;
                case VK_LMENU:
                case VK_RMENU:
                    if (!new_keyBoardState_changed[VK_MENU])
                        new_keyBoardState[VK_MENU] &= (~0x80);
                    break;
                }
                new_keyBoardState_changed[bind.first.vk] = true;
            }
        }
        if (IS_KEY_DOWN(new_keyBoardState[VK_LSHIFT]) || IS_KEY_DOWN(new_keyBoardState[VK_RSHIFT])) {
            new_keyBoardState[VK_SHIFT] |= 0x80;
        }
        if (IS_KEY_DOWN(new_keyBoardState[VK_LCONTROL]) || IS_KEY_DOWN(new_keyBoardState[VK_RCONTROL])) {
            new_keyBoardState[VK_CONTROL] |= 0x80;
        }
        if (IS_KEY_DOWN(new_keyBoardState[VK_LMENU]) || IS_KEY_DOWN(new_keyBoardState[VK_RMENU])) {
            new_keyBoardState[VK_MENU] |= 0x80;
        }
        memcpy_s(keyBoardState, 256, new_keyBoardState, 256);
    }
    if (g_input_opt.disable_xkey) {
        keyBoardState['X'] = 0x0;
    }
    if (g_input_opt.disable_xkey && g_input_opt.disable_Ckey_at_same_time) {
        keyBoardState['C'] = 0x0;
    }
    if (g_input_opt.disable_shiftkey) {
        keyBoardState[VK_LSHIFT] = keyBoardState[VK_LSHIFT] = keyBoardState[VK_SHIFT] = 0x0;
    }
    if (g_input_opt.disable_zkey) {
        keyBoardState['Z'] = 0x0;
    }
    if (g_input_opt.disable_f10_11_13) {
        keyBoardState[VK_F10] = 0x0;
    }
    if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_Default) {
        g_input_latency_test.Input();
        return res;
    }
    static BYTE last_keyBoardState[256] = { 0 };
    static uint32_t cur_time = 0;
    static uint32_t keyBoard_press_time[4] = { 0 };

    cur_time++;

    if (IS_KEY_DOWN(keyBoardState[VK_LEFT]) && !IS_KEY_DOWN(last_keyBoardState[VK_LEFT]))
        keyBoard_press_time[K_LEFT] = cur_time;
    if (IS_KEY_DOWN(keyBoardState[VK_RIGHT]) && !IS_KEY_DOWN(last_keyBoardState[VK_RIGHT]))
        keyBoard_press_time[K_RIGHT] = cur_time;
    if (IS_KEY_DOWN(keyBoardState[VK_UP]) && !IS_KEY_DOWN(last_keyBoardState[VK_UP]))
        keyBoard_press_time[K_UP] = cur_time;
    if (IS_KEY_DOWN(keyBoardState[VK_DOWN]) && !IS_KEY_DOWN(last_keyBoardState[VK_DOWN]))
        keyBoard_press_time[K_DOWN] = cur_time;

    memcpy_s(last_keyBoardState, 256, keyBoardState, 256);

    if (IS_KEY_DOWN(keyBoardState[VK_LEFT]) && IS_KEY_DOWN(keyBoardState[VK_RIGHT])) {
        if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_2) {
            if (keyBoard_press_time[K_LEFT] > keyBoard_press_time[K_RIGHT]) {
                keyBoardState[VK_RIGHT] = 0;
            } else {
                keyBoardState[VK_LEFT] = 0;
            }
        } else if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_N) {
            keyBoardState[VK_RIGHT] = 0;
            keyBoardState[VK_LEFT] = 0;
        }
    }
    if (IS_KEY_DOWN(keyBoardState[VK_DOWN]) && IS_KEY_DOWN(keyBoardState[VK_UP])) {
        if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_2) {
            if (keyBoard_press_time[K_UP] > keyBoard_press_time[K_DOWN]) {
                keyBoardState[VK_DOWN] = 0;
            } else {
                keyBoardState[VK_UP] = 0;
            }
        } else if (g_input_opt.g_socd_setting == InputOpt::SOCD_Setting::SOCD_N) {
            keyBoardState[VK_DOWN] = 0;
            keyBoardState[VK_UP] = 0;
        }
    }
    g_input_latency_test.Input();
    return res;
}
#pragma endregion


void HookKeyboardInput()
{
    HookIAT(GetModuleHandle(NULL), "user32.dll", "GetKeyboardState", GetKeyboardState_Changed, (void**)&g_input_opt.g_realGetKeyboardState);
    HookIAT(GetModuleHandle(NULL), "dinput8.dll", "DirectInput8Create", DirectInput8Create_Changed, (void**)&g_input_opt.g_realDirectInput8Create);
}

void SetDinput8DeviceData()
{
    if (g_input_opt.use_get_device_data) {
        if (g_input_opt.use_get_device_data && g_input_opt.ddevice) {
            DIPROPDWORD dipdw;
            dipdw.diph.dwSize = sizeof(DIPROPDWORD);
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            dipdw.diph.dwObj = 0;
            dipdw.diph.dwHow = DIPH_DEVICE;
            dipdw.dwData = 64;
            SetProperty_Changed(g_input_opt.ddevice, DIPROP_BUFFERSIZE, &dipdw.diph);
        } else {
            DIPROPDWORD dipdw;
            dipdw.diph.dwSize = sizeof(DIPROPDWORD);
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            dipdw.diph.dwObj = 0;
            dipdw.diph.dwHow = DIPH_DEVICE;
            dipdw.dwData = 0;
            SetProperty_Changed(g_input_opt.ddevice, DIPROP_BUFFERSIZE, &dipdw.diph);
        }
    }
}

void InitInput()
{
    if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
        if (g_gameGuiHwnd == nullptr)
            return;
        if (g_input_opt.ddevice != nullptr) {
            g_input_opt.ddevice->Unacquire();
        }
        if (g_input_opt.is_ri_inited) {
            RAWINPUTDEVICE Rid;
            Rid.usUsagePage = 0x01;
            Rid.usUsage = 0x06;
            Rid.dwFlags = RIDEV_REMOVE;
            Rid.hwndTarget = NULL;
            auto res = RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
            if (res) {
                g_input_opt.is_ri_inited = false;
            } else {
                return;
            }
        }
        RAWINPUTDEVICE Rid;
        Rid.usUsagePage = 0x01;
        Rid.usUsage = 0x06; // HID_USAGE_GENERIC_KEYBOARD
        Rid.dwFlags = 0;
        Rid.hwndTarget = (HWND)*g_gameGuiHwnd;

        if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
            return;
        }
        g_input_opt.is_ri_inited = true;
        memset(g_input_opt.fake_di_State, 0, 256);
        return;
    } else if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI) {
        if (g_gameGuiHwnd == nullptr)
            return;
        if (g_input_opt.is_ri_inited) {
            RAWINPUTDEVICE Rid;
            Rid.usUsagePage = 0x01;
            Rid.usUsage = 0x06;
            Rid.dwFlags = RIDEV_REMOVE;
            Rid.hwndTarget = NULL;
            auto res = RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
            if (res) {
                g_input_opt.is_ri_inited = false;
            }
        }
        if (g_input_opt.ddevice != nullptr) {
            g_input_opt.ddevice->Unacquire();
            g_input_opt.ddevice->Acquire();
            return;
        }
        bool is_failed_dinput8 = false;
        // if (is_failed_dinput8)
        //     return;

        if (g_input_opt.dinput == nullptr) {
            if (g_input_opt.g_realDirectInput8Create) {
                DWORD d;
                auto res = DirectInput8Create_Changed(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&d, NULL);
                if (res != DI_OK || g_input_opt.dinput == nullptr) {
                    is_failed_dinput8 = true;
                    return;
                }
            } else {
                is_failed_dinput8 = true;
                return;
            }
        }
        DWORD dev;
        auto res1 = g_input_opt.dinput->CreateDevice(GUID_SysKeyboard, (LPDIRECTINPUTDEVICE8*)&dev, NULL);
        if (res1 != DI_OK || g_input_opt.ddevice == nullptr) {
            is_failed_dinput8 = true;
            return;
        }
        is_failed_dinput8 |= FAILED(g_input_opt.ddevice->SetDataFormat(&c_dfDIKeyboard));
        is_failed_dinput8 |= FAILED(g_input_opt.ddevice->SetCooperativeLevel((HWND)*g_gameGuiHwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
        is_failed_dinput8 |= FAILED(g_input_opt.ddevice->Acquire());
        memset(g_input_opt.fake_di_State, 0, 256);
        return;
    }
}

}