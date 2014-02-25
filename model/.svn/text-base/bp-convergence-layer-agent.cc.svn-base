/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/packet-socket-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/wifi-net-device.h"

#include "bp-convergence-layer-agent.h"
#include "bp-convergence-layer-header.h"
#include "bp-bundle-router.h"
#include "bp-neighbourhood-detection-agent.h"

#include <cmath>

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("ConvergenceLayerAgent");

ostream& operator<< (ostream& os, const SendQueueElement& sqe)
{
  os << "SendQueueElement ";
  if (sqe.m_sqeType == SQE_DATA_BUNDLE)
    {
      os << "(Data bundle)" << endl;
    }
  else if (sqe.m_sqeType == SQE_ROUTER_BUNDLE)
    {
      os << "(Router bundle)" << endl;
    }
  else if (sqe.m_sqeType == SQE_ACK)
    {
      os << "(Ack)" << endl;
    }
  else
    {
      os << "(Unkown)" << endl;
    }
  
  os << "Mac: " << sqe.m_mac << endl;
  os << "GlobalBundleIdentifier: " << sqe.m_gbid << endl;
  return os;
}

struct SortBySegmentNumber : public std::binary_function<Ptr<Packet>, Ptr<Packet>, bool >
{
  bool operator() (Ptr<Packet> left, Ptr<Packet> right) const
  {
    ConvergenceLayerHeader leftHeader;
    ConvergenceLayerHeader rightHeader;
    left->PeekHeader (leftHeader);
    left->PeekHeader (rightHeader);

    return leftHeader.GetSegmentNumber () < rightHeader.GetSegmentNumber ();
  }
};

struct EqGbid : public unary_function <SendQueueElement, bool>
{
  GlobalBundleIdentifier m_gbid;
  EqGbid (const GlobalBundleIdentifier& gbid)
    : m_gbid (gbid)
  {}
  
  bool operator () (SendQueueElement& el)
  {
    return el.m_gbid == m_gbid;
  }
};

NS_OBJECT_ENSURE_REGISTERED (ConvergenceLayerAgent);

TypeId
ConvergenceLayerAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::ConvergenceLayerAgent")
    .SetParent<Object> ()
    .AddConstructor<ConvergenceLayerAgent> ()
    .AddAttribute ("DataRate",
                   "Sets the specified transmission speed of the convergence layer in Mbs.",
                   DataRateValue (string ("2Mbps")),
                   MakeDataRateAccessor (&ConvergenceLayerAgent::m_dataRate),
                   MakeDataRateChecker ())
    .AddAttribute ("Range",
                   "Sets the specified transmission range of the convergence layer in meters.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&ConvergenceLayerAgent::m_range),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("AckWaitTime",
                   "Sets the time to wait for an ack",
                   TimeValue (Seconds (0.1)),
                   MakeTimeAccessor (&ConvergenceLayerAgent::m_ackWaitTime),
                   MakeTimeChecker ())
    .AddTraceSource ("AbortedDataBundle", "A data bundle tranfer has been \"aborted\".",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_abortDataLogger))
    .AddTraceSource ("RealAbortedDataBundle", "A data bundle tranfer has been \"aborted\".",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realAbortDataLogger))
    .AddTraceSource ("StartedDataBundle", "A data bundle transfer has been started.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_startDataLogger))
    .AddTraceSource ("RealStartedDataBundle", "A data bundle transfer has been started.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realStartDataLogger))
    .AddTraceSource ("AbortedRouterBundle", "A rotuer bundle tranfer has been \"aborted\".",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_abortRouterLogger))
    .AddTraceSource ("RealAbortedRouterBundle", "A rotuer bundle tranfer has been \"aborted\".",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realAbortRouterLogger))
    .AddTraceSource ("StartedRouterBundle", "A router bundle transfer has been started.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_startRouterLogger))
    .AddTraceSource ("RealStartedRouterBundle", "A router bundle transfer has been started.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realStartRouterLogger))
    .AddTraceSource ("StartedSegment", "A segment transfer has been started.",
                      MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_startSegmentLogger))
    .AddTraceSource ("AbortedSegment", "A segment transfer has been aborted.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_abortSegmentLogger))
    .AddTraceSource ("DeliveredSegment", "A segment transfer has suceeded",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_deliveredSegmentLogger))
    .AddTraceSource ("RealRelayed", "A bundle has been relayed",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realRelayLogger))
    .AddTraceSource ("RealDeliveredRouter", "A router bundle has been delivered",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_realRouterDeliveryLogger))
    .AddTraceSource ("ContactClosed", "A contact has been closed.",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_contactClosedLogger))
    .AddTraceSource ("Acks", "A acks has been received or a ack timeout has occured",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_ackLogger))
    .AddTraceSource ("SendBundle", "The node wants to start sending a bundle",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_sendBundleLogger))
    .AddTraceSource ("StartSending", "The node has started sending a bundle",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_startSendingBundleLogger))
    .AddTraceSource ("FinishedSending", "The node has finished sending a bundle",
                     MakeTraceSourceAccessor (&ConvergenceLayerAgent::m_finishedSendingBundleLogger))
    ;
  return tid;
}

