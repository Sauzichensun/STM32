#include "stm32f10x.h"                  // Device header
#include "Delay.h"


#define WavePort GPIOA
#define Trig GPIO_Pin_6
#define Echo GPIO_Pin_7


uint16_t num=0;
uint8_t superwaveflag=0;/*回响开始与结束的标志位*/
uint32_t distancewave=0;/*超声波测的距离*/
uint32_t superwavetimes=0;/*回响时间存储*/
uint16_t offsetsuperwave=0;/*回响开始是计数器的偏置量*/
uint16_t superwavecnt=1;/*采样计数*/

/*超声波模块，仅在触发信号时进行了阻塞延时
采样频率：每秒采样一次数据，
采样间隔由定时器定时*/
/*使用时仅需调用初始化函数*/
void SuperWave_Init(void)
{
	if(WavePort==GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(WavePort==GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(WavePort==GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef itd;
	itd.GPIO_Mode=GPIO_Mode_Out_PP;                       
	itd.GPIO_Pin=Trig;                                    
	itd.GPIO_Speed=GPIO_Speed_50MHz; 
	//默认选择50MHz
	if(WavePort==GPIOA) GPIO_Init(GPIOA,&itd);
	else if(WavePort==GPIOB) GPIO_Init(GPIOB,&itd);
	else if(WavePort==GPIOC) GPIO_Init(GPIOC,&itd);
	itd.GPIO_Mode=GPIO_Mode_IN_FLOATING;                        //选择浮空输入模式
	itd.GPIO_Pin=Echo;      

	if(WavePort==GPIOA) GPIO_Init(GPIOA,&itd);
	else if(WavePort==GPIOB) GPIO_Init(GPIOB,&itd);
	else if(WavePort==GPIOC) GPIO_Init(GPIOC,&itd);
	
	//AFIO映射中断引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);         //使能AFIO的外设时针
	if(WavePort==GPIOA) GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);  //选择外部中断源和中断通道
	else if(WavePort==GPIOB) GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	else if(WavePort==GPIOC) GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);
	//EXTI中断配置
	EXTI_InitTypeDef itd1;
	itd1.EXTI_Line=EXTI_Line7;                                  //echo使用的端口7,因此选择7号中断线
	itd1.EXTI_LineCmd=ENABLE;
	itd1.EXTI_Mode=EXTI_Mode_Interrupt;
	itd1.EXTI_Trigger=EXTI_Trigger_Rising_Falling;              //上升沿和下降沿都触发中断
	EXTI_Init(&itd1);
	
	//NVIC分配外部中断的中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);            
	NVIC_InitTypeDef itd2;
	itd2.NVIC_IRQChannel=EXTI9_5_IRQn;                         
	itd2.NVIC_IRQChannelCmd=ENABLE;
	itd2.NVIC_IRQChannelPreemptionPriority=1;                  
	itd2.NVIC_IRQChannelSubPriority=1;                         
	NVIC_Init(&itd2);
	
	//配置定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_InternalClockConfig(TIM2); 
	TIM_TimeBaseInitTypeDef itd3;
	itd3.TIM_ClockDivision=TIM_CKD_DIV1;                       
	itd3.TIM_CounterMode=TIM_CounterMode_Up;                   
	
	itd3.TIM_Period=1000-1;                                    
	itd3.TIM_Prescaler=72-1;                                   
	itd3.TIM_RepetitionCounter=0;                              
	TIM_TimeBaseInit(TIM2,&itd3);
	                   
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);					
																
															
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			

	NVIC_InitTypeDef itd4;
	itd4.NVIC_IRQChannel=TIM2_IRQn;                             
	itd4.NVIC_IRQChannelCmd=ENABLE;
	itd4.NVIC_IRQChannelPreemptionPriority=1;                   
	itd4.NVIC_IRQChannelSubPriority=1;                          
	NVIC_Init(&itd4);
	
	TIM_Cmd(TIM2,ENABLE);
}



void detect_distance_start(void)
{
	GPIO_SetBits(WavePort,Trig);
	Delay_us(15);
	GPIO_ResetBits(WavePort,Trig);
}


void TIM2_IRQHandler(void){
    if(SET==TIM_GetITStatus(TIM2,TIM_FLAG_Update))
		{
			num++;
			/*当收到Echo回响信号，并计算出频率后，superwavecnt=1，开始计时采样周期*/
			if(superwavecnt>0)
			{
				superwavecnt++;
			}
			/*定时1s进行采样*/
			if(superwavecnt>1000)
			{
				detect_distance_start();
				superwavecnt=0;
			}
      TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
    }
}

void EXTI9_5_IRQHandler(void)
{
	if(SET==EXTI_GetITStatus(EXTI_Line7)){
			if(superwaveflag==0)
			{
				num=0;
					//上升沿即回响电平开始,打开计数器
				superwaveflag=1;
				/*这里获取当前计数器，并设立一个偏置值，这样避免了对其他定时功能产生的影响，可以根据需求来改动*/
				offsetsuperwave=TIM_GetCounter(TIM2);
			}
			else
			{		
				/*乘的数值与周期有关*/
				superwavetimes=num*1000+TIM_GetCounter(TIM2)-offsetsuperwave;  //得到回响的高电平持续的us
				/*回响结束，恢复原始的标志位*/
				superwaveflag=0;
				superwavecnt=1;
				/*计算距离*/
				distancewave=superwavetimes*0.34/2;
			}
			EXTI_ClearITPendingBit(EXTI_Line7);
	}
}
