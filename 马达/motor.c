#include "stm32f10x.h"  // Device header
#include "DataType.h"

/*以L298N为例，两个In口控制转向，一个PWM输入口*/

/*in1,in2为GPIO_Pin*/
void Motor_Param_Init(Motor *motor,uint8_t channel,uint16_t in1,uint16_t in2,
	GPIO_TypeDef* In_GPIOx,GPIO_TypeDef* PWM_GPIOx,uint16_t PWM_Pin,
TIM_TypeDef* TIM_X)
{
	motor->channel=channel;
	motor->In1=in1;motor->In2=in2;
	motor->In_GPIOx=In_GPIOx;
	motor->PWM_GPIOx=PWM_GPIOx;
	motor->PWM_Pin=PWM_Pin;
	motor->TIM_X=TIM_X;
}

void Motor_Init(Motor *motor)
{
	/*In1，In2-控制转向*/
	if(motor->In_GPIOx==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(motor->In_GPIOx==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(motor->In_GPIOx==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GpioInitStructure;
	GpioInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GpioInitStructure.GPIO_Pin = motor->In1;
	GpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(motor->In_GPIOx,&GpioInitStructure);
	
	GpioInitStructure.GPIO_Pin = motor->In2;
	GPIO_Init(motor->In_GPIOx,&GpioInitStructure);
	
	/*PWM输出-控制转速*/
	if(motor->PWM_GPIOx==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(motor->PWM_GPIOx==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(motor->PWM_GPIOx==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = motor->PWM_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(motor->PWM_GPIOx, &GPIO_InitStructure);		
	
	if(motor->TIM_X==TIM1) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	else if(motor->TIM_X==TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(motor->TIM_X==TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(motor->TIM_X==TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);		

	TIM_InternalClockConfig(motor->TIM_X);
		
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;                 //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(motor->TIM_X, &TIM_TimeBaseInitStructure); 
	
	TIM_OCInitTypeDef TIM_OCInitStructure;							
	TIM_OCStructInit(&TIM_OCInitStructure);                         
	                                                               
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	if (motor->channel==1) TIM_OC1Init(motor->TIM_X, &TIM_OCInitStructure);
	else if(motor->channel==2) TIM_OC2Init(motor->TIM_X, &TIM_OCInitStructure);
	else if(motor->channel==3) TIM_OC3Init(motor->TIM_X, &TIM_OCInitStructure);
	else if(motor->channel==4) TIM_OC4Init(motor->TIM_X, &TIM_OCInitStructure);
	                 //将结构体变量交给TIM_OC3Init，配置TIM2的输出比较通道3
	
	/*TIM使能*/
	TIM_Cmd(motor->TIM_X, ENABLE);			//使能TIM2，定时器开始运行
	
}


void PWM_SetCompare_Motor(Motor *motor,uint16_t compare)
{
	if(motor->channel==1) TIM_SetCompare1(motor->TIM_X,compare);
	else if(motor->channel==2) TIM_SetCompare2(motor->TIM_X,compare);
	else if(motor->channel==3) TIM_SetCompare3(motor->TIM_X,compare);
	else if(motor->channel==4) TIM_SetCompare4(motor->TIM_X,compare);
}

void Motor_Forward(Motor *motor,uint16_t compare)
{
	GPIO_SetBits(motor->In_GPIOx,motor->In1);
	GPIO_ResetBits(motor->In_GPIOx,motor->In2);
	PWM_SetCompare_Motor(motor,compare);
}

void Motor_BackForward(Motor *motor,uint16_t compare)
{
	GPIO_ResetBits(motor->In_GPIOx,motor->In1);
	GPIO_SetBits(motor->In_GPIOx,motor->In2);
	PWM_SetCompare_Motor(motor,compare);
}
