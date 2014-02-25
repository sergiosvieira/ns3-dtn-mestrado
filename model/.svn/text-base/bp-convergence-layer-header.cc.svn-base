/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-convergence-layer-header.h"

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (ConvergenceLayerHeader);

TypeId
ConvergenceLayerHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::ConvergenceLayerHeader")
    .SetParent<Header> ()
    .AddConstructor<ConvergenceLayerHeader> ()
    ;
  return tid;
}

TypeId
ConvergenceLayerHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

ConvergenceLayerHeader::ConvergenceLayerHeader ()
  : m_type (CLA_SEGMENT),
    m_nSegments (0),
    m_segmentNumber (0),
    m_sequenceNumber (0),
    m_startFlag (false),
    m_endFlag (false),
    m_response (ACK_FAILED)
{}

ConvergenceLayerHeader::ConvergenceLayerHeader (ClaHeaderType type)
  : m_type (type)
{}
    
ConvergenceLayerHeader::~ConvergenceLayerHeader() 
{}

void 
ConvergenceLayerHeader::SetType (ClaHeaderType type)
{
  m_type = type;
}

ClaHeaderType
ConvergenceLayerHeader::GetType () const
{
  return m_type;
}

void 
ConvergenceLayerHeader::SetStartFlag (bool val)
{
  m_startFlag = val;
}

bool
ConvergenceLayerHeader::GetStartFlag () const
{
  return m_startFlag;
}

void 
ConvergenceLayerHeader::SetEndFlag (bool val)
{
  m_endFlag = val;
}

bool
ConvergenceLayerHeader::GetEndFlag () const
{
  return m_endFlag;
}
  
void
ConvergenceLayerHeader::SetNumberOfSegments (uint16_t nSegments)
{
  m_nSegments = nSegments;
}

uint16_t
ConvergenceLayerHeader::GetNumberOfSegments () const
{
  return m_nSegments;
}

void
ConvergenceLayerHeader::SetSegmentNumber (uint16_t segmentNumber)
{
  m_segmentNumber = segmentNumber;
}

uint16_t
ConvergenceLayerHeader::GetSegmentNumber () const
{
  return m_segmentNumber;
}

void
ConvergenceLayerHeader::SetSequenceNumber (uint16_t sequenceNumber)
{
  m_sequenceNumber = sequenceNumber;
}

uint16_t
ConvergenceLayerHeader::GetSequenceNumber () const
{
  return m_sequenceNumber;
}

void
ConvergenceLayerHeader::SetResponse (AckResponse response)
{
  m_response = response;
}

AckResponse
ConvergenceLayerHeader::GetResponse () const
{
  return m_response;
}

string
ReasonToString (const AckResponse& reason)
{
  switch (reason)
    {
    case ACK_SUCCEEDED:
      return "Ack succeeded";
    case ACK_FAILED:
      return "Ack failed";
    case FINAL_DELIVERY_SUCCEEDED:
      return "Final delivery succeeded";
    case FINAL_DELIVERY_FAILED:
      return "Final delivery failed";
    default:
      return "Unkown response";
    }
}

void
ConvergenceLayerHeader::Print (std::ostream &os) const
{
  os << "Header type: " << (uint32_t) m_type << endl;
  if (m_type == CLA_SEGMENT)
    {
      os << "Segment " << m_segmentNumber << " of " << m_nSegments << endl;
      os << "Sequence number: " << m_sequenceNumber << endl;
    }
  else
    {
      os << "Ack value: " << ReasonToString (m_response) << endl;
    }
}



uint32_t
ConvergenceLayerHeader::GetSerializedSize (void) const
{
  uint32_t size = 1;
  if (m_type == CLA_SEGMENT)
    {
      size += 6;
    }
  else
    {
      size += 1;
    }
  return size;
}

void
ConvergenceLayerHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 ((uint8_t) m_type);

 if (m_type == CLA_SEGMENT)
    {
      i.WriteHtonU16 (m_nSegments);
      i.WriteHtonU16 (m_segmentNumber);
      i.WriteHtonU16 (m_sequenceNumber);
    }
  else
    {
      i.WriteU8 ((uint8_t) m_response);
    }
}
  
uint32_t
ConvergenceLayerHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_type = (ClaHeaderType) i.ReadU8 ();

  if (m_type == CLA_SEGMENT)
    {
      m_nSegments = i.ReadNtohU16 ();
      m_segmentNumber = i.ReadNtohU16 ();
      m_sequenceNumber = i.ReadNtohU16 ();
      
      if (m_segmentNumber == 1)
        SetStartFlag (true);
      
      if (m_segmentNumber == m_nSegments)
        SetEndFlag (true);
      
      m_response = ACK_FAILED;
    }
  else
    {
      m_nSegments = 0;
      m_segmentNumber = 0;
      m_sequenceNumber = 0;
      
      SetStartFlag (false);
      SetEndFlag (false);

      m_response = (AckResponse) i.ReadU8 ();
    }

  return GetSerializedSize ();
}

}} // namespace bundleProtocol, ns3
