#ifndef __SPI_H
#define __SPI_H
#include "delay.h"
#include "sys.h"

void SPI2_GPIO_SlaveInit(void);
void SPI2_SlaveInit(void);
uint8_t SPI2_ReceiveByte(void);
void SPI2_SendByte(uint8_t data);
void SPI2_Test(void);

#endif
