/**
 * @file joystick.c
 * @brief Analog joystick driver implementation for CH32X035
 */

#include "joystick.h"
#include "ch32x035_conf.h"
#include "timer.h"
#include "log.h"

#if JOYSTICK_DEBUG
#  define JOY_DBG(fmt, ...)  LOG_DEBUG(fmt, ##__VA_ARGS__)
#else
#  define JOY_DBG(fmt, ...)  ((void)0)
#endif

/* Pin / channel mapping */
#define JOY_GPIO_PORT_ANALOG   GPIOA
#define JOY_GPIO_PIN_VRX       GPIO_Pin_1
#define JOY_GPIO_PIN_VRY       GPIO_Pin_2
#define JOY_ADC_CHANNEL_VRX    ADC_Channel_1
#define JOY_ADC_CHANNEL_VRY    ADC_Channel_2

#define JOY_GPIO_PORT_SW       GPIOA
#define JOY_GPIO_PIN_SW        GPIO_Pin_3

/* Sampling time used for both channels. 11 cycles gives a good
 * balance between speed and accuracy for a resistive potentiometer. */
#define JOY_ADC_SAMPLE_TIME    ADC_SampleTime_11Cycles

static volatile bool             s_initialized = false;
static volatile joystick_data_t  s_data        = { 0U, 0U, 0U };

/**
 * @brief Perform a single software-triggered ADC conversion.
 *
 * Selects @p channel, triggers the conversion, polls the EOC flag with
 * a bounded timeout, then returns the 12-bit result.  Returns 0 on
 * timeout to keep the value within the valid [0, ADC_MAX] range.
 */
static uint16_t adc_read_channel(uint8_t channel)
{
    uint32_t timeout = JOYSTICK_ADC_TIMEOUT;

    ADC_RegularChannelConfig(ADC1, channel, 1U, JOY_ADC_SAMPLE_TIME);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {
        if (--timeout == 0U) {
            JOY_DBG("ADC timeout on channel %u", (unsigned)channel);
            return 0U;
        }
    }

    uint16_t value = ADC_GetConversionValue(ADC1);

    /* Clamp defensively — result should always be 12-bit. */
    if (value > (uint16_t)JOYSTICK_ADC_MAX) {
        value = (uint16_t)JOYSTICK_ADC_MAX;
    }

    return value;
}

/**
 * @brief Read VRx, VRy and SW into the module's internal state.
 */
static void joystick_sample(void)
{
    uint16_t vrx = adc_read_channel(JOY_ADC_CHANNEL_VRX);
    uint16_t vry = adc_read_channel(JOY_ADC_CHANNEL_VRY);

    /* Active-low: pressed  -> PA3 low  -> sw = 1
     *             released -> PA3 high -> sw = 0  */
    uint8_t sw = (GPIO_ReadInputDataBit(JOY_GPIO_PORT_SW, JOY_GPIO_PIN_SW) == Bit_RESET)
                 ? 1U : 0U;

    s_data.vrx = vrx;
    s_data.vry = vry;
    s_data.sw  = sw;
}

void joystick_init(void)
{
    GPIO_InitTypeDef gpio = {0};
    ADC_InitTypeDef  adc  = {0};

    /* PA1, PA2 -> analog input (no pull) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin  = JOY_GPIO_PIN_VRX | JOY_GPIO_PIN_VRY;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(JOY_GPIO_PORT_ANALOG, &gpio);

    /* PA3 -> digital input with internal pull-up */
    gpio.GPIO_Pin   = JOY_GPIO_PIN_SW;
    gpio.GPIO_Mode  = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JOY_GPIO_PORT_SW, &gpio);

    /* ADC1: single-channel, single-conversion, software-triggered */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* ADC clock = PCLK2 / 8  (48 MHz / 8 = 6 MHz, within spec) */
    ADC_CLKConfig(ADC1, ADC_CLK_Div8);

    ADC_StructInit(&adc);
    adc.ADC_Mode               = ADC_Mode_Independent;
    adc.ADC_ScanConvMode       = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel       = 1U;
    ADC_Init(ADC1, &adc);

    ADC_Cmd(ADC1, ENABLE);

    /* Take an initial sample so data is valid before the first poll. */
    joystick_sample();

    s_initialized = true;

    JOY_DBG("joystick_init done — VRx=%u VRy=%u SW=%u",
            (unsigned)s_data.vrx, (unsigned)s_data.vry, (unsigned)s_data.sw);
}

void joystick_poll(void)
{
    if (!s_initialized) {
        return;
    }

    static uint32_t s_last_poll_ms = 0U;
    uint32_t now = get_tick();

    /* uint32 subtraction wraps correctly even when get_tick() rolls over. */
    if ((now - s_last_poll_ms) < JOYSTICK_POLL_PERIOD_MS) {
        return;
    }
    s_last_poll_ms = now;

    joystick_sample();

    JOY_DBG("joystick — VRx=%u VRy=%u SW=%u",
             (unsigned)s_data.vrx, (unsigned)s_data.vry, (unsigned)s_data.sw);
}

bool joystick_get_data(joystick_data_t *out)
{
    if (out == NULL) {
        return false;
    }

    if (!s_initialized) {
        return false;
    }

    out->vrx = s_data.vrx;
    out->vry = s_data.vry;
    out->sw  = s_data.sw;

    return true;
}
