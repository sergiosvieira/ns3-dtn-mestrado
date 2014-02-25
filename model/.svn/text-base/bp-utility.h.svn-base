/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_UTILITY_H
#define BP_UTILITY_H

#include <stdint.h>
#include "ns3/vector.h"

#include <iostream>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

static inline void 
SetBit(uint64_t& bits, uint8_t bitpos, bool value)
{
  if(value)
    {
      bits |= 1 << bitpos;
    }
  else
    {
      uint64_t test = 1 << bitpos;
      bits &= ~test;
    }
}

static inline bool 
GetBit(uint64_t bits, uint8_t bitpos)
{
  uint64_t flag = 1 << bitpos;
  return (bits & flag) == flag;
}

static inline void
SerializeDouble (double d, uint8_t *buffer)
{
  uint8_t *ptr = reinterpret_cast<uint8_t *> (&d);

  for(int i = 0; i < 8; ++i)
    buffer[i] = *(ptr+i);
}

static inline double
DeserializeDouble (uint8_t const*buffer)
{
  double deserialized;
  uint8_t *ptr = reinterpret_cast<uint8_t *> (&deserialized);
  
  for(int i = 0; i < 8; ++i)
    *(ptr+i) = buffer[i];

  return deserialized;
}

}} // namespace bundleProtocol, ns3

#endif /* BP_UTILITY_H */
