#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

void SPI2_GPIO_MasterInit(void);
void SPI2_MasterInit(void);
void SPI2_SendByte(uint8_t data);
uint8_t SPI2_ReceiveByte(void);
uint8_t SPI2_TransferByte(uint8_t data);
void SPI2_test(void);
#endif
