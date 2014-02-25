/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-data-gatherer.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

DataGatherer::DataGatherer (string experimentName, uint32_t nNodes)
  :
 m_created (0), m_createdPerUtility (), m_createdUtility (0), m_createdSize (0), m_createdSizeWith (0),

    m_delivered (0), m_deliveredPerUtility (), m_deliveredUtility (0), m_latencies (),
    m_latenciesPerUtility (), m_deliveredSize (0), m_deliveredSizeWith (0), m_deliveredList (),

    m_relayed (0), m_relayedPerUtility (), m_relayedUtility (0), m_relayedSize (0), m_relayedSizeWith (0), m_relayedSizeReal (0),

    m_redundantRelay (0), m_redundantRelayPerUtility (), m_redundantRelayUtility (0), m_redundantRelaySize (0), m_redundantRelaySizeWith (0), m_redundantRelaySizeReal (0),

    m_dropped (0), m_droppedPerUtility (), m_droppedUtility (0), m_droppedSize (0), m_droppedSizeWith (0), m_neverReplicated (0), m_neverReplicatedPerUtility (),
    m_neverReplicatedUtility (0), m_neverReplicatedSize (0), m_neverReplicatedSizeWith (0),

    m_removed (0), m_removedPerUtility (), m_removedUtility (0), m_removedSize (0), m_removedSizeWith (0), 

    m_startedData (0), m_startedDataPerUtility (), m_startedDataUtility (0), m_startedDataSize (0), m_startedDataSizeWith (0),  m_startedDataSizeReal (0),

    m_abortedData (0), m_abortedDataPerUtility (), m_abortedDataUtility (0), m_abortedDataSize (0), m_abortedDataSizeWith (0), m_abortedDataSizeReal (0),

    m_createdRouter (0), m_createdRouterSize (0), m_createdRouterSizeWith (0),
   
    m_routerRemoved (0), m_routerFailedRemoved (0), m_routerCancelledRemoved (0), m_routerDropped (0),

    m_kdmRemoved (0), m_kdmFailedRemoved (0), m_kdmCancelledRemoved (0), m_kdmDropped (0),

    m_cwiRemoved (0), m_cwiFailedRemoved (0), m_cwiCancelledRemoved (0), m_cwiDropped (0),
 
    m_startedRouter (0), m_startedRouterSize (0), m_startedRouterSizeWith (0), m_startedRouterSizeReal (0),

    m_abortedRouter (0), m_abortedRouterSize (0), m_abortedRouterSizeWith (0), m_abortedRouterSizeReal (0),

    m_deliveredRouter (0), m_deliveredRouterSize (0), m_deliveredRouterSizeWith (0), m_deliveredRouterSizeReal (0),
    
    m_createdKdm (0), m_createdKdmSize (0), m_createdKdmSizeWith (0),

    m_startedKdm (0), m_startedKdmSize (0), m_startedKdmSizeWith (0), m_startedKdmSizeReal (0),
   
    m_abortedKdm (0), m_abortedKdmSize (0), m_abortedKdmSizeWith (0), m_abortedKdmSizeReal (0),
    
    m_deliveredKdm (0), m_deliveredKdmSize (0), m_deliveredKdmSizeWith (0), m_deliveredKdmSizeReal (0),
    
    m_createdCwi (0), m_createdCwiSize (0), m_createdCwiSizeWith (0),
    
    m_startedCwi (0), m_startedCwiSize (0), m_startedCwiSizeWith (0), m_startedCwiSizeReal (0),
    
    m_abortedCwi (0), m_abortedCwiSize (0), m_abortedCwiSizeWith (0), m_abortedCwiSizeReal (0),
    
    m_deliveredCwi (0), m_deliveredCwiSize (0), m_deliveredCwiSizeWith (0), m_deliveredCwiSizeReal (0),
    
    m_cancelledData (0), m_cancelledRouter (0), m_cancelledKdm (0), m_cancelledCwi (0),

    m_contacts (0), m_contactsOutOfSynch (0), m_failedContacts (0), 
    m_closedBeforeCwWith (0), m_closedBeforeCwWithout (0), m_cwMax (0), 
    m_totEstimatedCwWith (), m_totEstimatedCwWithout (), m_totTimeLeftWith (), m_totTimeLeftWithout (),
    m_totEstimatedCwWithSec (0), m_totEstimatedCwWithoutSec (0), m_totTimeLeftWithSec (0), m_totTimeLeftWithoutSec (0),
    m_openContactWindowList (), m_estimatedCwList (),
    
    m_hello (0), m_helloSize (0), m_helloRecv (0),
    m_leftInQueue (), m_droppedList (),
    m_startedSegments (0),  m_startedSegmentsSize (0),
    m_abortedSegments (0), m_abortedTransmissions (0),  m_abortedSegmentsSize (0),  m_deliveredSegments (0),  m_deliveredSegmentsSize (0),

    m_contactsOpp (0), m_currentHour (1), m_contactsCurrentHour (0), m_contactsPerHour (), m_nodeContactsOppList (),
    
    m_acks (0), m_ackTimeouts (0), m_ackSucc (0), m_ackResponse (),
    
    m_currentTransmissions (), m_startDiffsBundle (), m_sendDiffsBundle (), m_startDiffsHello (),
    
    m_measureFile (), m_rawBundlesFile (), m_cphFile (), m_measure (), m_rawBundles (), m_cph (),
    m_nNodes (nNodes)
{
  stringstream ss;
  stringstream ss2;
  stringstream ss3;
  ss << "Result-" << experimentName << ".txt";
  ss2 << "RawBundles-" << experimentName << ".txt";
  ss3 << "Contact-" << experimentName << ".txt";

  m_measureFile = ss.str ();
  m_rawBundlesFile = ss2.str ();
  m_cphFile = ss3.str ();
}

