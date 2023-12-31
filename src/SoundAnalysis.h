#pragma once
#include "Core/Vector.h"
#include <driver/i2s.h>

class SoundAnalyzer
{
  public:
    SoundAnalyzer( i2s_port_t port, int samplingFrequency, i2s_channel_fmt_t channelFormat,
                   i2s_bits_per_sample_t bitsPerSample = I2S_BITS_PER_SAMPLE_16BIT, uint8_t bufferCount = 2, int bufferSize = 512 );

  private:
    int _bufferSize = 512;
    int _sampleRate = 44100;

    i2s_port_t            _port              = I2S_NUM_0;
    adc_bits_width_t      _adcBitsWidth      = ADC_WIDTH_BIT_12;
    adc_unit_t            _adcUnit           = ADC_UNIT_1;
    adc1_channel_t        _adcChannel        = ADC1_CHANNEL_0;
    adc_atten_t           _adcAttenuation    = ADC_ATTEN_DB_0;
    i2s_channel_fmt_t     _channelFormat     = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_bits_per_sample_t _bitsPerSample     = I2S_BITS_PER_SAMPLE_16BIT;
    uint8_t               _bufferCount       = 2;
    size_t                _bufferSizeInBytes = 0;

    vector_t<uint8_t> _sampleBuffer;

    vector_t<double> _peaks;
    vector_t<double> _real;
    vector_t<double> _imaginary;

  private:
    void FillBuffer();
};