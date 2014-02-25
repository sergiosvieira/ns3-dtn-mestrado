/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_CONVERGENCE_LAYER_AGENT_H
#define BP_CONVERGENCE_LAYER_AGENT_H

#include <map>
#include <set>
#include <list>
#include <deque>
#include <vector>
#include <functional>
#include <algorithm>

#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/tag.h"
#include "ns3/ipv4-address.h"
#include "ns3/header.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/socket.h"
#include "ns3/nstime.h"
#include "ns3/mac48-address.h"
#include "ns3/ref-count-base.h"
#include "ns3/traced-callback.h"
#include "ns3/data-rate.h"
#include "ns3/adhoc-wifi-mac.h"
#include "ns3/packet-socket-address.h"

#include "bp-link.h"
#include "bp-contact.h"
#include "bp-bundle.h"
#include "bp-convergence-layer-header.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

class Contact;
class Link;

typedef list<Ptr<Packet> > Segments;

struct SegmentsId
{
  uint16_t m_sequenceNumber;
  Mac48Address m_source;

  SegmentsId (uint16_t sequenceNumber, Mac48Address source)
    : m_sequenceNumber (sequenceNumber), m_source (source)
  {}
  
  bool operator== (const SegmentsId& segmentsId) const
  {
    return m_sequenceNumber == segmentsId.m_sequenceNumber && m_source == segmentsId.m_source;
  }
  
  bool operator< (const SegmentsId& segmentsId) const
  {
    if(m_sequenceNumber == segmentsId.m_sequenceNumber)
      {
        return m_source < segmentsId.m_source;
      }
    else
      {
        return m_sequenceNumber < segmentsId.m_sequenceNumber;
      }
  }
};


struct MatchingSegment : public unary_function <Ptr<Packet>, bool>
{
  uint32_t m_seqnum;
  uint32_t m_segnum;

  MatchingSegment (uint32_t seqnum, uint32_t segnum)
    : m_seqnum (seqnum), m_segnum (segnum)
  {}

  bool operator() (Ptr<Packet> p)
  {
    ConvergenceLayerHeader header;
    p->PeekHeader (header); 
    
    if (header.GetType () == CLA_SEGMENT)
    {
      return (m_seqnum == header.GetSequenceNumber ()) && (m_segnum == header.GetSegmentNumber ());
    }
    else
      {
        cout << "This should not happen..., MatchingSegmentsId should only be used on Segments and not on acks" << endl;
        return false;
      }
  }
};

enum SendQueueElementType
{
  SQE_DATA_BUNDLE,
  SQE_ROUTER_BUNDLE,
  SQE_ACK,
  SQE_UNKOWN
};

struct SendQueueElement 
{
  SendQueueElement ()
    : m_mac (), m_bundle (), m_gbid (),  m_destination (), m_segments (), 
      m_retransmissions (0), m_cancelled (false), m_sqeType (SQE_UNKOWN)
  {}
  
  ~SendQueueElement ()
  {
    m_segments.clear ();
  }

  void ClearRetransmissions ()
  {
    m_retransmissions = 0;
  }

  bool IsNull ()
  {
    return m_sqeType == SQE_UNKOWN;
  }

  BundleEndpointId m_toEid;
  Mac48Address m_mac;
  Ptr<Bundle> m_bundle;
  GlobalBundleIdentifier m_gbid;
  PacketSocketAddress m_destination;
  Segments m_segments;
  uint32_t m_retransmissions;  
  bool m_cancelled;
  SendQueueElementType m_sqeType;

  friend ostream& operator<< (ostream& os, const SendQueueElement& sqe);
  bool operator == (const SendQueueElement& other)
  {
    return (m_sqeType == other.m_sqeType) && (m_mac == other.m_mac) && (m_gbid == other.m_gbid) && (m_toEid == other.m_toEid) ;
  }
};


typedef deque<SendQueueElement> SendQueue;

/**
 * \ingroup convergenceLayer
 *
 * \brief Handles the sending and receiving of bundles over the 802.11a mac layer.
 *
 *
 */

class ConvergenceLayerAgent : public Object
{
public:
  static TypeId GetTypeId (void);
  ConvergenceLayerAgent ();
  ConvergenceLayerAgent (Ptr<Node> node, Ptr<NetDevice> netDevice);
  ~ConvergenceLayerAgent();
  virtual void DoDispose ();

