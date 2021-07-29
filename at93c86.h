#pragma once

void AT93C86_HandleOutput(unsigned short value);
unsigned short AT93C86_HandleInput();
void AT93C86_Init(char* eeprom_path);