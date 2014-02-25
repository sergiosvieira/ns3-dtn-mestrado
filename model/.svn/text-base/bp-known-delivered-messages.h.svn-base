/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_KNOWN_DELIVERED_MESSAGES_H
#define BP_KNOWN_DELIVERED_MESSAGES_H

#include <map>
#include <list>
#include <functional>

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"

#include "bp-bundle.h"
#include "bp-global-bundle-identifier.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

typedef pair<GlobalBundleIdentifier, uint64_t> KdmPair;
typedef list<GlobalBundleIdentifier> KdmList; 

/**
 * \ingroup orwarRouter
 *
 * \brief A log of known delivered messages.
 *
 *
 */

class KnownDeliveredMessages
{
private:
typedef map<GlobalBundleIdentifier, uint64_t> KdmMap;

struct MatchingGbid : public unary_function <KdmPair, bool>
{
  GlobalBundleIdentifier m_gbid;
  MatchingGbid (const GlobalBundleIdentifier& gbid)
    : m_gbid (gbid)
  {}
  
  bool
  operator () (const KdmPair& pair)
  {
    return m_gbid == pair.first;
  }
};

public:
  KnownDeliveredMessages ();
  ~KnownDeliveredMessages ();
  
  bool IsInitiator () const;
  void SetIsInitiator (bool isSender);

  void Insert (Ptr<Bundle> bundle);
  void Insert (const KdmPair& kp);
  void Merge (const KnownDeliveredMessages& kdm);
  bool Has (const GlobalBundleIdentifier& gbid) const;
  
  KdmList Get () const;
  uint32_t NEntries () const;

  void RemoveExpiredBundles ();
  void Clear ();

  uint32_t GetSerializedSize () const;
  uint32_t Serialize (uint8_t *buffer) const;
  static KnownDeliveredMessages Deserialize (uint8_t const*buffer);

  static uint32_t GetKdmPairSerializedSize (const KdmPair& pair);
  uint32_t SerializeKdmPair (uint8_t *buffer, const KdmPair& pair) const;
  static KdmPair DeserializeKdmPair (uint8_t const*buffer);
  
private:
  bool TimeExpired (const KdmPair& p) const;
  KdmMap m_kdm;
  bool m_isInitiator;
};

}} // namespace bundleProtocol, ns3



#endif /* BP_KNOWN_DELIVERED_MESSAGES_H */
