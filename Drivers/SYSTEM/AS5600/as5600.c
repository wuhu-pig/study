/**
 * @file as5600.c
 * @brief AS5600磁编码器驱动实现
 * 
 * 本文件实现了AS5600 12位可编程磁旋转位置传感器的驱动程序。
 * AS5600通过I2C接口通信，提供高分辨率的绝对位置测量。
 * 主要功能：
 * - 原始和缩放角度读取
 * - 起始和结束位置编程
 * - 最大角度编程
 * - 磁体检测和强度测量
 * - 永久配置烧录命令
 * 
 * @note 传感器的永久编程（烧录）只能执行有限次数
 */

#include "as5600.h"  // 包含AS5600驱动的头文件
//extern  UART_HandleTypeDef huart1;

/* 标准宽度整型的类型定义 */
typedef unsigned char uint8_t;      // 定义8位无符号整型
typedef unsigned short int uint16_t; // 定义16位无符号整型
typedef unsigned int uint32_t;      // 定义32位无符号整型

/* 角度配置的全局变量 */
uint16_t _rawStartAngle = 0;  // 存储原始起始角度位置
uint16_t _zPosition = 0;      // 零位置存储
uint16_t _rawEndAngle = 0;    // 存储原始结束角度位置
uint16_t _mPosition = 0;      // 最大位置存储
uint16_t _maxAngle = 0;       // 最大角度范围存储
/*******************************************************************************/
/**
  * @brief 模拟I2C延时函数
  * @param delay: 延时循环次数
  * @note 在SYSCLK=72MHz时：delay=100约为8.75us，delay=1000约为85.58us
  */
void Sim_I2C1_Delay(uint32_t delay)
{
	while(--delay);	//dly=100: 8.75us; dly=100: 85.58 us (SYSCLK=72MHz)
}

/**
  * @brief 生成I2C起始条件
  * @return Sim_I2C1_READY表示成功，否则返回错误代码
  * @note 在SCL为高电平时将SDA拉低，然后将SCL拉低
  */
uint8_t Sim_I2C1_START(void)
{
	SDA1_OUT();
	Sim_I2C1_SDA_HIG;
	Sim_I2C1_SCL_HIG;
	Sim_I2C1_NOP;

// if(!Sim_I2C1_SDA_STATE) return Sim_I2C1_BUS_BUSY;

	Sim_I2C1_SDA_LOW;
	Sim_I2C1_NOP;

	Sim_I2C1_SCL_LOW;
	Sim_I2C1_NOP;

	//if(Sim_I2C1_SDA_STATE) return Sim_I2C1_BUS_ERROR;

	return Sim_I2C1_READY;
}

/**
  * @brief 生成I2C停止条件
  * @note 在SCL为高电平时将SDA从低电平拉高
  */
void Sim_I2C1_STOP(void)
{
	SDA1_OUT();
	Sim_I2C1_SCL_LOW;
	Sim_I2C1_SDA_LOW;
	Sim_I2C1_NOP;

//	Sim_I2C1_SCL_LOW;
//  Sim_I2C1_NOP;

	Sim_I2C1_SCL_HIG;
	Sim_I2C1_SDA_HIG;
	Sim_I2C1_NOP;
}

/**
 * @brief 等待从机应答信号
 * @return 0表示收到应答，1表示无应答
 */
unsigned char Sim_I2C1_Wait_Ack(void)
{
	volatile unsigned char ucErrTime=0;
	SDA1_IN();
	Sim_I2C1_SDA_HIG;
	Sim_I2C1_NOP;;
	Sim_I2C1_SCL_HIG;
	Sim_I2C1_NOP;;
	while(Sim_I2C1_SDA_STATE)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			Sim_I2C1_STOP();
			return 1;
		}
	}
	Sim_I2C1_SCL_LOW;
	return Sim_I2C1_READY;
}

/**
  * @brief 产生ACK应答信号
  */
void Sim_I2C1_SendACK(void)
{
	Sim_I2C1_SCL_LOW;
	SDA1_OUT();
	Sim_I2C1_SDA_LOW;
	Sim_I2C1_NOP;
	Sim_I2C1_SCL_HIG;
	Sim_I2C1_NOP;
	Sim_I2C1_SCL_LOW;
	Sim_I2C1_NOP;
}

/**
  * @brief 产生NACK非应答信号
  */
