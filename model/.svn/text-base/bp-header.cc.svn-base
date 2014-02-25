/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-header.h"
#include "bp-sdnv.h"
#include "bp-utility.h"
#include <algorithm>

namespace ns3 {
namespace bundleProtocol {

// ***** Primary Bundle Header ********
NS_OBJECT_ENSURE_REGISTERED (PrimaryBundleHeader);

PrimaryBundleHeader::PrimaryBundleHeader () 
  : m_version (6), m_replicationFactor (1), m_processingControlFlags (),
    m_blockLength (0), m_creationTimestamp (), m_lifetime (0), m_dictionary (),
    m_fragmentOffset (0), m_totalApplicationDataUnitLength (0)
    
{}

PrimaryBundleHeader::~PrimaryBundleHeader ()
{}

void 
PrimaryBundleHeader::SetVersion (uint8_t version)
{
  m_version = version;
}

uint8_t 
PrimaryBundleHeader::GetVersion () const
{
  return m_version;
}

void
PrimaryBundleHeader::SetProcessingControlFlags (PrimaryProcessingControlFlags flags)
{
  m_processingControlFlags = flags;
}

PrimaryProcessingControlFlags
PrimaryBundleHeader::GetProcessingControlFlags () const
{
  return m_processingControlFlags;
}

void 
PrimaryBundleHeader::SetFragment (bool val)
{
  m_processingControlFlags.SetFragment (val);
}

bool 
PrimaryBundleHeader::IsFragment () const
{
  return m_processingControlFlags.IsFragment ();
}

void 
PrimaryBundleHeader::SetAdministrativeRecord (bool val)
{
  m_processingControlFlags.SetAdministrativeRecord (val);
}

bool 
PrimaryBundleHeader::IsAdministrativeRecord () const
{
  return m_processingControlFlags.IsAdministrativeRecord ();
}

void 
PrimaryBundleHeader::SetDoNotFragment (bool val)
{
  m_processingControlFlags.SetDoNotFragment (val);
}

bool 
PrimaryBundleHeader::IsDoNotFragment () const
{
  return m_processingControlFlags.IsDoNotFragment ();
}

void 
PrimaryBundleHeader::SetCustodyTransferRequested (bool val)
{
  m_processingControlFlags.SetCustodyTransferRequested (val);
}

bool 
PrimaryBundleHeader::IsCustodyTransferRequested () const
{
  return m_processingControlFlags.IsCustodyTransferRequested ();
}

void 
PrimaryBundleHeader::SetSingletonEndpoint (bool val)
{
  m_processingControlFlags.SetSingletonEndpoint (val);
}

bool 
PrimaryBundleHeader::IsSingletonEndpoint () const
{
  return m_processingControlFlags.IsSingletonEndpoint ();
}

void 
PrimaryBundleHeader::SetApplicationAckRequested (bool val)
{
  m_processingControlFlags.SetApplicationAckRequested (val);
}

bool 
PrimaryBundleHeader::IsApplicationAckRequested () const
{
  return m_processingControlFlags.IsApplicationAckRequested ();
}

void
PrimaryBundleHeader::SetReplicationFactor (uint64_t replicationFactor)
{
  m_replicationFactor = replicationFactor;
}

uint64_t
PrimaryBundleHeader::GetReplicationFactor () const
{
  return m_replicationFactor;
}

void 
PrimaryBundleHeader::SetPriority (const BundlePriority& priority)
{
  m_processingControlFlags.SetPriority (priority);
}

BundlePriority 
PrimaryBundleHeader::GetPriority () const
{
  return m_processingControlFlags.GetPriority ();
}

void 
PrimaryBundleHeader::SetReportBundleReception (bool val)
{
  m_processingControlFlags.SetReportBundleReception (val);
}
  
bool 
PrimaryBundleHeader::GetReportBundleReception () const
{
  return m_processingControlFlags.GetReportBundleReception ();
}

void 
PrimaryBundleHeader::SetReportCustodyAcceptance (bool val)
{
  m_processingControlFlags.SetReportCustodyAcceptance (val);
}
  
bool 
PrimaryBundleHeader::GetReportCustodyAcceptance () const
{
  return m_processingControlFlags.GetReportCustodyAcceptance ();
}

void 
PrimaryBundleHeader::SetReportBundleForwarding (bool val)
{
  m_processingControlFlags.SetReportBundleForwarding (val);
}
  
bool 
PrimaryBundleHeader::GetReportBundleForwarding () const
{
  return m_processingControlFlags.GetReportBundleForwarding ();
}

void 
PrimaryBundleHeader::SetReportBundleDelivery (bool val)
{
  m_processingControlFlags.SetReportBundleDelivery (val);
}
  
bool 
PrimaryBundleHeader::GetReportBundleDelivery () const
{
  return m_processingControlFlags.GetReportBundleDelivery ();
}

void 
PrimaryBundleHeader::SetReportBundleDeletion (bool val)
{
  m_processingControlFlags.SetReportBundleDeletion (val);
}

bool 
PrimaryBundleHeader::GetReportBundleDeletion () const
{
  return m_processingControlFlags.GetReportBundleDeletion ();
}

uint64_t 
PrimaryBundleHeader::GetBlockLength () const
{
  return m_blockLength;
}
 
void
PrimaryBundleHeader::SetDestinationEndpoint (const BundleEndpointId& destinationEid)
{
  m_dictionary.SetDestinationEndpoint (destinationEid);
  CalcBlockLength ();
}

void 
PrimaryBundleHeader::RemoveDestinationEndpoint () 
{
  m_dictionary.RemoveDestinationEndpoint ();
  CalcBlockLength ();
}

BundleEndpointId
PrimaryBundleHeader::GetDestinationEndpoint () const
{
  return m_dictionary.GetDestinationEndpoint ();
}

void
PrimaryBundleHeader::SetSourceEndpoint (const BundleEndpointId& sourceEid)
{
  m_dictionary.SetSourceEndpoint (sourceEid);
  CalcBlockLength ();
}

void 
PrimaryBundleHeader::RemoveSourceEndpoint () 
{
  m_dictionary.RemoveSourceEndpoint ();
  CalcBlockLength ();
}

BundleEndpointId
PrimaryBundleHeader::GetSourceEndpoint () const
{
  return m_dictionary.GetSourceEndpoint ();
}

