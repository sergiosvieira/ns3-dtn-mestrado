/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_HEADER_H
#define BP_HEADER_H

#include "bp-creation-timestamp.h"
#include "bp-global-bundle-identifier.h"
#include "ns3/header.h"
#include "ns3/nstime.h"
#include <vector>
#include "bp-dictionary.h"
#include "vector2d.h"

namespace ns3 {
namespace bundleProtocol {

enum BundlePriority {
  BULK = 1,
  NORMAL = 2,
  EXPEDITED = 3,
  UNKOWN_PRIORITY
};


struct PrimaryProcessingControlFlags
{
uint64_t m_flags;
  
PrimaryProcessingControlFlags ()
  :  m_flags (0)
{}

PrimaryProcessingControlFlags (uint64_t flags)
  :  m_flags (flags)
{}

void 
SetProccesingControlFlags (uint64_t flags)
{
  m_flags = flags;
}

uint64_t 
GetProcessingControlFlags () const
{
  return m_flags;
}

void 
SetFragment (bool val)
{
  SetBit (m_flags, 0, val);
}

bool 
IsFragment () const
{
  return GetBit (m_flags, 0);
}

void 
SetAdministrativeRecord (bool val)
{
  SetBit (m_flags, 1, val);
}

bool 
IsAdministrativeRecord () const
{
  return GetBit (m_flags, 1);
}

void 
SetDoNotFragment (bool val)
{
  SetBit (m_flags, 2, val);
}

bool 
IsDoNotFragment () const
{
  return GetBit (m_flags, 2);
}

void 
SetCustodyTransferRequested (bool val)
{
  SetBit (m_flags, 3, val);
}

bool 
IsCustodyTransferRequested () const
{
  return GetBit (m_flags, 3);
}

void 
SetSingletonEndpoint (bool val)
{
  SetBit (m_flags, 4, val);
}

bool 
IsSingletonEndpoint () const
{
  return GetBit (m_flags, 4);
}

void 
SetApplicationAckRequested (bool val)
{
  SetBit (m_flags, 5, val);
}

bool 
IsApplicationAckRequested() const
{
  return GetBit (m_flags, 5);
}

void 
SetPriority (const BundlePriority& priority)
{
  if (priority == NORMAL)
    {
      SetBit (m_flags, 7, false);
      SetBit (m_flags, 8, true);
    }
  else if (priority == EXPEDITED)
    {
      SetBit (m_flags, 7, true);
      SetBit (m_flags, 8, true);
    }
  else
    {
      SetBit (m_flags, 7, true);
      SetBit (m_flags, 8, false);
    }

}

BundlePriority 
GetPriority () const
{
  bool b7 = GetBit (m_flags, 7);
  bool b8 = GetBit (m_flags, 8);
  if (b8 && b7)
    {
      return EXPEDITED;
    }
  else if (b8 && !b7)
    {
      return NORMAL;
    }
  else if (!b8 && b7)
    {
      return BULK;
    }
  else
    {
      return BULK;
    }  /* sergiosvieira */;
  void SetPos(const double &x, const double &y);
  Vector2d GetPos();
  /* sergiosvieira */
}

void 
SetReportBundleReception (bool val)
{
  SetBit (m_flags, 14, val);
}
  
bool 
GetReportBundleReception () const
{
  return GetBit (m_flags, 14);
}

void 
SetReportCustodyAcceptance (bool val)
{
  SetBit (m_flags, 15, val);
}
  
bool 
GetReportCustodyAcceptance () const
{
  return GetBit (m_flags, 15);
}

void 
SetReportBundleForwarding (bool val)
{
  SetBit (m_flags, 16, val);
}
  
bool 
GetReportBundleForwarding () const
{
  return GetBit (m_flags, 16);
}

void
SetReportBundleDelivery (bool val)
{
  SetBit (m_flags, 17, val);
}
  
bool 
GetReportBundleDelivery () const
{
  return GetBit (m_flags, 17);
}

void 
SetReportBundleDeletion (bool val)
{
  SetBit (m_flags, 18, val);
}
  
bool 
GetReportBundleDeletion () const
{
  return GetBit (m_flags, 18);
}

};
 
/**
 * \ingroup bundle
 *
 * \brief Holds the primary controlflags and information of a bundle.
 *
 *
 */

class PrimaryBundleHeader : public Header
{
public:
  PrimaryBundleHeader ();
  virtual ~PrimaryBundleHeader ();
  
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

  /*
  void SetReportToEndpoint (const BundleEndpointId& reportToEid);
  void RemoveReportToEndpoint ();
  BundleEndpointId GetReportToEndpoint () const;
  */
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
  /* sergiosvieira */;
  void SetPos(const double &x, const double &y);
  Vector2d GetPos();
  /*Joao*/
  void SetGlobalId(const uint64_t &id);
  uint64_t GetGlobalId()const;
  void SetCustody(GlobalBundleIdentifier gbid);
  GlobalBundleIdentifier GetCustody();

