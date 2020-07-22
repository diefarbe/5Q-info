extern void LED_IRQHandler(void);
extern void LED_Start(void);
extern void LED_Set_LED(unsigned row, unsigned col, uint16_t c0, uint16_t c1, uint16_t c2);
void LED_Set_LED_RGB(unsigned row, unsigned col, uint16_t r, uint16_t g, uint16_t b);
