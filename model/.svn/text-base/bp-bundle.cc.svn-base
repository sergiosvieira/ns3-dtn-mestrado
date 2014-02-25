/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <algorithm>
#include <iostream>

#include "ns3/simulator.h"
#include "ns3/log.h"

#include "bp-bundle.h"
#include "bp-link.h"
#include "bp-global-bundle-identifier.h"


using namespace std;

NS_LOG_COMPONENT_DEFINE ("Bundle");

namespace ns3 {
namespace bundleProtocol {


uint64_t m_bundle_global_id_inc = 0;

NS_OBJECT_ENSURE_REGISTERED (Bundle);

TypeId
Bundle::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::Bundle")
    .SetParent<Object> ()
    .AddConstructor<Bundle> ()
    ;
  return tid;
}

Bundle::Bundle ()
  :
	m_primaryHeader (),
	m_canonicalHeaders (),
	m_payload (),
	m_gbid (),
	m_rcs (),
	m_receivedFrom ()
{

  NS_LOG_DEBUG("Bundle::Bundle ()");
}

Bundle::Bundle (Ptr<Packet> bundle)
  :
	m_primaryHeader (),
	m_canonicalHeaders (),
	m_payload (),
	m_gbid (),
	m_rcs (),
	m_receivedFrom ()
{
  NS_LOG_DEBUG("Bundle::Bundle (Ptr<Packet>)");
  Ptr<Packet> tmp = bundle->Copy ();
  tmp->RemoveHeader (m_primaryHeader);
  BlockList::iterator iter;
  CanonicalBundleHeader canonicalHeader;
  while (true)
    {
      tmp->RemoveHeader (canonicalHeader);
      AddCanonicalHeader (canonicalHeader);
      if (canonicalHeader.IsLastBlock ())
        break;
    }
  m_payload = tmp;
  m_gbid = GlobalBundleIdentifier (bundle);
  //m_bundle_global_id = bundle->GetGlobalId();
}

  
Bundle::Bundle (const Bundle& bundle)
  :
	m_primaryHeader (bundle.m_primaryHeader),
	m_canonicalHeaders (bundle.m_canonicalHeaders),
	m_payload (bundle.m_payload->Copy ()),
	m_gbid (bundle.m_gbid),
	m_rcs (bundle.m_rcs),
	m_receivedFrom (bundle.m_receivedFrom)
{
  NS_LOG_DEBUG("Bundle::Bundle (bundle)");
}
 
Bundle::~Bundle ()
{
  NS_LOG_DEBUG("Bundle::~Bundle");
}

void
Bundle::DoDispose ()
{
  m_payload = 0;
  m_rcs.clear ();
  m_receivedFrom.clear ();
  m_canonicalHeaders.clear ();
  Object::DoDispose ();
}

Ptr<Bundle>
Bundle::Copy () const
{
  NS_LOG_DEBUG("Bundle::Copy");
  return Create<Bundle> (*this);
}

void
Bundle::SetPrimaryHeader (const PrimaryBundleHeader& header)
{
  NS_LOG_DEBUG("Bundle::SetPrimaryHeader");
  m_primaryHeader = header;
  m_gbid = GlobalBundleIdentifier (m_primaryHeader.GetSourceEndpoint (), m_primaryHeader.GetCreationTimestamp ());
}
  
PrimaryBundleHeader
Bundle::GetPrimaryHeader ()
{
  NS_LOG_DEBUG("Bundle::GetPrimaryHeader");
  return m_primaryHeader;
}

void
Bundle::AddCanonicalHeader (const CanonicalBundleHeader& header)
{
  NS_LOG_DEBUG("Bundle::AddCanonicalHeader");
  m_canonicalHeaders.push_back (header);
}

void 
Bundle::SetCanonicalHeaders (BlockList blocks)
{
  NS_LOG_DEBUG("Bundle::SetCanonicalHeaders");
  m_canonicalHeaders = blocks;
}

BlockList
Bundle::GetCanonicalHeaders () const
{
  NS_LOG_DEBUG("Bundle::GetCanonicalHeaders");
  return m_canonicalHeaders;
}
  
GlobalBundleIdentifier
Bundle::GetBundleId () const
{
  NS_LOG_DEBUG("Bundle::GetBundleId");
  return m_gbid;
}

void
Bundle::AddRetentionConstraint (RetentionConstraint rc)
{
  NS_LOG_DEBUG("Bundle::AddRetentionConstraint");
  m_rcs.insert (rc);
}

RcList
Bundle::GetRetentionConstraints () const
{
  NS_LOG_DEBUG("Bundle::GetRetentionConstraints");
  return m_rcs;
}

bool
Bundle::HasRetentionConstraint (RetentionConstraint rc) const
{
  NS_LOG_DEBUG("Bundle::HasRetentionConstraint");
  RcList::iterator iter = find (m_rcs.begin (), m_rcs.end (), rc);
  if (iter != m_rcs.end ())
    return true;
  else
  return false;
}

bool
Bundle::NoRetentionConstraints ()
{
  NS_LOG_DEBUG("Bundle::NoRetentionConstraints");
  return m_rcs.empty ();
}

void
Bundle::RemoveRetentionConstraint (RetentionConstraint rc)
{
  NS_LOG_DEBUG("Bundle::RemoveRetentionConstraint");
  RcList::iterator iter = m_rcs.find (rc);
  
  if (iter != m_rcs.end ())
  m_rcs.erase (iter);
}

void
Bundle::RemoveAllRetentionConstraints ()
{
  NS_LOG_DEBUG("Bundle::RemoveAllRetentionConstraints");
  m_rcs.clear ();
}

void
Bundle::SetPayload (Ptr<Packet> payload)
{
  NS_LOG_DEBUG("Bundle::SetPayload");
  m_payload = payload;
}

Ptr<Packet>
Bundle::GetPayload () const
{
  NS_LOG_DEBUG("Bundle::GetPayload");
  return m_payload;
}

uint32_t
Bundle::GetSize () const
{
  NS_LOG_DEBUG("Bundle::GetSize");
  uint32_t size = 0;
  
  size += m_primaryHeader.GetSerializedSize ();
  BlockList::const_iterator iter;
  for (iter = m_canonicalHeaders.begin (); iter < m_canonicalHeaders.end (); ++iter) 
    {
      size += (*iter).GetSerializedSize ();
    }
  size += m_payload->GetSize ();
  return size;
}

BundlePriority
Bundle::GetUtility () const
{
  return m_primaryHeader.GetPriority ();
}

void
Bundle::AddReceivedFrom (const EidAddress& ea)
{
  NS_LOG_DEBUG ("");
  if (!HaveBeenReceivedFrom (ea))
      m_receivedFrom.push_back (ea);
}
void
Bundle::AddReceivedFrom (const int& id)
{
	for(unsigned int i = 0; i < reclist.size(); i++){
		if(reclist[i] == id){return;}
	}
	reclist.push_back(id);
}

bool
Bundle::HaveBeenReceivedFrom(const int& id){
	  for (RecList::iterator iter = reclist.begin (); iter != reclist.end (); ++iter){
	      if (*iter == id){return true;}
	  }
	  return false;

}

EidAddressList
Bundle::GetReceivedFrom () const
{
  NS_LOG_DEBUG ("Bundle::GetReceivedFrom");
  return m_receivedFrom;
}

bool
Bundle::HaveBeenReceivedFrom (const EidAddress& ea)
{
  NS_LOG_DEBUG ("Bundle::HaveBeenReceivedFrom");
  return HaveBeenReceivedFrom (ea.GetEndpoint ());
}

bool
Bundle::HaveBeenReceivedFrom (Ptr<Link> link)
{
  NS_LOG_DEBUG ("Bundle::HaveBeenReceivedFrom");
  return HaveBeenReceivedFrom (link->GetRemoteEndpointId ());
}

bool
Bundle::HaveBeenReceivedFrom (const BundleEndpointId& eid)
{
  NS_LOG_DEBUG ("Bundle::HaveBeenReceivedFrom");
  for (EidAddressList::iterator iter = m_receivedFrom.begin (); iter != m_receivedFrom.end (); ++iter)
    {
      if (iter->GetEndpoint () == eid)
        return true;
    }
  return false;
}

bool
Bundle::HaveBeenReceivedFrom (const Address& address)
{
  NS_LOG_DEBUG ("Bundle::HaveBeenReceivedFrom");
  for (EidAddressList::iterator iter = m_receivedFrom.begin (); iter != m_receivedFrom.end (); ++iter)
    {
      if (iter->GetAddress () == address)
        return true;
    }
  return false;
}


Bundle::operator Ptr<Packet> () const
{
  NS_LOG_DEBUG("Bundle::operator Ptr<Packet> ()");
  return ToPacket ();
}

Ptr<Packet>
Bundle::ToPacket () const
{
  NS_LOG_DEBUG("Bundle::ToPacket");
  
  Ptr<Packet> bundle = m_payload->Copy ();
  BlockList::const_reverse_iterator iter; 
  for (iter = m_canonicalHeaders.rbegin (); iter < m_canonicalHeaders.rend (); ++iter) 
    {
      bundle->AddHeader (*iter);
    }
  bundle->AddHeader (m_primaryHeader);
  
  return bundle;
}

bool
Bundle::operator== (const Bundle& other) const
{
  return m_gbid == other.GetBundleId ();
}

ostream&
operator<< (ostream& os, const Bundle& bundle)
{
  os << bundle.m_primaryHeader << endl << endl;
  
  os << "Canonical headers" << endl;
  for (BlockList::const_iterator iter = bundle.m_canonicalHeaders.begin (); iter != bundle.m_canonicalHeaders.end (); ++iter) 
    {
      os << *iter << endl;
    }

  os << "Size with headers: " << bundle.GetSize () << endl;

  os << endl << "Have been received from: " << endl;
  for (EidAddressList::const_iterator iter = bundle.m_receivedFrom.begin (); iter != bundle.m_receivedFrom.end (); ++iter)
    {
      os << iter->GetEndpoint () << endl;
    }
  return os;
}


