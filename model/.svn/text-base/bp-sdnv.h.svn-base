/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BUNDLE_PROTOCOL_SDNV_H
#define BUNDLE_PROTOCOL_SDNV_H

#include "ns3/buffer.h"

namespace ns3 {
namespace bundleProtocol {


/**
 * \ingroup utility
 *
 * \brief Self-Delimiting Numeric Values (SDNVs) are an encoding scheme used to encode a numeric values.
 *
 * An SDNV is
 * a numeric value encoded in N octets, the last of which has its most
 * significant bit (MSB) set to zero; the MSB of every other octet in
 * the SDNV must be set to 1.  The value encoded in an SDNV is the
 * unsigned binary number obtained by concatenating into a single bit
 * string the 7 least significant bits of each octet of the SDNV.
 *
 * 
 */
class Sdnv 
{
public:
  Sdnv ();
  virtual ~Sdnv ();
  
  // Encode the value to a SDNV and add it to the buffer
  static void Encode (uint64_t val, Buffer::Iterator& iter);
  static void Encode (uint64_t val, uint8_t *buffer);

  // Decode one SDNV from the buffer and return it.
  static uint64_t Decode (Buffer::Iterator& iter);
  static uint64_t Decode (uint8_t const*buffer);

  // Return the length in bytes needed to encode the value.
  static uint32_t EncodingLength (uint64_t val);

};

}} // namespace bundleProtocol, ns3 

#endif /* BUNDLE_PROTOCOL_H */
