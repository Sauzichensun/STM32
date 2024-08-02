#ifndef _STEERE_H
#define _STEERE_H

void Servo_Param_Init(Servo *servo,uint8_t channel,GPIO_TypeDef* Servo_GPIOx,uint16_t Servo_OutPin,
	TIM_TypeDef* TIM_X);
void Servo_Init(Servo *servo);
void Servo_SetAngle(Servo *servo,float Angle);


#endif

