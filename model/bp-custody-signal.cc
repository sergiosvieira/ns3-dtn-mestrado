/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-administrative-record.h"
#include "bp-custody-signal.h"
#include "bp-sdnv.h"
#include "bp-utility.h"
#include "bp-header.h"

namespace ns3 {
namespace bundleProtocol {

CustodySignal::CustodySignal (Ptr<Bundle> bundle, CustodySignalReason reason, bool succeeded)
  : AdministrativeRecord (bundle, CUSTODY_SIGNAL), 
    m_succeeded (succeeded), 
    m_reason (reason),
    m_timeOfSignal (),
    m_fragmentOffset (0),
    m_fragmentLength (0),
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

CustodySignal::CustodySignal (Ptr<Bundle> bundle, bool succeeded)
  : AdministrativeRecord (bundle, CUSTODY_SIGNAL), 
    m_succeeded (succeeded), 
    m_reason (CUSTODY_NO_ADDITIONAL_INFORMATION),
    m_timeOfSignal () 
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

CustodySignal::CustodySignal ()
  : AdministrativeRecord (),
    m_succeeded (false),
    m_reason (CUSTODY_NO_ADDITIONAL_INFORMATION),
    m_timeOfSignal (),
    m_fragmentOffset (0),
    m_fragmentLength (0),
    m_creationTimestamp (),
    m_sourceEndpointLength (9),
    m_sourceEndpoint (BundleEndpointId::GetAnyBundleEndpointId ())
{}

CustodySignal::~CustodySignal ()
{}

void
CustodySignal::SetCustodyTransferSucceeded (bool b)
{
  m_succeeded = b;
}

bool
CustodySignal::GetCustodyTransferSucceeded () const
{
  return m_succeeded;
}
  
void
CustodySignal::SetReasonCode (CustodySignalReason reason)
{
  m_reason = reason;
}

CustodySignalReason
CustodySignal::GetReasonCode () const
{
  return m_reason;
}

void
CustodySignal::SetFragmentOffset (uint64_t offset)
{
  m_fragmentOffset = offset;
}
 
uint64_t
CustodySignal::GetFragmentOffset () const
{
  return m_fragmentOffset;
}
  
void
CustodySignal::SetFragmentLength (uint64_t length)
{
  m_fragmentLength = length;
}
 
uint64_t
CustodySignal::GetFragmentLength () const
{
  return m_fragmentLength;
}

void
CustodySignal::SetTimeOfSignal (Time timeOfSignal)
{
  m_timeOfSignal = timeOfSignal;
}
 
Time
CustodySignal::GetTimeOfSignal () const
{
  return m_timeOfSignal;
}

void
CustodySignal::SetCreationTimestamp (CreationTimestamp creationTimestamp)
{
  m_creationTimestamp = creationTimestamp;
}

CreationTimestamp
CustodySignal::GetCreationTimestamp () const
{
  return m_creationTimestamp;
}
 
uint64_t
CustodySignal::GetCreationTimestampTime () const
{
  return m_creationTimestamp.GetSeconds ();
}
 
uint64_t
CustodySignal::GetCreationTimestampSequence () const
{
  return  m_creationTimestamp.GetSequence ();
}

void
CustodySignal::SetSourceBundleEndpointIdLength (uint64_t length)
{
  m_sourceEndpointLength = length;
}

uint64_t
CustodySignal::GetSourceBundleEndpointIdLength () const
{
  return m_sourceEndpointLength;
}
  
void
CustodySignal::SetSourceBundleEndpointId (BundleEndpointId sourceEndpoint)
{
  m_sourceEndpoint = sourceEndpoint;
}

BundleEndpointId
CustodySignal::GetSourceBundleEndpointId () const
{
  return m_sourceEndpoint;
}

// FIXME Implementera Print
void
CustodySignal::Print (std::ostream &os) const
{
  os << "CUSTODY SIGNAL SAYS: IMPLEMENT MY PRINT FUNCTION";
}
 
uint32_t
CustodySignal::GetSerializedSize (void) const
{
  uint32_t size = this->AdministrativeRecord::GetSerializedSize ();
  size +=  1; // status
  if(m_flag == RECORD_IS_FOR_A_FRAGMENT) 
    {
      size += Sdnv::EncodingLength (m_fragmentOffset);
      size += Sdnv::EncodingLength (m_fragmentLength);
    }  
  size += Sdnv::EncodingLength (m_creationTimestamp.GetSeconds ());
  size += Sdnv::EncodingLength (m_creationTimestamp.GetSequence ());
  size += Sdnv::EncodingLength (m_sourceEndpointLength);
  size += m_sourceEndpoint.GetSerializedSize ();
  return size;
}
 
void
CustodySignal::Serialize (uint8_t *buffer) const
{
  uint32_t i = 0;

  this->AdministrativeRecord::Serialize (buffer);
  i+= this->AdministrativeRecord::GetSerializedSize ();

  buffer[i] = ((m_succeeded << 7) | m_reason);
  i += 1;

  if(m_flag == RECORD_IS_FOR_A_FRAGMENT)
    {
      Sdnv::Encode (m_fragmentOffset, buffer+i);
      i += Sdnv::EncodingLength (m_fragmentOffset);
      
      Sdnv::Encode (m_fragmentLength, buffer+i);
      i += Sdnv::EncodingLength (m_fragmentLength);
    }

  uint64_t tmp = m_creationTimestamp.GetSeconds ();
  Sdnv::Encode (tmp, buffer+i);
  i += Sdnv::EncodingLength (tmp);

  tmp = m_creationTimestamp.GetSequence ();
  Sdnv::Encode (tmp, buffer+i);
  i += Sdnv::EncodingLength (tmp);

  Sdnv::Encode (m_sourceEndpointLength, buffer+i);
  i += Sdnv::EncodingLength (m_sourceEndpointLength);

  m_sourceEndpoint.Serialize (buffer+i);
}
 
CustodySignal
CustodySignal::Deserialize (uint8_t const*buffer)
{
  uint32_t i = 0;
  AdministrativeRecord adminRecord = AdministrativeRecord::Deserialize (buffer);
  i += adminRecord.GetSerializedSize ();  

  CustodySignal custodySignal = CustodySignal ();
  custodySignal.SetRecordType (adminRecord.GetRecordType ());
  custodySignal.SetAdminRecordFlag (adminRecord.GetAdminRecordFlag ());

  uint8_t status = buffer[i];
  custodySignal.SetCustodyTransferSucceeded ((status & 0x80) == 0x80);
  CustodySignalReason reason = (CustodySignalReason) (status & 0x7F);
  custodySignal.SetReasonCode (reason);
  i += 1;

  if (custodySignal.GetAdminRecordFlag () == RECORD_IS_FOR_A_FRAGMENT)
    {
      custodySignal.SetFragmentOffset(Sdnv::Decode (buffer+i));
      i += Sdnv::EncodingLength (custodySignal.GetFragmentOffset());
      
      custodySignal.SetFragmentLength (Sdnv::Decode (buffer+i));
      i += Sdnv::EncodingLength (custodySignal.GetFragmentLength ());
    }
  
  custodySignal.SetTimeOfSignal (Time (Sdnv::Decode (buffer+i)));
  i += Sdnv::EncodingLength (custodySignal.GetTimeOfSignal ().GetSeconds ());
  
  uint64_t time = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (time);

  uint64_t sequence = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (sequence);

  custodySignal.SetCreationTimestamp (CreationTimestamp (time,sequence));

  custodySignal.SetSourceBundleEndpointIdLength (Sdnv::Decode (buffer+i));
  i += Sdnv::EncodingLength (custodySignal.GetSourceBundleEndpointIdLength());

  custodySignal.SetSourceBundleEndpointId (BundleEndpointId::Deserialize (buffer+i));

  return custodySignal;
}

}} // Namespace bundleProtocol, ns3