ConvergenceLayerAgent::ConvergenceLayerAgent ()
  : m_node (), 
    m_netDevice (),
    m_socket (),
    m_sequenceNumber (0),
    m_isSending (false),
    m_ackQueue (),
    m_sendQueue (),
    m_current (),
    m_waitingForAck (),
    m_ackTimer (Timer::CANCEL_ON_DESTROY),
    m_recvQueue (),
    m_started ()
{}

ConvergenceLayerAgent::ConvergenceLayerAgent (Ptr<Node> node, Ptr<NetDevice> device)
  : m_node (), 
    m_netDevice (),
    m_socket (),
    m_sequenceNumber (0),
    m_isSending (false),
    m_ackQueue (),
    m_sendQueue (),
    m_current (),
    m_waitingForAck (),
    m_ackTimer (Timer::CANCEL_ON_DESTROY),
    m_recvQueue (),
    m_started ()
{}

ConvergenceLayerAgent::~ConvergenceLayerAgent ()
{}

void
ConvergenceLayerAgent::DoDispose ()
{
  m_node = 0;
  m_netDevice = 0;
  m_socket = 0;
  m_recvQueue.clear ();
  m_ackQueue.clear ();
  m_sendQueue.clear ();
  m_bundleRecvCb = MakeNullCallback<void, Ptr<Bundle> > ();
  m_bundleSentOkCb = MakeNullCallback<void, const Mac48Address&, GlobalBundleIdentifier, bool> ();
  m_bundleSentFailedCb = MakeNullCallback<void, const Mac48Address&, GlobalBundleIdentifier> ();
  m_transmissionCancelledCb = MakeNullCallback<void, const Mac48Address&, GlobalBundleIdentifier> ();
  Object::DoDispose ();
}

void
ConvergenceLayerAgent::HackFixme (uint32_t toNodeId, Time timeleft, bool cwMax)
{
  m_contactClosedLogger (m_node->GetId (), toNodeId, timeleft, cwMax);
}
  
void
ConvergenceLayerAgent::SetBundleReceivedCallback (Callback<void, Ptr<Bundle> > bundleRecvCb)
{
  m_bundleRecvCb = bundleRecvCb;
}
  
void
ConvergenceLayerAgent::SetBundleSentOkCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier, bool> bundleSentOkCb)
{
  m_bundleSentOkCb = bundleSentOkCb;
}
  
void
ConvergenceLayerAgent::SetBundleSentFailedCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier> bundleSentFailedCb)
{
  m_bundleSentFailedCb = bundleSentFailedCb;
}

void
ConvergenceLayerAgent::SetTransmissionCancelledCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier > transmissionCancelledCb)
{
  m_transmissionCancelledCb = transmissionCancelledCb;
}
                                                         
void
ConvergenceLayerAgent::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node>
ConvergenceLayerAgent::GetNode () const
{
  return m_node;
}

void
ConvergenceLayerAgent::SetNetDevice (Ptr<NetDevice> netDevice)
{
  m_netDevice = netDevice;
}

Ptr<NetDevice>
ConvergenceLayerAgent::GetNetDevice () const
{
  return m_netDevice;
}

void
ConvergenceLayerAgent::SetTransmissionRange (uint32_t range)
{
  m_range = range;
}

uint32_t
ConvergenceLayerAgent::GetTransmissionRange () const
{
  return m_range;
}

void
ConvergenceLayerAgent::SetTransmissionSpeed (const DataRate& dataRate) // Mbs
{
  m_dataRate = dataRate;
}

DataRate
ConvergenceLayerAgent::GetTransmissionSpeed () const
{
  return m_dataRate;
}

void
ConvergenceLayerAgent::Init ()
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::Init");
 // Start listening for bundles
  if (m_socket == 0)
    {
      PacketSocketAddress bindAddr;
      bindAddr.SetSingleDevice (m_netDevice->GetIfIndex ());
      bindAddr.SetPhysicalAddress  (m_netDevice->GetBroadcast ());
      bindAddr.SetProtocol (200); 
      m_socket = Socket::CreateSocket (m_node, PacketSocketFactory::GetTypeId ());
      m_socket->Bind (bindAddr);
      m_socket->Connect (bindAddr);
      m_socket->SetRecvCallback (MakeCallback (&ConvergenceLayerAgent::ReceiveSegment, this));
    }
  
  Ptr<AdhocWifiMac> mac = m_netDevice->GetObject<WifiNetDevice> ()->GetMac ()->GetObject<AdhocWifiMac> ();
  mac->SetTxCallbacks (MakeCallback (&ConvergenceLayerAgent::SegmentSentOk, this),
                       MakeCallback (&ConvergenceLayerAgent::SegmentSentFailed, this));
}

