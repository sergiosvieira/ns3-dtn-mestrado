/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_BUNDLE_H
#define BP_BUNDLE_H

#include <set>
#include <vector>

#include "ns3/ptr.h"
#include "ns3/ref-count-base.h"
#include "ns3/packet.h"

#include "bp-header.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-global-bundle-identifier.h"

#include "ns3/simulator.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

struct EidAddress 
{
  BundleEndpointId m_eid;
  Address m_address;

  EidAddress (const BundleEndpointId& eid, const Address& address)
    : m_eid (eid), m_address (address)
  {}

  BundleEndpointId
  GetEndpoint () const
  {
    return m_eid;
  }

  void
  SetEndpoint (const BundleEndpointId& eid)
  {
    m_eid = eid;
  }

  Address
  GetAddress () const
  {
    return m_address;
  }
  
  void
  SetAddress (const Address& address)
  {
    m_address = address;
  }
};

enum RetentionConstraint {
  RC_DISPATCH_PENDING,
  RC_CUSTODY_ACCEPTED,
  RC_FORWARDING_PENDING
};

typedef vector<EidAddress> EidAddressList;
typedef vector<CanonicalBundleHeader> BlockList;
typedef set<RetentionConstraint> RcList;
typedef vector<int> RecList;
class Link;
class GlobalBundleIdentifier;

/**
 * \ingroup bundle
 *
 * \brief Represents a bundle.
 *
 *
 */

class Bundle : public Object
{
 public:
  RecList reclist;
  static TypeId GetTypeId (void);
  Bundle ();
  Bundle (Ptr<Packet> bundle);
  Bundle (const Bundle& bundle);
  ~Bundle ();
  virtual void DoDispose ();

  Ptr<Bundle> Copy () const;
  void SetPrimaryHeader (const PrimaryBundleHeader& header);
  PrimaryBundleHeader GetPrimaryHeader ();

  void AddCanonicalHeader (const CanonicalBundleHeader& header);
  void SetCanonicalHeaders (BlockList blocks);
  BlockList GetCanonicalHeaders () const;
    
  GlobalBundleIdentifier GetBundleId () const;

  void AddRetentionConstraint (RetentionConstraint rc);
  bool HasRetentionConstraint (RetentionConstraint rc) const;
  bool NoRetentionConstraints ();
  RcList GetRetentionConstraints () const;
  void RemoveRetentionConstraint (RetentionConstraint rc);
  void RemoveAllRetentionConstraints ();

  void SetPayload (Ptr<Packet> payload);
  Ptr<Packet> GetPayload () const;

  uint32_t GetSize () const;
  BundlePriority GetUtility () const;

  void AddReceivedFrom (const EidAddress& ea);
  EidAddressList GetReceivedFrom () const;
  bool HaveBeenReceivedFrom (const EidAddress& ea);
  bool HaveBeenReceivedFrom (Ptr<Link> link);
  bool HaveBeenReceivedFrom (const BundleEndpointId& eid);
  bool HaveBeenReceivedFrom (const Address& address);
  operator Ptr<Packet> () const;
  Ptr<Packet> ToPacket () const;

  // Primary block interface
  void SetVersion (uint8_t version);
  uint8_t GetVersion () const;
  
  void SetProcessingControlFlags (PrimaryProcessingControlFlags flags);
  PrimaryProcessingControlFlags GetProcessingControlFlags () const;
  void SetFragment (bool val);
  bool IsFragment () const;
  void SetAdministrativeRecord (bool val);
  bool IsAdministrativeRecord () const;
  void SetDoNotFragment (bool val);
  bool IsDoNotFragment () const;
  void SetCustodyTransferRequested (bool val);
  bool IsCustodyTransferRequested () const;
  void SetSingletonEndpoint (bool val);
  bool IsSingletonEndpoint () const;
  void SetApplicationAckRequested (bool val);
  bool IsApplicationAckRequested () const;
  void SetReplicationFactor (uint64_t replicationFactor);
  uint64_t GetReplicationFactor () const;
  void SetPriority (const BundlePriority& priority);
  BundlePriority GetPriority () const;
  void SetReportBundleReception (bool val);
  bool GetReportBundleReception () const;
  void SetReportCustodyAcceptance (bool val);
  bool GetReportCustodyAcceptance () const;
  void SetReportBundleForwarding (bool val);
  bool GetReportBundleForwarding () const;
  void SetReportBundleDelivery (bool val);
  bool GetReportBundleDelivery () const;
  void SetReportBundleDeletion (bool val);
  bool GetReportBundleDeletion () const;

  uint64_t GetBlockLength () const;

  void SetDestinationEndpoint (const BundleEndpointId& destinationEid);
  void RemoveDestinationEndpoint ();
  BundleEndpointId GetDestinationEndpoint () const;

  void SetSourceEndpoint (const BundleEndpointId& sourceEid);
  void RemoveSourceEndpoint ();
  BundleEndpointId GetSourceEndpoint () const;

  void SetCustodianEndpoint (const BundleEndpointId& custodianEid);
  void RemoveCustodianEndpoint ();
  BundleEndpointId GetCustodianEndpoint () const;

  void SetCreationTimestamp (const CreationTimestamp& timestamp);
  uint64_t GetCreationTimestampTime () const;
  uint64_t GetCreationTimestampSequence () const;
  CreationTimestamp GetCreationTimestamp () const;
  void SetLifetime (const Time& lifetime);
  void SetLifetime (uint64_t lifetime);
  Time GetLifetime () const;
  uint64_t GetLifetimeSeconds () const;

  uint64_t GetDictionaryLength () const;

  void SetFragmentOffset (int64_t offset);
  uint64_t GetFragmentOffset () const;
  void SetTotalApplicationLength (uint64_t length);
  uint64_t GetTotalApplicationLength () const;

  bool operator== (const Bundle& other) const;

  uint64_t GetGlobalId()const;
  void GenerateID();
  GlobalBundleIdentifier GetCustody();
  void SetCustody(GlobalBundleIdentifier gbid);

  /*Joao*/
  bool HaveBeenReceivedFrom(const int& id);
  void AddReceivedFrom (const int& id);
  /*Joao*/
 private:

  	PrimaryBundleHeader m_primaryHeader;
	BlockList m_canonicalHeaders;
	Ptr<Packet> m_payload;
	GlobalBundleIdentifier m_gbid;
	RcList m_rcs;
	EidAddressList m_receivedFrom;

  friend ostream& operator<< (ostream& os, const Bundle& bundle);
};

ostream& operator<< (ostream& os, const Bundle& bundle);

}

} // namespace bundleProtocol, ns3



#endif /* BP_BUNDLE_H */