void Sim_I2C1_SendNACK(void)
{
	Sim_I2C1_SCL_LOW;
	SDA1_OUT();
	Sim_I2C1_SDA_HIG;
	Sim_I2C1_NOP;
	Sim_I2C1_SCL_HIG;
	Sim_I2C1_NOP;
	Sim_I2C1_SCL_LOW;
	Sim_I2C1_NOP;
}

/**
  * @brief 通过I2C发送一个字节
  * @param Sim_i2c_data: 要发送的数据
  * @return Sim_I2C1_READY表示发送成功
  */
uint8_t Sim_I2C1_SendByte(uint8_t Sim_i2c_data)
{
	uint8_t i;
	SDA1_OUT();
	Sim_I2C1_SCL_LOW;
	for(i=0; i<8; i++)
	{
		if(Sim_i2c_data&0x80) Sim_I2C1_SDA_HIG;
		else Sim_I2C1_SDA_LOW;

		Sim_i2c_data<<=1;
		Sim_I2C1_NOP;

		Sim_I2C1_SCL_HIG;
		Sim_I2C1_NOP;
		Sim_I2C1_SCL_LOW;
		Sim_I2C1_NOP;
	}
	return Sim_I2C1_READY;
}

/**
 * @brief 从I2C接收一个字节
 * @return 接收到的数据
 */
uint8_t Sim_I2C1_ReceiveByte(void)
{
    uint8_t i, Sim_i2c_data;
    SDA1_IN();               // 配置SDA为输入模式
    Sim_i2c_data = 0;       // 清零接收数据缓存

    // 按位接收数据
    for(i = 0; i < 8; i++)
    {
        Sim_I2C1_SCL_LOW;    // SCL拉低
        Sim_I2C1_NOP;        // 短暂延时
        Sim_I2C1_SCL_HIG;    // SCL拉高
        Sim_i2c_data <<= 1;  // 数据左移一位

        if(Sim_I2C1_SDA_STATE) Sim_i2c_data |= 0x01;  // 读取数据位

        Sim_I2C1_NOP;        // 短暂延时
    }
    Sim_I2C1_SendNACK();     // 发送非应答信号
    return Sim_i2c_data;     // 返回接收到的数据
}

/**
  * @brief 通过I2C接收一个字节
  * @param
  * @note
  * @retval void
  * @author ????????
  */
uint8_t Sim_I2C1_ReceiveByte_WithACK(void)
{

	uint8_t i,Sim_i2c_data;
	SDA1_IN();
	//Sim_I2C1_SDA_HIG;
// Sim_I2C1_SCL_LOW;
	Sim_i2c_data=0;

	for(i=0; i<8; i++)
	{
		Sim_I2C1_SCL_LOW;
		Sim_I2C1_NOP;
		Sim_I2C1_SCL_HIG;
		// Sim_I2C1_NOP;
		Sim_i2c_data<<=1;

		if(Sim_I2C1_SDA_STATE)	Sim_i2c_data|=0x01;

		// Sim_I2C1_SCL_LOW;
		Sim_I2C1_NOP;
	}
	Sim_I2C1_SendACK();
	return Sim_i2c_data;
}


/**
  * @brief 通过I2C读取多个字节
  * @param
  * @note
  * @retval void
  * @author ????????
  */
uint8_t Sim_I2C_Read8(uint8_t moni_dev_addr, uint8_t moni_reg_addr, uint8_t moni_i2c_len, uint8_t *moni_i2c_data_buf)
{

	Sim_I2C1_START();
	Sim_I2C1_SendByte(moni_dev_addr << 1 | I2C1_Direction_Transmitter);
	Sim_I2C1_Wait_Ack();
	Sim_I2C1_SendByte(moni_reg_addr);
	Sim_I2C1_Wait_Ack();
	//Sim_I2C1_STOP();
	
	Sim_I2C1_START();
	Sim_I2C1_SendByte(moni_dev_addr << 1 | I2C1_Direction_Receiver);
	Sim_I2C1_Wait_Ack();
	while (moni_i2c_len)
	{
		if (moni_i2c_len==1) *moni_i2c_data_buf =Sim_I2C1_ReceiveByte();
		else *moni_i2c_data_buf =Sim_I2C1_ReceiveByte_WithACK();
		moni_i2c_data_buf++;
		moni_i2c_len--;
	}
	Sim_I2C1_STOP();
	return 0x00;
}

/*******************************************************************************/
/**
  * @brief 通过I2C写入多个字节
  * @param
  * @note   ??????check?????????????????????????????????????check????
  * @retval void
  * @author ????????
  */