void
ConvergenceLayerAgent::Stop ()
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::Stop");
  m_socket->Close ();
}


// Functionality for sending
uint16_t
ConvergenceLayerAgent::GetSequenceNumber ()
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" << " ConvergenceLayerAgent::GetSequenceNumber");
  return m_sequenceNumber++;
}

void
ConvergenceLayerAgent::BundleQueued (Ptr<Link> link, Ptr<Bundle> bundle)
{
  NS_LOG_DEBUG ( "(" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::BundleQueued");
  if (m_node->GetObject<BundleProtocolAgent> ()->GetBundleRouter ()->IsRouterSpecific (bundle))
    {
      m_startRouterLogger (bundle);
    }
  else
    {
      m_startDataLogger (bundle);
    }
  SendBundle (bundle, link);
}

void
ConvergenceLayerAgent::SendBundle (Ptr<Bundle> bundle, Ptr<Link> link)
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" << "ConvergenceLayerAgent::SendBundle");
  Mac48Address to = Mac48Address::ConvertFrom (link->GetRemoteAddress ());
  Ptr<BundleRouter> br = m_node->GetObject<BundleProtocolAgent> ()->GetBundleRouter ();
 
  Ptr<Packet> send = bundle->ToPacket ();
     
  uint32_t size = send->GetSize ();
  // FIXME 
  // Lite fulhack f�r att ber�kna maximala storleken p� segmenten. 
  // wifi header = 34 bytes, convergene header = 6 bytes, dvs 40 bytes totalt
  uint32_t maxSegmentSize = m_netDevice->GetMtu () - 40;
  uint16_t nSegments =  ceil (((double) size) / ((double) maxSegmentSize));
  NS_LOG_DEBUG ("ConvergenceLayerAgent::SendBundle::nSegments = " << nSegments);
  uint32_t offset = 0;

  uint32_t sequenceNumber = GetSequenceNumber ();

  Segments segments;

  //PrimaryBundleHeader pheader = bundle->GetPrimaryHeader ();
  //CanonicalBundleHeader cheader = bundle->GetCanonicalHeaders ().front ();

  if (nSegments > 1) // The bundle needs to be diveded into segments (i.e fragments)
    {
      for (int i = 1; i <= nSegments; ++i)
	{          
          // Setup the convergence layer header
          ConvergenceLayerHeader header;
          header.SetType (CLA_SEGMENT);

          if (i == 1)
            header.SetStartFlag (true);
          
          if (i == nSegments)
            header.SetEndFlag (true);
          
          header.SetNumberOfSegments (nSegments);
          header.SetSegmentNumber (i);
          header.SetSequenceNumber (sequenceNumber);
          
          // Create the segment
          Ptr<Packet> segment;
          
	  if (i != nSegments) 
            {
              segment = send->CreateFragment (offset, maxSegmentSize);
            }
          else 
            {
              segment = send->CreateFragment (offset, size); 
            }
          
          // Calculate remaining bytes and the offset for the next segment
	  size = size - segment->GetSize ();
	  offset += segment->GetSize ();
          
          // Add the header to the segment
          segment->AddHeader (header);

          // Add the segment to the queue of segments to be sent
          segments.push_back (segment); 
	}
    }
  else // No need to segment the bundle.
    {
      // Setup the convergence layer header
      ConvergenceLayerHeader header;
      header.SetType (CLA_SEGMENT);
      header.SetStartFlag (true);
      header.SetEndFlag (true);
      header.SetNumberOfSegments (nSegments);
      header.SetSegmentNumber (1);
      header.SetSequenceNumber (sequenceNumber);

      send->AddHeader (header);
      // Add the "segment" to the queue of segments to be sent
      segments.push_back (send);
    }

  PacketSocketAddress destAddr;
  destAddr.SetSingleDevice (m_netDevice->GetIfIndex ());
  destAddr.SetPhysicalAddress  (to);
  destAddr.SetProtocol (200);

  SendQueueElement el = SendQueueElement ();
  el.m_toEid = link->GetRemoteEndpointId ();
  el.m_mac = to;
  el.m_bundle = bundle;
  el.m_gbid = bundle->GetBundleId ();
  el.m_destination = destAddr;
  el.m_segments = segments;
  CanonicalBundleHeader header = bundle->GetCanonicalHeaders ().front ();
  if (header.GetBlockType () == PAYLOAD_BLOCK)
    {
      el.m_sqeType = SQE_DATA_BUNDLE;
    }
  else
    {
      el.m_sqeType = SQE_ROUTER_BUNDLE;
    }
  
  m_sendQueue.push_back (el);

  // Begin sending the bundle/segments
   Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
}


