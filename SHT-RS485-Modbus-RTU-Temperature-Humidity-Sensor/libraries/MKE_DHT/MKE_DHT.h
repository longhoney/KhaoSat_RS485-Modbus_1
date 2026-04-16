#ifndef MKE_DHT_H_
#define MKE_DHT_H_
#include "MKL_DHT.h"

class MKE_DHT: public MKL_DHT
{
  public:
  MKE_DHT(uint8_t pin, uint8_t type, uint8_t count = 6):MKL_DHT(pin, type, count) {}
};

#endif