  /* 
void
PrimaryBundleHeader::SetReportToEndpoint (const BundleEndpointId& reportToEid)
{
  m_dictionary.SetReportToEndpoint (reportToEid);
  CalcBlockLength ();
}

void 
PrimaryBundleHeader::RemoveReportToEndpoint () 
{
  m_dictionary.RemoveReportToEndpoint ();
  CalcBlockLength ();
}

BundleEndpointId
PrimaryBundleHeader::GetReportToEndpoint () const
{
  return m_dictionary.GetReportToEndpoint ();
}
  */
void 
PrimaryBundleHeader::SetCustodianEndpoint (const BundleEndpointId& custodianEid)
{
  m_dictionary.SetCustodianEndpoint (custodianEid);
  CalcBlockLength ();
}

void 
PrimaryBundleHeader::RemoveCustodianEndpoint () 
{
  m_dictionary.RemoveCustodianEndpoint ();
  CalcBlockLength ();
}

BundleEndpointId
PrimaryBundleHeader::GetCustodianEndpoint () const
{
  return m_dictionary.GetCustodianEndpoint ();
}

void 
PrimaryBundleHeader::SetCreationTimestamp (const CreationTimestamp& timestamp)
{
  m_creationTimestamp = timestamp;
  CalcBlockLength ();
}

uint64_t 
PrimaryBundleHeader::GetCreationTimestampTime () const
{
  return m_creationTimestamp.GetSeconds ();
}

uint64_t 
PrimaryBundleHeader::GetCreationTimestampSequence () const
{
  return m_creationTimestamp.GetSequence ();
}

CreationTimestamp 
PrimaryBundleHeader::GetCreationTimestamp () const
{
  return m_creationTimestamp;
}

void 
PrimaryBundleHeader::SetLifetime (uint64_t lifetime) 
{
  m_lifetime = lifetime;
  CalcBlockLength ();
}

void 
PrimaryBundleHeader::SetLifetime (const Time& lifetime) 
{
  m_lifetime = ceil (lifetime.GetSeconds ());
  CalcBlockLength ();
}

Time 
PrimaryBundleHeader::GetLifetime () const 
{
  return Seconds (m_lifetime);
}

uint64_t 
PrimaryBundleHeader::GetLifetimeSeconds () const 
{
  return m_lifetime;
}

uint64_t 
PrimaryBundleHeader::GetDictionaryLength () const
{
  return m_dictionary.GetSerializedSize ();
}

void 
PrimaryBundleHeader::SetFragmentOffset (int64_t offset)
{
  m_fragmentOffset = offset;
  CalcBlockLength ();
}

uint64_t 
PrimaryBundleHeader::GetFragmentOffset () const
{
  return m_fragmentOffset;
}

void 
PrimaryBundleHeader::SetTotalApplicationLength (uint64_t length)
{
  m_totalApplicationDataUnitLength = length;
  CalcBlockLength ();
}

uint64_t 
PrimaryBundleHeader::GetTotalApplicationLength () const
{
  return m_totalApplicationDataUnitLength;
}

TypeId 
PrimaryBundleHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::PrimaryBundleHeader")
    .SetParent<Header> ()
    .AddConstructor<PrimaryBundleHeader> ()
    ;
  return tid;
}

TypeId 
PrimaryBundleHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void 
PrimaryBundleHeader::Print (std::ostream &os) const
{
  os << "Primary Bundle Block" << endl;
  os << "version = " << "|" << (int) m_version << "| ";
  os << "Proc. Flags = " << m_processingControlFlags << endl;
  os << "Replication factor: " << m_replicationFactor << endl;
  os << "Block length = " << m_blockLength << endl;
  os << m_dictionary << endl;
  os << m_creationTimestamp << endl;
  os << "Lifetime = " << Seconds (m_lifetime).GetSeconds ();
  if (IsFragment ())
    {
      os << endl << "Framgent offset = " << m_fragmentOffset << endl;
      os << "Total applcation data unit length = " << m_totalApplicationDataUnitLength << endl;
    }
}

uint32_t 
PrimaryBundleHeader::GetSerializedSize (void) const
{
  uint32_t size = 0;
  size += 1; // m_version
  size += Sdnv::EncodingLength (m_processingControlFlags.GetProcessingControlFlags ());
  size += Sdnv::EncodingLength (m_blockLength);
  size += Sdnv::EncodingLength (m_replicationFactor);
  size += m_creationTimestamp.GetSerializedSize ();
  size += Sdnv::EncodingLength (m_lifetime);
  /*Joao*/
  size += Sdnv::EncodingLength (m_bundle_global_id);
  size += Sdnv::EncodingLength(m_custody.GetSerializedSize());
  size += m_custody.GetSerializedSize();
  /*Joao*/
  size += Sdnv::EncodingLength (m_dictionary.GetSerializedSize ());
  size += m_dictionary.GetSerializedSize (); // Size of the dictionary
  
  if(IsFragment ()) {
    size += Sdnv::EncodingLength (m_fragmentOffset);
    size += Sdnv::EncodingLength (m_totalApplicationDataUnitLength);
  }
  return size;
}

void 
PrimaryBundleHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_version);
  Sdnv::Encode (m_processingControlFlags.GetProcessingControlFlags (), i);
  Sdnv::Encode (GetBlockLength (), i);
  Sdnv::Encode (m_replicationFactor, i);
  m_creationTimestamp.Serialize (i);
  Sdnv::Encode (m_lifetime, i);
  /*Joao*/
  Sdnv::Encode (m_bundle_global_id, i);
  Sdnv::Encode (m_custody.GetSerializedSize (), i);
  uint8_t bufferc[m_custody.GetSerializedSize ()];
  m_custody.Serialize (bufferc);
  i.Write (bufferc, m_custody.GetSerializedSize ());
  /*Joao*/
  Sdnv::Encode (m_dictionary.GetSerializedSize (), i);
  uint8_t buffer[m_dictionary.GetSerializedSize ()];
  m_dictionary.Serialize (buffer);
  i.Write (buffer, m_dictionary.GetSerializedSize ());

  if(IsFragment ()) {
    Sdnv::Encode (m_fragmentOffset, i);
    Sdnv::Encode (m_totalApplicationDataUnitLength, i);
  }
}