void
ConvergenceLayerAgent::SendSegments ()
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<" ConvergenceLayerAgent::SendSegments");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::SendSegments" << endl;
  if (!m_isSending)
    {
      m_current = SendQueueElement ();
      SendQueue::iterator iter;
      if (!m_ackQueue.empty ())
        {
          iter = m_ackQueue.begin ();
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Want to send a ACK to " << iter->m_mac << endl;
          m_current = *iter;
          m_isSending = true;
          m_startSegmentLogger (iter->m_segments.front ());
          NS_LOG_DEBUG(iter->m_destination <<" Socket SendTo");
          m_socket->SendTo (iter->m_segments.front ()->Copy (),0, iter->m_destination);
        }
      else if (!m_sendQueue.empty ())
        {
          iter = m_sendQueue.begin ();

          if (!iter->m_cancelled && m_waitingForAck.IsNull ())
            {
              ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Want to send a data segment to " << iter->m_mac << endl;
              m_isSending = true;
              m_current = *iter;
                  
              if (iter->m_sqeType == SQE_DATA_BUNDLE)
                {
                  m_realStartDataLogger  (iter->m_segments.front ()->GetSize ());
                }
              else
                {
                  CanonicalBundleHeader cbheader = iter->m_bundle->GetCanonicalHeaders ().front ();
                  m_realStartRouterLogger (iter->m_segments.front ()->GetSize (), (uint8_t) cbheader.GetBlockType ());
                }
                  
              m_startSegmentLogger (iter->m_segments.front ());
              NS_LOG_DEBUG(iter->m_destination <<" Socket SendTo2");
              m_socket->SendTo (iter->m_segments.front ()->Copy (),0, iter->m_destination);
            }
          else if (iter->m_cancelled)
            {
              ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Canceling a bundle to " << iter->m_mac << endl;
              
              uint32_t bytesSent = 0;
              if (iter->m_segments.empty ())
                {
                  uint32_t maxSegmentSize = m_netDevice->GetMtu () - 40;
                  uint32_t numSegments =  ceil (((double) iter->m_bundle->GetSize ()) / ((double) maxSegmentSize));
                  uint32_t lastSegmentSize = iter->m_bundle->GetSize () % maxSegmentSize;
                  bytesSent = iter->m_bundle->GetSize () + 40 * (numSegments - 1) + lastSegmentSize;
                  m_abortSegmentLogger (bytesSent, numSegments);
                }
              else 
                {
                  ConvergenceLayerHeader header;
                  iter->m_segments.front ()->PeekHeader (header);
                  uint32_t numSegments = header.GetNumberOfSegments () - iter->m_segments.size ();
                  uint32_t bytesSent =  numSegments * m_netDevice->GetMtu ();
                  m_abortSegmentLogger (bytesSent, numSegments);
                }    
              
              uint8_t type = iter->m_bundle->GetCanonicalHeaders ().front ().GetBlockType ();
              if (iter->m_sqeType == SQE_DATA_BUNDLE)
                {
                  m_abortDataLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);                  
                  m_realAbortDataLogger (bytesSent);
                }
              else
                {
                  m_abortRouterLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);
                  m_realAbortRouterLogger (bytesSent, (uint8_t) type);
                }
              
              if (m_waitingForAck == *iter)
                {
                  m_ackLogger (Seconds (0), true);
                  m_ackTimer.Cancel ();
                  m_waitingForAck = SendQueueElement ();
                }
              
              Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
              m_sendQueue.erase (iter);
            }
        }
    }
}


void
ConvergenceLayerAgent::SegmentSentOk ()
{
  NS_LOG_DEBUG ( "(" << m_node->GetId () << ")" << " ConvergenceLayerAgent::SegmentSentOk");
  ///cout << Simulator::Now ().GetSeconds () <<  " (" << m_node->GetId () << ")" << " ConvergenceLayerAgent::SegmentSentOk" << endl;

  SendQueue::iterator iter;
  if (m_current.m_sqeType == SQE_ACK)
    {
     iter = find (m_ackQueue.begin (), m_ackQueue.end (), m_current);
    }
  else
    {
      iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_current);
    }

  if (iter->m_sqeType == SQE_ACK)
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Succeeded in sending a ACK to " << iter->m_mac << endl;
      m_ackQueue.erase (iter);
    }
  else
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Succeeded in sending a Data segment to " << iter->m_mac << endl;
      iter->m_segments.pop_front ();      
      iter->ClearRetransmissions ();

      if (iter->m_segments.empty ())
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << " Uj uj! vill v�nta p� ack nu! " << endl;
          m_waitingForAck = *iter;
          //cout << "WTF is *iter" << endl;
          //cout << *iter << endl;
          m_ackTimer.SetFunction (&ConvergenceLayerAgent::BundleSentFailed, this);
          m_ackTimer.SetArguments (m_waitingForAck.m_mac, m_waitingForAck.m_gbid, true);
          m_ackTimer.Schedule (m_ackWaitTime);
          m_started = Simulator::Now ();
        }
    }
  m_isSending = false;
  Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
}