DataGatherer::DataGatherer (string experimentName, uint32_t nNodes, int point, int mobility)
  : m_created (0), m_createdPerUtility (), m_createdUtility (0), m_createdSize (0), m_createdSizeWith (0),

    m_delivered (0), m_deliveredPerUtility (), m_deliveredUtility (0), m_latencies (),
    m_latenciesPerUtility (), m_deliveredSize (0), m_deliveredSizeWith (0), m_deliveredList (),

    m_relayed (0), m_relayedPerUtility (), m_relayedUtility (0), m_relayedSize (0), m_relayedSizeWith (0), m_relayedSizeReal (0),

    m_redundantRelay (0), m_redundantRelayPerUtility (), m_redundantRelayUtility (0), m_redundantRelaySize (0), m_redundantRelaySizeWith (0), m_redundantRelaySizeReal (0),

    m_dropped (0), m_droppedPerUtility (), m_droppedUtility (0), m_droppedSize (0), m_droppedSizeWith (0), m_neverReplicated (0), m_neverReplicatedPerUtility (),
    m_neverReplicatedUtility (0), m_neverReplicatedSize (0), m_neverReplicatedSizeWith (0),

    m_removed (0), m_removedPerUtility (), m_removedUtility (0), m_removedSize (0), m_removedSizeWith (0), 

    m_startedData (0), m_startedDataPerUtility (), m_startedDataUtility (0), m_startedDataSize (0), m_startedDataSizeWith (0),  m_startedDataSizeReal (0),

    m_abortedData (0), m_abortedDataPerUtility (), m_abortedDataUtility (0), m_abortedDataSize (0), m_abortedDataSizeWith (0), m_abortedDataSizeReal (0),

    m_createdRouter (0), m_createdRouterSize (0), m_createdRouterSizeWith (0),
   
    m_routerRemoved (0), m_routerFailedRemoved (0), m_routerCancelledRemoved (0), m_routerDropped (0),

    m_kdmRemoved (0), m_kdmFailedRemoved (0), m_kdmCancelledRemoved (0), m_kdmDropped (0),

    m_cwiRemoved (0), m_cwiFailedRemoved (0), m_cwiCancelledRemoved (0), m_cwiDropped (0),
 
    m_startedRouter (0), m_startedRouterSize (0), m_startedRouterSizeWith (0), m_startedRouterSizeReal (0),

    m_abortedRouter (0), m_abortedRouterSize (0), m_abortedRouterSizeWith (0), m_abortedRouterSizeReal (0),

    m_deliveredRouter (0), m_deliveredRouterSize (0), m_deliveredRouterSizeWith (0), m_deliveredRouterSizeReal (0),
    
    m_createdKdm (0), m_createdKdmSize (0), m_createdKdmSizeWith (0),

    m_startedKdm (0), m_startedKdmSize (0), m_startedKdmSizeWith (0), m_startedKdmSizeReal (0),
   
    m_abortedKdm (0), m_abortedKdmSize (0), m_abortedKdmSizeWith (0), m_abortedKdmSizeReal (0),
    
    m_deliveredKdm (0), m_deliveredKdmSize (0), m_deliveredKdmSizeWith (0), m_deliveredKdmSizeReal (0),
    
    m_createdCwi (0), m_createdCwiSize (0), m_createdCwiSizeWith (0),
    
    m_startedCwi (0), m_startedCwiSize (0), m_startedCwiSizeWith (0), m_startedCwiSizeReal (0),
    
    m_abortedCwi (0), m_abortedCwiSize (0), m_abortedCwiSizeWith (0), m_abortedCwiSizeReal (0),
    
    m_deliveredCwi (0), m_deliveredCwiSize (0), m_deliveredCwiSizeWith (0), m_deliveredCwiSizeReal (0),
    
    m_cancelledData (0), m_cancelledRouter (0), m_cancelledKdm (0), m_cancelledCwi (0),

    m_contacts (0), m_contactsOutOfSynch (0), m_failedContacts (0), 
    m_closedBeforeCwWith (0), m_closedBeforeCwWithout (0), m_cwMax (0), 
    m_totEstimatedCwWith (), m_totEstimatedCwWithout (), m_totTimeLeftWith (), m_totTimeLeftWithout (),
    m_totEstimatedCwWithSec (0), m_totEstimatedCwWithoutSec (0), m_totTimeLeftWithSec (0), m_totTimeLeftWithoutSec (0),
    m_openContactWindowList (), m_estimatedCwList (),
    
    m_hello (0), m_helloSize (0), m_helloRecv (0),
    m_leftInQueue (), m_droppedList (),
    m_startedSegments (0),  m_startedSegmentsSize (0),
    m_abortedSegments (0), m_abortedTransmissions (0),  m_abortedSegmentsSize (0),  m_deliveredSegments (0),  m_deliveredSegmentsSize (0),

    m_contactsOpp (0), m_currentHour (1), m_contactsCurrentHour (0), m_contactsPerHour (), m_nodeContactsOppList (),
    
    m_acks (0), m_ackTimeouts (0), m_ackSucc (0), m_ackResponse (),
    
    m_currentTransmissions (), m_startDiffsBundle (), m_sendDiffsBundle (), m_startDiffsHello (),
    
    m_measureFile (), m_rawBundlesFile (), m_cphFile (), m_measure (), m_rawBundles (), m_cph (),
    m_nNodes (nNodes)
{
  stringstream ss;
  stringstream ss2;
  stringstream ss3;
  if (point != -1 && mobility != -1)
    {
      ss << "Result-" << experimentName << "[" << point << "," << mobility << "].txt";
      ss2 << "RawBundles-" << experimentName << "[" << point << "," << mobility << "].txt"; 
      ss3 << "Contact-" << experimentName << "[" << point << "," << mobility << "].txt";
    }
  else
    {
      ss << "Result-" << experimentName << ".txt";
      ss2 << "RawBundles-" << experimentName << ".txt";
      ss3 << "Contact-" << experimentName << ".txt";
    }

  m_measureFile = ss.str ();
  m_rawBundlesFile = ss2.str ();
  m_cphFile = ss3.str ();
}


DataGatherer::~DataGatherer ()
{
  m_measure.close ();
  m_rawBundles.close ();
  m_cph.close ();

  m_createdPerUtility.clear ();
  m_deliveredPerUtility.clear ();
  m_latencies.clear ();
  m_latenciesPerUtility.clear ();
  m_deliveredList.clear ();
  m_relayedPerUtility.clear ();
  m_redundantRelayPerUtility.clear ();
  m_droppedPerUtility.clear ();
  m_neverReplicatedPerUtility.clear ();
  m_removedPerUtility.clear ();
  m_startedDataPerUtility.clear ();
  m_abortedDataPerUtility.clear ();
  m_openContactWindowList.clear ();
  m_estimatedCwList.clear ();
  m_leftInQueue.clear ();
  m_droppedList.clear ();
  m_contactsPerHour.clear ();
  m_nodeContactsOppList.clear ();
  m_ackResponse.clear ();
  m_currentTransmissions.clear ();
  m_startDiffsBundle.clear ();
  m_sendDiffsBundle.clear ();
  m_startDiffsHello.clear ();
}

void
DataGatherer::SendBundle(uint32_t from, uint32_t to, uint32_t startUid, uint32_t endUid, GlobalBundleIdentifier gbid, Time estimatedTime)
{
  PacketEntry pe = PacketEntry (from, to, startUid, endUid, gbid, estimatedTime);
  PacketKey pk = PacketKey (from, to, startUid, endUid, gbid);
  m_currentTransmissions.insert (make_pair<PacketKey,PacketEntry> (pk,pe));
}

void
DataGatherer::SendBundleFailed (uint32_t from, uint32_t to, GlobalBundleIdentifier gbid)
{
  for (CurrentTransmissionsList::iterator iter = m_currentTransmissions.begin (); iter != m_currentTransmissions.end (); ++iter)
    {
      PacketKey pk = iter->first;
      if (pk.m_lk.m_nodeId == from && pk.m_lk.m_toNodeId == to && pk.m_gbid == gbid)
        {
          m_currentTransmissions.erase (iter);
          break;
        }
    }
}

void
DataGatherer::SendHello (uint32_t from, uint32_t uid)
{
  PacketEntry pe = PacketEntry (from, uid);
  PacketKey pk = PacketKey (from, uid);
  m_currentTransmissions.insert (make_pair<PacketKey,PacketEntry> (pk,pe));
}

void
DataGatherer::PhyTxBegin (Ptr<const Packet> packet)
{

}

void
DataGatherer::PhyTxEnd (Ptr<const Packet> packet)
{

}


void
DataGatherer::Created (Ptr<const Bundle> bundle)
{
  ++m_created;
  uint8_t utility = (uint8_t) bundle->GetUtility ();
  Increase (utility, m_createdPerUtility);
  m_createdUtility += utility;
  m_createdSizeWith += bundle->GetSize ();
  m_createdSize += bundle->GetPayload ()->GetSize ();
  m_droppedList.push_back (*bundle);
}

void
DataGatherer::Delivered (Ptr<const Bundle> bundle)
{
  ++m_delivered;
  uint8_t utility = (uint8_t) bundle->GetUtility ();
  Increase (utility, m_deliveredPerUtility);
  m_deliveredUtility += utility;
  Time latency = Simulator::Now () - bundle->GetCreationTimestamp ().GetTime ();
  m_latencies.push_back (latency);
  
  LatenciesPerUtility::iterator iter = m_latenciesPerUtility.find (utility);
 
  if (iter != m_latenciesPerUtility.end ())
    {
      iter->second.push_back (latency);
    }
  else
    {
      LatencyList ll;
      ll.push_back (latency);
      m_latenciesPerUtility.insert(make_pair<uint8_t, LatencyList> (utility, ll));
    }

  m_deliveredSizeWith += bundle->GetSize ();
  m_deliveredSize += bundle->GetPayload ()->GetSize ();

  m_deliveredList.push_back (make_pair<Bundle,Time> (*bundle, Simulator::Now ()));
}

void
DataGatherer::Relayed (Ptr<const Bundle> bundle)
{
  ++m_relayed;
  uint8_t utility = (uint8_t) bundle->GetUtility ();
  Increase (utility, m_relayedPerUtility);
  m_relayedUtility += utility;
  m_relayedSizeWith += bundle->GetSize ();
  m_relayedSize += bundle->GetPayload ()->GetSize ();

}

void
DataGatherer::RealRelayed (uint32_t size)
{
  m_relayedSizeReal += size;
}

