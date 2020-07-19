extern void _Error_Handler(char *file, int line);
#define CHECK_HAL_RESULT(x) do { if((x) != HAL_OK) _Error_Handler(__FILE__, __LINE__); } while(0)
