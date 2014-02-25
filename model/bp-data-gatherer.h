/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DATA_GATHERER_H
#define DATA_GATHERER_H

#include <stdint.h>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>

#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/node.h"
#include "ns3/system-mutex.h"

#include "bp-bundle.h"
#include "bp-header.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-bundle-router.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

typedef string TrafficFile;
typedef string MobilityFile;

typedef vector<TrafficFile> TrafficFiles;
typedef vector<MobilityFile> MobilityFiles;

typedef pair<double,double> GnuplotData;

typedef deque<Time> LatencyList;
typedef map<uint8_t, uint32_t> IntPerUtility;
typedef map<uint8_t, LatencyList> LatenciesPerUtility;

typedef pair<Bundle,Time> blPair;
typedef deque<blPair> DeliveredList;

typedef pair<uint32_t,BundleList> BundlesLeft;
typedef deque<BundlesLeft> BundlesLeftList;

typedef pair<uint32_t, uint32_t> HourContactsPair;
typedef deque<HourContactsPair> ContactsPerHourList;
  
//           nodeId    time
typedef pair<uint32_t, Time> NodeTimePair;
typedef deque<NodeTimePair> NtpList;
typedef map<uint32_t, NtpList> NodeContactList;

struct CwEntry
{
  CwEntry (uint32_t nodeId, uint32_t toNodeId, Time at, Time Cw)
    : m_nodeId (nodeId), 
      m_toNodeId (toNodeId), 
      m_at (at),
      m_cw (Cw),
      m_timeLeft (Seconds (0)),
      m_closed (Seconds (0)),
      m_fin (false)
  {}

  bool operator< (const CwEntry& cwe) const
  {
    return m_at < cwe.m_at;
  }

  bool operator == (const CwEntry& cwe) const
  {
    return m_nodeId == cwe.m_nodeId && m_toNodeId == cwe.m_toNodeId && m_at == cwe.m_at && m_cw == cwe.m_cw;
  }

  bool operator != (const CwEntry& cwe) const
  {
    return ! (*this == cwe);
  }

  uint32_t m_nodeId;
  uint32_t m_toNodeId;
  Time m_at;
  Time m_cw;
  Time m_timeLeft;
  Time m_closed;
  bool m_fin;
};

struct Matching : public unary_function <CwEntry, bool>
{
  Time m_at;
  uint32_t m_toNodeId;

  Matching (Time at, uint32_t toNodeId)
    : m_at (at), m_toNodeId (toNodeId)
  {}

  bool operator() (CwEntry cwe)
  {
    if (m_toNodeId == cwe.m_toNodeId)
      {
        return m_at <= cwe.m_at && ((cwe.m_at - m_at) < Seconds (2));
      }
    else
      {
        return false;
      }
  }
};

struct LinkKey 
{
  LinkKey (uint32_t nodeId, uint32_t toNodeId)
    : m_nodeId (nodeId), m_toNodeId (toNodeId)
  {}
  uint32_t m_nodeId;
  uint32_t m_toNodeId;

  bool operator< (const LinkKey& lk) const
  {
    if (m_nodeId == lk.m_nodeId)
      {
        return m_toNodeId < lk.m_toNodeId;
      }
    else
      {
        return m_nodeId < lk.m_nodeId;
      }

  }

  bool operator == (const LinkKey& lk) const
  {
    return m_nodeId == lk.m_nodeId && m_toNodeId == lk.m_toNodeId;
  }

  bool operator != (const LinkKey& lk) const
  {
    return !(*this == lk);
  }
};

typedef deque<CwEntry> CwList;
typedef map<LinkKey, CwEntry> OpenContactWindowList;
typedef map<uint32_t, CwList> NodeContactWindowList;

struct PacketEntry
{
  PacketEntry (uint32_t nodeId, uint32_t toNodeId, uint32_t startUid, uint32_t endUid, const GlobalBundleIdentifier& gbid, Time estimated)
    : m_type (0), 
      m_nodeId (nodeId), 
      m_toNodeId (toNodeId), 
      m_startUid (startUid),
      m_endUid (endUid),
      m_gbid (gbid),
      m_start (Simulator::Now ()),
      m_estimatedTime (estimated)
  {}

  PacketEntry (uint32_t nodeId, uint32_t uid)
    : m_type (1),
      m_nodeId (nodeId), 
      m_toNodeId (nodeId), 
      m_startUid (uid),
      m_endUid (uid),
      m_gbid (),
      m_start (Simulator::Now ()),
      m_estimatedTime ()
  {}