void
ConvergenceLayerAgent::SegmentSentFailed ()
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::SegmentSentFailed");

  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::SegmentSentFailed" << endl;
  SendQueue::iterator iter;
  if (m_current.m_sqeType == SQE_ACK)
    {
     iter = find (m_ackQueue.begin (), m_ackQueue.end (), m_current);
     if (iter == m_ackQueue.end ())
       {
         //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Dammit! could not find the ack ack" << endl;
       }
    }
  else
    {
      iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_current);
      if (iter == m_sendQueue.end ())
        {
          //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Dammit! could not find the bundle" << endl;
        }
    }

  m_abortSegmentLogger (iter->m_segments.front ()->GetSize (), 1);

  if (iter->m_retransmissions == 3)
    {
      if (iter->m_sqeType == SQE_ACK)
        {
          //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Failed in sending a ACK to " << iter->m_mac << endl;
          m_ackQueue.erase (iter);
        }
      else
       {
         //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Failed in sending a Data segment to " << iter->m_mac << endl;
         BundleSentFailed (iter->m_mac, iter->m_gbid, false);
       }
      Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
    }
  else 
    {
      Retransmit ();
    }
  m_isSending = false;
}

void
ConvergenceLayerAgent::Retransmit ()
{
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::Retransmit" << endl;
  SendQueue::iterator iter;
  if (m_current.m_sqeType == SQE_ACK)
    {
     iter = find (m_ackQueue.begin (), m_ackQueue.end (), m_current);
    }
  else
    {
      iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_current);
    }

  if (iter->m_retransmissions == 0)
    {
      Simulator::Schedule (Seconds (0), &ConvergenceLayerAgent::SendSegments, this);
    }
  else if (iter->m_retransmissions == 1)
    {
      Simulator::Schedule (Seconds (0.05), &ConvergenceLayerAgent::SendSegments, this);
    }
  else if (iter->m_retransmissions == 2)
    {
      Simulator::Schedule (Seconds (0.1), &ConvergenceLayerAgent::SendSegments, this);
    }

    ++iter->m_retransmissions;
}

void
ConvergenceLayerAgent::BundleSentOk (const Mac48Address& address, GlobalBundleIdentifier gbid, bool finalDelivery)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" << " ConvergenceLayerAgent::BundleSentOk");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::BundleSentOk" << endl;


  if (!m_waitingForAck.IsNull () && m_waitingForAck.m_gbid == gbid)
    {
      SendQueue::iterator iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_waitingForAck) ;
      if (iter != m_sendQueue.end ())
        {
          m_ackLogger (m_started, false);
          m_ackTimer.Cancel ();
          m_waitingForAck = SendQueueElement ();
          m_sendQueue.erase (iter);
        }
    }
  else
    {
      //cout << "JUST IT!" << endl;
    }
  
  Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
  
  if (!m_bundleSentOkCb.IsNull ())
    {
      m_bundleSentOkCb (address, gbid, finalDelivery);
    }
}

