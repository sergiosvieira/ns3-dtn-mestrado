/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_HELLO_AGENT_H
#define BP_HELLO_AGENT_H

#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/header.h"
#include "ns3/net-device.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"

#include "bp-bundle-endpoint-id.h"
#include "bp-link-manager.h"
#include "bp-link.h"
#include "bp-bundle-protocol-agent.h"
#include "bp-convergence-layer-agent.h"
#include "bp-neigh-header.h"
#include <iostream>
#include <string>

namespace ns3 {
namespace bundleProtocol {
    
class BundleProtocolAgent;

/**
 * \ingroup neighbourhoodDetectionAgent
 *
 * \brief Determines a nodes neigbourhood nodes by sending periodical hello messages.
 *
 *
 */

class NeighbourhoodDetectionAgent : public Object
{
public:
  static TypeId GetTypeId (void);

  NeighbourhoodDetectionAgent ();
  NeighbourhoodDetectionAgent (Ptr<Node> node,
              Ptr<NetDevice> device,
              BundleEndpointId eid);
  ~NeighbourhoodDetectionAgent ();
  virtual void DoDispose ();
  
  virtual void SetDiscoveredLinkCallback (Callback<void, Ptr<Packet>, Address > discoveredLinkCb);

  void SetNode (Ptr<Node> node);
  void SetNetDevice (Ptr<NetDevice> device);
  void SetBundleEndpointId (BundleEndpointId eid);

  Ptr<NetDevice> GetDevice () const;
  BundleEndpointId GetBundleEndpointId () const;
  Time GetHelloIntervall () const;

  virtual void Init ();
  virtual void Start ();
  virtual void Stop ();

  /* sergiosvieira */
  void SetSendHelloCallback (Callback<void, Ptr<Socket>, BundleEndpointId > sendHelloCb) {
	  m_sendHelloCb = sendHelloCb;
  }
  void SetHandleHelloCallback (Callback<void, Ptr<Socket> > handleHelloCb) {
	  m_handleHelloCb = handleHelloCb;
  }
  virtual void NotifyDiscoveredLink (Ptr<Packet> hello, Address fromAddress);
  /* sergiosvieira */

private:
  virtual void HelloTimerExpire ();
  virtual void SendHello ();
  virtual void HandleHello (Ptr<Socket> socket);


  Ptr<Node> m_node;
  Ptr<NetDevice> m_device;
  BundleEndpointId m_eid;
  Time m_helloIntervall;
  Timer m_helloTimer;

  bool m_started;
  Ptr<Socket> m_socket;
  Callback<void, Ptr<Packet>, Address > m_discoveredLinkCb;
  /* sergiosvieira */
  Callback<void, Ptr<Socket>, BundleEndpointId > m_sendHelloCb;
  Callback<void, Ptr<Socket> > m_handleHelloCb;
  /* sergiosvieira */
  TracedCallback<Ptr<const Packet> > m_sendLogger;
  TracedCallback<Ptr<const Packet> > m_receiveLogger;
};

}} // namespace bundleProtocol, ns3

#endif /* BP_HELLO_AGENT_H */
