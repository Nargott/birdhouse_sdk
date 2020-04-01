#include "birdhouse_sdk.h"
#include <Wire.h>

Eeprom_at24c256::Eeprom_at24c256(int address) {	
	Wire.begin();
	_address = address;
}

void Eeprom_at24c256::Write(unsigned  int writeAddress, byte* data, int len) {
	if (!IsConnected()) return;

	Wire.beginTransmission(_address);
	Wire.write((int)(writeAddress >> 8));
	Wire.write((int)(writeAddress & 0xFF));
	byte c;
	for (c = 0; c < len; c++)
		Wire.write(data[c]);
	Wire.endTransmission();
}

void Eeprom_at24c256::Read(unsigned  int readAdress, byte *buffer, int len) {
	if (!IsConnected()) return;

	Wire.beginTransmission(_address);
	Wire.write((int)(readAdress >> 8));
	Wire.write((int)(readAdress & 0xFF)); 
	Wire.endTransmission();
	Wire.requestFrom(_address, len);
	int c = 0;
	for (c = 0; c < len; c++) {
		if (Wire.available()) buffer[c] = Wire.read();
  }
}

bool Eeprom_at24c256::IsConnected() {
	Wire.beginTransmission (_address);
	return (Wire.endTransmission() == 0); //if endTransmission ret code not equals to 0, then no device was connected
}