void
ConvergenceLayerAgent::BundleSentFailed (const Mac48Address& address, GlobalBundleIdentifier gbid, bool timeout)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" << "ConvergenceLayerAgent::BundleSentFailed");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::BundleSentFailed" << endl;
  if (timeout)
    {
      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Failed to send a Bundle to " << address << " due to ack timeout " << endl;
      if (!m_waitingForAck.IsNull () && m_waitingForAck.m_gbid == gbid)
        {
          SendQueue::iterator iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_waitingForAck) ;
          if (iter != m_sendQueue.end ())
            {
              // Found the bundle in the queue.
              m_ackLogger (Seconds (0), true);
              m_ackTimer.Cancel ();
              m_waitingForAck = SendQueueElement ();

              if (!iter->m_cancelled)
                {
                  uint32_t bytesSent = 0;
                  if (iter->m_segments.empty ())
                    {
                      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Segments is empty" << endl;
                      uint32_t maxSegmentSize = m_netDevice->GetMtu () - 40;
                      uint32_t numSegments =  ceil (((double) iter->m_bundle->GetSize ()) / ((double) maxSegmentSize));
                      uint32_t lastSegmentSize = iter->m_bundle->GetSize () % maxSegmentSize;
                      bytesSent = iter->m_bundle->GetSize () + 40 * (numSegments - 1) + lastSegmentSize;
                      m_abortSegmentLogger (bytesSent, numSegments);
                    }
                  else 
                    {
                      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Segments is not empty" << endl;
                      ConvergenceLayerHeader header;
                      iter->m_segments.front ()->PeekHeader (header);
                      uint32_t numSegments = header.GetNumberOfSegments () - iter->m_segments.size ();
                      uint32_t bytesSent =  numSegments * m_netDevice->GetMtu () + iter->m_segments.front ()->GetSize ();
                      m_abortSegmentLogger (bytesSent, numSegments+1);
                    }

                  uint8_t type = iter->m_bundle->GetCanonicalHeaders ().front ().GetBlockType ();
                  if (iter->m_sqeType == SQE_DATA_BUNDLE)
                    {
                      m_abortDataLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);
                      m_realAbortDataLogger (bytesSent);
                    }
                  else if (iter->m_sqeType == SQE_ROUTER_BUNDLE)
                    {
                      m_abortRouterLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);
                      m_realAbortRouterLogger (bytesSent, type);
                    }
                  
                  m_sendQueue.erase (iter);
                  
                  if (!m_bundleSentFailedCb.IsNull ())
                    {
                      m_bundleSentFailedCb (address, gbid);
                    }
                }
            }
          else
            {
              // Could not find it in the queue i.e the bundle has been removed due to
              // cancellation of the transmission for some reason, so i do nothing here
              // the logging of aborted bytes and such has been done when it was cancelled.
            }
        }
    }
  else
    {
      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Failed to send a Bundle to " << address << " due to no more retransmissions" << endl;
      
      SendQueue::iterator iter;
      iter = find (m_sendQueue.begin (), m_sendQueue.end (), m_current);
      

      if (iter != m_sendQueue.end () && !iter->m_cancelled)
        {
          uint32_t bytesSent = 0;
          
          if (iter->m_segments.empty ())
            {
              //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Segments is empty" << endl;
              uint32_t maxSegmentSize = m_netDevice->GetMtu () - 40;
              uint32_t numSegments =  ceil (((double) iter->m_bundle->GetSize ()) / ((double) maxSegmentSize));
              uint32_t lastSegmentSize = iter->m_bundle->GetSize () % maxSegmentSize;
              bytesSent = iter->m_bundle->GetSize () + 40 * (numSegments - 1) + lastSegmentSize;
              m_abortSegmentLogger (bytesSent, numSegments);
            }
          else 
            {
              //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Segments is not empty" << endl;
              ConvergenceLayerHeader header;
              iter->m_segments.front ()->PeekHeader (header);
              uint32_t numSegments = header.GetNumberOfSegments () - iter->m_segments.size ();
              uint32_t bytesSent =  numSegments * m_netDevice->GetMtu () + iter->m_segments.front ()->GetSize ();
              m_abortSegmentLogger (bytesSent, numSegments+1);
            }

          uint8_t type = iter->m_bundle->GetCanonicalHeaders ().front ().GetBlockType ();
          if (iter->m_sqeType == SQE_DATA_BUNDLE)
            {
              m_abortDataLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);
              m_realAbortDataLogger (bytesSent);
            }
          else if (iter->m_sqeType == SQE_ROUTER_BUNDLE)
            {
              m_abortRouterLogger ((iter->m_bundle->GetSize () - iter->m_bundle->GetPayload ()->GetSize ()), iter->m_bundle->GetPayload ()->GetSize (), type);
              m_realAbortRouterLogger (bytesSent, type);
            }
  
          m_sendQueue.erase (iter);
      
          if (!m_bundleSentFailedCb.IsNull ())
            {
              m_bundleSentFailedCb (address, gbid);
            }
        }
      else
        {
          // This is the same case as above for timeout. The "bundle" could not be found
          // in the queue and hence it has already been cancelled.
        }
    }
  Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
}

void
ConvergenceLayerAgent::CancelTransmission (GlobalBundleIdentifier gbid, BundleEndpointId to)
{
  // A null bundle endpoint id is interperted as that the transmission of the bundle
  // should be cancelled to all links, e.g when a bundle is deleted from the system.
  if (to == BundleEndpointId::GetAnyBundleEndpointId ())
    {
      for (SendQueue::iterator iter = m_sendQueue.begin (); iter != m_sendQueue.end (); ++iter)
        {
          if (iter->m_gbid == gbid && !iter->m_cancelled)
            {
              iter->m_cancelled = true;
          
              if (*iter == m_current)
                {
                  m_current = *iter;
                }
              
              if (*iter == m_waitingForAck)
                {
                  m_waitingForAck = *iter;
                }
              TransmissionCancelled (iter->m_mac, iter->m_gbid);
            }
        }
    }
  else
    {
      for (SendQueue::iterator iter = m_sendQueue.begin (); iter != m_sendQueue.end (); ++iter)
        {
          if ((iter->m_gbid == gbid) && (iter->m_toEid == to) && !iter->m_cancelled)
            {
              iter->m_cancelled = true;
          
              if (*iter == m_current)
                {
                  m_current = *iter;
                }
              
              if (*iter == m_waitingForAck)
                {
                  m_waitingForAck = *iter;
                }
              TransmissionCancelled (iter->m_mac, iter->m_gbid);
            }
        }
    }
}