  // Primary block interface
void
Bundle::SetVersion (uint8_t version)
{
  m_primaryHeader.SetVersion (version);
}

uint8_t
Bundle::GetVersion () const
{
  return m_primaryHeader.GetVersion ();
}
  
void
Bundle::SetProcessingControlFlags (PrimaryProcessingControlFlags flags)
{
  m_primaryHeader.SetProcessingControlFlags (flags);
}

PrimaryProcessingControlFlags
Bundle::GetProcessingControlFlags () const
{
  return m_primaryHeader.GetProcessingControlFlags ();
}

void
Bundle::SetFragment (bool val)
{
  m_primaryHeader.SetFragment (val);
}

bool
Bundle::IsFragment () const
{
  return m_primaryHeader.IsFragment ();
}

void
Bundle::SetAdministrativeRecord (bool val)
{
  m_primaryHeader.SetAdministrativeRecord (val);
}

bool
Bundle::IsAdministrativeRecord () const
{
  return m_primaryHeader.IsAdministrativeRecord ();
}

void
Bundle::SetDoNotFragment (bool val)
{
  m_primaryHeader.SetDoNotFragment (val);
}

 bool
Bundle::IsDoNotFragment () const
{
  return m_primaryHeader.IsDoNotFragment ();
}

void
Bundle::SetCustodyTransferRequested (bool val)
{
  m_primaryHeader.SetCustodyTransferRequested (val);
}

bool Bundle::IsCustodyTransferRequested () const
{
	NS_LOG_DEBUG("bool Bundle::IsCustodyTransferRequested () const");
	return m_primaryHeader.IsCustodyTransferRequested();
}

void
Bundle::SetSingletonEndpoint (bool val)
{
  m_primaryHeader.SetSingletonEndpoint (val);
}

bool
Bundle::IsSingletonEndpoint () const
{
  return m_primaryHeader.IsSingletonEndpoint ();
}

void
Bundle::SetApplicationAckRequested (bool val)
{
  m_primaryHeader.SetApplicationAckRequested (val);
}

bool
Bundle::IsApplicationAckRequested () const
{
  return m_primaryHeader.IsApplicationAckRequested ();
}

void
Bundle::SetReplicationFactor (uint64_t replicationFactor)
{
  m_primaryHeader.SetReplicationFactor (replicationFactor);
}

uint64_t
Bundle::GetReplicationFactor () const
{
  return m_primaryHeader.GetReplicationFactor ();
}

void
Bundle::SetPriority (const BundlePriority& priority)
{
  m_primaryHeader.SetPriority (priority);
}

BundlePriority
Bundle::GetPriority () const
{
  return m_primaryHeader.GetPriority ();
}

void
Bundle::SetReportBundleReception (bool val)
{
  m_primaryHeader.SetReportBundleReception (val);
}

bool
Bundle::GetReportBundleReception () const
{
  return m_primaryHeader.GetReportBundleReception ();
}

void
Bundle::SetReportCustodyAcceptance (bool val)
{
  m_primaryHeader.SetReportCustodyAcceptance (val);
}

bool
Bundle::GetReportCustodyAcceptance () const
{
  return m_primaryHeader.GetReportCustodyAcceptance ();
}

void
Bundle::SetReportBundleForwarding (bool val)
{
  m_primaryHeader.SetReportBundleForwarding (val);
}

bool
Bundle::GetReportBundleForwarding () const
{
  return m_primaryHeader.GetReportBundleForwarding ();
}

void
Bundle::SetReportBundleDelivery (bool val)
{
  m_primaryHeader.SetReportBundleDelivery (val);
}

bool
Bundle::GetReportBundleDelivery () const
{
  return m_primaryHeader.GetReportBundleDelivery ();
}

void
Bundle::SetReportBundleDeletion (bool val)
{
  m_primaryHeader.SetReportBundleDeletion (val);
}

bool
Bundle::GetReportBundleDeletion () const
{
  return m_primaryHeader.GetReportBundleDeletion ();
}

uint64_t
Bundle::GetBlockLength () const
{
  return m_primaryHeader.GetBlockLength ();
}

void
Bundle::SetDestinationEndpoint (const BundleEndpointId& destinationEid)
{
  return m_primaryHeader.SetDestinationEndpoint (destinationEid);
}

void
Bundle::RemoveDestinationEndpoint ()
{
  m_primaryHeader.RemoveDestinationEndpoint ();
}

BundleEndpointId
Bundle::GetDestinationEndpoint () const
{
  return m_primaryHeader.GetDestinationEndpoint ();
}


void
Bundle::SetSourceEndpoint (const BundleEndpointId& sourceEid)
{
  return m_primaryHeader.SetSourceEndpoint (sourceEid);
}

void
Bundle::RemoveSourceEndpoint ()
{
  m_primaryHeader.RemoveSourceEndpoint ();
}

BundleEndpointId
Bundle::GetSourceEndpoint () const
{
  return m_primaryHeader.GetSourceEndpoint ();
}
  /*
void
Bundle::SetReportToEndpoint (const BundleEndpointId& reportToEid)
{
  return m_primaryHeader.SetReportToEndpoint (reportToEid);
}

void
Bundle::RemoveReportToEndpoint ()
{
  m_primaryHeader.RemoveReportToEndpoint ();
}

BundleEndpointId
Bundle::GetReportToEndpoint() const
{
  return m_primaryHeader.GetReportToEndpoint();
}
  */
void
Bundle::SetCustodianEndpoint (const BundleEndpointId& custodianEid)
{
  return m_primaryHeader.SetCustodianEndpoint (custodianEid);
}

void
Bundle::RemoveCustodianEndpoint ()
{
  m_primaryHeader.RemoveCustodianEndpoint ();
}

BundleEndpointId
Bundle::GetCustodianEndpoint () const
{
  return m_primaryHeader.GetCustodianEndpoint ();
}

void
Bundle::SetCreationTimestamp (const CreationTimestamp& timestamp)
{
  m_primaryHeader.SetCreationTimestamp (timestamp);
}

uint64_t
Bundle::GetCreationTimestampTime () const
{
  return m_primaryHeader.GetCreationTimestampTime ();
}

uint64_t
Bundle::GetCreationTimestampSequence () const
{
  return m_primaryHeader.GetCreationTimestampSequence ();
}

CreationTimestamp
Bundle::GetCreationTimestamp () const
{
  return m_primaryHeader.GetCreationTimestamp ();
}

void
Bundle::SetLifetime (const Time& lifetime)
{
  m_primaryHeader.SetLifetime (lifetime);
}

void
Bundle::SetLifetime (uint64_t lifetime)
{
  m_primaryHeader.SetLifetime (lifetime);
}

Time
Bundle::GetLifetime () const
{
  return m_primaryHeader.GetLifetime ();
}

uint64_t 
Bundle::GetLifetimeSeconds () const
{
  return m_primaryHeader.GetLifetimeSeconds ();
}

uint64_t
Bundle::GetDictionaryLength () const
{
  return m_primaryHeader.GetDictionaryLength ();
}

void
Bundle::SetFragmentOffset (int64_t offset)
{
  m_primaryHeader.SetFragmentOffset (offset);
}

uint64_t
Bundle::GetFragmentOffset () const
{
  return m_primaryHeader.GetFragmentOffset ();
}

void
Bundle::SetTotalApplicationLength (uint64_t length)
{
  m_primaryHeader.SetTotalApplicationLength (length);
}

uint64_t
Bundle::GetTotalApplicationLength () const
{
  return m_primaryHeader.GetTotalApplicationLength ();
}

/*Joao*/
void Bundle::GenerateID()
{
   m_primaryHeader.SetGlobalId(m_bundle_global_id_inc);
   m_bundle_global_id_inc++;
}

uint64_t Bundle::GetGlobalId()const
{
  return m_primaryHeader.GetGlobalId();
}

GlobalBundleIdentifier Bundle::GetCustody()
{
	return m_primaryHeader.GetCustody();
}
void Bundle::SetCustody(GlobalBundleIdentifier gbid)
{
	m_primaryHeader.SetCustody(gbid);
}

/*Joao*/



}} // namespace bundleProtocol, ns3
