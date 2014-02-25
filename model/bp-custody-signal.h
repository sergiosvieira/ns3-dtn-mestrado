/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_CUSTODY_SIGNAL_H
#define BP_CUSTODY_SIGNAL_H

#include "ns3/header.h"
#include "ns3/nstime.h"

#include "bp-bundle.h"
#include "bp-administrative-record.h"
#include "bp-creation-timestamp.h"
#include "bp-bundle-endpoint-id.h"


namespace ns3 {
namespace bundleProtocol {

enum CustodySignalReason {
    CUSTODY_NO_ADDITIONAL_INFORMATION = 0,
    CUSTODY_RESERVED_1 = 1,
    CUSTODY_RESERVED_2 = 2,
    CUSTODY_REDUNDANT_RECEPTION = 3,
    CUSTODY_DEPLETED_STORAGE = 4,
    CUSTODY_DESTINATION_UNINTELLIGIBLE = 5,
    CUSTODY_NO_KNOWN_ROUTE = 6,
    CUSTODY_NO_TIMELY_CONTACT = 7,
    CUSTODY_BLOCK_UNINTELLIGIBLE = 8
};

class CustodySignal : public AdministrativeRecord
{
public:
  CustodySignal (Ptr<Bundle> bundle, CustodySignalReason reason, bool succeeded);
  CustodySignal (Ptr<Bundle> bundle, bool succeeded);
  ~CustodySignal ();

  void SetCustodyTransferSucceeded (bool b);
  bool GetCustodyTransferSucceeded () const;
  
  void SetReasonCode (CustodySignalReason reason);
  CustodySignalReason GetReasonCode () const;

  void SetFragmentOffset (uint64_t offset);
  uint64_t GetFragmentOffset () const;
  
  void SetFragmentLength (uint64_t lenght);
  uint64_t GetFragmentLength () const;

  void SetTimeOfSignal (Time timeOfSignal);
  Time GetTimeOfSignal () const;
                             
  void SetCreationTimestamp (CreationTimestamp timestamp);
  CreationTimestamp GetCreationTimestamp () const;
  uint64_t GetCreationTimestampTime () const;
  uint64_t GetCreationTimestampSequence () const;

  void SetSourceBundleEndpointIdLength (uint64_t length);
  uint64_t GetSourceBundleEndpointIdLength () const;
  
  void SetSourceBundleEndpointId (BundleEndpointId sourceEndpoint);
  BundleEndpointId GetSourceBundleEndpointId () const;

  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (uint8_t *buffer) const;
  static CustodySignal Deserialize (uint8_t const*buffer);

private:
  CustodySignal ();
  bool m_succeeded;
  CustodySignalReason m_reason;
  Time m_timeOfSignal;
  uint64_t m_fragmentOffset;
  uint64_t m_fragmentLength;
  CreationTimestamp m_creationTimestamp;
  uint64_t m_sourceEndpointLength;
  BundleEndpointId m_sourceEndpoint;
};

}} // namespace bundleProtocol, ns3

#endif /* BP_CUSTODY_SIGNAL_H */
