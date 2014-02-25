/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-bundle-status-report.h"
#include "bp-sdnv.h"
#include "bp-header.h"

namespace ns3 {
namespace bundleProtocol {

BundleStatusReport::BundleStatusReport ()
  : AdministrativeRecord (),
    m_statusFlags (),
    m_reason (STATUS_NO_ADDITIONAL_INFORMATION),
    m_fragmentOffset (0),
    m_fragmentLength (0),
    m_receipt (),
    m_custodyAcceptance (),
    m_forwarding (),
    m_delivery (),
    m_deletion (),
    m_creationTimestamp (),
    m_sourceEndpointLength (9),
    m_sourceEndpoint (BundleEndpointId::GetAnyBundleEndpointId ())

{}

BundleStatusReport::BundleStatusReport (Ptr<Bundle> bundle, BundleStatusReportStatusFlags flags, BundleStatusReportReason reason)
  : AdministrativeRecord (bundle, BUNDLE_STATUS_REPORT),
    m_statusFlags (flags),
    m_reason (reason),
    m_receipt (),
    m_custodyAcceptance (),
    m_forwarding (),
    m_delivery (),
    m_deletion (),
    m_creationTimestamp (),
    m_sourceEndpointLength (9),
    m_sourceEndpoint (BundleEndpointId::GetAnyBundleEndpointId ())
{
  PrimaryBundleHeader header = bundle->GetPrimaryHeader ();
  
  if(header.IsFragment ())
    {
      m_fragmentOffset = header.GetFragmentOffset ();
      m_fragmentLength = header.GetTotalApplicationLength ();
    }
                        
  m_creationTimestamp = header.GetCreationTimestamp ();
  m_sourceEndpointLength = header.GetSourceEndpoint ().GetSerializedSize ();
  m_sourceEndpoint = header.GetSourceEndpoint ();
}

BundleStatusReport::~BundleStatusReport ()
{}
  
void
BundleStatusReport::SetReasonCode (BundleStatusReportReason reason)
{
  m_reason = reason;
}

BundleStatusReportReason
BundleStatusReport::GetReasonCode () const
{
  return m_reason;
}

void
BundleStatusReport::SetStatusFlags (BundleStatusReportStatusFlags flags)
{
  m_statusFlags = flags;
}

BundleStatusReportStatusFlags
BundleStatusReport::GetStatusFlags () const
{
  return m_statusFlags;
}

void 
BundleStatusReport::SetReceivedBundle (bool val)
{
    SetReceivedBundle (val);
}

bool
BundleStatusReport::GetReceivedBundle () const
{
  return m_statusFlags.GetReceivedBundle ();
}
  
void
BundleStatusReport::SetAcceptedCustodyOfBundle (bool val)
{
  m_statusFlags.SetAcceptedCustodyOfBundle (val);
}
  
bool
BundleStatusReport::GetAcceptedCustodyOfBundle () const
{
  return m_statusFlags.GetAcceptedCustodyOfBundle ();
}

void
BundleStatusReport::SetForwardedTheBundle (bool val)
{
  m_statusFlags.SetForwardedTheBundle (val);
}
  
bool
BundleStatusReport::GetForwardedTheBundle () const
{
    return m_statusFlags.GetForwardedTheBundle ();
}

void
BundleStatusReport::SetDeliveredBundle (bool val)
{
  m_statusFlags.SetDeliveredBundle (val);
}

bool
BundleStatusReport::GetDeliveredBundle () const
{
  return m_statusFlags.GetDeliveredBundle ();
}

void
BundleStatusReport::SetDeletedTheBundle (bool val)
{
  m_statusFlags.SetDeletedTheBundle (val);
}

bool
BundleStatusReport::GetDeletedTheBundle () const
{
  return m_statusFlags.GetDeletedTheBundle ();
}

void
BundleStatusReport::SetFragmentOffset (uint64_t offset)
{
  m_fragmentOffset = offset;
}

uint64_t
BundleStatusReport::GetFragmentOffset () const
{
  return m_fragmentOffset;
}
  
void
BundleStatusReport::SetFragmentLength (uint64_t length)
{
  m_fragmentLength = length;
}

uint64_t
BundleStatusReport::GetFragmentLength () const
{
  return m_fragmentLength;
}

void
BundleStatusReport::SetTimeOfReception (Time time)
{
  m_receipt = time;
}


Time
BundleStatusReport::GetTimeOfReception () const
{
  return m_receipt;
}

  
void
BundleStatusReport::SetTimeOfCustodyAcceptance (Time time)
{
  m_custodyAcceptance = time;
}

Time
BundleStatusReport::GetTimeOfCustodyAcceptance () const
{
  return m_custodyAcceptance;
}
                             
void
BundleStatusReport::SetTimeOfForwarding (Time time)
{
  m_forwarding = time;
}

Time
BundleStatusReport::GetTimeOfForwarding () const
{
  return m_forwarding;
}

void
BundleStatusReport::SetTimeOfDelivery (Time time)
{
  m_delivery = time;
}

Time
BundleStatusReport::GetTimeOfDelivery () const
{
  return m_delivery;
}

void
BundleStatusReport::SetTimeOfDeletion (Time time)
{
  m_deletion = time;
}

Time
BundleStatusReport::GetTimeOfDeletion () const
{
  return m_deletion;
}


void
BundleStatusReport::SetCreationTimestamp (CreationTimestamp creationTimestamp)
{
  m_creationTimestamp = creationTimestamp;
}

CreationTimestamp
BundleStatusReport::GetCreationTimestamp () const
{
  return m_creationTimestamp;
}
 
uint64_t
BundleStatusReport::GetCreationTimestampTime () const
{
  return m_creationTimestamp.GetSeconds ();
}
 
uint64_t
BundleStatusReport::GetCreationTimestampSequence () const
{
  return  m_creationTimestamp.GetSequence ();
}

void
BundleStatusReport::SetSourceBundleEndpointIdLength (uint64_t length)
{
  m_sourceEndpointLength = length;
}

uint64_t
BundleStatusReport::GetSourceBundleEndpointIdLength () const
{
  return m_sourceEndpointLength;
}
  
void
BundleStatusReport::SetSourceBundleEndpointId (BundleEndpointId sourceEndpoint)
{
  m_sourceEndpoint = sourceEndpoint;
}

BundleEndpointId
BundleStatusReport::GetSourceBundleEndpointId () const
{
  return m_sourceEndpoint;
}

void
BundleStatusReport::Print (std::ostream &os) const
{
  os << "BUNDLE STATUS REPORT SAYS: IMPLEMENT MY PRINT FUNCTION";
}

uint32_t
BundleStatusReport::GetSerializedSize (void) const
{
  uint32_t size = this->AdministrativeRecord::GetSerializedSize ();
  size += 2; // Reason and the flags

  if (m_flag == RECORD_IS_FOR_A_FRAGMENT) 
    {
      size += Sdnv::EncodingLength (m_fragmentOffset);
      size += Sdnv::EncodingLength (m_fragmentLength);
    }
  
  if (GetReceivedBundle ())
    {
      size += Sdnv::EncodingLength (m_receipt.GetSeconds ());
    }

  if (GetAcceptedCustodyOfBundle ())
    {
      size += Sdnv::EncodingLength (m_custodyAcceptance.GetSeconds ());
    }
    
  if (GetForwardedTheBundle ())
    {
      size += Sdnv::EncodingLength (m_forwarding.GetSeconds ());
    }
  
  if (GetDeliveredBundle ())
    {
      size += Sdnv::EncodingLength (m_delivery.GetSeconds ());
    }
  
  if (GetDeletedTheBundle ())
    {
      size += Sdnv::EncodingLength (m_deletion.GetSeconds ());
    }
  
  return size;
}

void
BundleStatusReport::Serialize (uint8_t *buffer) const
{
  uint32_t i = 0;

  this->AdministrativeRecord::Serialize (buffer);
  i+= this->AdministrativeRecord::GetSerializedSize ();
  
  buffer[i] = m_reason;
  i += 1;
  
  buffer[i] = m_statusFlags.GetStatusFlags ();
  i += 1;
  
  if (m_flag == RECORD_IS_FOR_A_FRAGMENT) 
    {
      Sdnv::Encode (m_fragmentOffset, buffer+i);
      i += Sdnv::EncodingLength (m_fragmentOffset);
      
      Sdnv::Encode (m_fragmentLength, buffer+i);
      i += Sdnv::EncodingLength (m_fragmentLength);
    }
  
  uint64_t tmp;
  if (GetReceivedBundle ())
    {
      tmp = m_receipt.GetSeconds ();
      Sdnv::Encode (tmp, buffer+i);
      i += Sdnv::EncodingLength (tmp);
    }

  if (GetAcceptedCustodyOfBundle ())
    {
      tmp = m_custodyAcceptance.GetSeconds ();
      Sdnv::Encode (tmp, buffer+i);
      i += Sdnv::EncodingLength (tmp);
    }
    
  if (GetForwardedTheBundle ())
    {
      tmp = m_forwarding.GetSeconds ();
      Sdnv::Encode (tmp, buffer+i);
      i += Sdnv::EncodingLength (tmp);
    }
  
  if (GetDeliveredBundle ())
    {
      tmp = m_delivery.GetSeconds ();
      Sdnv::Encode (tmp, buffer+i);
      i += Sdnv::EncodingLength (tmp);
    }
  
  if (GetDeletedTheBundle ())
    {
      tmp = m_deletion.GetSeconds ();
      Sdnv::Encode (tmp, buffer+i);
    }

  tmp = m_creationTimestamp.GetSeconds ();
  Sdnv::Encode (tmp, buffer+i);
  i += Sdnv::EncodingLength (tmp);

  tmp = m_creationTimestamp.GetSequence ();
  Sdnv::Encode (tmp, buffer+i);
  i += Sdnv::EncodingLength (tmp);

  Sdnv::Encode (m_sourceEndpointLength, buffer+i);
  i += Sdnv::EncodingLength (m_sourceEndpointLength);

  m_sourceEndpoint.Serialize (buffer+i);
}

BundleStatusReport
BundleStatusReport::Deserialize (uint8_t const*buffer)
{
  uint32_t i = 0;
  AdministrativeRecord adminRecord = AdministrativeRecord::Deserialize (buffer);
  i += adminRecord.GetSerializedSize ();  


  BundleStatusReport statusReport = BundleStatusReport ();
  statusReport.SetRecordType (adminRecord.GetRecordType ());
  statusReport.SetAdminRecordFlag (adminRecord.GetAdminRecordFlag ());

  statusReport.SetReasonCode ((BundleStatusReportReason) buffer[i]);
  i += 1;
  
  statusReport.SetStatusFlags (BundleStatusReportStatusFlags (buffer[i]));
  i+= 1;

  if (statusReport.GetAdminRecordFlag () == RECORD_IS_FOR_A_FRAGMENT) 
    {
      statusReport.SetFragmentOffset(Sdnv::Decode (buffer+i));
      i += Sdnv::EncodingLength (statusReport.GetFragmentOffset());
      
      statusReport.SetFragmentLength (Sdnv::Decode (buffer+i));
      i += Sdnv::EncodingLength (statusReport.GetFragmentLength ());
    }
  
  if (statusReport.GetReceivedBundle ())
    {
      statusReport.SetTimeOfReception (Time (Sdnv::Decode (buffer+i)));
      i += Sdnv::EncodingLength (statusReport.GetTimeOfReception ().GetSeconds ());
    }

  if (statusReport.GetAcceptedCustodyOfBundle ())
    {
      statusReport.SetTimeOfCustodyAcceptance (Time (Sdnv::Decode (buffer+i)));
      i += Sdnv::EncodingLength (statusReport.GetTimeOfCustodyAcceptance ().GetSeconds ());
    }
    
  if (statusReport.GetForwardedTheBundle ())
    {
      statusReport.SetTimeOfForwarding (Time (Sdnv::Decode (buffer+i)));
      i += Sdnv::EncodingLength (statusReport.GetTimeOfForwarding ().GetSeconds ());
    }
  
  if (statusReport.GetDeliveredBundle ())
    {
      statusReport.SetTimeOfDelivery (Time (Sdnv::Decode (buffer+i)));
      i += Sdnv::EncodingLength (statusReport.GetTimeOfDelivery ().GetSeconds ());
    }
  
  if (statusReport.GetDeletedTheBundle ())
    {
      statusReport.SetTimeOfDeletion (Time (Sdnv::Decode (buffer+i)));
      i += Sdnv::EncodingLength (statusReport.GetTimeOfDeletion ().GetSeconds ());
    }

  uint64_t time = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (time);

  uint64_t sequence = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (sequence);

  statusReport.SetCreationTimestamp (CreationTimestamp (time,sequence));

  statusReport.SetSourceBundleEndpointIdLength (Sdnv::Decode (buffer+i));
  i += Sdnv::EncodingLength (statusReport.GetSourceBundleEndpointIdLength());

  statusReport.SetSourceBundleEndpointId (BundleEndpointId::Deserialize (buffer+i));

  return statusReport;
}

}} // namespace bundleProtocol, ns3
