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
#include "bp-neighbourhood-detection-agent.h"
#include "ns3/mobility-model.h"

// FIXME st�ll in jittret till ett bra v�rde


namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("NeighbourhoodDetectionAgent");
NS_OBJECT_ENSURE_REGISTERED (NeighbourhoodDetectionAgent);

TypeId
NeighbourhoodDetectionAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::NeighbourhoodDetectionAgent")
    .SetParent<Object> ()
    .AddConstructor<NeighbourhoodDetectionAgent> ()
    .AddAttribute ("BundleEndpointId",
                   "Sets the bundle endpoint id.",
                   BundleEndpointIdValue (BundleEndpointId ()),
                   MakeBundleEndpointIdAccessor (&NeighbourhoodDetectionAgent::m_eid),
                   MakeBundleEndpointIdChecker ())
    .AddAttribute ("HelloIntervall",
                   "Sets the intervall between hello broadcasts",
                   TimeValue (Time (Seconds (0.8))),
                   MakeTimeAccessor (&NeighbourhoodDetectionAgent::m_helloIntervall),
                   MakeTimeChecker ())
    .AddTraceSource ("SendHello", "A hello message has been broadcasted.",
                     MakeTraceSourceAccessor (&NeighbourhoodDetectionAgent::m_sendLogger))
    .AddTraceSource ("ReceivedHello", "A hello message has been received.",
                     MakeTraceSourceAccessor (&NeighbourhoodDetectionAgent::m_receiveLogger))
    ;
  return tid;
}

NeighbourhoodDetectionAgent::NeighbourhoodDetectionAgent ()
  : m_node (0), 
    m_device (0),
    m_helloTimer (Timer::CANCEL_ON_DESTROY),
    m_started (false)
{
  m_helloIntervall = Seconds(0.3);
  m_helloTimer.SetFunction (&NeighbourhoodDetectionAgent::HelloTimerExpire, this);
}

NeighbourhoodDetectionAgent::NeighbourhoodDetectionAgent (Ptr<Node> node, 
                        Ptr<NetDevice> device, 
                        BundleEndpointId eid)
  : 
  m_node (node),
  m_device (device),
  m_eid (eid), 
  m_helloTimer (Timer::CANCEL_ON_DESTROY),
  m_started (false)
{
  m_helloTimer.SetFunction (&NeighbourhoodDetectionAgent::HelloTimerExpire, this);
}

NeighbourhoodDetectionAgent::~NeighbourhoodDetectionAgent ()
{}

void
NeighbourhoodDetectionAgent::DoDispose ()
{
  m_node = 0;
  m_device = 0;
  m_socket = 0;
  m_discoveredLinkCb = MakeNullCallback<void, Ptr<Packet>, Address > ();
  Object::DoDispose ();
}
 
void 
NeighbourhoodDetectionAgent::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
NeighbourhoodDetectionAgent::SetNetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}

Ptr<NetDevice> 
NeighbourhoodDetectionAgent::GetDevice () const
{
  return m_device;
}

void
NeighbourhoodDetectionAgent::SetBundleEndpointId (BundleEndpointId eid)
{
  m_eid = eid;
}

BundleEndpointId
NeighbourhoodDetectionAgent::GetBundleEndpointId () const
{
  return m_eid;
}

Time
NeighbourhoodDetectionAgent::GetHelloIntervall () const
{
  return m_helloIntervall;
}

void
NeighbourhoodDetectionAgent::SetDiscoveredLinkCallback (Callback<void, Ptr<Packet>, Address > discoveredLinkCb)
{
  m_discoveredLinkCb = discoveredLinkCb;
 
}

void
NeighbourhoodDetectionAgent::Init ()
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" << "NeighbourhoodDetectionAgent::Start");
  if (m_socket == 0)
    {
      NS_ASSERT (m_node != 0 && m_device != 0 && m_eid != BundleEndpointId ());
      // Setup the socket
      PacketSocketAddress sendAddr;
      sendAddr.SetSingleDevice (m_device->GetIfIndex ());
      sendAddr.SetPhysicalAddress  (m_device->GetBroadcast ());
      sendAddr.SetProtocol (140);
      m_socket = Socket::CreateSocket (m_node, PacketSocketFactory::GetTypeId ());
      NS_ASSERT (m_socket != 0);
      m_socket->Bind (sendAddr);
      m_socket->Connect (sendAddr);
      m_socket->SetRecvCallback (MakeCallback (&NeighbourhoodDetectionAgent::HandleHello, this));
    }
}

int blabla = 0;

void
NeighbourhoodDetectionAgent::Start ()
{
  //blabla++;
  //cout<<"Start() Call "<<blabla<<"("<<this->m_node->GetId()<<")"<<" Time = "<<Simulator::Now().GetSeconds()<<"\n";
  if (!m_started)
    {
      m_started = true;
      m_helloTimer.Schedule (JITTER);
    }
}

void 
NeighbourhoodDetectionAgent::Stop ()
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" << "NeighbourhoodDetectionAgent::Stop");
  if (m_started)
    {
      m_started = false;
      m_helloTimer.Remove ();
    }
}

void
NeighbourhoodDetectionAgent::HelloTimerExpire ()
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")");
  SendHello ();
  m_helloTimer.Schedule (m_helloIntervall-JITTER);
  //m_helloTimer.Schedule (Seconds(1.0));
}

void
NeighbourhoodDetectionAgent::SendHello ()
{
  /*
  Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel> ();
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " <<"############################# " <<  "NeighbourhoodDetectionAgent::SendHello" << "(" << mm->GetPosition () << ")");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " <<"############################# " <<  "NeighbourhoodDetectionAgent::SendHello" << "(" << mm->GetPosition () << ")" << endl;
  HelloHeader header;
  header.SetBundleEndpointId (m_eid);
  Ptr<Packet> hello = Create<Packet> ();
  hello->AddHeader (header);
  m_socket->Send (hello);
  m_sendLogger (hello);
  */
	if (!m_sendHelloCb.IsNull()) {
		m_sendHelloCb (m_socket, m_eid);
	}
}

void
NeighbourhoodDetectionAgent::HandleHello (Ptr<Socket> socket)
{
  /*
   *
  Ptr<Packet> receivedHello;
  Address fromAddress;
  receivedHello = socket->RecvFrom (fromAddress);

  m_receiveLogger (receivedHello);

  HelloHeader header;
  receivedHello->PeekHeader (header);

  Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel> (); 
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " <<"############################# " <<  "NeighbourhoodDetectionAgent::HandleHello from endpoint id " << header.GetBundleEndpointId ()  << " size = " << receivedHello->GetSize () <<  " (" << mm->GetPosition () << ")");
  cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " <<"############################# " <<  "NeighbourhoodDetectionAgent::HandleHello from endpoint id " << header.GetBundleEndpointId ()  << " size = " << receivedHello->GetSize () <<  " (" << mm->GetPosition () << ")" << endl;

  Simulator::ScheduleNow (&NeighbourhoodDetectionAgent::NotifyDiscoveredLink, this, receivedHello, fromAddress);
  */
	if (!m_handleHelloCb.IsNull()) {
		m_handleHelloCb (socket);
	}
}

void
NeighbourhoodDetectionAgent::NotifyDiscoveredLink (Ptr<Packet> hello, Address fromAddress)
{
  if (!m_discoveredLinkCb.IsNull ())
    m_discoveredLinkCb (hello, fromAddress);    
}

}} // namespace bundleProtocol, ns3
