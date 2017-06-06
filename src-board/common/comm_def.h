#ifndef COMM_DEF_H_
#define COMM_DEF_H_

#include <stdbool.h>
#include "stdint.h"

#include "rscs/error.h" //FIXME этого файла нет на STM. Нужно поменять

//Вспомогательный тип для хранения ускорений
typedef struct {
	int16_t x, y, z;
} accelerations_t;

//Телеметрийные пакеты
typedef struct {
	uint16_t marker; //Must be 0xACCA

	uint16_t number;
	uint32_t tick;

	accelerations_t accelerations;
	rscs_e adxl345_error;

	struct {
		uint16_t v0, v1;
	} luminosity[3];
	rscs_e tsl2561_A_error, tsl2561_B_error, tsl2561_C_error;

	uint32_t time;

	uint32_t checksumm;
} gr_telemetry_fast_t;

typedef struct {
	uint16_t marker; //Must be 0xFCFC

	uint16_t number;
	uint32_t tick;

	int32_t pressure;
	int32_t temperature_bmp;
	rscs_e bmp280_error;

	float latitude, longtitude, height;
	bool gps_hasFix;

	uint32_t time;

	uint32_t checksumm;
} gr_telemetry_slow_t;

typedef struct {
	uint16_t marker; //Must be 0xFC1A

	uint16_t number;
	uint32_t tick;

	int16_t temperature_ds18;
	rscs_e ds18b20_error;

	int16_t temperature_dht;
	uint16_t humidity;
	rscs_e dht22_error;

	int32_t temperature_soil[3];
	rscs_e thermistor_A_error, thermistor_B_error, thermistor_C_error;

	struct {
		uint16_t adc_low, adc_high;
		float resistance;
	} soilresist_data[3];

	uint32_t time;

	uint32_t checksumm;
} gr_telemetry_so_slow_t;

typedef struct {
	uint16_t marker; //Must be 0xFA7B
	uint16_t start_i, end_i;
	accelerations_t data[];
} gr_telemetry_adxl375_t;

//Статусные пакеты TODO: Василий: Рассмотреть вомзожность упразднения режима LIFTING, присвоить енумам значения
typedef struct {
	enum {
		GR_MODE_IDLE = 0,
		GR_MODE_AWAITING_START,
		GR_MODE_LIFTING,
		GR_MODE_AWAITING_PARACHUTE,
		GR_MODE_AWAITING_LEGS,
		GR_MODE_LANDING,
		GR_MODE_ONGROUND
	} mode;

	bool seeds_activated;
} gr_status_t;

enum {
	ADXL_STATUS_IDLE,
	ADXL_STATUS_COLLECTING,
	ADXL_STATUS_FINISHED
} stm_adxl_status;

typedef struct {

	uint8_t adxl_status; //Одно из значений enum stm_adxl_status
	bool hasFix;
	float lat, lon, alt;

} gr_status_stm_t;


//Запросы от атмеги к стм
#define AMRQ_STATUS_Rx 		0xAA
#define AMRQ_SELFSTATUS_Tx	0xBB
#define AMRQ_ACC_DATA		0xCC

#endif //COMM_DEF_H_