  bool operator< (const PacketEntry& pe) const
  {
    if (m_nodeId == pe.m_nodeId)
      {
        return m_start < pe.m_start;
      }
    else
      {
        return m_nodeId < pe.m_nodeId;
      }
  }

  bool operator == (const PacketEntry& pe) const
  {
    if (m_type == 0)
      {
        return m_nodeId == pe.m_nodeId && m_toNodeId == pe.m_toNodeId && m_startUid == pe.m_startUid && m_endUid == pe.m_endUid && m_gbid == pe.m_gbid && m_start == pe.m_start;
      }
    else
      {
        return m_nodeId == pe.m_nodeId && m_toNodeId == pe.m_toNodeId && m_startUid == pe.m_startUid && m_endUid == pe.m_endUid && m_start == pe.m_start;
      }
  }

  bool operator != (const PacketEntry& pe) const
  {
    return ! (*this == pe);
  }
  
  uint32_t m_type;
  uint32_t m_nodeId;
  uint32_t m_toNodeId;
  uint32_t m_startUid;
  uint32_t m_endUid;
  GlobalBundleIdentifier m_gbid;
  Time m_start;
  Time m_estimatedTime;
};

struct PacketKey 
{
  PacketKey (uint32_t nodeId, uint32_t toNodeId, uint32_t startUid, uint32_t endUid, const GlobalBundleIdentifier& gbid)
    : m_lk (nodeId, toNodeId), m_startUid (startUid), m_endUid (endUid),  m_gbid (gbid)
  {}
  
 PacketKey (uint32_t nodeId, uint32_t uid)
    : m_lk (nodeId, nodeId), m_startUid (uid), m_endUid (uid),  m_gbid ()
  {}

  LinkKey m_lk;
  uint32_t m_startUid;
  uint32_t m_endUid;
  GlobalBundleIdentifier m_gbid;

  bool operator< (const PacketKey& pk) const
  {
    return m_lk < pk.m_lk;  
  }

  bool operator == (const PacketKey& pk) const
  {
    return m_lk == pk.m_lk && m_gbid == pk.m_gbid && m_startUid == pk.m_startUid && m_endUid == pk.m_endUid;
  }

  bool operator != (const PacketKey& pk) const
  {
    return ! (*this == pk);
  }
};

typedef map<PacketKey, PacketEntry> CurrentTransmissionsList;

/**
 * \ingroup utility
 *
 * \brief utility class used to gatherer simulation data.
 *
 *
 */

class DataGatherer 
{
public:
  DataGatherer (string experimentName, uint32_t nNodes);
  DataGatherer (string experimentName, uint32_t nNodes, int point, int mobility);
  ~DataGatherer ();
  void Created (Ptr<const Bundle> bundle);
  void Delivered (Ptr<const Bundle> bundle);
  void Relayed (Ptr<const Bundle> bundle);
  void RealRelayed (uint32_t size);
  void RedundantRelay (Ptr<const Bundle> bundle);
  void Removed (Ptr<const Bundle> bundle, bool dropped);
  void StartedData (Ptr<const Bundle> bundle);
  void RealStartedData (uint32_t realStarted);
  void AbortedData (uint32_t headerSize, uint32_t payloadSize , uint8_t  utility);
  void RealAbortedData (uint32_t realAborted);
  void CreatedRouter (Ptr<const Bundle> bundle);
  void RemovedRouter (Ptr<const Bundle> bundle, uint8_t reason);
  void StartedRouter (Ptr<const Bundle> bundle);
  void RealStartedRouter (uint32_t realStarted, uint8_t type);
  void AbortedRouter (uint32_t headerSize, uint32_t payloadSize, uint8_t type);
  void RealAbortedRouter (uint32_t realAborted, uint8_t type);
  void DeliveredRouter (Ptr<const Bundle> bundle);
  void RealDeliveredRouter (uint32_t size, uint8_t type);
  void Contacts (uint32_t notUsed);
  void ContactOutOfSynch (uint32_t nodeId);
  void LeftInQueue (uint32_t nodeId, BundleList bundles);
  void Hello (Ptr<const Packet> packet);
  void HelloReceived (Ptr<const Packet> packet);

