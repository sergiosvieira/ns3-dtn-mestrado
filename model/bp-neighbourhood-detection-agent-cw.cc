/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/random-variable.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "bp-neighbourhood-detection-agent-cws.h"
#include "ns3/mobility-model.h"

// FIXME ställ in jittret till ett bra värde
#define HELLO_MAX_JITTER (m_helloIntervall.GetSeconds() / 5)
#define JITTER (Seconds (UniformVariable ().GetValue (0, HELLO_MAX_JITTER)))

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("NeighbourhoodDetectionAgentCw");

// --------------- HELLO AGENT CW --------------------------------
NS_OBJECT_ENSURE_REGISTERED (NeighbourhoodDetectionAgentCw);

TypeId
NeighbourhoodDetectionAgentCw::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::NeighbourhoodDetectionAgentCw")
    .SetParent<NeighbourhoodDetectionAgent> ()
    ;
  return tid;
}

NeighbourhoodDetectionAgentCw::NeighbourhoodDetectionAgentCw ()
  : NeighbourhoodDetectionAgent ()
{}

NeighbourhoodDetectionAgentCw::NeighbourhoodDetectionAgentCw (Ptr<Node> node, 
                        Ptr<NetDevice> device, 
                        BundleEndpointId eid)
  : NeighbourhoodDetectionAgent (node, device, eid);
{}

NeighbourhoodDetectionAgentCw::~NeighbourhoodDetectionAgentCw ()
{}

void
NeighbourhoodDetectionAgentCw::DoDispose ()
{
  NeighbourhoodDetectionAgent::DoDispose ();
}
 
void
NeighbourhoodDetectionAgentCw::SendHello ()
{
  Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" <<"############################# " <<  "NeighbourhoodDetectionAgentCw::SendHello" << "(" << mm->GetPosition () << ")");
  
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<ConvergenceLayerAgent> ();
  ContactWindowInformation cwi = ContactWindowInformation (mobility->GetPosition (), mobility->GetVelocity (), cla->GetTransmissionRange (), cla->GetTransmissionSpeed (), true);
  uint8_t buffer[cwi.GetSerializedSize ()];
  cwi.Serialize (buffer);

  Ptr<Packet> hello= Create<Packet> (buffer, cwi.GetSerializedSize ());
  
  HelloHeader header;
  header.SetBundleEndpointId (m_eid);
  hello->AddHeader (header);
  m_socket->Send (hello);
  m_sendLogger (hello);
}


}} // namespace bundleProtocol, ns3
