/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_REGISTRATION_H
#define BP_REGISTRATION_H

#include <stdint.h>
#include <deque>

#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/tag.h"
#include "ns3/object.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/packet.h"

#include "bp-bundle-endpoint-id.h"
#include "bp-registration-endpoint.h"
#include "bp-bundle-protocol-agent.h"
#include "bp-header.h"

namespace ns3 {
namespace bundleProtocol {

class BundleProtocolAgent;

/**
 * \ingroup bundleProtocolAgent
 *
 * \brief Enables communications between a application and the bundle protocol.
 *
 *
 */

class Registration : public Object 
{
public:
  static TypeId GetTypeId (void);
  
  Registration (void);
  virtual ~Registration (void);
  virtual void DoDispose ();
  
  void SetBundleProtocol (Ptr<BundleProtocolAgent> bpa);

  void SetRegistrationCallback (Callback<void, Ptr<Registration> > registrationSucceeded,
                                Callback<void,  Ptr<Registration> > registrationFailed);
  void SetSendCallback (Callback<void, Ptr<Registration>, uint32_t> sendCb);
  void SetRecvCallback (Callback<void, Ptr<Registration> > recvCb);
  
  int Register ();
  int Register (BundleEndpointId eid);
  
  int Unregister (void);
  int Send (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority,  Time ttl, const PrimaryProcessingControlFlags& flags = PrimaryProcessingControlFlags ());
  // int SendWithReport (Ptr<Packet> adu, const BundleEndpointId& destinationBundlePriority priority, Time ttl, const PrimaryProcessingControlFlags& flags = PrimaryProcessingControlFlags ());
  Ptr<Packet> Recv (BundleEndpointId &fromAddress);

protected:
  void NotifyRegistrationSucceeded (void);
  void NotifyRegistrationFailed (void);
  void NotifySend (uint32_t freeBytes);
  void NotifyRecv (void);

private:
  int DoRegister (void);
  void ForwardUp (Ptr<Packet> adu, BundleEndpointId address);
  void Destroy (void);
  
  Callback<void, Ptr<Registration> > m_registrationSucceeded;
  Callback<void, Ptr<Registration> > m_registrationFailed;
  Callback<void, Ptr<Registration>, uint32_t> m_sendCb;
  Callback<void, Ptr<Registration> > m_recvCb;

  Ptr<BundleProtocolAgent> m_bpa;
  RegistrationEndpoint *m_regEndpoint;
  bool m_registered;
  std::deque<Ptr<Packet> > m_deliveryQueue;
};

class RegistrationAddressTag : public Tag
{
public:
  RegistrationAddressTag (void);
  void SetAddress (BundleEndpointId addr);
  BundleEndpointId GetAddress (void) const;

  static TypeId GetTypeId (void);  
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  BundleEndpointId m_address;
};

}} // namespace bundleProtocol, ns3

#endif /* BP_REGISTRATION_H */