  void StartedSegment (Ptr<const Packet> segment);
  void AbortedSegment (uint32_t size, uint32_t numSegments);
  void DeliveredSegment (Ptr<const Packet> segment);

  void EstimatedContactWindow (uint32_t nodeId, uint32_t toNodeId, Time cw, bool cwMax);
  void ContactClosedBeforeCw (uint32_t nodeId, uint32_t toNodeId, Time timeLeft, bool cwMax);

  void ContactFailed (uint32_t nodeId);

  void ContactOpportunity (uint32_t notUsed);
  void ContactOpportunityBetween (uint32_t nodeId, uint32_t fromNodeId);

  void Cancelled (Ptr<const Bundle> bundle);

  void Ack (Time started, bool timeout);

  void SendBundle (uint32_t from, uint32_t to, uint32_t startUid, uint32_t endUid, GlobalBundleIdentifier gbid, Time estimatedTime);
  void SendBundleFailed (uint32_t from, uint32_t to, GlobalBundleIdentifier gbid);
  void SendHello (uint32_t from, uint32_t uid);

  void PhyTxBegin (Ptr<const Packet> packet);
  void PhyTxEnd (Ptr<const Packet> packet);

  void Finish ();
  void GenerateResult ();

  bool operator!= (const DataGatherer& other) const
  {
    return true;
  }

  uint32_t m_created;
  IntPerUtility m_createdPerUtility;
  uint32_t m_createdUtility;
  uint32_t m_createdSize;
  uint32_t m_createdSizeWith;

  uint32_t m_delivered;
  IntPerUtility m_deliveredPerUtility;
  uint32_t m_deliveredUtility;
  LatencyList m_latencies; 
  LatenciesPerUtility m_latenciesPerUtility;
  uint32_t m_deliveredSize; 
  uint32_t m_deliveredSizeWith; 
  DeliveredList m_deliveredList;

  uint32_t m_relayed;
  IntPerUtility m_relayedPerUtility;
  uint32_t m_relayedUtility;
  uint32_t m_relayedSize;
  uint32_t m_relayedSizeWith;
  uint32_t m_relayedSizeReal;

  uint32_t m_redundantRelay;
  IntPerUtility m_redundantRelayPerUtility;
  uint32_t m_redundantRelayUtility;
  uint32_t m_redundantRelaySize;
  uint32_t m_redundantRelaySizeWith;
  uint32_t m_redundantRelaySizeReal;

  uint32_t m_dropped;
  IntPerUtility m_droppedPerUtility;
  uint32_t m_droppedUtility;
  uint32_t m_droppedSize;
  uint32_t m_droppedSizeWith;

  uint32_t m_neverReplicated;
  IntPerUtility m_neverReplicatedPerUtility;
  uint32_t m_neverReplicatedUtility;
  uint32_t m_neverReplicatedSize;
  uint32_t m_neverReplicatedSizeWith;

  uint32_t m_removed;
  IntPerUtility m_removedPerUtility;
  uint32_t m_removedUtility;
  uint32_t m_removedSize;
  uint32_t m_removedSizeWith;

  uint32_t m_startedData;
  IntPerUtility m_startedDataPerUtility;
  uint32_t m_startedDataUtility;
  uint32_t m_startedDataSize;
  uint32_t m_startedDataSizeWith;
  uint32_t m_startedDataSizeReal;

  uint32_t m_abortedData;
  IntPerUtility m_abortedDataPerUtility;
  uint32_t m_abortedDataUtility; 
  uint32_t m_abortedDataSize;
  uint32_t m_abortedDataSizeWith;
  uint32_t m_abortedDataSizeReal;

  uint32_t m_createdRouter;
  uint32_t m_createdRouterSize;
  uint32_t m_createdRouterSizeWith;

  uint32_t m_routerRemoved;
  uint32_t m_routerFailedRemoved;
  uint32_t m_routerCancelledRemoved;
  uint32_t m_routerDropped;

  uint32_t m_kdmRemoved;
  uint32_t m_kdmFailedRemoved;
  uint32_t m_kdmCancelledRemoved;
  uint32_t m_kdmDropped;

  uint32_t m_cwiRemoved;
  uint32_t m_cwiFailedRemoved;
  uint32_t m_cwiCancelledRemoved;
  uint32_t m_cwiDropped;

  uint32_t m_startedRouter;
  uint32_t m_startedRouterSize;
  uint32_t m_startedRouterSizeWith;
  uint32_t m_startedRouterSizeReal;

