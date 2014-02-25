/*
 * bp-neigh-header.cc
 *
 *  Created on: 13/04/2012
 *      Author: sergio
 */

#include "bp-neigh-header.h"

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (NeighHeader);

NeighHeader::NeighHeader () : m_eid (BundleEndpointId::GetAnyBundleEndpointId ())
{

}

NeighHeader::~NeighHeader ()
{}

void
NeighHeader::SetBundleEndpointId (BundleEndpointId eid)
{
  m_eid = eid;
}

BundleEndpointId
NeighHeader::GetBundleEndpointId () const
{
  return m_eid;
}

TypeId
NeighHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::NeighHeader")
    .SetParent<Header> ()
    .AddConstructor<NeighHeader> ()
    ;
  return tid;
}

TypeId
NeighHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
NeighHeader::Print (std::ostream &os) const
{
  os << "NeighHeader: eid = " << m_eid;
}

uint32_t
NeighHeader::GetSerializedSize (void) const
{
  return m_eid.GetSerializedSize () + 4;
}

void
NeighHeader::Serialize (Buffer::Iterator start) const
{
  int length = m_eid.GetSerializedSize ();
  Buffer::Iterator i = start;
  i.WriteHtonU32 (length);
  uint8_t buf [length];
  m_eid.Serialize (buf);
  i.Write (buf, length);
}

uint32_t
NeighHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint32_t length = i.ReadNtohU32 ();
  uint8_t buf [length];
  i.Read (buf,length);
  m_eid = BundleEndpointId::Deserialize (buf);
  return GetSerializedSize ();
}

}
}
