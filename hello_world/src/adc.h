extern void ADC_Setup_ADC(void);
extern void ADC_Start(uint8_t column);
extern void ADC_MaskCallback(uint8_t column, uint16_t mask);
extern int16_t ADC_ExtraChannels[14];
#define ADC_EXTRACHANNEL_11   3
#define ADC_EXTRACHANNEL_12   4
#define ADC_EXTRACHANNEL_13   5
#define ADC_EXTRACHANNEL_15_0 6
#define ADC_EXTRACHANNEL_14_0 7
#define ADC_EXTRACHANNEL_15_1 8
#define ADC_EXTRACHANNEL_14_1 9
#define ADC_EXTRACHANNEL_15_2 10
#define ADC_EXTRACHANNEL_14_2 11
#define ADC_EXTRACHANNEL_15_3 12
#define ADC_EXTRACHANNEL_14_3 13
