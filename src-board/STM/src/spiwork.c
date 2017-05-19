/*
 * spiwork.c
 *
 *  Created on: 08 апр. 2017 г.
 *      Author: developer
 */
#include <stdbool.h>

#include "ringbuf.h"
#include "adxl375.h"

#include <stm32f10x_conf.h>

#define AMRQ_STATUS_Rx 	0xAA
#define AMRQ_STATUS_Tx 	0x9B //TODO
#define AMRQ_GPS 		0xCC
#define AMRQ_ACC_DATA	0xDD
#define AMRQ_ACC_STOP 	0xEE

static uint32_t _acc_low, _acc_high, _acc_now;
static uint8_t _acc_params_shift;

static void _receive();
static void _transmit();

enum { //FIXME нужен статик
	RECEIVER_IDLE,
	RECEIVEING_STATUS,
	RECEIVEING_ACC_PARAMS
} receiver_state;

enum {
	TRANSMITTER_IDLE,
	TRANSMITTING_STATUS,
	TRANSMITTING_GPS,
	TRANSMITTING_ACC,
} transmitter_state;

void SPI2_IRQHandler(rscs_ringbuf_t * buf) {
	volatile bool RXNE = SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE);
	volatile bool TXE = SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE);
	/*if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE))*/ _receive();
	/*else if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE))*/ _transmit();
	RXNE = SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE);
	TXE = SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE);
}

void  spiwork_init() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	SPI_InitTypeDef spiconf;
	spiconf.SPI_CRCPolynomial = 7; // Отключено (так Василий сказал)
	spiconf.SPI_DataSize = SPI_DataSize_8b;
	spiconf.SPI_NSS = SPI_NSS_Hard;
	spiconf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiconf.SPI_FirstBit = SPI_FirstBit_MSB;
	spiconf.SPI_Mode = SPI_Mode_Slave;
	spiconf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	spiconf.SPI_CPHA = SPI_CPHA_1Edge;//FIXME настройки SPI как в атмеге
	spiconf.SPI_CPOL = SPI_CPOL_Low;

	SPI_Init(SPI2, &spiconf);

	GPIO_InitTypeDef portInit;

	portInit.GPIO_Speed = GPIO_Speed_50MHz;
	portInit.GPIO_Mode = GPIO_Mode_AF_PP;
	portInit.GPIO_Pin = GPIO_Pin_14; //MISO
	GPIO_Init(GPIOB, &portInit);

	portInit.GPIO_Pin = GPIO_Pin_15, GPIO_Pin_13, GPIO_Pin_12;// MOSI, SCLK, CS
	portInit.GPIO_Mode = GPIO_Mode_IN_FLOATING; //FIXME мб AF_OD
	GPIO_Init(GPIOB, &portInit);

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
	//SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = SPI2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);

	NVIC_EnableIRQ(SPI2_IRQn);

	_transmit();

	SPI_Cmd(SPI2, ENABLE);
}

int daata = 0;

static void _receive() {
	uint16_t data;
	data = SPI_I2S_ReceiveData(SPI2);
	switch(receiver_state) {
	case RECEIVER_IDLE:
		switch(data) {
		case AMRQ_STATUS_Rx:
			receiver_state = RECEIVEING_STATUS;
			break;

		case AMRQ_STATUS_Tx:
			transmitter_state = TRANSMITTING_STATUS;
			break;

		case AMRQ_ACC_DATA:
			receiver_state = RECEIVEING_ACC_PARAMS;
			_acc_high = 0;
			_acc_params_shift = 0;
			_acc_params_shift = 0;
			break;

		case AMRQ_GPS:
			transmitter_state = TRANSMITTING_GPS;
			break;

		default: break;
		}
		break;

	case RECEIVEING_STATUS:
		//TODO прием пакета статуса
		break;

	case RECEIVEING_ACC_PARAMS:
		if(_acc_params_shift < 32) {
			_acc_low |= (data << _acc_params_shift);
		}

		else {
			if(_acc_params_shift < 64) {
				_acc_high |= (data << (_acc_params_shift - 32));
			}

			else {
				_acc_high *= 6;
				_acc_low *= 6;
				_acc_now = _acc_low;
				receiver_state = RECEIVER_IDLE;
				transmitter_state = TRANSMITTING_ACC;
			}
		}
		break;

	default: break;
	}
}

static void _transmit() {
	switch(transmitter_state) {
	case TRANSMITTER_IDLE:
		SPI_I2S_SendData(SPI2, 0xFF);
		break;

	case TRANSMITTING_STATUS:
		//TODO передача пакета статуса
		break;

	case TRANSMITTING_ACC:

		if(_acc_now > _acc_high) {
			transmitter_state = TRANSMITTER_IDLE;
		}

		else {
			SPI_I2S_SendData(SPI2, rscs_ringbuf_see_from_tail(adxl_buf, _acc_high));
		}

		break;

	case TRANSMITTING_GPS:
		//TODO передача данных GPS
		break;
	}
}
