/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-global-bundle-identifier.h"
#include "bp-header.h"
#include "bp-bundle.h"
#include "bp-sdnv.h"

#include "ns3/simulator.h"

namespace ns3 {
namespace bundleProtocol {

GlobalBundleIdentifier::GlobalBundleIdentifier ()
  : m_eid (BundleEndpointId::GetAnyBundleEndpointId ()),
    m_creationTimestamp ()
{}

GlobalBundleIdentifier::GlobalBundleIdentifier (Ptr<Packet> bundle)
  : m_eid (BundleEndpointId::GetAnyBundleEndpointId ()),
    m_creationTimestamp ()
{
  PrimaryBundleHeader primaryHeader;
  bundle->PeekHeader (primaryHeader);
  m_eid = primaryHeader.GetSourceEndpoint ();
  m_creationTimestamp = primaryHeader.GetCreationTimestamp ();
}

GlobalBundleIdentifier::GlobalBundleIdentifier (Ptr<Bundle> bundle)
  : m_eid (bundle->GetBundleId ().m_eid),
    m_creationTimestamp (bundle->GetCreationTimestamp ())
{
}

GlobalBundleIdentifier::GlobalBundleIdentifier (const GlobalBundleIdentifier& gbid)
  : m_eid (gbid.m_eid), m_creationTimestamp (gbid.m_creationTimestamp)
{}

GlobalBundleIdentifier::GlobalBundleIdentifier (const BundleEndpointId& sourceEid, const CreationTimestamp& creationTimestamp)
  : m_eid (sourceEid), m_creationTimestamp (creationTimestamp)
{}

GlobalBundleIdentifier::~GlobalBundleIdentifier ()
{}

void
GlobalBundleIdentifier::SetSourceEndpoint (const BundleEndpointId& sourceEid)
{
  m_eid = sourceEid;
}

void
GlobalBundleIdentifier::SetCreationTimestamp (const CreationTimestamp& creationTimestamp)
{
  m_creationTimestamp = creationTimestamp;
}
  
BundleEndpointId
GlobalBundleIdentifier::GetSourceEid () const
{
  return m_eid;
}

CreationTimestamp
GlobalBundleIdentifier::GetCreationTimestamp () const
{
  return m_creationTimestamp;
}

uint32_t
GlobalBundleIdentifier::GetSerializedSize () const
{
  uint32_t size = 0;
  size += m_eid.GetSerializedSize ();
  size += m_creationTimestamp.GetSerializedSize ();
  return size;
}
  
void
GlobalBundleIdentifier::Serialize (uint8_t *buffer) const
{
  uint32_t i = 0;
  m_eid.Serialize (buffer);
  i += m_eid.GetSerializedSize ();
  m_creationTimestamp.Serialize(buffer+i);
}

GlobalBundleIdentifier
GlobalBundleIdentifier::Deserialize (uint8_t const*buffer)
{
  GlobalBundleIdentifier gbid;
  uint32_t i = 0;
  BundleEndpointId eid = BundleEndpointId::Deserialize (buffer);
  gbid.SetSourceEndpoint (eid);
  i += eid.GetSerializedSize ();
  gbid.SetCreationTimestamp (CreationTimestamp::Deserialize (buffer+i));
  
  return gbid;
}


bool
GlobalBundleIdentifier::operator == (const GlobalBundleIdentifier& other) const
{
  return (m_eid == other.m_eid) && (m_creationTimestamp == other.m_creationTimestamp);
}

bool
GlobalBundleIdentifier::operator != (const GlobalBundleIdentifier& other) const
{
  return !(*this ==  other);
}

bool
GlobalBundleIdentifier::operator < (const GlobalBundleIdentifier& other) const
{
  if (m_eid == other.m_eid)
    {
      return m_creationTimestamp < other.m_creationTimestamp;
    }
  else
    {
      return m_eid < other.m_eid;
    }
}

ostream& 
operator<< (ostream& os, const GlobalBundleIdentifier& gbid)
{
  os << "Source Endpoint = " << gbid.m_eid << endl;
  os << gbid.m_creationTimestamp.GetSeconds () << "|" << gbid.m_creationTimestamp.GetSequence ();
  return os;
}

}} // namespace bundleProtocol, ns3
