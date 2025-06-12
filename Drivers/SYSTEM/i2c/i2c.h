#ifndef __I2C_H
#define __I2C_H

#include "sys.h"

// AS5600 I2C 地址（7位地址，实际通信中左移一位后加上R/W位）
#define AS5600_I2C_ADDR        0x36

// 寄存器地址
#define AS5600_ANGLE_REGISTER  0x0C

// 初始化接口
void AS5600_Init(void);

// 获取角度（返回值为0~4095）
uint16_t AS5600_ReadRawAngle(void);

// 获取角度（0~360°）
float AS5600_ReadAngle_Degrees(void);

// 获取角度（-π~π）
float AS5600_ReadAngle_Radians(void);

#endif