int8_t Sim_I2C1_Write8(uint8_t moni_dev_addr, uint8_t moni_reg_addr, uint8_t moni_i2c_len, uint8_t *moni_i2c_data_buf)
{
	uint8_t i;
	Sim_I2C1_START();
	Sim_I2C1_SendByte(moni_dev_addr << 1 | I2C1_Direction_Transmitter);
	Sim_I2C1_Wait_Ack();
	Sim_I2C1_SendByte(moni_reg_addr);
	Sim_I2C1_Wait_Ack();
	
	//Sim_I2C1_START();
	for (i=0; i<moni_i2c_len; i++)
	{
		Sim_I2C1_SendByte(moni_i2c_data_buf[i]);
		Sim_I2C1_Wait_Ack();
	}
	Sim_I2C1_STOP();	
		return 0;
}


/************************************************************************************************************/
/************************************************************************************************************/
/*******************************************************************************/


/**
 * @brief 从16位值中提取高字节
 * @param value: 16位输入值
 * @return 高字节值（位15-8）
 */
uint8_t highByte(uint16_t value)
{
    uint8_t ret;
    value = value >> 8;     // 右移8位
    ret = (uint8_t)value;   // 转换为8位
    return ret;
}

/**
 * @brief 从16位值中提取低字节
 * @param value: 16位输入值
 * @return 低字节值（位7-0）
 */
uint8_t lowByte(uint16_t value)
{
    uint8_t ret;
    value = value & 0x00ff;  // 与操作保留低8位
    ret = (uint8_t)value;    // 转换为8位
    return ret;
}

/**
 * @brief 从AS5600寄存器读取一个字节
 * @param in_adr: 要读取的寄存器地址
 * @return 读取到的寄存器值
 */
uint8_t readOneByte(uint8_t in_adr)
{
    uint8_t retVal = -1;
    
    // 通过I2C读取数据
    Sim_I2C_Read8(_ams5600_Address, in_adr, 1, &retVal);
    Sim_I2C1_NOP;
    return retVal;
}

/**
 * @brief 从AS5600连续读取两个字节
 * @param in_adr_hi: 高字节寄存器地址
 * @param in_adr_lo: 低字节寄存器地址
 * @return 组合后的16位值
 */
uint16_t readTwoBytes(uint8_t in_adr_hi, uint8_t in_adr_lo)
{
    uint16_t retVal = -1;
    uint8_t low = 0, high = 0;
    
    /* 读取低字节 */
    low = readOneByte(in_adr_lo);
    
    /* 读取高字节 */  
    high = readOneByte(in_adr_hi);
    
    // 组合高低字节
    retVal = high << 8;
    retVal = retVal | low;
    return retVal;
}

/**
 * @brief 向AS5600写入一个字节
 * @param adr_in: 目标寄存器地址
 * @param dat_in: 要写入的数据
 */
void writeOneByte(uint8_t adr_in, uint8_t dat_in)
{
    uint8_t dat = dat_in;
    Sim_I2C1_Write8(_ams5600_Address, adr_in, 1, &dat);
}

/**
 * @brief 获取AS5600的I2C地址
 * @return AS5600的I2C地址
 */
int16_t getAddress()
{
    return _ams5600_Address; 
}

/**
 * @brief 获取最大角度值
 * @return 最大角度寄存器的值
 */
int16_t getMaxAngle(void)
{
    return readTwoBytes(_mang_hi, _mang_lo);
}

/**
 * @brief 获取原始角度值
 * @return 磁体位置的原始值
 * @note 不受起始、结束和最大角度设置的影响
 */
int16_t getRawAngle(void)
{
    return readTwoBytes(_raw_ang_hi, _raw_ang_lo);
}

/**
 * @brief 获取起始位置
 * @return 起始位置寄存器的值
 */
int16_t getStartPosition(void)
{
    return readTwoBytes(_zpos_hi, _zpos_lo);
}

/**
 * @brief 获取结束位置
 * @return 结束位置寄存器的值
 */
int16_t getEndPosition(void)
{
    int16_t retVal = readTwoBytes(_mpos_hi, _mpos_lo);
    return retVal;
}

/**
 * @brief 获取缩放角度值
 * @return 缩放角度寄存器的值
 */
int16_t getScaledAngle(void)
{
    return readTwoBytes(_ang_hi, _ang_lo);
}

/**
 * @brief 获取AGC值
 * @return AGC寄存器的值
 */
int16_t getAgc()
{
    return readOneByte(_agc);
}

/**
 * @brief 获取幅度值
 * @return 幅度寄存器的值
 */
