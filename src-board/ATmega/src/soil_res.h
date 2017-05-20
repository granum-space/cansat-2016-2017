/*
 * digipot.h
 *
 *  Created on: 29 апр. 2017 г.
 *      Author: developer
 */

#ifndef DIGIPOT_H_
#define DIGIPOT_H_

#include "granum_config.h"
#include "rscs/spi.h"
#include "rscs/ads1115.h"

// шаг потенциометра
#define DP_STEP 392
/* перед каждой записью сопротивления необходимо передавать
	 * так называемый "command byte"
	 * 0 и 1 биты ставятся в 0 и 1 соответственно, если мы используем терминалы
	   PA, PB и PW с индексом 0
	 * 4 и 5 биты ставятся в 0 и 1, если мы хотим, чтобы дигипот прочитал байт,
       идущий после command byte */
#define COMMAND_BYTE_DP0 0b00010001
#define COMMAND_BYTE_DP1 0b00010010
#define RES1 94000
#define RES2 98000

// Инициализация
void rscs_soil_res_init(void);

// Запись сопротивления от 0 до 200кОм
void rscs_digipot_set_res(uint32_t resistance);

// Функция снимает показания сопротивления грунта с трех пар стержней
rscs_e rscs_get_soil_res(uint32_t *res12, uint32_t *res23, uint32_t *res13);

rscs_e get_res_test();

#endif /* DIGIPOT_H_ */