void
ConvergenceLayerAgent::TransmissionCancelled (const Mac48Address& mac, GlobalBundleIdentifier gbid)
{
  if (!m_transmissionCancelledCb.IsNull ())
    {
      m_transmissionCancelledCb (mac, gbid);
    }
}


// Functionality for receiving

void
ConvergenceLayerAgent::ReceiveSegment (Ptr<Socket> socket)
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::ReceiveSegment");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "ConvergenceLayerAgent::ReceiveSegment" << endl;
  Address fromAddress;
  Ptr<Packet> receivedSegment = socket->RecvFrom (fromAddress);
  
  m_deliveredSegmentLogger (receivedSegment);

  PacketSocketAddress packetAddress = PacketSocketAddress::ConvertFrom (fromAddress);
  Mac48Address peerMac = Mac48Address::ConvertFrom (packetAddress.GetPhysicalAddress ());

  ConvergenceLayerHeader header;
  receivedSegment->PeekHeader (header);

  if (header.GetType () == CLA_SEGMENT)
    {
      // The received packet is an segment of a bundle
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Received a Data segement from " << peerMac << endl;
      uint16_t sequenceNumber = header.GetSequenceNumber ();
      
      SegmentsId segmentsId = SegmentsId (sequenceNumber, peerMac);
      
      map<SegmentsId, Segments>::iterator iter = m_recvQueue.find (segmentsId);
      
      if (iter != m_recvQueue.end ()) // One or more segments from this bundle already exsist
        {
          Segments::iterator it = find_if (iter->second.begin (), iter->second.end (), MatchingSegment (header.GetSequenceNumber (), header.GetSegmentNumber ()));

          if (it == iter->second.end ())
            {
              // Add it to the exsisting list of segments, since it has not previously been added.
              iter->second.push_back (receivedSegment);
              
              // Test if the full bundle has been received
              if (iter->second.size () == header.GetNumberOfSegments ())
                {
                  Simulator::ScheduleNow (&ConvergenceLayerAgent::ReassembleBundle, this, segmentsId);
                }
            }
        } 
      else // No previous segment of this bundle has been received
        {
          Segments tmp;
          tmp.push_back (receivedSegment);
          // Insert a new entry for this bundle in the receive queue
          m_recvQueue.insert (pair<SegmentsId, Segments> (segmentsId, tmp)); 
          
          // Check if there are more segments, or if the bundle is only one segment. 
          // If this is the only segment is then the full bundle has been received.
          if (header.GetStartFlag () && header.GetEndFlag ())
            {
              Simulator::ScheduleNow (&ConvergenceLayerAgent::ReassembleBundle, this, segmentsId);
            }
        }
    }
  else
    {
      // The received packet is an ack
      receivedSegment->RemoveHeader (header);
      uint8_t *buffer = new uint8_t[receivedSegment->GetSize ()];
      receivedSegment->CopyData (buffer, receivedSegment->GetSize ());
      GlobalBundleIdentifier ackedGbid = GlobalBundleIdentifier::Deserialize (buffer);

      if (!m_waitingForAck.IsNull () && m_waitingForAck.m_gbid == ackedGbid)
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Received a ACK from " << peerMac << " and I was waiting for it!" << endl;
          if (header.GetResponse () == FINAL_DELIVERY_SUCCEEDED)
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Final delivery ack" << endl;
              BundleSentOk (peerMac, ackedGbid, true);
            }
          else if (header.GetResponse () == ACK_SUCCEEDED)
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Forward ack" << endl;
              BundleSentOk (peerMac, ackedGbid, false);
            }
          else
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Negative ack" << endl;
              BundleSentFailed (peerMac, ackedGbid, false);
            }
        }
      else
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Received a ACK from " << peerMac << " but are no longer waiting for it" << endl;
          if (header.GetResponse () == FINAL_DELIVERY_SUCCEEDED)
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Final delivery ack" << endl;
              BundleSentOk (peerMac, ackedGbid, true);
            }
          else if (header.GetResponse () == ACK_SUCCEEDED)
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Forward ack" << endl;
              BundleSentOk (peerMac, ackedGbid, false);
            }
          else
            {
              //cout << Simulator::Now ().GetSeconds ()  << " (" << m_node->GetId () << ") " << "Negative ack but have already signalt its fail" << endl;
            }
        }
      delete [] buffer;
    }
}