int16_t getMagnitude()
{
    return readTwoBytes(_mag_hi, _mag_lo);  
}

/**
 * @brief 获取烧录次数
 * @return 烧录寄存器的值
 */
int16_t getBurnCount()
{
    return readOneByte(_zmco);
}

/**
 * @brief 获取原始角度值
 * @return 磁体位置的原始值
 * @note 不受起始、结束和最大角度设置的影响
 */
int16_t AgetRawAngle(void)
{
    return readTwoBytes(_raw_ang_hi, _raw_ang_lo);
}

/**
 * @brief 设置结束位置
 * @param endAngle: 新的结束角度位置
 * @return 设置后的结束位置
 * @note 如果未提供值，方法将读取磁体的位置
 */
int16_t setEndPosition(int16_t endAngle)
{
    if(endAngle == -1)
        _rawEndAngle = getRawAngle();
    else
        _rawEndAngle = endAngle;
    
    writeOneByte(_mpos_hi, highByte(_rawEndAngle));
    delay_ms(2); 
    writeOneByte(_mpos_lo, lowByte(_rawEndAngle)); 
    delay_ms(2);               
    _mPosition = readTwoBytes(_mpos_hi, _mpos_lo);
    
    return(_mPosition);
}

/**
 * @brief 设置起始位置
 * @param startAngle: 新的起始角度位置
 * @return 设置后的起始位置
 * @note 如果未提供值，方法将读取磁体的位置
 */
int16_t setStartPosition(int16_t startAngle)
{
    if(startAngle == -1)
    {
        _rawStartAngle = getRawAngle();
    }
    else
        _rawStartAngle = startAngle;

    writeOneByte(_zpos_hi, highByte(_rawStartAngle));
    delay_ms(2); 
    writeOneByte(_zpos_lo, lowByte(_rawStartAngle)); 
    delay_ms(2);                
    _zPosition = readTwoBytes(_zpos_hi, _zpos_lo);
    
    return(_zPosition);
}

/**
 * @brief 设置最大角度
 * @param newMaxAngle: 新的最大角度设置或无
 * @return 最大角度寄存器的值
 * @note 如果未提供值，方法将读取磁体的位置。设置此寄存器会将最大位置寄存器清零。
 */
int16_t setMaxAngle(int16_t newMaxAngle)
{
    int32_t retVal;
    if(newMaxAngle == -1)
    {
        _maxAngle = getRawAngle();
    }
    else
        _maxAngle = newMaxAngle;

    writeOneByte(_mang_hi, highByte(_maxAngle));
    delay_ms(2);
    writeOneByte(_mang_lo, lowByte(_maxAngle)); 
    delay_ms(2);     

    retVal = readTwoBytes(_mang_hi, _mang_lo);
    return retVal;
}

/**
 * @brief 检测磁体存在
 * @return 1如果检测到磁体，0如果没有检测到
 * @note 读取状态寄存器并检查MH（磁体高）位
 */
uint8_t detectMagnet(void)
{
    uint8_t magStatus;
    uint8_t retVal = 0;
    /*0 0 MD ML MH 0 0 0*/
    /* MD high = AGC minimum overflow, Magnet to strong */
    /* ML high = AGC Maximum overflow, magnet to weak*/ 
    /* MH high = magnet detected*/ 
    magStatus = readOneByte(_stat);
    
    if(magStatus & 0x20)
        retVal = 1; 
    
    return retVal;
}

/**
 * @brief 获取磁体强度状态
 * @return 0: No magnet detected
 *         1: Magnet too weak
 *         2: Magnet strength optimal
 *         3: Magnet too strong
 */
uint8_t getMagnetStrength(void)
{
    uint8_t magStatus;
    uint8_t retVal = 0;
    /*0 0 MD ML MH 0 0 0*/
    /* MD high = AGC minimum overflow, Magnet to strong */
    /* ML high = AGC Maximum overflow, magnet to weak*/ 
    /* MH high = magnet detected*/ 
    magStatus = readOneByte(_stat);
    if(detectMagnet() ==1)
    {
        retVal = 2; /*just right */
        if(magStatus & 0x10)
            retVal = 1; /*to weak */
        else if(magStatus & 0x08)
            retVal = 3; /*to strong */
    }
    
    return retVal;
}

/**
 * @brief 将当前角度配置烧录到永久存储器
 * @return 1: Success
 *         -1: No magnet detected
 *         -2: Burn limit exceeded (can only be done 3 times)
 *         -3: Start and end positions not set
 * @warning This operation is permanent and can only be performed 3 times
 */
