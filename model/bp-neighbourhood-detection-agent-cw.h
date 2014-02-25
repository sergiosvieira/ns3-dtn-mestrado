/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_HELLO_AGENT_CW_H
#define BP_HELLO_AGENT_CW_H

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

class NeighbourhoodDetectionAgentCw : public Object
{
public:
  static TypeId GetTypeId (void);

  NeighbourhoodDetectionAgentCw ();
  NeighbourhoodDetectionAgentCw (Ptr<Node> node,
              Ptr<NetDevice> device,
              BundleEndpointId eid);
  ~NeighbourhoodDetectionAgentCw ();
  virtual void DoDispose ();

private:
  virtual void SendHello ();

};

}} // namespace bundleProtocol, ns3

#endif /* BP_HELLO_AGENT_CW_H */
