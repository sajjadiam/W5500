#ifndef FACTORY_PROTO_H
#define FACTORY_PROTO_H
#include <stdint.h>
void Proto_PackMessage(uint8_t cmd, uint8_t* payload, uint16_t len);
void Proto_UnpackMessage(uint8_t* buffer);
#endif //FACTORY_PROTO_H