uint8_t burnAngle()
{
    uint8_t retVal = 1;
    _zPosition = getStartPosition();
    _mPosition = getEndPosition();
    _maxAngle  = getMaxAngle();
    
    if(detectMagnet() == 1)
    {
        if(getBurnCount() < 3)
        {
            if((_zPosition == 0)&&(_mPosition ==0))
                retVal = -3;
            else
                writeOneByte(_burn, 0x80);
        }
        else
            retVal = -2;
    } 
    else
        retVal = -1;
    
    return retVal;
}

/**
 * @brief 将最大角度和配置数据烧录到芯片
 * @return 1 success
 *         -1 burn limit exceeded
 *         -2 max angle is to small, must be at or above 18 degrees
 * @note This can only be done 1 time
 */
uint8_t burnMaxAngleAndConfig()
{
    uint8_t retVal = 1;
    _maxAngle  = getMaxAngle();
    
    if(getBurnCount() ==0)
    {
        if(_maxAngle*0.087 < 18)
            retVal = -2;
        else
            writeOneByte(_burn, 0x40);    
    }  
    else
        retVal = -1;
    
    return retVal;
}

/**
 * @brief 初始化IIC
 * @param
 * @retval
 */
void iic_init(void)
{
    IIC_SCL_GPIO_CLK_ENABLE();  /* SCL时钟使能 */
    IIC_SDA_GPIO_CLK_ENABLE();  /* SDA时钟使能 */

    /* SCL模式设置,输出,上拉 */
    sys_gpio_set(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    /* SDA模式设置,开漏,上拉, 内部IO口, 上拉延时(=1), 也可以读外部信号的高低 */
    sys_gpio_set(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_OD, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    Sim_I2C1_STOP();     /* 停止所有设备 */
}

/**
 * @brief 从控制台接收输入数据
 * @return 接收到的数据
 */
uint8_t PrintInFromConsole(void)
{
    uint8_t data;
    //HAL_UART_Receive(&huart1, &data, 1, 0xFFFF);
    return data;
}

int8_t MenuChoice = 0;  // 菜单选择变量

/**
 * @brief 打印AS5600操作菜单
 * @note 显示所有可用的操作选项和剩余烧录次数
 */
void PrintMenu(void)
{
    printf("AS5600 Serial Interface Programe----SC-electronic\r\n");
    printf("1 - Set start position\t|  ");    printf(" 6 - get MPOS\r\n");
    printf("2 - Set end position\t|  ");      printf(" 7 - get raw angle\r\n");
    printf("3 - Set max angle range\t|  ");   printf(" 8 - get scaled angle\r\n");
    printf("4 - Get max angle range\t|  ");   printf(" 9 - detect magnet\r\n");
    printf("5 - Get ZPOS \t\t|  ");          printf("10 - get magnet strength\r\n");
    printf("\r\n");
    printf("Number of burns remaining: ");     printf("%d\r\n",(3 - getBurnCount()));
    printf("96 - Burn Angle\r\n");
    printf("98 - Burn Settings (one time)\r\n");
    MenuChoice = PrintInFromConsole();
    printf("You choice NO.%d programe\r\n",MenuChoice);
}

/**
 * @brief 将原始角度值转换为度数
 * @param newAngle: 来自AS5600的原始角度数据
 * @return 转换后的角度值（浮点数）
 * @note 原始数据范围0-4095，每单位代表0.087度
 */
float convertRawAngleToDegrees(int16_t newAngle)
{
    float retVal = newAngle * 0.087;  // 转换系数0.087度/单位
    return retVal;
}

// 全局变量定义
uint16_t rawdata = 0;   // 存储原始角度数据
float degress = 0;      // 存储转换后的角度值

/**
 * @brief 程序主运行函数
 * @note 执行基本的传感器读取操作：
 *       1. 检测磁体是否存在
 *       2. 读取原始角度值
 *       3. 将原始角度转换为度数
 */
void Programe_Run(void)
{
//    uint8_t dect = 0;
//    dect = detectMagnet();                              // 检测磁体
//    printf("detectMagnet is %d\r\n", dect);            // 打印检测结果
//    
    rawdata = getRawAngle();                           // 获取原始角度值
//    printf("rawdata is %d\r\n", rawdata);              // 打印原始数据
//    
    degress = convertRawAngleToDegrees(rawdata);       // 转换为角度值
//    printf("degress is %f\r\n", degress);              // 打印角度值
}
