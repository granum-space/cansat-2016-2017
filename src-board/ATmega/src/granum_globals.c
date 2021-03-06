#include "granum_globals.h"

//Дескрипторы устройств
rscs_ads1115_t * ads1115;
rscs_adxl345_t * adxl345;
rscs_bmp280_descriptor_t * bmp280;
rscs_dht22_t * dht22;
rscs_ds18b20_t * ds18b20;
rscs_tsl2561_t * tsl2561_A, * tsl2561_B, * tsl2561_C;
rscs_uart_bus_t * uart_data, * uart_debug;

//Счётчик тиков
uint32_t tick_counter;

//Пакеты
gr_telemetry_fast_t telemetry_fast;
gr_telemetry_slow_t telemetry_slow;
gr_telemetry_so_slow_t telemetry_so_slow;

//Статусы
gr_stm_state_t gr_stm_state;
gr_status_t gr_status;

//Порог освещённости для опознания выхода из ракеты (в люксах)
uint8_t gr_luminosity_threshhold = 20;
