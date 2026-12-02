#include "ldr_sensors.h"
#include "tracker_config.h"
uint8_t LDR_ReadAll(ADC_HandleTypeDef *adc, uint16_t values[4]) {
    const uint32_t channels[4] = {ADC_CHANNEL_4, ADC_CHANNEL_1, ADC_CHANNEL_5, ADC_CHANNEL_0};
    for (unsigned n=0; n<4; ++n) {
        ADC_ChannelConfTypeDef config = {0};
        config.Channel=channels[n]; config.Rank=ADC_REGULAR_RANK_1;
        config.SamplingTime=ADC_SAMPLINGTIME_COMMON_1;
        if (HAL_ADC_ConfigChannel(adc,&config)!=HAL_OK) return 0;
        uint32_t sum=0;
        for (unsigned i=0;i<LDR_SAMPLES;++i) {
            if (HAL_ADC_Start(adc)!=HAL_OK || HAL_ADC_PollForConversion(adc,1)!=HAL_OK) {
                HAL_ADC_Stop(adc); return 0;
            }
            sum+=HAL_ADC_GetValue(adc);
            if (HAL_ADC_Stop(adc)!=HAL_OK) return 0;
        }
        values[n]=(uint16_t)(sum/LDR_SAMPLES);
    }
    return 1;
}
