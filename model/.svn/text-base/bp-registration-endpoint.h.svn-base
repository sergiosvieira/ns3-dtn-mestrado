/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_REGISTRATION_ENDPOINT_H
#define BP_REGISTRATION_ENDPOINT_H

#include "bp-bundle-endpoint-id.h"
#include "ns3/ptr.h"
#include "ns3/callback.h"
#include "ns3/packet.h"

namespace ns3 {
namespace bundleProtocol {

class Bundle;

class RegistrationEndpoint 
{
 public:
  RegistrationEndpoint (BundleEndpointId eid);
  ~RegistrationEndpoint ();
  
  BundleEndpointId GetBundleEndpointId () const;
  void SetBundleEndpointId (BundleEndpointId eid);
  
  void SetRxCallback (Callback<void, Ptr<Packet>, BundleEndpointId> callback);
  void SetDestroyCallback (Callback<void> callback);
  void SetSendCallback (Callback<void, uint32_t> callback);

  void ForwardUp (Ptr<Packet> adu, BundleEndpointId address);
  void NotifySend (uint32_t freeBytes);
 private:
  void DoForwardUp (Ptr<Packet> adu, BundleEndpointId address);
  void DoNotifySend (uint32_t freeBytes);

  BundleEndpointId m_endpointId;
  Callback<void, Ptr<Packet>, BundleEndpointId>  m_rxCallback;
  Callback<void, uint32_t> m_sendCallback;
  Callback<void> m_destroyCallback;
};

}} // namespace bundleProtocol, ns3


#endif /* BP_REGISTRATION_ENDPOINT_H */
