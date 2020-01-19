/*
 * crc8.h
 *
 * Created: 24.11.2015 22:05:26
 *  Author: atmega
 */ 


#ifndef CRC8_H_
#define CRC8_H_
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

uint8_t crc8(uint8_t *ptr,uint8_t cnt);

#endif /* CRC8_H_ */