  void SetBundleReceivedCallback (Callback<void, Ptr<Bundle> > bundleRecvCb);
  void SetBundleSentOkCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier, bool> bundleSentOkCb);
  void SetBundleSentFailedCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier> bundleSentFailedCb);
  void SetTransmissionCancelledCallback (Callback<void, const Mac48Address&, GlobalBundleIdentifier > transmissionCancelledCb);

  void SetNode (Ptr<Node> node);
  Ptr<Node> GetNode () const;

  void SetNetDevice (Ptr<NetDevice> netDevice);
  Ptr<NetDevice> GetNetDevice () const;

  void SetTransmissionRange (uint32_t range);
  uint32_t GetTransmissionRange () const;
  void SetTransmissionSpeed (const DataRate& dataRate);
  DataRate GetTransmissionSpeed () const;

  void Init ();
  void Stop ();
  void LinkLost (Address address);

  void BundleQueued (Ptr<Link> link, Ptr<Bundle> bundle);
  
  void CancelTransmission (GlobalBundleIdentifier gbid, BundleEndpointId to);
  
  void HackFixme (uint32_t toNodeId, Time timeleft, bool cwMax); 
private:
  friend class Contact;
  
  void SendBundle (Ptr<Bundle> bundle, Ptr<Link> link);
  void SendSegments ();
  void SegmentSentOk ();
  void SegmentSentFailed ();
  
  void BundleSentOk (const Mac48Address& address, GlobalBundleIdentifier gbid, bool finalDelivery);
  void BundleSentFailed (const Mac48Address& address, GlobalBundleIdentifier gbid, bool timeout);

  void ReceiveSegment (Ptr<Socket> socket);
  void ReassembleBundle (SegmentsId segmentsId);
  void SendAck (AckResponse response, Ptr<Bundle> bundle, const Mac48Address& to);
  void BundleReceived (Ptr<Bundle> bundle);

  void TransmissionCancelled (const Mac48Address& address, GlobalBundleIdentifier gbid);
  uint16_t GetSequenceNumber ();

  void RemoveOrphanedSegments (Mac48Address mac);
  void Retransmit ();

  Ptr<Node> m_node;
  Ptr<NetDevice> m_netDevice;
  Ptr<Socket> m_socket;

  uint32_t m_range;
  DataRate m_dataRate;

  uint16_t m_sequenceNumber;
  bool m_isSending;
  SendQueue m_ackQueue;
  SendQueue m_sendQueue;
  SendQueueElement m_current;
  SendQueueElement m_waitingForAck;
  Timer m_ackTimer;
  Time m_ackWaitTime;

  map<SegmentsId, Segments> m_recvQueue;

  Callback<void, Ptr<Bundle> > m_bundleRecvCb;
  Callback<void, const Mac48Address&, GlobalBundleIdentifier, bool> m_bundleSentOkCb;
  Callback<void, const Mac48Address&, GlobalBundleIdentifier> m_bundleSentFailedCb;
  Callback<void, const Mac48Address&, GlobalBundleIdentifier > m_transmissionCancelledCb;

  // Traced callbacks used to log intresseting information
  TracedCallback<Ptr<const Bundle> > m_startDataLogger;
  TracedCallback<uint32_t> m_realStartDataLogger;
  TracedCallback<uint32_t, uint32_t, uint8_t> m_abortDataLogger;
  TracedCallback<uint32_t> m_realAbortDataLogger;
  TracedCallback<Ptr<const Bundle> > m_startRouterLogger;
  TracedCallback<uint32_t, uint8_t> m_realStartRouterLogger;
  TracedCallback<uint32_t, uint32_t, uint8_t> m_abortRouterLogger;
  TracedCallback<uint32_t, uint8_t> m_realAbortRouterLogger;

  TracedCallback<Ptr<const Packet> > m_startSegmentLogger;
  TracedCallback<uint32_t, uint32_t > m_abortSegmentLogger;
  TracedCallback<Ptr<const Packet> > m_deliveredSegmentLogger;
  TracedCallback<uint32_t> m_realRelayLogger;
  TracedCallback<uint32_t,uint8_t> m_realRouterDeliveryLogger;
  TracedCallback<Time, bool> m_ackLogger;
  Time m_started;
  
  //              From       To          The bundle     Estimated send time
  TracedCallback<uint32_t, uint32_t, Ptr<const Bundle>, Time> m_sendBundleLogger;

  //              From       To          The bundle  
  TracedCallback<uint32_t, uint32_t, Ptr<const Bundle> > m_startSendingBundleLogger;
  
  //              From       To          The bundle      Reason i.e 0 = failed, 1 = timeout, 2 = cancel, 3 = succeed
  TracedCallback<uint32_t, uint32_t, Ptr<const Bundle>, uint8_t> m_finishedSendingBundleLogger;

  TracedCallback<uint32_t, uint32_t, Time, bool> m_contactClosedLogger;
};

}} //namespace bundleProtocol, ns3

#endif /* BP_CONVERGENCE_LAYER_AGENT_H */
