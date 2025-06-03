//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//  功能描述   : 1.8寸LCD 4接口演示例程(STM32系列)
/******************************************************************************
//本程序适用与STM32F103C8
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PD0（SCL）
//              SDA   接PD1（SDA）
//              RES   接PD2
//              DC    接PD3
//              CS    接PD4 
//							BL		接PD5
*******************************************************************************/

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111
#define u8 uint8_t
#define u16 uint16_t
/* 引脚 定义 */
#define LCD_GPIO_CLK_ENABLE()          do{ RCC->AHB1ENR |= 1 << 3; }while(0)   /* PD口时钟使能 */

#define LCD_GPIO_PORT                  		GPIOD
#define LCD_GPIO_SCL_PIN                  SYS_GPIO_PIN0
#define LCD_GPIO_SDA_PIN                  SYS_GPIO_PIN1
#define LCD_GPIO_RES_PIN                  SYS_GPIO_PIN2
#define LCD_GPIO_DC_PIN                  	SYS_GPIO_PIN3
#define LCD_GPIO_CS_PIN                  	SYS_GPIO_PIN4
#define LCD_GPIO_BLK_PIN                  SYS_GPIO_PIN5
#define LCD_ALL_PIN 									SYS_GPIO_PIN0|SYS_GPIO_PIN1|SYS_GPIO_PIN2|SYS_GPIO_PIN3|SYS_GPIO_PIN4|SYS_GPIO_PIN5


#define LCD_SCL        	LCD_GPIO_SCL_PIN	//TFT --SCL/SCK
#define LCD_SDA        	LCD_GPIO_SDA_PIN	//TFT --SDA/DIN
#define LCD_CS        	LCD_GPIO_CS_PIN  //TFT --CS/CE

#define LCD_LED        	LCD_GPIO_BLK_PIN  //TFT --BLK_PIN
#define LCD_RS         	LCD_GPIO_DC_PIN	//TFT --DC
#define LCD_RST     		LCD_GPIO_RES_PIN	//TFT --RST

//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define	LCD_SCL_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_SCL_PIN, 1)   
#define	LCD_SDA_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_SDA_PIN, 1)   
#define	LCD_CS_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_CS_PIN, 1)   

    
#define	LCD_LED_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_BLK_PIN, 1)     
#define	LCD_RS_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_DC_PIN, 1)  
#define	LCD_RST_SET  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_RES_PIN, 1)  
//液晶控制口置0操作语句宏定义
#define	LCD_SCL_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_SCL_PIN, 0)     
#define	LCD_SDA_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_SDA_PIN, 0)    
#define	LCD_CS_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_CS_PIN, 0)    
    
#define	LCD_LED_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_BLK_PIN, 0)   
#define	LCD_RST_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_RES_PIN, 0)   
#define	LCD_RS_CLR  	sys_gpio_pin_set(LCD_GPIO_PORT, LCD_GPIO_DC_PIN, 0)   


void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
unsigned int Lcd_ReadPoint(u16 x,u16 y);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);

