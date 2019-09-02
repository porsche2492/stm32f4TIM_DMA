#ifndef __RS232_H
#define __RS232_H
#include "stdint.h"

void RS232Init(void);
void RS232Send(uint8_t data);
void RS232SendString(char *str);

#endif /* __RS232_H */
