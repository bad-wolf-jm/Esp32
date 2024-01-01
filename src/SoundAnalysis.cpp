#include "SoundAnalysis.h"

#include <HardwareSerial.h>
#include <driver/adc.h>

SoundAnalyzer::SoundAnalyzer( i2s_port_t port, int samplingFrequency, i2s_channel_fmt_t channelFormat,
                              i2s_bits_per_sample_t bitsPerSample, uint8_t bufferCount, int bufferSize )
    : _port{ port }
    , _sampleRate{ samplingFrequency }
    , _bitsPerSample{ bitsPerSample }
    , _bufferCount{ bufferCount }
    , _bufferSize{ bufferSize }
    , _channelFormat{ channelFormat }
{
    // clang-format off
    const i2s_config_t i2s_config = { //
        .mode                 = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN ),
        .sample_rate          = _sampleRate,
        .bits_per_sample      = _bitsPerSample,
        .channel_format       = _channelFormat,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count        = bufferCount,
        .dma_buf_len          = bufferSize,
        .use_apll             = false 
    };
    // clang-format on

    switch( _bitsPerSample )
    {
    case I2S_BITS_PER_SAMPLE_8BIT:
        _bufferSizeInBytes = _bufferSize;
        break;
    case I2S_BITS_PER_SAMPLE_16BIT:
        _bufferSizeInBytes = _bufferSize * 2;
        break;
    case I2S_BITS_PER_SAMPLE_24BIT:
        _bufferSizeInBytes = _bufferSize * 3;
        break;
    case I2S_BITS_PER_SAMPLE_32BIT:
    default:
        _bufferSizeInBytes = _bufferSize * 4;
        break;
    }

    _sampleBuffer = vector_t<uint8_t>( _bufferSizeInBytes );

    _real      = vector_t<double>( _bufferSize );
    _imaginary = vector_t<double>( _bufferSize );

    ESP_ERROR_CHECK( adc1_config_width( _adcBitsWidth ) );
    ESP_ERROR_CHECK( adc1_config_channel_atten( _adcChannel, _adcAttenuation ) );
    ESP_ERROR_CHECK( i2s_driver_install( _port, &i2s_config, 0, NULL ) );
    ESP_ERROR_CHECK( i2s_set_adc_mode( _adcUnit, _adcChannel ) );

    Serial.println( "I2S driver installed." );
}

void SoundAnalyzer::FillBuffer()
{
    size_t bytesRead = 0;

    ESP_ERROR_CHECK( i2s_adc_enable( _port ) );
    ESP_ERROR_CHECK( i2s_read( _port, (void *)_sampleBuffer.data(), _bufferSizeInBytes, &bytesRead, ( 100 / portTICK_RATE_MS ) ) );
    ESP_ERROR_CHECK( i2s_adc_disable( _port ) );

    if( bytesRead != _bufferSizeInBytes )
    {
        Serial.printf( "Could only read %u bytes of %u in FillBuffer()\n", bytesRead, _bufferSizeInBytes );
        return;
    }

    // BUG: _take sample type info account
    std::fill( _imaginary.begin(), _imaginary.end(), 0.0 );
    switch( _bitsPerSample )
    {
    case I2S_BITS_PER_SAMPLE_8BIT:
        FillVector( _real, (uint8_t *)_sampleBuffer.data(), _bufferSize );
        break;
    case I2S_BITS_PER_SAMPLE_16BIT:
        FillVector( _real, (uint16_t *)_sampleBuffer.data(), _bufferSize );
        break;
    case I2S_BITS_PER_SAMPLE_24BIT:
        // THIS WILL NOT WORK, do template specialization and conversion!!!!
        FillVector( _real, (uint16_t *)_sampleBuffer.data(), _bufferSize );
        break;
    case I2S_BITS_PER_SAMPLE_32BIT:
    default:
        FillVector( _real, (uint32_t *)_sampleBuffer.data(), _bufferSize );
        break;
    }

    for( int i = 0; i < _bufferSize; i++ )
        _real[i] = _sampleBuffer[i];
}