void
ConvergenceLayerAgent::ReassembleBundle (SegmentsId segmentsId)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ")" <<" ConvergenceLayerAgent::ReassembleBundle");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Received a Bundle from " << segmentsId.m_source << endl;
  map<SegmentsId, Segments>::iterator i = m_recvQueue.find (segmentsId);

  Segments segments = i->second;
  segments.sort (SortBySegmentNumber ());
  Ptr<Packet> bundle = *(segments.begin ());

  bundle->RemoveAllPacketTags ();
  segments.erase (segments.begin ());

  uint32_t size = bundle->GetSize ();
  
  ConvergenceLayerHeader tmp;
  bundle->RemoveHeader (tmp);

  for (Segments::iterator iter = segments.begin (); iter != segments.end (); ++iter)
    {
      Ptr<Packet> segment = *iter;
      size += segment->GetSize ();
      segment->RemoveAllPacketTags ();
      segment->RemoveHeader (tmp);
      bundle->AddAtEnd (segment);
    }

  m_recvQueue.erase (segmentsId);
  
  Ptr<Bundle> b = Create<Bundle> (bundle);

  uint8_t type = b->GetCanonicalHeaders ().front ().GetBlockType ();
  if (type == 1)
    {
      m_realRelayLogger (size);
    }
  else
    {
      m_realRouterDeliveryLogger (size, type);
    }

  // FIXME: Here I want to have access to either the destination mac of
  // the bundle or my own eid, so that I can see if I am the final destination
  // or not.
  Ptr<NeighbourhoodDetectionAgent> nda = m_node->GetObject<BundleProtocolAgent> ()->GetBundleRouter ()->GetNeighbourhoodDetectionAgent ();
  BundleEndpointId myEid = nda->GetBundleEndpointId ();
  ///cout << "myEid = " << myEid << endl;
  ///cout << "Destination eid = " << b->GetDestinationEndpoint () << endl;
  if (b->GetDestinationEndpoint () == myEid && type == 1)
    {
	  NS_LOG_DEBUG("(" << m_node->GetId() << ") FINAL_DELIVERY_SUCCEEDED TO " << b->GetCustodianEndpoint());
      SendAck (FINAL_DELIVERY_SUCCEEDED, b, segmentsId.m_source);
    }
  else
    {
	  NS_LOG_DEBUG("(" << m_node->GetId() << ") ACK_SUCCEEDED TO " << b->GetCustodianEndpoint());
      SendAck (ACK_SUCCEEDED, b, segmentsId.m_source);
    }

  b->AddReceivedFrom (EidAddress (b->GetCustodianEndpoint (), segmentsId.m_source));
  m_bundleRecvCb (b);
}

void
ConvergenceLayerAgent::BundleReceived (Ptr<Bundle> bundle)
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" << " ConvergenceLayerAgent::BundleReceived");
  if (!m_bundleRecvCb.IsNull ())
    {
      m_bundleRecvCb (bundle);
    }
}

void
ConvergenceLayerAgent::SendAck (AckResponse response, Ptr<Bundle> bundle, const Mac48Address& to)
{
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << " I want to send an ACK!" << endl;
  PacketSocketAddress destAddr;
  destAddr.SetSingleDevice (m_netDevice->GetIfIndex ());
  destAddr.SetPhysicalAddress  (to);
  destAddr.SetProtocol (200);

  GlobalBundleIdentifier gbid = bundle->GetBundleId ();
  uint8_t buffer[gbid.GetSerializedSize ()];
  gbid.Serialize (buffer);

  Ptr<Packet> ack = Create<Packet> (buffer, gbid.GetSerializedSize ());
  ConvergenceLayerHeader claHeader;
  claHeader.SetType (CLA_ACK);
  claHeader.SetResponse (response);

  ack->AddHeader (claHeader);
  
  Segments segments;
  segments.push_back (ack);

  SendQueueElement el = SendQueueElement ();
  el.m_mac = to;
  el.m_bundle = bundle;
  el.m_gbid = bundle->GetBundleId ();
  el.m_destination = destAddr;
  el.m_segments = segments;
  el.m_sqeType = SQE_ACK;
  m_ackQueue.push_back (el);

  Simulator::ScheduleNow (&ConvergenceLayerAgent::SendSegments, this);
}

void
ConvergenceLayerAgent::LinkLost (Address address)
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<" ConvergenceLayerAgent::LinkLost");
  Mac48Address mac = Mac48Address::ConvertFrom (address);

  RemoveOrphanedSegments (mac);
}

void 
ConvergenceLayerAgent::RemoveOrphanedSegments (Mac48Address mac)
{
  NS_LOG_DEBUG ( " (" << m_node->GetId () << ")" <<"ConvergenceLayerAgent::RemoveOrphanedSegments");
  map<SegmentsId, Segments>::iterator iter = m_recvQueue.begin ();
  while (iter != m_recvQueue.end ())
    {
      if (iter->first.m_source == mac)
        {
          m_recvQueue.erase (iter++);
        }
      else
        ++iter;
    }
}

}} // namespace bundleProtocol, ns3 
