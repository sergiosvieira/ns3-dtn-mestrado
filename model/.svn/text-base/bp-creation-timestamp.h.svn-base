/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_CREATION_TIMESTAMP_H
#define BP_CREATION_TIMESTAMP_H

#include <stdint.h>
#include <iostream>
#include "ns3/buffer.h"
#include "ns3/nstime.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

/**
 * \ingroup bundle
 *
 * \brief Implements a timestamp holding creation time and a sequence number.
 *
 *
 */

class CreationTimestamp 
{
public:
  CreationTimestamp ();
  explicit  CreationTimestamp (uint64_t time, uint64_t sequence);
  // explicit  CreationTimestamp (Time time, uint64_t sequence);
  CreationTimestamp (const CreationTimestamp& timestamp);
  virtual ~CreationTimestamp ();
  Time GetTime () const;
  uint64_t GetSeconds () const;
  uint64_t GetSequence () const;

  uint64_t GetSerializedSize () const;

  uint64_t Serialize (uint8_t *buffer) const;
  static CreationTimestamp Deserialize (uint8_t const*buffer);

  uint64_t Serialize (Buffer::Iterator& start) const;
  static CreationTimestamp Deserialize (Buffer::Iterator& start);

  bool operator == (const CreationTimestamp& other) const;
  bool operator != (const CreationTimestamp& other) const;
  bool operator < (const CreationTimestamp& other) const;
  bool operator > (const CreationTimestamp& other) const;
private:
  uint64_t m_time;
  uint64_t m_sequence;
  static Time m_previousTime;
  static uint64_t m_previousSeq;
};

ostream& operator<< (ostream& os, const CreationTimestamp& timestamp);

}} // namespace bundleProtocol, ns3 

#endif /* BP_CREATION_TIMESTAMP_H */
