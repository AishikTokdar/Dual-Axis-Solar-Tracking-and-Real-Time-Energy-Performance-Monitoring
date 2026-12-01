#ifndef LDR_SENSORS_H
#define LDR_SENSORS_H
#include "main.h"
uint8_t LDR_ReadAll(ADC_HandleTypeDef *adc, uint16_t values[4]);
#endif
