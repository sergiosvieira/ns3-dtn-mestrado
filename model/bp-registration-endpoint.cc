/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/simulator.h"
#include "ns3/log.h"

#include "bp-registration-endpoint.h"
#include "bp-bundle.h"

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("RegistrationEndpoint");

RegistrationEndpoint::RegistrationEndpoint (BundleEndpointId eid)
  : m_endpointId (eid)
{
}

RegistrationEndpoint::~RegistrationEndpoint () 
{
  if (!m_destroyCallback.IsNull ())
    {
      m_destroyCallback ();
    }
  m_rxCallback = MakeNullCallback<void, Ptr<Packet>, BundleEndpointId> ();
  m_sendCallback = MakeNullCallback<void, uint32_t> ();
  m_destroyCallback = MakeNullCallback<void> ();
}

BundleEndpointId 
RegistrationEndpoint::GetBundleEndpointId () const
{
  return m_endpointId;
}

void 
RegistrationEndpoint::SetBundleEndpointId (BundleEndpointId eid)
{
  m_endpointId = eid;
}

void 
RegistrationEndpoint::SetRxCallback (Callback<void, Ptr<Packet>, BundleEndpointId> callback)
{
  m_rxCallback = callback;
}
  
void 
RegistrationEndpoint::SetDestroyCallback (Callback<void> callback)
{
  m_destroyCallback = callback;
}

void 
RegistrationEndpoint::SetSendCallback (Callback<void, uint32_t> callback)
{
  m_sendCallback = callback;
}
  
void 
RegistrationEndpoint::ForwardUp (Ptr<Packet> adu, BundleEndpointId address)
{
  if (!m_rxCallback.IsNull ())
    {
      Simulator::ScheduleNow (&RegistrationEndpoint::DoForwardUp, this, adu, address);
    }
}

void
RegistrationEndpoint::NotifySend (uint32_t freeBytes)
{
  if (!m_sendCallback.IsNull ())
    {
      Simulator::ScheduleNow (&RegistrationEndpoint::DoNotifySend, this, freeBytes);
    } 
}
  
void
RegistrationEndpoint::DoNotifySend (uint32_t freeBytes)
{
  m_sendCallback (freeBytes);
}

void 
RegistrationEndpoint::DoForwardUp (Ptr<Packet> p, BundleEndpointId address)
{
  m_rxCallback (p, address);
}

}} // namespace bundleProtocol, ns3
