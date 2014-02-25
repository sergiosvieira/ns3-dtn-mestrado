/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_BUNDLE_STATUS_REPORT_H
#define BP_BUNDLE_STATUS_REPORT_H

#include "ns3/nstime.h"

#include "bp-bundle.h"
#include "bp-administrative-record.h"
#include "bp-creation-timestamp.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-utility.h"

namespace ns3 {
namespace bundleProtocol {

struct BundleStatusReportStatusFlags 
{
uint64_t m_flags;

BundleStatusReportStatusFlags ()
  : m_flags (0)
{}

BundleStatusReportStatusFlags (uint8_t flags)
  : m_flags (flags)
{}

~BundleStatusReportStatusFlags ()
{}

  void SetStatusFlags (uint8_t flags)
  {
    m_flags = flags;
  }
  
  uint64_t GetStatusFlags () const
  {
    return m_flags;
  }

  void SetReceivedBundle (bool val)
  {
    SetBit (m_flags, 0, val);
  }

  bool GetReceivedBundle () const
  {
    return GetBit (m_flags, 0);
  }

  void SetAcceptedCustodyOfBundle (bool val)
  {
    SetBit (m_flags, 1, val);
  }

  bool GetAcceptedCustodyOfBundle () const
  {
    return GetBit (m_flags, 1);
  }

  void SetForwardedTheBundle (bool val)
  {
    SetBit (m_flags, 2, val);
  }

  bool GetForwardedTheBundle () const
  {
    return GetBit (m_flags, 2);
  }

  void SetDeliveredBundle (bool val)
  {
    SetBit (m_flags, 3, val);
  }

  bool GetDeliveredBundle () const
  {
    return GetBit (m_flags, 3);
  }

  void SetDeletedTheBundle (bool val)
  {
    SetBit (m_flags, 4, val);
  }

  bool GetDeletedTheBundle () const
  {
    return GetBit (m_flags, 4);
  }

};

enum BundleStatusReportReason {
      STATUS_NO_ADDITIONAL_INFORMATION,
      STATUS_LIFETIME_EXPIRED,
      STATUS_FORWAREDED_OVER_UNIDIRECTIONAL_LINK,
      STATUS_TRANSMISSION_CANCELED,
      STATUS_DEPLETED_STORAGE,
      STATUS_DESTINATION_ENDPOINT_ID_UNINTELLIGIBLE,
      STATUS_NO_KNOWN_ROUTE_TO_DESTINATION_FROM_HERE,
      STATUS_NO_TIMELY_CONTACT_WITH_NEXT_NODE_ON_ROUTE,
      STATUS_BLOCK_UNINTELLIGIBLE,
      STATUS_UNKOWN
};


class BundleStatusReport : public AdministrativeRecord
{
public:
  BundleStatusReport (Ptr<Bundle> bundle, BundleStatusReportStatusFlags flags, BundleStatusReportReason reason);
  ~BundleStatusReport ();

  void SetReasonCode (BundleStatusReportReason reason);
  BundleStatusReportReason GetReasonCode () const;

  void SetStatusFlags (BundleStatusReportStatusFlags flags);
  BundleStatusReportStatusFlags GetStatusFlags () const;

  void SetReceivedBundle (bool val);
  bool GetReceivedBundle () const;
  void SetAcceptedCustodyOfBundle (bool val);
  bool GetAcceptedCustodyOfBundle () const;
  void SetForwardedTheBundle (bool val);
  bool GetForwardedTheBundle () const;
  void SetDeliveredBundle (bool val);
  bool GetDeliveredBundle () const;
  void SetDeletedTheBundle (bool val);
  bool GetDeletedTheBundle () const;

  void SetFragmentOffset (uint64_t offset);
  uint64_t GetFragmentOffset () const;
  
  void SetFragmentLength (uint64_t length);
  uint64_t GetFragmentLength () const;

  void SetTimeOfReception (Time time);
  Time GetTimeOfReception () const;
  
  void SetTimeOfCustodyAcceptance (Time time);
  Time GetTimeOfCustodyAcceptance () const;
                             
  void SetTimeOfForwarding (Time time);
  Time GetTimeOfForwarding () const;

  void SetTimeOfDelivery (Time time);
  Time GetTimeOfDelivery () const;

  void SetTimeOfDeletion (Time time);
  Time GetTimeOfDeletion () const;
  
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
  static BundleStatusReport Deserialize (uint8_t const*buffer);

private:
  BundleStatusReport ();
  BundleStatusReportStatusFlags m_statusFlags; 
  BundleStatusReportReason m_reason;
  uint64_t m_fragmentOffset;
  uint64_t m_fragmentLength;
  Time m_receipt;
  Time m_custodyAcceptance;
  Time m_forwarding;
  Time m_delivery;
  Time m_deletion;
  CreationTimestamp m_creationTimestamp;
  uint64_t m_sourceEndpointLength;
  BundleEndpointId m_sourceEndpoint;
  
};

}} // namespace bundleProtocol, ns3

#endif /* BP_BUNDLE_STATUS_REPORT_H */
