#include "stm32f10x.h"                  // Device header

void Angle_Sensor_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

	
	USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
   //USART 初始化设置
  USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
   

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART3, ENABLE);      
}


int16_t YAW=0,PITCH=0,ROLL=0;
uint8_t flag=0;
void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
	static uint8_t k=0,rebuf[8]={0};
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		{
		 rebuf[k++] =USART_ReceiveData(USART3);	//读取接收到的数据
		 if(!(rebuf[0]==0xaa))//如果帧头错误，清缓存
		 {
			k=0;
			rebuf[0]=0;
		 }
		 if(k==8)//数据接收完毕
		 {
			if(rebuf[7]==0x55)//判断帧尾，正确则解析姿态角
			{
			 YAW=(rebuf[1]<<8|rebuf[2]);//YAW，PITCH,ROLL为真实值的100倍
			 PITCH=(rebuf[3]<<8|rebuf[4]);
			 ROLL=(rebuf[5]<<8|rebuf[6]);
				flag=1;
			}
			k=0;//清缓存
		 }
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		
    } 
} 

