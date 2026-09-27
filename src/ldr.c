#include "ldr.h"
#include "stm32f1xx_hal.h"

/* LDR module analog output (AO) on PA0 = ADC1 channel 0 */
static ADC_HandleTypeDef hadc1;

void LDR_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin  = GPIO_PIN_0;
    g.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &g);

    hadc1.Instance                   = ADC1;
    hadc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode    = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion       = 1;
    HAL_ADC_Init(&hadc1);

    ADC_ChannelConfTypeDef c = {0};
    c.Channel      = ADC_CHANNEL_0;
    c.Rank         = ADC_REGULAR_RANK_1;
    c.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &c);

    /* Calibration skipped: Wokwi implements basic ADC1 conversion only */
}

/* One software-triggered conversion, polled with a 10 ms timeout */
bool LDR_ReadRaw(uint16_t *raw)
{
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return false;
    }
    if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return false;
    }
    *raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return true;
}

/* The LDR module's AO voltage RISES as it gets DARKER, so the value is
   inverted: 0 % = darkest, 100 % = brightest. This is a relative level,
   not calibrated lux. */
uint8_t LDR_RawToPercent(uint16_t raw)
{
    if (raw > 4095U) { raw = 4095U; }
    return (uint8_t)(((4095U - raw) * 100U) / 4095U);
}