void
DataGatherer::RedundantRelay (Ptr<const Bundle> bundle)
{
  ++m_redundantRelay;
  uint8_t utility = (uint8_t) bundle->GetUtility ();
  Increase (utility, m_redundantRelayPerUtility);
  m_redundantRelayUtility += utility;
  m_redundantRelaySizeWith += bundle->GetSize ();
  m_redundantRelaySize += bundle->GetPayload ()->GetSize ();

  uint16_t nFragments =  ceil (((double) bundle->GetSize ()) / ((double) 2304)); // This is a ugly? hack, 2304 is the MTU of a wifi netdevice
  m_redundantRelaySizeReal += bundle->GetSize () + (nFragments*48); // This is also a ugly? hack, 48 is the wifi header (34 Bytes) + convergence header (14 Bytes) = total of 48 Bytes of headers on each fragment.
}

void
DataGatherer::Removed (Ptr<const Bundle> bundle, bool dropped)
{
  if (dropped)
    {
      ++m_dropped;
      uint8_t utility = (uint8_t) bundle->GetUtility ();
      Increase (utility, m_droppedPerUtility);
      m_droppedUtility += utility;
      m_droppedSizeWith += bundle->GetSize ();
      m_droppedSize += bundle->GetPayload ()->GetSize ();

      if (bundle->GetReplicationFactor () == CalcReplicationFactor (bundle))
	{
	  ++m_neverReplicated;
	  uint8_t utility = (uint8_t) bundle->GetUtility ();
	  Increase (utility, m_neverReplicatedPerUtility);
	  m_neverReplicatedUtility += utility;
	  m_neverReplicatedSizeWith += bundle->GetSize ();
	  m_neverReplicatedSize += bundle->GetPayload ()->GetSize ();
	}
    }
  else
    {
      ++m_removed;
      uint8_t utility = (uint8_t) bundle->GetUtility ();
      Increase (utility, m_removedPerUtility);
      m_removedUtility += utility;
      m_removedSizeWith += bundle->GetSize ();
      m_removedSize += bundle->GetPayload ()->GetSize ();
    }
}

void
DataGatherer::StartedData (Ptr<const Bundle> bundle)
{
  ++m_startedData;
  uint8_t utility = (uint8_t) bundle->GetUtility ();
  Increase (utility, m_startedDataPerUtility);
  m_startedDataUtility += utility;
  m_startedDataSizeWith += bundle->GetSize ();
  m_startedDataSize += bundle->GetPayload ()->GetSize ();
}

void
DataGatherer::RealStartedData (uint32_t realStarted)
{
  m_startedDataSizeReal += realStarted;
}

void
DataGatherer::AbortedData (uint32_t headerSize, uint32_t payloadSize, uint8_t utility)
{
  ++m_abortedData;
  Increase (utility, m_abortedDataPerUtility);
  m_abortedDataUtility += utility;
  m_abortedDataSizeWith += headerSize + payloadSize;
  m_abortedDataSize += payloadSize;
}

void
DataGatherer::RealAbortedData (uint32_t realAborted)
{
  m_abortedDataSizeReal += realAborted;
}


void
DataGatherer::CreatedRouter (Ptr<const Bundle> bundle)
{
  ++m_createdRouter;
  m_createdRouterSizeWith += bundle->GetSize ();
  m_createdRouterSize += bundle->GetPayload ()->GetSize ();
  
  if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
    {
      ++m_createdKdm;
      m_createdKdmSizeWith += bundle->GetSize ();
      m_createdKdmSize += bundle->GetPayload ()->GetSize ();
    }
  else
    {
      ++m_createdCwi;
      m_createdCwiSizeWith += bundle->GetSize ();
      m_createdCwiSize += bundle->GetPayload ()->GetSize ();
    }
}


void
DataGatherer::RemovedRouter (Ptr<const Bundle> bundle, uint8_t reason)
{
  if (reason == 0)
    {
      ++m_routerRemoved;
      if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
        {
          ++m_kdmRemoved;
        }
      else
        {
          ++m_cwiRemoved;
        }
    }
  else if (reason == 1)
    {
      ++m_routerFailedRemoved;
      if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
        {
          ++m_kdmFailedRemoved;
        }
      else
        {
          ++m_cwiFailedRemoved;
        }     
    }
  else if (reason == 2)
    {
      ++m_routerCancelledRemoved;
      if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
        {
          ++m_kdmCancelledRemoved;
        }
      else
        {
          ++m_cwiCancelledRemoved;
        }
    }
  else if (reason == 3)
    {
      ++m_routerDropped;
      if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
        {
          ++m_kdmDropped;
        }
      else
        {
          ++m_cwiDropped;
        }
    }
}

void
DataGatherer::StartedRouter (Ptr<const Bundle> bundle)
{
  ++m_startedRouter;
  m_startedRouterSizeWith += bundle->GetSize ();
  m_startedRouterSize += bundle->GetPayload ()->GetSize ();
 
  if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
    {
      ++m_startedKdm;
      m_startedKdmSizeWith += bundle->GetSize ();
      m_startedKdmSize += bundle->GetPayload ()->GetSize ();
    }
  else
    {
      ++m_startedCwi;
      m_startedCwiSizeWith += bundle->GetSize ();
      m_startedCwiSize += bundle->GetPayload ()->GetSize ();
    }
}

void
DataGatherer::RealStartedRouter (uint32_t realStarted, uint8_t type)
{
  m_startedRouterSizeReal += realStarted;

  if (type == 192)
    {
      m_startedKdmSizeReal += realStarted;
    }
  else
    {
      m_startedCwiSizeReal += realStarted;
    }
}

void
DataGatherer::AbortedRouter (uint32_t headerSize, uint32_t payloadSize, uint8_t type)
{
  ++m_abortedRouter;
  m_abortedRouterSizeWith += headerSize + payloadSize;
  m_abortedRouterSize += payloadSize;

  if (type == 192)
    {
      ++m_abortedKdm;
      m_abortedKdmSizeWith += headerSize + payloadSize;
      m_abortedKdmSize += payloadSize;
    }
  else
    {
      ++m_abortedCwi;
      m_abortedCwiSizeWith += headerSize + payloadSize;
      m_abortedCwiSize += payloadSize;
    }
}

void
DataGatherer::RealAbortedRouter (uint32_t realAborted, uint8_t type)
{
   m_abortedRouterSizeReal += realAborted;

  if (type == 192)
    {
      m_abortedKdmSizeReal += realAborted;
    }
  else
    {
      m_abortedCwiSizeReal += realAborted;
    }
}

void
DataGatherer::RealDeliveredRouter (uint32_t size, uint8_t type)
{
  m_deliveredRouterSizeReal +=size;
  if (type == 192)
    {
      
      m_deliveredKdmSizeReal += size;
    }
  else
    {
      m_deliveredCwiSizeReal += size;
    }
}

void
DataGatherer::DeliveredRouter (Ptr<const Bundle> bundle)
{
  ++m_deliveredRouter;
  m_deliveredRouterSizeWith += bundle->GetSize ();
  m_deliveredRouterSize += bundle->GetPayload ()->GetSize ();

  if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
    {
      ++m_deliveredKdm;
      m_deliveredKdmSizeWith += bundle->GetSize ();
      m_deliveredKdmSize += bundle->GetPayload ()->GetSize ();
    }
  else
    {
      ++m_deliveredCwi;
      m_deliveredCwiSizeWith += bundle->GetSize ();
      m_deliveredCwiSize += bundle->GetPayload ()->GetSize ();
    }
}

void
DataGatherer::Contacts (uint32_t notUsed)
{
  ++m_contacts;
}

void
DataGatherer::ContactOutOfSynch (uint32_t notUsed)
{
  ++m_contactsOutOfSynch;
}

void
DataGatherer::Increase (uint8_t utility, IntPerUtility& ipu)
{
  IntPerUtility::iterator iter = ipu.find (utility);
 
  if (iter != ipu.end ())
    {
      iter->second = iter->second + 1;
    }
  else
    {
      ipu.insert(make_pair<uint8_t, uint32_t> (utility, 1));
    }
}

uint8_t
DataGatherer::CalcReplicationFactor (Ptr<const Bundle> bundle) const
{
  BundlePriority priority = bundle->GetPriority ();
   switch (priority)
    {
    case BULK:
      return 4;
      break;
    case NORMAL:
            return 6;
      break;
    case EXPEDITED:
            return 8;
      break;
    default:
      return 1;
    }
  return 1;
}

void
DataGatherer::Hello (Ptr<const Packet> packet)
{
  ++m_hello;
  m_helloSize += packet->GetSize ();
}

void
DataGatherer::HelloReceived (Ptr<const Packet> packet)
{
  ++m_helloRecv;
}

