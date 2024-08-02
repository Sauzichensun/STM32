#include "stm32f10x.h"                  // Device header
#include "DataType.h"


void Servo_Param_Init(Servo *servo,uint8_t channel,GPIO_TypeDef* Servo_GPIOx,uint16_t Servo_OutPin,
	TIM_TypeDef* TIM_X)
{
	servo->channel=channel;
	servo->Servo_GPIOx=Servo_GPIOx;
	servo->Servo_OutPin=Servo_OutPin;
	servo->TIM_X=TIM_X;
}


void Servo_Init(Servo *servo)
{
	if(servo->Servo_GPIOx==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(servo->Servo_GPIOx==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(servo->Servo_GPIOx==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = servo->Servo_OutPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(servo->Servo_GPIOx, &GPIO_InitStructure);		
	
	if(servo->TIM_X==TIM1) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	else if(servo->TIM_X==TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(servo->TIM_X==TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(servo->TIM_X==TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);		

	TIM_InternalClockConfig(servo->TIM_X);
		
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;                 //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(servo->TIM_X, &TIM_TimeBaseInitStructure); 
	
	TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);                         //结构体初始化，若结构体没有完整赋值
	                                                                //则最好执行此函数，给结构体所有成员都赋一个默认值
	                                                                //避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	if (servo->channel==1) TIM_OC1Init(servo->TIM_X, &TIM_OCInitStructure);
	else if(servo->channel==2) TIM_OC2Init(servo->TIM_X, &TIM_OCInitStructure);
	else if(servo->channel==3) TIM_OC3Init(servo->TIM_X, &TIM_OCInitStructure);
	else if(servo->channel==4) TIM_OC4Init(servo->TIM_X, &TIM_OCInitStructure);
	
	/*TIM使能*/
	TIM_Cmd(servo->TIM_X, ENABLE);			//使能TIM2，定时器开始运行
}

void PWM_SetCompare_Servo(Servo *servo,uint16_t compare)
{
	if(servo->channel==1) TIM_SetCompare1(servo->TIM_X,compare);
	else if(servo->channel==2) TIM_SetCompare2(servo->TIM_X,compare);
	else if(servo->channel==3) TIM_SetCompare3(servo->TIM_X,compare);
	else if(servo->channel==4) TIM_SetCompare4(servo->TIM_X,compare);
	
}

void Servo_SetAngle(Servo *servo,float Angle)
{
	PWM_SetCompare_Servo(servo,Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}

