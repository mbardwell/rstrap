#ifndef TENSION_H
#define TENSION_H

#include "stdint.h"
#include "ble_nus.h"


typedef uint16_t conn_handle_t; // 
void TensionCommunicationInit(ble_nus_t *nus, conn_handle_t *handle);
void StartTensionSample();


#endif