uint32_t 
PrimaryBundleHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_version = i.ReadU8 ();
  m_processingControlFlags = PrimaryProcessingControlFlags (Sdnv::Decode (i));
  m_blockLength  = Sdnv::Decode (i);
  m_replicationFactor = Sdnv::Decode (i);
  m_creationTimestamp = CreationTimestamp::Deserialize (i);
  m_lifetime = Sdnv::Decode (i);
  /*joao*/
  m_bundle_global_id = Sdnv::Decode (i);
  uint64_t custodyLength = Sdnv::Decode (i);
  uint8_t bufferc[custodyLength];
  i.Read (bufferc, custodyLength);
  m_custody = GlobalBundleIdentifier::Deserialize (bufferc);
  /*joao*/
  uint64_t dictionaryLength = Sdnv::Decode (i);
  uint8_t buffer[dictionaryLength];
  i.Read (buffer, dictionaryLength);
  m_dictionary = Dictionary::Deserialize (buffer);
  if(IsFragment ()) {
    m_fragmentOffset = Sdnv::Decode (i);
    m_totalApplicationDataUnitLength = Sdnv::Decode (i);
  }
  return GetSerializedSize ();
}

void 
PrimaryBundleHeader::CalcBlockLength ()
{
  uint64_t temp = GetSerializedSize ();
  temp -= Sdnv::EncodingLength (m_processingControlFlags.GetProcessingControlFlags ());
  temp -= Sdnv::EncodingLength (m_blockLength);
  m_blockLength =  temp;
}