  uint32_t m_abortedRouter;
  uint32_t m_abortedRouterSize;
  uint32_t m_abortedRouterSizeWith;
  uint32_t m_abortedRouterSizeReal;

  uint32_t m_deliveredRouter;
  uint32_t m_deliveredRouterSize;
  uint32_t m_deliveredRouterSizeWith;
  uint32_t m_deliveredRouterSizeReal;

  uint32_t m_createdKdm;
  uint32_t m_createdKdmSize;
  uint32_t m_createdKdmSizeWith;

  uint32_t m_startedKdm;
  uint32_t m_startedKdmSize;
  uint32_t m_startedKdmSizeWith;
  uint32_t m_startedKdmSizeReal;

  uint32_t m_abortedKdm;
  uint32_t m_abortedKdmSize;
  uint32_t m_abortedKdmSizeWith;
  uint32_t m_abortedKdmSizeReal;

  uint32_t m_deliveredKdm;
  uint32_t m_deliveredKdmSize;
  uint32_t m_deliveredKdmSizeWith;
  uint32_t m_deliveredKdmSizeReal;

  uint32_t m_createdCwi;
  uint32_t m_createdCwiSize;
  uint32_t m_createdCwiSizeWith;

  uint32_t m_startedCwi;
  uint32_t m_startedCwiSize;
  uint32_t m_startedCwiSizeWith;
  uint32_t m_startedCwiSizeReal;

  uint32_t m_abortedCwi;
  uint32_t m_abortedCwiSize;
  uint32_t m_abortedCwiSizeWith;
  uint32_t m_abortedCwiSizeReal;

  uint32_t m_deliveredCwi;
  uint32_t m_deliveredCwiSize;
  uint32_t m_deliveredCwiSizeWith;
  uint32_t m_deliveredCwiSizeReal;

  uint32_t m_cancelledData;
  uint32_t m_cancelledRouter;
  uint32_t m_cancelledKdm;
  uint32_t m_cancelledCwi;
 
  uint32_t m_contacts;
  uint32_t m_contactsOutOfSynch;

  uint32_t m_failedContacts;
  uint32_t m_closedBeforeCwWith;
  uint32_t m_closedBeforeCwWithout;
  uint32_t m_cwMax;
  Time m_totEstimatedCwWith;
  Time m_totEstimatedCwWithout; 
  Time m_totTimeLeftWith;
  Time m_totTimeLeftWithout;
  uint64_t m_totEstimatedCwWithSec;
  uint64_t m_totEstimatedCwWithoutSec; 
  uint64_t m_totTimeLeftWithSec;
  uint64_t m_totTimeLeftWithoutSec;
  OpenContactWindowList m_openContactWindowList;
  NodeContactWindowList m_estimatedCwList;

  uint32_t m_hello;
  uint32_t m_helloSize;
  uint32_t m_helloRecv;

  BundlesLeftList m_leftInQueue;
  deque<Bundle> m_droppedList;

  uint64_t m_startedSegments;
  uint64_t m_startedSegmentsSize;
  uint64_t m_abortedSegments;
  uint64_t m_abortedTransmissions;
  uint64_t m_abortedSegmentsSize;
  uint64_t m_deliveredSegments;
  uint64_t m_deliveredSegmentsSize;

  uint32_t m_contactsOpp;
  uint32_t m_currentHour;
  uint32_t m_contactsCurrentHour;
  ContactsPerHourList m_contactsPerHour;
  NodeContactList m_nodeContactsOppList;

  uint32_t m_acks;
  uint32_t m_ackTimeouts;
  uint32_t m_ackSucc;
  LatencyList m_ackResponse;

  CurrentTransmissionsList m_currentTransmissions;
  LatencyList m_startDiffsBundle;
  LatencyList m_sendDiffsBundle;
  LatencyList m_startDiffsHello;
  
private:
  void Increase (uint8_t utility, IntPerUtility& ipu);
  uint8_t CalcReplicationFactor (Ptr<const Bundle> bundle) const;
  string m_measureFile;
  string m_rawBundlesFile;
  string m_cphFile;
  ofstream m_measure;
  ofstream m_rawBundles;
  ofstream m_cph;
  uint32_t m_nNodes;
};


}} // namespace bundleProtocol, ns3 

#endif /* DATA_GATHERER_H */
