/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <sstream>
#include "bundle-protocol-helper.h"

using namespace ns3::bundleProtocol;
using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BundleProtocolHelper");


BundleProtocolHelper::BundleProtocolHelper ()
{
  m_claFactory = ObjectFactory ();
  m_claFactory.SetTypeId ("ns3::bundleProtocol::ConvergenceLayerAgent");

  m_neighbourhoodDetectionAgentFactory = ObjectFactory ();
  m_neighbourhoodDetectionAgentFactory.SetTypeId ("ns3::bundleProtocol::NeighbourhoodDetectionAgent");

  m_linkManagerFactory = ObjectFactory ();
  m_linkManagerFactory.SetTypeId ("ns3::bundleProtocol::LinkManager");
  
  m_bundleRouterFactory = ObjectFactory ();
  //m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::OrwarRouterChangedOrder");
  //m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::RTEpidemic");
  //m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::DirectDeliveryRouter");
 // m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::RTProphet");
  //m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::RTTrendOfDelivery");
  //m_bundleRouterFactory.SetTypeId ("ns3::bundleProtocol::RTSprayAndWait");
  
  m_bundleProtocolAgentFactory = ObjectFactory ();
  m_bundleProtocolAgentFactory.SetTypeId ("ns3::bundleProtocol::BundleProtocolAgent");
}

void
BundleProtocolHelper::SetConvergenceLayer (std::string tid,
					   std::string n0, const AttributeValue &v0,
					   std::string n1, const AttributeValue &v1,
					   std::string n2, const AttributeValue &v2,
					   std::string n3, const AttributeValue &v3,
					   std::string n4, const AttributeValue &v4,
					   std::string n5, const AttributeValue &v5,
					   std::string n6, const AttributeValue &v6,
					   std::string n7, const AttributeValue &v7)
{
  m_claFactory = ObjectFactory (); 
  m_claFactory.SetTypeId (tid);
  m_claFactory.Set (n0, v0);
  m_claFactory.Set (n1, v1);
  m_claFactory.Set (n2, v2);
  m_claFactory.Set (n3, v3);
  m_claFactory.Set (n4, v4);
  m_claFactory.Set (n5, v5);
  m_claFactory.Set (n6, v6);
  m_claFactory.Set (n7, v7);
}
  
void
BundleProtocolHelper::SetNeighbourhoodDetectionAgent (std::string tid,
				     std::string n0, const AttributeValue &v0,
				     std::string n1, const AttributeValue &v1,
				     std::string n2, const AttributeValue &v2,
				     std::string n3, const AttributeValue &v3,
				     std::string n4, const AttributeValue &v4,
				     std::string n5, const AttributeValue &v5,
				     std::string n6, const AttributeValue &v6,
				     std::string n7, const AttributeValue &v7)
{
  m_neighbourhoodDetectionAgentFactory = ObjectFactory ();
  m_neighbourhoodDetectionAgentFactory.SetTypeId (tid);
  m_neighbourhoodDetectionAgentFactory.Set (n0, v0);
  m_neighbourhoodDetectionAgentFactory.Set (n1, v1);
  m_neighbourhoodDetectionAgentFactory.Set (n2, v2);
  m_neighbourhoodDetectionAgentFactory.Set (n3, v3);
  m_neighbourhoodDetectionAgentFactory.Set (n4, v4);
  m_neighbourhoodDetectionAgentFactory.Set (n5, v5);
  m_neighbourhoodDetectionAgentFactory.Set (n6, v6);
  m_neighbourhoodDetectionAgentFactory.Set (n7, v7);
}
  
void
BundleProtocolHelper::SetLinkManager (std::string tid,
					 std::string n0, const AttributeValue &v0,
					 std::string n1, const AttributeValue &v1,
					 std::string n2, const AttributeValue &v2,
					 std::string n3, const AttributeValue &v3,
					 std::string n4, const AttributeValue &v4,
					 std::string n5, const AttributeValue &v5,
					 std::string n6, const AttributeValue &v6,
					 std::string n7, const AttributeValue &v7)
{
  m_linkManagerFactory = ObjectFactory ();
  m_linkManagerFactory.SetTypeId (tid);
  m_linkManagerFactory.Set (n0, v0);
  m_linkManagerFactory.Set (n1, v1);
  m_linkManagerFactory.Set (n2, v2);
  m_linkManagerFactory.Set (n3, v3);
  m_linkManagerFactory.Set (n4, v4);
  m_linkManagerFactory.Set (n5, v5);
  m_linkManagerFactory.Set (n6, v6);
  m_linkManagerFactory.Set (n7, v7);
}

