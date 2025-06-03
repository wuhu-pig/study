#ifndef __AS5600_H        /* 防止头文件重复包含 */
#define __AS5600_H

/* 包含需要的头文件 */
#include "./SYSTEM/sys/sys.h"      /* 系统配置头文件 */
#include "./SYSTEM/delay/delay.h"   /* 延时函数头文件 */
#include "stdio.h"                  /* 标准输入输出头文件 */

/* I2C引脚定义 */
/* SCL引脚配置 */
#define IIC_SCL_GPIO_PORT               GPIOB                                          /* SCL使用GPIOB端口 */
#define IIC_SCL_GPIO_PIN                SYS_GPIO_PIN8                                 /* SCL使用PIN8引脚 */
#define IIC_SCL_GPIO_CLK_ENABLE()       do{ RCC->AHB1ENR |= 1 << 1; }while(0)       /* 使能GPIOB时钟 */

/* SDA引脚配置 */
#define IIC_SDA_GPIO_PORT               GPIOB                                          /* SDA使用GPIOB端口 */
#define IIC_SDA_GPIO_PIN                SYS_GPIO_PIN9                                 /* SDA使用PIN9引脚 */
#define IIC_SDA_GPIO_CLK_ENABLE()       do{ RCC->AHB1ENR |= 1 << 1; }while(0)       /* 使能GPIOB时钟 */

/* SDA方向控制宏定义 */
#define SDA1_IN()  IIC_SDA_GPIO_PORT->MODER &= ~(0x3<<(10*2))                        /* 将SDA配置为输入模式 */
#define SDA1_OUT() IIC_SDA_GPIO_PORT->MODER &= ~(0x3<<(10*2));IIC_SDA_GPIO_PORT->MODER |= (0x1<<(10*2))  /* 将SDA配置为输出模式 */

/* I2C信号线控制宏定义 */
#define Sim_I2C1_SCL_LOW          (IIC_SCL_GPIO_PORT->ODR &= ~(IIC_SCL_GPIO_PIN))    /* SCL输出低电平 */
#define Sim_I2C1_SCL_HIG          (IIC_SCL_GPIO_PORT->ODR |=  (IIC_SCL_GPIO_PIN))    /* SCL输出高电平 */
#define Sim_I2C1_SDA_LOW          (IIC_SDA_GPIO_PORT->ODR &= ~(IIC_SDA_GPIO_PIN))    /* SDA输出低电平 */
#define Sim_I2C1_SDA_HIG          (IIC_SDA_GPIO_PORT->ODR |=  (IIC_SDA_GPIO_PIN))    /* SDA输出高电平 */

#define Sim_I2C1_SDA_STATE        (IIC_SDA_GPIO_PORT->IDR &= (IIC_SDA_GPIO_PIN))     /* 读取SDA引脚状态 */

/* I2C时序延时定义 */
#define Sim_I2C1_DELAY            Sim_I2C1_Delay(100000)                             /* 长延时 */
#define Sim_I2C1_NOP              Sim_I2C1_Delay(400)                                /* 短延时 */

/* I2C通信状态定义 */
#define Sim_I2C1_READY            0x00        /* I2C就绪 */
#define Sim_I2C1_BUS_BUSY         0x01        /* I2C总线忙 */
#define Sim_I2C1_BUS_ERROR        0x02        /* I2C总线错误 */

/* I2C应答状态定义 */
#define Sim_I2C1_NACK             0x00        /* 无应答 */
#define Sim_I2C1_ACK              0x01        /* 有应答 */

/* I2C通信方向定义 */
#define I2C1_Direction_Transmitter 0x00        /* 发送方向 */
#define I2C1_Direction_Receiver    0x01        /* 接收方向 */

#define AS5600_ADDRESS_MAG        0x36        /* AS5600的I2C地址 */

/* AS5600寄存器地址枚举定义 */
typedef enum
{
    _ams5600_Address = 0x36,    /* AS5600的I2C地址 */
    _zmco = 0x00,               /* 烧录计数寄存器 */
    _zpos_hi = 0x01,            /* 零位置高字节 */
    _zpos_lo = 0x02,            /* 零位置低字节 */
    _mpos_hi = 0x03,            /* 最大位置高字节 */
    _mpos_lo = 0x04,            /* 最大位置低字节 */
    _mang_hi = 0x05,            /* 最大角度高字节 */
    _mang_lo = 0x06,            /* 最大角度低字节 */
    _conf_hi = 0x07,            /* 配置寄存器高字节 */
    _conf_lo = 0x08,            /* 配置寄存器低字节 */
    _raw_ang_hi = 0x0c,         /* 原始角度高字节 */
    _raw_ang_lo = 0x0d,         /* 原始角度低字节 */
    _ang_hi = 0x0e,             /* 角度高字节 */
    _ang_lo = 0x0f,             /* 角度低字节 */
    _stat = 0x0b,               /* 状态寄存器 */
    _agc = 0x1a,                /* 自动增益控制寄存器 */
    _mag_hi = 0x1b,             /* 磁场强度高字节 */
    _mag_lo = 0x1c,             /* 磁场强度低字节 */
    _burn = 0xff                /* 烧录命令寄存器 */
}AMS5600Registers_t;
extern float degress;
/* 函数声明 */
void iic_init(void);            /* I2C初始化函数 */
void PrintMenu(void);           /* 打印菜单函数 */
void Programe_Run(void);        /* 程序运行主函数 */

#endif  /* __AS5600_H */
		