void
DataGatherer::LeftInQueue (uint32_t nodeId, BundleList bundles)
{
  m_leftInQueue.push_back (make_pair<uint32_t,BundleList> (nodeId, bundles));
}

void
DataGatherer::StartedSegment (Ptr<const Packet> segment)
{ 
  ++m_startedSegments;
  m_startedSegmentsSize += segment->GetSize ();
}

void
DataGatherer::AbortedSegment (uint32_t size, uint32_t numSegments)
{
  //  ++m_abortedTransmissions;
  m_abortedSegments += numSegments;
  m_abortedSegmentsSize += size;
}

void
DataGatherer::DeliveredSegment (Ptr<const Packet> segment)
{
  ++m_deliveredSegments;
  m_deliveredSegmentsSize += segment->GetSize ();
}

void
DataGatherer::EstimatedContactWindow (uint32_t nodeId, uint32_t toNodeId, Time cw, bool cwMax)
{
  if (cwMax)
    {
      ++m_cwMax;
      m_totEstimatedCwWith += cw;
      m_totEstimatedCwWithSec += (uint64_t) cw.GetSeconds ();
    }
  else
    {
      m_totEstimatedCwWith += cw;
      m_totEstimatedCwWithout += cw;
    
      m_totEstimatedCwWithSec += (uint64_t) cw.GetSeconds ();
      m_totEstimatedCwWithoutSec += (uint64_t) cw.GetSeconds ();
    }

  CwEntry cwe = CwEntry (nodeId, toNodeId, Simulator::Now (), cw);
  LinkKey lk = LinkKey (nodeId, toNodeId);
  m_openContactWindowList.insert (make_pair<LinkKey,CwEntry> (lk, cwe));
}

void
DataGatherer::ContactFailed (uint32_t nodeId)
{
  ++m_failedContacts;
}

void
DataGatherer::ContactClosedBeforeCw (uint32_t nodeId, uint32_t toNodeId, Time timeLeft, bool cwMax)
{  
  if (timeLeft != Seconds (0))
    {
      if (cwMax)
        {
          m_closedBeforeCwWith += 1;
          m_totTimeLeftWith += timeLeft;
          m_totTimeLeftWithSec += (uint64_t) timeLeft.GetSeconds ();
        }
      else
        {
          m_closedBeforeCwWith += 1;
          m_totTimeLeftWith += timeLeft;          
          m_totTimeLeftWithSec += (uint64_t) timeLeft.GetSeconds ();


          m_closedBeforeCwWithout += 1;
          m_totTimeLeftWithout += timeLeft;
          m_totTimeLeftWithoutSec += (uint64_t) timeLeft.GetSeconds ();
        }
    }

  OpenContactWindowList::iterator it = m_openContactWindowList.find (LinkKey (nodeId, toNodeId));
 
  CwEntry cwe = it->second;
  cwe.m_closed = Simulator::Now ();
  cwe.m_fin = true;
  cwe.m_timeLeft = timeLeft;

  m_openContactWindowList.erase (it);

  NodeContactWindowList::iterator iter = m_estimatedCwList.find (nodeId);
 
  if (iter != m_estimatedCwList.end ())
    {
      iter->second.push_back (cwe);
    }
  else
    {
      CwList cwList;
      cwList.push_back (cwe);
      m_estimatedCwList.insert (make_pair<uint32_t,CwList> (nodeId, cwList));
    }
}

void
DataGatherer::ContactOpportunity (uint32_t notUsed)
{
  ++m_contactsOpp;
  
  double timeNow = Simulator::Now ().GetSeconds ();
  uint32_t hour = ceil (timeNow / 3600.0);

  if (m_currentHour < hour)
    {
      m_contactsPerHour.push_back (make_pair<uint32_t,uint32_t> (m_currentHour, m_contactsCurrentHour));
      m_currentHour = hour;
      m_contactsCurrentHour = 1;
    }
  else
    {
      ++m_contactsCurrentHour;
    }
}

void
DataGatherer::ContactOpportunityBetween (uint32_t nodeId, uint32_t fromNodeId)
{
  NodeTimePair ntp = make_pair<uint32_t,Time> (fromNodeId, Simulator::Now ());
  
  NodeContactList::iterator iter = m_nodeContactsOppList.find (nodeId);
 
  if (iter != m_nodeContactsOppList.end ())
    {
      iter->second.push_back (ntp);
    }
  else
    {
      NtpList ntpList;
      ntpList.push_back (ntp);
      m_nodeContactsOppList.insert(make_pair<uint32_t, NtpList> (nodeId, ntpList));
    }
}

void
DataGatherer::Cancelled (Ptr<const Bundle> bundle)
{
  if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
    {
      ++m_cancelledRouter;
      ++m_cancelledKdm;
    }
  else if (bundle->GetCanonicalHeaders ().front ().GetBlockType () == 192)
    {
      ++m_cancelledRouter;
      ++m_cancelledCwi;
    }
  else
    {
      ++m_cancelledData;
    }
}

void
DataGatherer::Ack (Time started, bool timeout)
{
  ++m_acks;
  if (timeout)
    {
      ++m_ackTimeouts;
    }
  else
    {
      ++m_ackSucc;
      m_ackResponse.push_back ((Simulator::Now () - started));
    }
}

// Here I do everything that needs to be done before the result can be generated,
// tying up some lose ends.
void
DataGatherer::Finish ()
{
  m_contactsPerHour.push_back (make_pair<uint32_t,uint32_t> (m_currentHour, m_contactsCurrentHour));
  OpenContactWindowList::iterator iter = m_openContactWindowList.begin ();
  for (;
       iter != m_openContactWindowList.end (); ++iter)
    {
      LinkKey lk = iter->first;
      CwEntry cwe = iter->second;
      //cwe.m_closed = Simulator::Now ();
      cwe.m_closed = Seconds (43000); // FIXME: This is an quick hack to get correct results...
      
      NodeContactWindowList::iterator iter = m_estimatedCwList.find (lk.m_nodeId);
      
      if (iter != m_estimatedCwList.end ())
        {
          iter->second.push_back (cwe);
        }
      else
        {
          CwList cwList;
          cwList.push_back (cwe);
          m_estimatedCwList.insert (make_pair<uint32_t,CwList> (lk.m_nodeId, cwList));
        }
    }
}