void
BundleProtocolHelper::SetBundleRouter (std::string tid,
				       std::string n0, const AttributeValue &v0,
				       std::string n1, const AttributeValue &v1,
				       std::string n2, const AttributeValue &v2,
				       std::string n3, const AttributeValue &v3,
				       std::string n4, const AttributeValue &v4,
				       std::string n5, const AttributeValue &v5,
				       std::string n6, const AttributeValue &v6,
				       std::string n7, const AttributeValue &v7)
{
  m_bundleRouterFactory = ObjectFactory ();
  m_bundleRouterFactory.SetTypeId (tid);
  m_bundleRouterFactory.Set (n0, v0);
  m_bundleRouterFactory.Set (n1, v1);
  m_bundleRouterFactory.Set (n2, v2);
  m_bundleRouterFactory.Set (n3, v3);
  m_bundleRouterFactory.Set (n4, v4);
  m_bundleRouterFactory.Set (n5, v5);
  m_bundleRouterFactory.Set (n6, v6);
  m_bundleRouterFactory.Set (n7, v7);
}

void
BundleProtocolHelper::SetBundleProtocolAgent (std::string tid,
					      std::string n0, const AttributeValue &v0,
					      std::string n1, const AttributeValue &v1,
					      std::string n2, const AttributeValue &v2,
					      std::string n3, const AttributeValue &v3,
					      std::string n4, const AttributeValue &v4,
					      std::string n5, const AttributeValue &v5,
					      std::string n6, const AttributeValue &v6,
					      std::string n7, const AttributeValue &v7)
{
  m_bundleProtocolAgentFactory = ObjectFactory ();
  m_bundleProtocolAgentFactory.SetTypeId (tid);
  m_bundleProtocolAgentFactory.Set (n0, v0);
  m_bundleProtocolAgentFactory.Set (n1, v1);
  m_bundleProtocolAgentFactory.Set (n2, v2);
  m_bundleProtocolAgentFactory.Set (n3, v3);
  m_bundleProtocolAgentFactory.Set (n4, v4);
  m_bundleProtocolAgentFactory.Set (n5, v5);
  m_bundleProtocolAgentFactory.Set (n6, v6);
  m_bundleProtocolAgentFactory.Set (n7, v7);
}


void
BundleProtocolHelper::Install (NodeContainer container,int num)
{
  Install (container.Begin (), container.End (),num);
}

void
BundleProtocolHelper::Install (NodeContainer::Iterator begin, NodeContainer::Iterator end,int num)
{

  for (NodeContainer::Iterator i = begin; i != end; ++i)
    {
      Ptr<Node> node = *i;
	  NS_LOG_DEBUG("--> Node (" << node->GetId() << ")");
      Install (node,num);
    }

}

void
BundleProtocolHelper::Install (Ptr<Node> node,int num)
{
  Ptr<NetDevice> device = node->GetDevice (0);
  if (device == 0)
    {
      NS_FATAL_ERROR ("BundleProtocolHelper::Install (): Needs a net device installed");
    }

  Ptr<PacketSocketFactory> psFactory = node->GetObject<PacketSocketFactory> ();
  if (psFactory == 0)
    {
      PacketSocketHelper helper = PacketSocketHelper ();
      helper.Install (node);
    }

  stringstream ss;
  ss << node->GetId ();
  BundleEndpointId eid = BundleEndpointId ("dtn", ss.str ());
  
  cla = m_claFactory.Create<ConvergenceLayerAgent> ();
  cla->SetNode (node);
  cla->SetNetDevice (device);

  nda = m_neighbourhoodDetectionAgentFactory.Create<NeighbourhoodDetectionAgent> ();
  nda->SetNode (node);
  nda->SetNetDevice (device);
  nda->SetBundleEndpointId (eid);

  Ptr<LinkManager> lm = m_linkManagerFactory.Create<LinkManager> ();  

  br = m_bundleRouterFactory.Create<BundleRouter> ();
  br->SetNode (node);
  br->SetLinkManager (lm);
  br->SetNeighbourhoodDetectionAgent (nda);
  br->SetBundleEndpointId (eid);

  bpa = m_bundleProtocolAgentFactory.Create<BundleProtocolAgent> ();

  bpa->SetBundleEndpointId (eid);
  bpa->SetNode (node);
  
  bpa->SetNumNode(num);

  bpa->SetConvergenceLayer (cla);
  bpa->SetBundleRouter (br);
  node->AggregateObject (bpa);

  node->AggregateObject (br);
  node->AggregateObject (cla);
  node->AggregateObject (nda);
 
  Ptr<RegistrationFactory> registrationFactory = CreateObject<RegistrationFactory> ();
  registrationFactory->SetBpa (bpa);
  node->AggregateObject (registrationFactory);

  cla->Init ();
  bpa->Init ();
  nda->Init ();
  br->Init ();

  Start();

}

void BundleProtocolHelper::Start() {
	cla->Start();
	bpa->Start();
	//nda->Start();
	br->Start();
}

} // namespace ns3