  /*Joao*/
  /* sergiosvieira */
private:
  /* sergiosvieira */
  Vector2d m_pos;
  /* sergiosvieira */


  void CalcBlockLength ();
  uint8_t m_version;
  uint64_t m_replicationFactor;
  PrimaryProcessingControlFlags m_processingControlFlags;
  uint64_t m_blockLength;

  CreationTimestamp m_creationTimestamp;
  uint64_t m_lifetime;
  /*Joao*/
  uint64_t m_bundle_global_id;
  GlobalBundleIdentifier m_custody;
  /*Joao*/
  Dictionary m_dictionary;
  uint64_t m_fragmentOffset;
  uint64_t m_totalApplicationDataUnitLength;

public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
};

struct CanonicalProcessingControlFlags 
{
uint64_t m_flags;

CanonicalProcessingControlFlags () 
  : m_flags (0)
{}
  
CanonicalProcessingControlFlags (uint64_t flags)
  : m_flags (flags)
{}

void 
SetCanonicalProcessingControlFlags (uint64_t flags)
{
  m_flags = flags;
}

uint64_t
GetCanonicalProcessingControlFlags () const
{
  return m_flags;
}
  
void 
SetMustBeReplicated (bool val)
{
  SetBit (m_flags, 0, val);
}

bool 
IsMustBeReplicated () const
{
  return GetBit (m_flags, 0);
}

void 
SetStatusReport (bool val)
{
  SetBit (m_flags, 1, val);
}

bool 
IsStatusReport () const
{
  return GetBit (m_flags, 1);
}

void 
SetDeleteBundle (bool val)
{
  SetBit (m_flags, 2, val);
}

bool 
IsDeleteBundle () const
{
  return GetBit (m_flags, 2);
}

void SetLastBlock (bool val)
{
  SetBit (m_flags, 3, val);
}

bool 
IsLastBlock () const
{
  return GetBit (m_flags, 3);
}

void 
SetDiscardBlock (bool val)
{
  SetBit (m_flags, 4, val);
}

bool 
IsDiscardBlock () const
{
  return GetBit (m_flags, 4);
}

void 
SetForwarded (bool val)
{
  SetBit (m_flags, 5, val);
}

bool 
IsForwarded () const
{
  return GetBit (m_flags, 5);
}

void 
SetContainsEid (bool val)
{
  SetBit (m_flags, 6, val);
}

bool 
IsContainsEid () const
{
  return GetBit (m_flags, 6);
}

};




enum BlockType {
  PAYLOAD_BLOCK = 1,
  KNOWN_DELIVERED_MESSAGES_BLOCK = 192,
  CONTACT_WINDOW_INFORMATION_BLOCK = 193,
};

/**
 * \ingroup bundle
 *
 * \brief Holds additional processflags and information of a bundle.
 *
 *
 */

class CanonicalBundleHeader : public Header
{
public:
  CanonicalBundleHeader ();
  CanonicalBundleHeader (BlockType blockType);
  virtual ~CanonicalBundleHeader ();
  void SetBlockType (BlockType blockType);
  BlockType GetBlockType () const;
  void SetProcessingControlFlags (CanonicalProcessingControlFlags flags);
  CanonicalProcessingControlFlags GetProcessingControlFlags () const;
  void SetMustBeReplicated (bool val);
  bool IsMustBeReplicated () const;
  void SetStatusReport (bool val);
  bool IsStatusReport () const;
  void SetDeleteBundle (bool val);
  bool IsDeleteBundle () const;
  void SetLastBlock (bool val);
  bool IsLastBlock () const;
  void SetDiscardBlock (bool val);
  bool IsDiscardBlock () const;
  void SetForwarded (bool val);
  bool IsForwarded () const;
  void SetContainsEid (bool val);
  bool IsContainsEid () const;
  uint64_t GetEidReferenceCount () const;
  void SetBlockLength (uint64_t length);
  uint64_t GetBlockLength () const;

private:
  BlockType m_blockType;
  CanonicalProcessingControlFlags m_processingControlFlags;
  uint64_t m_blockLength;
  uint64_t m_eidReferenceCount;

public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  bool operator== (const CanonicalBundleHeader& other) const;
};

std::ostream& operator<< (std::ostream& os, PrimaryProcessingControlFlags const& flags);
std::ostream& operator<< (std::ostream& os, CanonicalProcessingControlFlags const& flags);

}} // namespace bundleProtocol, ns3

#endif /* BP_HEADER_H */