void
DataGatherer::GenerateResult ()
{
  Finish ();
  m_measure.open (m_measureFile.c_str ());
  m_rawBundles.open (m_rawBundlesFile.c_str ());
  m_cph.open (m_cphFile.c_str ());

  m_measure << "Total delivery ratio: " << ((m_delivered > 0) ? ((double) m_delivered / (double) m_created) : 0) << endl;
  m_measure << "Total delivered byte ratio: " << ((m_createdSize > 0) ? ((double) m_deliveredSize / (double) m_createdSize) : 0) << endl;
  m_measure << "Total overhead: " << ((m_delivered > 0) ? ((double) m_relayed / (double) m_delivered) : 0) << endl;
  m_measure << "Total overhead (2): " << ((m_delivered > 0) ? ((double) (m_relayed - m_delivered) / (double) m_delivered) : 0) << endl;
  m_measure << "Accumulated utility: " << (m_deliveredPerUtility[1] + (m_deliveredPerUtility[2] * 2) + (m_deliveredPerUtility[3] * 3)) << endl <<endl;
  m_measure << "NeverReplicated ratio: " << ((m_neverReplicated > 0) ? ((double) m_neverReplicated / (double) m_created) : 0) << endl << endl;

  m_measure << "########## Interesting information" << endl;
  m_measure << "Created: " << m_created << endl;
  m_measure << "Delivered: " << m_delivered << endl;
  m_measure << "Relayed: " << m_relayed << endl;
  m_measure << "Redundant Relays: " << m_redundantRelay << endl;
  m_measure << "Dropped: " << m_dropped << endl;
  m_measure << "DroppedAndNeverReplicated: " << m_neverReplicated << endl;
  m_measure << "Removed: " << m_removed << endl;
  m_measure << "StartedData: " << m_startedData << endl;
  m_measure << "Cancelled Data: " << m_cancelledData << endl;
  m_measure << "AbortedData: " << m_abortedData << endl;

  m_measure << "CreatedRouter: " << m_createdRouter << endl;
  m_measure << "StartedRouter: " << m_startedRouter << endl;
  m_measure << "Cancelled router: " << m_cancelledRouter << endl;
  m_measure << "AbortedRouter: " << m_abortedRouter << endl;
  m_measure << "DeliveredRouter: " << m_deliveredRouter << endl;
  m_measure << "DeliveredRouterSizeReal: " << m_deliveredRouterSizeReal << endl;

  m_measure << "Router removed: " << m_routerRemoved << endl;
  m_measure << "Router Failed Removed: " << m_routerFailedRemoved << endl;
  m_measure << "Router Cancelled Removed: " << m_routerCancelledRemoved << endl;
  m_measure << "Router dropped: " << m_routerDropped << endl;


  m_measure << "DeliveredKdm: " << m_deliveredKdm << endl;
  m_measure << "DeliveredKdmSizeReal: " << m_deliveredKdmSizeReal << endl;
  m_measure << "DeliveredCwi: " << m_deliveredCwi << endl;
  m_measure << "DeliveredCwiSizeReal: " << m_deliveredCwiSizeReal << endl;

  m_measure << "Contacts Opportunities: " << m_contactsOpp << endl;
  m_measure << "FinishedContacts: " << m_contacts << endl;
  m_measure << "Failed contacts: " <<  m_failedContacts << endl;

  m_measure << "Contacts out of synch: " << m_contactsOutOfSynch << endl;

  m_measure << "Closed before cw with: " << m_closedBeforeCwWith << endl;
  m_measure << "Closed before cw without: " << m_closedBeforeCwWithout << endl;
 

  m_measure << "Max CWs: "  << m_cwMax << endl;
  m_measure << "Estimated cw with: "  << m_totEstimatedCwWith << " (" << m_totEstimatedCwWith.GetSeconds ()  << ")" <<  endl;
  m_measure << "Estimated cw without: "  << m_totEstimatedCwWithout << " (" << m_totEstimatedCwWithout.GetSeconds ()  << ")"  << endl;
  m_measure << "Estimated cw with sec: "  << m_totEstimatedCwWithSec << endl;
  m_measure << "Estimated cw without sec: "  << m_totEstimatedCwWithoutSec << endl;

  m_measure << "Total time left with: "  << m_totTimeLeftWith << " (" << m_totTimeLeftWith.GetSeconds ()  << ")"  << endl;
  m_measure << "Total time left without: "  << m_totTimeLeftWithout << " (" << m_totTimeLeftWithout.GetSeconds ()  << ")"  << endl;

  m_measure << "Total time left with sec: "  << m_totTimeLeftWithSec << endl;
  m_measure << "Total time left without sec: "  << m_totTimeLeftWithoutSec << endl;

  m_measure << "Sent Hellos: " << m_hello << endl;
  m_measure << "Received Hellos: " << m_helloRecv << endl;
  m_measure << "HelloSize: " << m_helloSize << endl;

  m_measure << "#####################################" << endl << endl;

  m_measure << "########## Raw transmitted data" << endl; 
  m_measure << "Started segments: " << m_startedSegments << endl;
  m_measure << "Aborted segments: " << m_abortedSegments << endl;
  m_measure << "Aborted transmissions: " << m_abortedTransmissions << endl;
  m_measure << "Delivered segments: " << m_deliveredSegments << endl;

  m_measure << "Started segments size: " << m_startedSegmentsSize << endl; 
  m_measure << "Aborted segments size: " << m_abortedSegmentsSize << endl;
  m_measure << "Delivered segments size: " << m_deliveredSegmentsSize << endl;
  m_measure << "#####################################" << endl << endl;

  m_measure << "########## Raw data for data bundles" << endl;
  m_measure << "Created: " << m_created << endl;
  m_measure << "Delivered: " << m_delivered << endl;
  m_measure << "Relayed: " << m_relayed << endl;
  m_measure << "Redundant Relays: " << m_redundantRelay << endl;
  m_measure << "Dropped: " << m_dropped << endl;
  m_measure << "DroppedAndNeverReplicated: " << m_neverReplicated << endl;
  m_measure << "Removed: " << m_removed << endl;
  m_measure << "StartedData: " << m_startedData << endl;
  m_measure << "Cancelled data: " << m_cancelledRouter << endl;
  m_measure << "AbortedData: " << m_abortedData << endl;

  m_measure << "CreatedSize: " << m_createdSizeWith << endl;
  m_measure << "DeliveredSize: " << m_deliveredSize << endl;
  m_measure << "DeliveredSizeWith: " << m_deliveredSizeWith << endl;

  m_measure << "RelayedSize: " << m_relayedSize << endl;
  m_measure << "RelayedSizeWith: " << m_relayedSizeWith << endl;
  m_measure << "RelayedSizeReal: " << m_relayedSizeReal << endl;

  m_measure << "RedundantRelaySize: " << m_redundantRelaySize << endl;
  m_measure << "RedundantRelaySizeWith: " << m_redundantRelaySizeWith << endl;
  m_measure << "RedundantRelaySizeWithReal: " << m_redundantRelaySizeReal << endl;

  m_measure << "DroppedSize: " << m_droppedSize << endl;
  m_measure << "DroppedSizeWith: " << m_droppedSizeWith << endl;

  m_measure << "DroppedAndNeverReplicatedSize: " << m_neverReplicatedSize << endl;
  m_measure << "DroppedAndNeverReplicatedSizeWith: " << m_neverReplicatedSizeWith << endl;

  m_measure << "RemovedSize: " << m_removedSize << endl;
  m_measure << "RemovedSizeWith: " << m_removedSizeWith << endl;

  m_measure << "StartedDataSize: " << m_startedDataSize << endl;
  m_measure << "StartedDataSizeWith: " << m_startedDataSizeWith << endl;
  m_measure << "StartedDataSizeReal: " << m_startedDataSizeReal << endl;

  m_measure << "AbortedDataSize: " << m_abortedDataSize << endl;
  m_measure << "AbortedDataSizeWith: " << m_abortedDataSizeWith << endl;
  m_measure << "AbortedDataSizeReal: " << m_abortedDataSizeReal << endl;
  m_measure << "#####################################" << endl << endl;
  
  m_measure << "########## Raw data for router specific bundles" << endl;
  m_measure << "CreatedRouter: " << m_createdRouter << endl;
  m_measure << "StartedRouter: " << m_startedRouter << endl;
  m_measure << "CancelledRouter: " << m_cancelledRouter << endl;
  m_measure << "AbortedRouter: " << m_abortedRouter << endl;
  m_measure << "DeliveredRouter: " << m_deliveredRouter << endl;

  m_measure << "Router removed: " << m_routerRemoved << endl;
  m_measure << "Router Failed Removed: " << m_routerFailedRemoved << endl;
  m_measure << "Router Cancelled Removed: " << m_routerCancelledRemoved << endl;
  m_measure << "Router dropped: " << m_routerDropped << endl;

  m_measure << "CreatedRouterSize: " << m_createdRouterSize << endl;
  m_measure << "CreatedRouterSizeWith: " << m_createdRouterSizeWith << endl;

  m_measure << "StartedRouterSize: " << m_startedRouterSize << endl;
  m_measure << "StartedRouterSizeWith: " << m_startedRouterSizeWith << endl;
  m_measure << "StartedRouterSizeReal: " << m_startedRouterSizeReal << endl;

  m_measure << "AbortedRouterSize: " << m_abortedRouterSize << endl;
  m_measure << "AbortedRouterSizeWith: " << m_abortedRouterSizeWith << endl;
  m_measure << "AbortedRouterSizeReal: " << m_abortedRouterSizeReal << endl;

  m_measure << "DeliveredRouterSize: " << m_deliveredRouterSize << endl;
  m_measure << "DeliveredRouterSizeWith: " << m_deliveredRouterSizeWith << endl;
  m_measure << "DeliveredRouterSizeReal: " << m_deliveredRouterSizeReal << endl;

  m_measure << "CreatedKdm: " << m_createdKdm << endl;
  m_measure << "StartedKdm: " << m_startedKdm << endl;
  m_measure << "CancelledKdm: " << m_cancelledKdm << endl;
  m_measure << "AbortedKdm: " << m_abortedKdm << endl;
  m_measure << "DeliveredKdm: " << m_deliveredKdm << endl;

  m_measure << "Kdm removed: " << m_kdmRemoved << endl;
  m_measure << "Kdm Failed Removed: " << m_kdmFailedRemoved << endl;
  m_measure << "Kdm Cancelled Removed: " << m_kdmCancelledRemoved << endl;
  m_measure << "Kdm dropped: " << m_kdmDropped << endl;


  m_measure << "CreatedKdmSize: " << m_createdKdmSize << endl;
  m_measure << "CreatedKdmSizeWith: " << m_createdKdmSizeWith << endl;

  m_measure << "StartedKdmSize: " << m_startedKdmSize << endl;
  m_measure << "StartedKdmSizeWith: " << m_startedKdmSizeWith << endl;
  m_measure << "StartedKdmSizeReal: " << m_startedKdmSizeReal << endl;

  m_measure << "AbortedKdmSize: " << m_abortedKdmSize << endl;
  m_measure << "AbortedKdmSizeWith: " << m_abortedKdmSizeWith << endl;
  m_measure << "AbortedKdmSizeReal: " << m_abortedKdmSizeReal << endl;

  m_measure << "DeliveredKdmSize: " << m_deliveredKdmSize << endl;
  m_measure << "DeliveredKdmSizeWith: " << m_deliveredKdmSizeWith << endl;
  m_measure << "DeliveredKdmSizeReal:" << m_deliveredKdmSizeReal << endl;

  m_measure << "CreatedCwi: " << m_createdCwi << endl;
  m_measure << "StartedCwi: " << m_startedCwi << endl;
  m_measure << "CancelledCwi: " << m_cancelledCwi << endl;
  m_measure << "AbortedCwi: " << m_abortedCwi << endl;
  m_measure << "DeliveredCwi: " << m_deliveredCwi << endl;

  m_measure << "Cwi removed: " << m_cwiRemoved << endl;
  m_measure << "Cwi Failed Removed: " << m_cwiFailedRemoved << endl;
  m_measure << "Cwi Cancelled Removed: " << m_cwiCancelledRemoved << endl;
  m_measure << "Cwi dropped: " << m_cwiDropped << endl;

  m_measure << "CreatedCwiSize: " << m_createdCwiSize << endl;
  m_measure << "CreatedCwiSizeWith: " << m_createdCwiSizeWith << endl;

  m_measure << "StartedCwiSize: " << m_startedCwiSize << endl;
  m_measure << "StartedCwiSizeWith: " << m_startedCwiSizeWith << endl;
  m_measure << "StartedCwiSizeReal: " << m_startedCwiSizeReal << endl;

  m_measure << "AbortedCwiSize: " << m_abortedCwiSize << endl;
  m_measure << "AbortedCwiSizeWith: " << m_abortedCwiSizeWith << endl;
  m_measure << "AbortedCwiSizeReal: " << m_abortedCwiSizeReal << endl << endl;

  m_measure << "DeliveredCwiSize: " << m_deliveredCwiSize << endl;
  m_measure << "DeliveredCwiSizeWith: " << m_deliveredCwiSizeWith << endl;
  m_measure << "DeliveredCwiSizeReal: " << m_deliveredCwiSizeReal << endl;

  m_measure << "Total acks waited for: " << m_acks << endl;
  m_measure << "Total succeeded acks: " << m_ackSucc << endl;
  m_measure << "Total timeouted acks: " << m_ackTimeouts << endl << endl;

  if (!m_ackResponse.empty ())
    {
      sort (m_ackResponse.begin (), m_ackResponse.end ());

      Time average;
      for (LatencyList::iterator iter = m_ackResponse.begin (); iter != m_ackResponse.end (); ++iter)
        {
          average += *iter;
        }
      
      average = Seconds (average.GetSeconds () / (double) m_ackResponse.size ());
      m_measure << "Average ack response: " << average.GetSeconds () <<  endl;
      m_measure << "Min ack response: " << m_ackResponse.front ().GetSeconds () << endl;
      m_measure << "Max ack response: " << m_ackResponse.back ().GetSeconds () << endl;
    }


  m_measure << "Sent Hellos: " << m_hello << endl;
  m_measure << "Received Hellos: " << m_helloRecv << endl;
  m_measure << "HelloSize: " << m_helloSize << endl;

  m_measure << "FinishedContacts: " << m_contacts << endl;
  m_measure << "Contacts out of synch: " << m_contactsOutOfSynch << endl;

  m_measure << "CreatedUtility: " << m_createdUtility << endl;
  m_measure << "Average CreatedUtility: " << ((m_created > 0) ?  ((double) m_createdUtility / (double) m_created) : 0) << endl;
  m_measure << "DeliveredUtility: " << m_deliveredUtility << endl;
  m_measure << "Average DeliveredUtility: " << ((m_delivered) ? ((double) m_deliveredUtility / (double) m_delivered) : 0) << endl;
  m_measure << "RelayedUtility: " << m_relayedUtility << endl;
  m_measure << "Average RelayedUtility: " << ((m_relayed > 0) ? ((double) m_relayedUtility / (double) m_relayed) : 0)  << endl;
  m_measure << "RedundantRelayUtility: " << m_redundantRelayUtility << endl;
  m_measure << "Average RedundantRelayUtility: " << ((m_redundantRelay > 0) ? ((double) m_redundantRelayUtility / (double) m_redundantRelay) : 0)  << endl;
  m_measure << "DroppedUtility: " << m_droppedUtility << endl;
  m_measure << "Average DroppedUtility: " << ((m_dropped > 0) ? ((double) m_droppedUtility / (double) m_dropped) : 0) << endl;
  m_measure << "DroppedAndNeverReplicatedUtility: " << m_neverReplicatedUtility << endl;
  m_measure << "Average DroppedAndNeverReplicatedUtility: " << ((m_neverReplicated > 0) ? ((double) m_neverReplicatedUtility / (double) m_neverReplicated) : 0) << endl;
  m_measure << "RemovedUtility: " << m_removedUtility << endl;
  m_measure << "Average RemovedUtility: " << ((m_removed > 0) ? ((double) m_removedUtility / (double) m_removed) : 0) << endl;
  m_measure << "StartedDataUtility: " << m_startedDataUtility << endl;
  m_measure << "Average StartedDataUtility: " << ((m_startedData > 0) ? ((double) m_startedDataUtility / (double) m_startedData) : 0) << endl;
  m_measure << "AbortedDataUtility: " << m_abortedDataUtility << endl;
  m_measure << "Average AbortedDataUtility: " << ((m_abortedData) ? ((double) m_abortedDataUtility / (double) m_abortedData) : 0) << endl;
  m_measure << "#####################################" << endl << endl;
  for (uint8_t i = 1; i <= 3; ++i)
    {
      m_measure << "########## Measurements for utility " << (uint32_t) i << endl;
      LatenciesPerUtility::iterator iter = m_latenciesPerUtility.find (i);
      if (iter != m_latenciesPerUtility.end ())
	{
	  LatencyList ll = iter->second;
	  
	  sort (ll.begin (), ll.end ());
	  if ((ll.size () % 2) == 0)
	    {
	      Time latency1 = ll.at ((ll.size () / 2) - 1);
	      Time latency2 = ll.at (ll.size () / 2);
	      Time median =  Seconds ((latency1.GetSeconds () + latency2.GetSeconds ()) / 2);
	      m_measure << "Median latency for utility " << (uint32_t) i << " is " << median << " (" << median.GetSeconds () << ")"  << endl;
	    }
	  else
	    {
	      m_measure << "Median latency for utility " << (uint32_t) i << " is " << ll.at (ceil (ll.size () / 2)) << " (" << (ll.at (ceil (ll.size () /2))).GetSeconds ()  << ")" << endl;
	    }
	  
	  m_measure << "Min latency for utility " << (uint32_t) i << " is " << ll.front () << " (" << ll.front ().GetSeconds ()  << ")"  << endl;
	  m_measure << "Max latency for utility " << (uint32_t) i << " is " << ll.back () << " (" << ll.back ().GetSeconds ()  << ")"  << endl;
	}
	  
      m_measure << "Created for utility " << (uint32_t) i << " is " << m_createdPerUtility[i] << endl;
      m_measure << "Delivered for uility " << (uint32_t) i << " is " << m_deliveredPerUtility [i] << endl;
      m_measure << "Relayed for uility " << (uint32_t) i << " is " << m_relayedPerUtility[i] << endl; 
      m_measure << "RedundantRelay for uility " << (uint32_t) i << " is " << m_redundantRelayPerUtility[i] << endl; 
      m_measure << "Dropped for uility " << (uint32_t) i << " is " << m_droppedPerUtility[i] << endl;
      m_measure << "DroppedAndNeverReplicated for uility " << (uint32_t) i << " is " << m_neverReplicatedPerUtility[i] << endl;
      m_measure << "Removed for uility " << (uint32_t) i << " is " << m_removedPerUtility[i] << endl;
      m_measure << "Started for uility " << (uint32_t) i << " is " << m_startedDataPerUtility[i] << endl;
      m_measure << "Aborted for uility " << (uint32_t) i << " is " << m_abortedDataPerUtility[i] << endl;
	  
      m_measure << "Delivery ratio for utility " << (uint32_t) i << " is " << ((m_createdPerUtility[i] > 0) ? ((double) m_deliveredPerUtility[i] / (double) m_createdPerUtility[i]) : 0) << endl;
      m_measure << "Relayed ratio for utility " << (uint32_t) i << " is " << ((m_deliveredPerUtility[i] > 0) ? ((double) m_relayedPerUtility[i] / (double) m_deliveredPerUtility[i]) : 0) << endl;
      m_measure << "Overhead ratio for utility " << (uint32_t) i << " is " << ((m_deliveredPerUtility[i] > 0) ? ((double) m_relayedPerUtility[i] / (double) m_deliveredPerUtility[i]) : 0) << endl;
      m_measure << "Dropped ratio for utility " << (uint32_t) i << " is " << ((m_relayedPerUtility[i] > 0) ? ((double) m_droppedPerUtility[i] / (double) m_relayedPerUtility[i]) : 0) << endl;
      m_measure << "Removed ratio for utility " << (uint32_t) i << " is " << ((m_relayedPerUtility[i] > 0) ? ((double) m_removedPerUtility[i] / (double) m_relayedPerUtility[i]) : 0) << endl;
      m_measure << "#####################################" << endl << endl;
    }
  m_measure << "############################################################" << endl << endl;

  m_cph << "########## General Contact Information" << endl;

  m_cph << "CreatedRouter: " << m_createdRouter << endl;
  m_cph << "StartedRouter: " << m_startedRouter << endl;
  m_cph << "CancelledRouter: " << m_cancelledRouter << endl;
  m_cph << "AbortedRouter: " << m_abortedRouter << endl;
  m_cph << "DeliveredRouter: " << m_deliveredRouter << endl;
  m_cph << "DeliveredRouterSizeReal: " << m_deliveredRouterSizeReal << endl << endl
;

  m_cph << "Router removed: " << m_routerRemoved << endl;
  m_cph << "Router Failed Removed: " << m_routerFailedRemoved << endl;
  m_cph << "Router Cancelled Removed: " << m_routerCancelledRemoved << endl;
  m_cph << "Router dropped: " << m_routerDropped << endl;


  m_cph << "CreatedKdm: " << m_createdKdm << endl;
  m_cph << "StartedKdm: " << m_startedKdm << endl;
  m_cph << "CancelledKdm: " << m_cancelledKdm << endl; 
  m_cph << "AbortedKdm: " << m_abortedKdm << endl;
  m_cph << "DeliveredKdm: " << m_deliveredKdm << endl;
  m_cph << "DeliveredKdmSizeReal: " << m_deliveredKdmSizeReal << endl << endl;

  m_cph << "Kdm removed: " << m_kdmRemoved << endl;
  m_cph << "Kdm Failed Removed: " << m_kdmFailedRemoved << endl;
  m_cph << "Kdm Cancelled Removed: " << m_kdmCancelledRemoved << endl;
  m_cph << "Kdm dropped: " << m_kdmDropped << endl;

  m_cph << "CreatedCwi: " << m_createdCwi << endl;
  m_cph << "StartedCwi: " << m_startedCwi << endl;
  m_cph << "CancelledCwi: " << m_cancelledCwi << endl; 
  m_cph << "AbortedCwi: " << m_abortedCwi << endl;
  m_cph << "DeliveredCwi: " << m_deliveredCwi << endl;
  m_cph << "DeliveredCwiSizeReal: " << m_deliveredCwiSizeReal << endl << endl;

  m_cph << "Cwi removed: " << m_cwiRemoved << endl;
  m_cph << "Cwi Failed Removed: " << m_cwiFailedRemoved << endl;
  m_cph << "Cwi Cancelled Removed: " << m_cwiCancelledRemoved << endl;
  m_cph << "Cwi dropped: " << m_cwiDropped << endl;

  m_cph << "Sent Hellos: " << m_hello << endl;
  m_cph << "Received Hellos: " << m_helloRecv << endl;
  m_cph << "HelloSize: " << m_helloSize << endl << endl;  

  m_cph << "Contacts Opportunities: " << m_contactsOpp << endl;
  m_cph << "FinishedContacts: " << m_contacts << endl;
  m_cph << "Failed contacts: " <<  m_failedContacts << endl;

  m_cph << "Contacts out of synch: " << m_contactsOutOfSynch << endl;

  m_cph << "Closed before cw with: " << m_closedBeforeCwWith << endl;
  m_cph << "Closed before cw without: " << m_closedBeforeCwWithout << endl;

  m_cph << "Max CWs: "  << m_cwMax << endl;
  m_cph << "Estimated cw with: "  << m_totEstimatedCwWith << " (" << m_totEstimatedCwWith.GetSeconds ()  << ")" <<  endl;
  m_cph << "Estimated cw without: "  << m_totEstimatedCwWithout << " (" << m_totEstimatedCwWithout.GetSeconds ()  << ")"  << endl;
  m_cph << "Estimated cw with sec: "  << m_totEstimatedCwWithSec << endl;
  m_cph << "Estimated cw without sec: "  << m_totEstimatedCwWithoutSec << endl;

  m_cph << "Total time left with: "  << m_totTimeLeftWith << " (" << m_totTimeLeftWith.GetSeconds ()  << ")"  << endl;
  m_cph << "Total time left without: "  << m_totTimeLeftWithout << " (" << m_totTimeLeftWithout.GetSeconds ()  << ")"  << endl;
  m_cph << "Total time left with sec: "  << m_totTimeLeftWithSec << endl;
  m_cph << "Total time left without sec: "  << m_totTimeLeftWithoutSec << endl;

  m_cph << "#####################################" << endl << endl;

  m_cph << "########## Contacts per hour" << endl << endl;
  m_cph << "Hour\tContacts" << endl;
  for (ContactsPerHourList::iterator iter = m_contactsPerHour.begin (); iter != m_contactsPerHour.end (); ++iter)
    {
      m_cph << iter->first << "\t" << iter->second << endl;
    }

  m_cph << endl << "#####################################" << endl << endl;

  
  m_cph << "########## Contact opportunities and contact windows per node summary" << endl << endl;

  m_cph << "Contact opportunities / Estimated Contact Windows Success-ratio (Missed)" << endl;
  for (uint32_t i = 0; i <= m_nNodes; ++i)
    {
      NodeContactList::iterator coIt = m_nodeContactsOppList.find (i);
      NodeContactWindowList::iterator cwIt = m_estimatedCwList.find (i);
      NtpList ntpList;
      CwList cwList;
        
      if (coIt != m_nodeContactsOppList.end ())
        ntpList = coIt->second;

      if (cwIt != m_estimatedCwList.end ())
        cwList = cwIt->second;
      
      m_cph << "#### node " << i << ": " << ntpList.size () << "/" << cwList.size () << " ";
      if (ntpList.size () != 0)
        {
          m_cph << ((double) cwList.size () / (double) ntpList.size ())*100;
        }
      else
        {
          m_cph << 0;
        }
        m_cph << "%" << " (" << (ntpList.size () - cwList.size ()) << ") " <<  endl;
    }

  m_cph << "#####################################" << endl << endl;


  m_cph << "########## Contact opportunities and contact windows per node" << endl << endl;

  //   Matchup code
  // ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
  m_cph.fill (' ');
  m_cph << right;
  for (uint32_t i = 0; i < 126; ++i)
    {
      NodeContactList::iterator coIt = m_nodeContactsOppList.find (i);
      NodeContactWindowList::iterator cwIt = m_estimatedCwList.find (i);
      NtpList ntpList;
      CwList cwList;
      
      if (coIt != m_nodeContactsOppList.end ())
        ntpList.insert (ntpList.begin (), coIt->second.begin (), coIt->second.end ());
      
      if (cwIt != m_estimatedCwList.end ())
        cwList.insert (cwList.begin (), cwIt->second.begin (), cwIt->second.end ());
        
      sort (cwList.begin (), cwList.end ());
        
      m_cph << "#### node " << i << " registered the following contacts opportunites and contact windows" << endl;
      m_cph << "Number of contact opportunites: " << ntpList.size () << endl;
      m_cph << "Number of estimated contact windows: " << cwList.size () << endl << endl;

      if (!ntpList.empty ())
        {
          m_cph << setw (11) << "At" << setw (11) << "To" << " | " << setw (11) << "At" << setw (11) << "To" << "" << setw (11) << "CW" << setw (11) << "Closed" << setw (11) << "Open" << setw (11) << "Used" << setw (11) << "Unused" << setw (11) << "Early" << setw (11) << "Finished" << endl;
          for (NtpList::iterator it = ntpList.begin (); it != ntpList.end (); ++it)
            {            
              m_cph << setw (11) << it->second.GetSeconds () << "" << setw (11) << it->first << " | ";// << endl;

              CwList::iterator iter = find_if (cwList.begin (), cwList.end (), Matching (it->second, it->first));
              if (iter != cwList.end ())
                {
                  CwEntry cwe = *iter;
                  m_cph << setw (11) << cwe.m_at.GetSeconds () << "" << setw (11) << cwe.m_toNodeId << "" << setw (11) << cwe.m_cw.GetSeconds () << "" << setw (11) << cwe.m_closed.GetSeconds ();
                  m_cph << "" << setw (11) << (cwe.m_closed - cwe.m_at).GetSeconds () << "" << setw (11) << (cwe.m_cw - cwe.m_timeLeft).GetSeconds () << "" << setw (11) << cwe.m_timeLeft.GetSeconds ();
                  m_cph << boolalpha << "" << setw (11) << cwe.m_timeLeft.IsStrictlyPositive () << noboolalpha;
                  m_cph << "" << setw (11) << cwe.m_fin << endl;
                
                  cwList.erase (iter);
                }
              else
                {
                  m_cph << setw (11) << "n/a" << "" << setw (11) << "n/a" << "" << setw (11) << "n/a" << "" << setw (11) << "n/a";
                  m_cph << "" << setw (11) << "n/a"  << "" << setw (11) << "n/a"  << "" << setw (11) << "n/a";
                  m_cph << "" << setw (11) << "n/a";
                  m_cph << "" << setw (11) << "n/a" << endl;
                }
            
            }
          if (!cwList.empty ())
            {
              m_cph << endl << endl;
              m_cph << "#### Contact windows with no found match" << endl << endl;
              m_cph << setw (11) << "At" << setw (11) << "To"  << "" << setw (11) << "At" << setw (11) << "To" << " | " << setw (11) << "CW" << setw (11) << "Closed" << setw (11) << "Open" << setw (11) << "Used" << setw (11) << "Unused" << setw (11) << "Early" << setw (11) << "Finished" << endl;
              for (CwList::iterator it = cwList.begin (); it != cwList.end (); ++it)
                {
                  CwEntry cwe = *it;
                  m_cph << setw (11) << cwe.m_at.GetSeconds () << "" << setw (11) << cwe.m_toNodeId << " | " << setw (11) << cwe.m_cw.GetSeconds () << "" << setw (11) << cwe.m_closed.GetSeconds ();
                  m_cph << "" << setw (11) << (cwe.m_closed - cwe.m_at).GetSeconds () << "" << setw (11) << (cwe.m_cw - cwe.m_timeLeft).GetSeconds () << "" << setw (11) << cwe.m_timeLeft.GetSeconds ();
                  m_cph << boolalpha << "" << setw (11) << cwe.m_timeLeft.IsStrictlyPositive () << noboolalpha;
                  m_cph << "" << setw (11) << cwe.m_fin << endl;
                }
              m_cph << "#####" << endl;
            }
        }
      m_cph << endl << "##########" << endl << endl;
    }

  
  m_rawBundles << "########## Rawdata for delivered bundles" << endl;
  
  m_rawBundles << "Created at\t From\t To\t Utility\t Payload size\t Header size\t Delivered at\t Latency\t" << endl;
  for (DeliveredList::iterator iter = m_deliveredList.begin (); iter != m_deliveredList.end (); ++iter)
    {
      Bundle bundle = iter->first;
      Time deliveredAt = iter->second;
      m_rawBundles <<  bundle.GetCreationTimestampTime () << "\t "  << bundle.GetSourceEndpoint ().GetId () << "\t " << bundle.GetDestinationEndpoint ().GetId () << "\t" << bundle.GetUtility () << "\t " << bundle.GetPayload ()->GetSize () << "\t " << bundle.GetSize () -  bundle.GetPayload ()->GetSize () << "\t " << deliveredAt.GetSeconds () << "\t " << (deliveredAt - bundle.GetCreationTimestamp ().GetTime ()).GetSeconds () << "\t " /*<< bundle.GetHopCount ()*/ << endl;
    }

  m_rawBundles << "#####################################" << endl << endl;

  m_rawBundles << "########## Rawdata for bundles left" << endl;
  
  for (BundlesLeftList::iterator iter = m_leftInQueue.begin (); iter != m_leftInQueue.end (); ++iter)
    {
      uint32_t nodeId = iter->first;
      BundleList bl = iter->second;

      m_rawBundles << "#### node " << nodeId  << " had the following bundles in its queue" <<  endl;
      if (bl.size () > 0)
        {
          m_rawBundles << "Created at\t From\t To\t Utility\t Payload size\t Header size\t Replication factor\t" << endl;
          for (BundleList::iterator it = bl.begin (); it != bl.end (); ++it)
            {
              Bundle bundle = **it;
              m_rawBundles << bundle.GetCreationTimestampTime () << "\t "  << bundle.GetSourceEndpoint ().GetId () << "\t " << bundle.GetDestinationEndpoint ().GetId () << "\t" << bundle.GetUtility () << "\t " << bundle.GetPayload ()->GetSize () << "\t " << bundle.GetSize () -  bundle.GetPayload ()->GetSize () << "\t " << bundle.GetReplicationFactor () << "\t " << endl;
            }
        }
      m_rawBundles << "##########" << endl << endl;
    }
  m_rawBundles << "#####################################" << endl << endl;


   m_rawBundles << "########## Rawdata for bundles dropped completely from the network" << endl;
   for (DeliveredList::iterator iter = m_deliveredList.begin (); iter != m_deliveredList.end (); ++iter)
    {
      Bundle bundle = iter->first;
      deque<Bundle>::iterator it = remove (m_droppedList.begin (), m_droppedList.end (), bundle);
      if (it != m_droppedList.end ())
	{
	  m_droppedList.erase (it, m_droppedList.end ());
	}
    }

   for (BundlesLeftList::iterator iter = m_leftInQueue.begin (); iter != m_leftInQueue.end (); ++iter)
    {
      BundleList bl = iter->second;
      for (BundleList::iterator it = bl.begin (); it != bl.end (); ++it)
        {
          deque<Bundle>::iterator i = remove (m_droppedList.begin (), m_droppedList.end (), **it);
          if (i != m_droppedList.end ())
            {
              m_droppedList.erase (i, m_droppedList.end ());
            }          
        }

    }
   
   m_rawBundles << "Dropped completely from the network: " << m_droppedList.size () << endl;
   
   m_rawBundles << "Created at\t From\t To\t Utility\t Payload size\t Header size\t Replication factor\t" << endl;
   for (deque<Bundle>::iterator iter = m_droppedList.begin (); iter != m_droppedList.end (); ++iter)
     {
       Bundle bundle = *iter;
       m_rawBundles  << bundle.GetCreationTimestampTime () << "\t "  << bundle.GetSourceEndpoint ().GetId () << "\t " << bundle.GetDestinationEndpoint ().GetId () << "\t" << bundle.GetUtility () << "\t " << bundle.GetPayload ()->GetSize () << "\t " << bundle.GetSize () -  bundle.GetPayload ()->GetSize () << "\t " << bundle.GetReplicationFactor () << "\t " << endl;
     }
   m_rawBundles << "#####################################";

}


}} // namespace bundleProtocol, ns3

