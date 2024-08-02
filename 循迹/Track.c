#include "stm32f10x.h"                  // Device header
#include "DataType.h"


void Track_Param_Init(Track *track,uint16_t track_pin,GPIO_TypeDef* track_x)
{
	track->GpioPin=track_pin;
	track->Track_GPIOx=track_x;
}

void Track_Monitor_Init(Track *track)
{
	if(track->Track_GPIOx==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(track->Track_GPIOx==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(track->Track_GPIOx==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GpioInitStructure;
	GpioInitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GpioInitStructure.GPIO_Pin = track->GpioPin;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(track->Track_GPIOx,&GpioInitStructure);
	
}

uint8_t Get_Track(Track *track)
{
	return GPIO_ReadInputDataBit(track->Track_GPIOx,track->GpioPin);
}

