#ifndef _MOTOR_H
#define _MOTOR_H

void Motor_Param_Init(Motor *motor,uint8_t channel,uint16_t in1,uint16_t in2,
	GPIO_TypeDef* In_GPIOx,GPIO_TypeDef* PWM_GPIOx,uint16_t PWM_Pin,
TIM_TypeDef* TIM_X);
void Motor_Init(Motor *motor);
void PWM_SetCompare(Motor *motor,uint16_t compare);
void Motor_Forward(Motor *motor,uint16_t compare);

#endif