// ***** Canonical Bundle Header ********

NS_OBJECT_ENSURE_REGISTERED (CanonicalBundleHeader);

CanonicalBundleHeader::CanonicalBundleHeader (BlockType blockType)
  : m_blockType (blockType), m_processingControlFlags (0),
    m_blockLength (0), m_eidReferenceCount (0)
{}

CanonicalBundleHeader::CanonicalBundleHeader ()
  : m_blockType (PAYLOAD_BLOCK), m_processingControlFlags (0),
    m_blockLength (0), m_eidReferenceCount (0)
{}

CanonicalBundleHeader::~CanonicalBundleHeader ()
{}

void 
CanonicalBundleHeader::SetBlockType (BlockType blockType) 
{
  m_blockType = blockType;
}
  
BlockType 
CanonicalBundleHeader::GetBlockType () const
{
  return m_blockType;
}

void
CanonicalBundleHeader::SetProcessingControlFlags (CanonicalProcessingControlFlags flags)
{
  m_processingControlFlags = flags;
}

CanonicalProcessingControlFlags
CanonicalBundleHeader::GetProcessingControlFlags () const
{
  return m_processingControlFlags;
}

void 
CanonicalBundleHeader::SetMustBeReplicated (bool val)
{
  m_processingControlFlags.SetMustBeReplicated (val);
}

bool 
CanonicalBundleHeader::IsMustBeReplicated () const
{
  return m_processingControlFlags.IsMustBeReplicated ();
}

void 
CanonicalBundleHeader::SetStatusReport (bool val)
{
  m_processingControlFlags.SetStatusReport (val);
}

bool 
CanonicalBundleHeader::IsStatusReport () const
{
  return m_processingControlFlags.IsStatusReport ();
}

void 
CanonicalBundleHeader::SetDeleteBundle (bool val)
{
  m_processingControlFlags.SetDeleteBundle (val);
}

bool 
CanonicalBundleHeader::IsDeleteBundle () const
{
  return m_processingControlFlags.IsDeleteBundle ();
}

void CanonicalBundleHeader::SetLastBlock (bool val)
{
  m_processingControlFlags.SetLastBlock (val);
}

bool 
CanonicalBundleHeader::IsLastBlock () const
{
  return m_processingControlFlags.IsLastBlock ();
}

void
CanonicalBundleHeader::SetDiscardBlock (bool val)
{
  m_processingControlFlags.SetDiscardBlock (val);
}

bool 
CanonicalBundleHeader::IsDiscardBlock () const
{
  return m_processingControlFlags.IsDiscardBlock ();
}

void 
CanonicalBundleHeader::SetForwarded (bool val)
{
  m_processingControlFlags.SetForwarded (val);
}

bool 
CanonicalBundleHeader::IsForwarded () const
{
  return m_processingControlFlags.IsForwarded ();
}

void 
CanonicalBundleHeader::SetContainsEid (bool val)
{
  m_processingControlFlags.SetContainsEid (val);
}

bool 
CanonicalBundleHeader::IsContainsEid () const
{
  return m_processingControlFlags.IsContainsEid ();
}

uint64_t 
CanonicalBundleHeader::GetEidReferenceCount () const 
{
  return m_eidReferenceCount;
}
  
void 
CanonicalBundleHeader::SetBlockLength (uint64_t length)
{
  m_blockLength = length;
}

uint64_t 
CanonicalBundleHeader::GetBlockLength () const
{
  return m_blockLength;
}

