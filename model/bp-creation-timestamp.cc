/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-creation-timestamp.h"
#include "bp-sdnv.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"

#include <cmath>
#include <iostream>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

Time CreationTimestamp::m_previousTime = Time ();
uint64_t CreationTimestamp::m_previousSeq (0);

CreationTimestamp::CreationTimestamp ()
  : m_time (0),
    m_sequence (0)
{
  Time now = ns3::Simulator::Now ();
  m_time = (uint64_t) floor (now.GetSeconds ());
  m_sequence = now.GetNanoSeconds () - (1000000000 * floor (now.GetSeconds ()));

  if (now == m_previousTime && m_sequence <= m_previousSeq)
    {
      m_sequence = ++m_previousSeq;
    }
  else
    {
      m_previousTime = now;
      m_previousSeq = m_sequence;
    }
}

CreationTimestamp::CreationTimestamp (uint64_t time, uint64_t sequence)
  : m_time (time),
    m_sequence (sequence)
{}

CreationTimestamp::CreationTimestamp (const CreationTimestamp& timestamp)
  : m_time (timestamp.m_time),
    m_sequence (timestamp.m_sequence)
{}

CreationTimestamp::~CreationTimestamp ()
{}

Time
CreationTimestamp::GetTime () const
{
  return Seconds (m_time);
}

uint64_t
CreationTimestamp::GetSeconds () const
{
  return m_time;
}

uint64_t
CreationTimestamp::GetSequence () const
{
  return m_sequence;
}

uint64_t
CreationTimestamp::GetSerializedSize () const
{
  uint64_t i = 0;
  i += Sdnv::EncodingLength (m_time);
  i += Sdnv::EncodingLength (m_sequence);
  return i;
}

uint64_t
CreationTimestamp::Serialize (uint8_t *buffer) const
{
  uint64_t i = 0;
  Sdnv::Encode (m_time, buffer+i);
  i += Sdnv::EncodingLength (m_time);
  Sdnv::Encode (m_sequence, buffer+i);
  i += Sdnv::EncodingLength (m_sequence);
  return i;
}

CreationTimestamp
CreationTimestamp::Deserialize (uint8_t const*buffer)
{
  uint64_t i = 0;
  uint64_t time = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (time);
  uint64_t sequence = Sdnv::Decode (buffer+i);
  return CreationTimestamp (time,sequence);
}

uint64_t
CreationTimestamp::Serialize (Buffer::Iterator& start) const
{
  Sdnv::Encode (m_time, start);
  Sdnv::Encode (m_sequence, start);
  return GetSerializedSize ();
}

CreationTimestamp
CreationTimestamp::Deserialize (Buffer::Iterator& start)
{
  uint64_t time = Sdnv::Decode (start);
  uint64_t sequence = Sdnv::Decode (start);
  return CreationTimestamp (time,sequence);
}

bool
CreationTimestamp::operator == (const CreationTimestamp& other) const
{
  return (m_time == other.m_time) && (m_sequence == other.m_sequence);
}

bool
CreationTimestamp::operator != (const CreationTimestamp& other) const
{
  return !(*this == other);
}

bool
CreationTimestamp::operator < (const CreationTimestamp& other) const
{
  if (m_time == other.m_time)
    {
      return m_sequence < other.m_sequence;
    }
  else
    {
      return m_time < other.m_time;
    }
 
}

bool
CreationTimestamp::operator > (const CreationTimestamp& other) const
{
  return !(*this < other);
}

ostream&
operator<< (ostream& os, const CreationTimestamp& timestamp)
{
  os << "Creation Timestamp time = " << timestamp.GetTime ().GetSeconds ();
  os << " | " <<  "Creation Timestamp sequence number = " << timestamp.GetSequence ();
  return os;
}

}} // namespace bundleProtocol, ns3


#ifdef RUN_SELF_TESTS

#include "ns3/test.h"
#include <iostream>

using namespace std;
using namespace ns3::bundleProtocol;

namespace ns3 {

class CreationTimestampTest : public ns3::Test {
private:
public:
  CreationTimestampTest ();
  virtual bool RunTests (void);

};

  CreationTimestampTest::CreationTimestampTest ()
    : ns3::Test ("CreationTimestamp")
  {}

bool
CreationTimestampTest::RunTests (void)
{
  bool result = true;

  CreationTimestamp ct1 = CreationTimestamp ();
  CreationTimestamp ct2 = CreationTimestamp ();
  CreationTimestamp ct3 = CreationTimestamp (7000,234432);
  CreationTimestamp ct4 = CreationTimestamp (7000,234432);
  
  NS_TEST_ASSERT (ct1 != ct2);
  NS_TEST_ASSERT_EQUAL (ct3,ct4);

  NS_TEST_ASSERT_EQUAL (ct3.GetSeconds (), 7000);
  NS_TEST_ASSERT_EQUAL (ct3.GetSequence (), 234432);

  uint8_t *buffer = new uint8_t[ct3.GetSerializedSize ()];
  ct3.Serialize (buffer);
  
  CreationTimestamp ct5 = CreationTimestamp::Deserialize (buffer);
  NS_TEST_ASSERT_EQUAL (ct5,ct3);

  return result;
}

static CreationTimestampTest gCreationTimestampTest;

} // namespace ns3

#endif
