#ifndef _TRACK_H
#define _TRACK_H

void Track_Param_Init(Track *track,uint16_t track_pin,GPIO_TypeDef* track_x);
void Track_Monitor_Init(Track *track);
uint8_t Get_Track(Track *track);


#endif