bool
CanonicalBundleHeader::operator== (const CanonicalBundleHeader& other) const
{
  return (m_blockType == other.m_blockType) && 
    (m_processingControlFlags.GetCanonicalProcessingControlFlags () == other.m_processingControlFlags.GetCanonicalProcessingControlFlags ()) &&
    (m_blockLength == other.m_blockLength) &&
    (m_eidReferenceCount == other.m_eidReferenceCount);
}

TypeId 
CanonicalBundleHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::CanonicalBundleHeader")
    .SetParent<Header> ()
    .AddConstructor<CanonicalBundleHeader> ()
    ;
  return tid;
}

TypeId 
CanonicalBundleHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void 
CanonicalBundleHeader::Print (std::ostream &os) const
{
  os << "Canonical Bundle Block" << endl;
  os << "BlockType = " << "|" << m_blockType << "| " << endl;
  os << "Proc. Flags = " << m_processingControlFlags << endl;
 
  if (m_processingControlFlags.IsContainsEid ())
    {
      os << "Eid Ref. count: " << m_eidReferenceCount << endl;
    }
  os << "Block length: " << m_blockLength << endl;
}

uint32_t 
CanonicalBundleHeader::GetSerializedSize (void) const
{
  uint32_t size = 0;
  size += 1; // m_blockType
  size += Sdnv::EncodingLength (m_processingControlFlags.GetCanonicalProcessingControlFlags ());
  size += Sdnv::EncodingLength (m_blockLength);
  
  if(IsContainsEid ()) 
    {
      size += Sdnv::EncodingLength (m_eidReferenceCount);   
    } 
  return size;
}

void 
CanonicalBundleHeader::Serialize (Buffer::Iterator start) const 
{
  Buffer::Iterator i = start;
  i.WriteU8 ((uint8_t) m_blockType);
  Sdnv::Encode (m_processingControlFlags.GetCanonicalProcessingControlFlags (), i);
  Sdnv::Encode (m_blockLength, i);
    
  if(IsContainsEid ()) 
    {
    Sdnv::Encode (m_eidReferenceCount, i);
    }
 }

uint32_t 
CanonicalBundleHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_blockType = (BlockType) i.ReadU8 ();
  m_processingControlFlags = CanonicalProcessingControlFlags (Sdnv::Decode (i));
  m_blockLength =  Sdnv::Decode (i);
  if(IsContainsEid ()) {
    m_eidReferenceCount = Sdnv::Decode (i);
  }

  return GetSerializedSize ();
}

std::ostream& 
operator<< (std::ostream& os, PrimaryProcessingControlFlags const& flags)
{
  os << "|" << flags.IsFragment () << "|" << flags.IsAdministrativeRecord () << "|" << flags.IsDoNotFragment ();
  os << "|" << flags.IsCustodyTransferRequested () << "|" << flags.IsSingletonEndpoint ();
  os << "|" << flags.IsApplicationAckRequested () << "|" << flags.GetPriority () << "|";
  os << "|" << flags.GetReportBundleReception () << "|" << flags.GetReportCustodyAcceptance () << "|";
  os << "|" << flags.GetReportBundleForwarding () << "|";
  os << "|" << flags.GetReportBundleDelivery () << "|";
  os << "|" << flags.GetReportBundleDeletion () << "|";
  return os;
}

std::ostream&
 operator<< (std::ostream& os, CanonicalProcessingControlFlags const& flags)
{
  os << "|" << flags.IsMustBeReplicated () << "|" << flags.IsStatusReport () << "|" << flags.IsDeleteBundle ();
  os << "|" << flags.IsLastBlock () << "|" << flags.IsDiscardBlock ();
  os << "|" << flags.IsForwarded () << "|" << flags.IsContainsEid () << "|";
  return os;
}

/* sergiosvieira */
void PrimaryBundleHeader::SetPos(const double &x, const double &y) {
	m_pos.x = x;
	m_pos.y = y;
}

Vector2d PrimaryBundleHeader::GetPos() {
	return m_pos;
}

/*Joao*/

void PrimaryBundleHeader::SetGlobalId(const uint64_t &id)
{
  m_bundle_global_id = id;
}
uint64_t PrimaryBundleHeader::GetGlobalId()const
{
  return m_bundle_global_id;
}

void PrimaryBundleHeader::SetCustody(GlobalBundleIdentifier gbid)
{
	m_custody = gbid;
}
GlobalBundleIdentifier PrimaryBundleHeader::GetCustody()
{
	return m_custody;
}


/*Joao*/


/* sergiosvieira */

  }} // namespace bundleProtocol, ns3 
