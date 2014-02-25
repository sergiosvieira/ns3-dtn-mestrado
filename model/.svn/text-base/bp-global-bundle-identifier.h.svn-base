/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_GLOBAL_BUNDLE_IDENTIFIER_H
#define BP_GLOBAL_BUNDLE_IDENTIFIER_H

#include "ns3/ptr.h"
#include "ns3/packet.h"

#include "bp-creation-timestamp.h"
#include "bp-bundle-endpoint-id.h"

namespace ns3 {
namespace bundleProtocol {

class Bundle;

/**
 * \ingroup bundle
 *
 * \brief Uniquely identifies a bundle.
 *
 *
 */
class GlobalBundleIdentifier
{
 public:
  GlobalBundleIdentifier ();
  GlobalBundleIdentifier (Ptr<Packet> bundle);
  GlobalBundleIdentifier (Ptr<Bundle> bundle);
  GlobalBundleIdentifier (const BundleEndpointId& sourceEid, const CreationTimestamp& creationTimestamp);
  GlobalBundleIdentifier (const GlobalBundleIdentifier& gbid);
  ~GlobalBundleIdentifier ();
  
  void SetSourceEndpoint (const BundleEndpointId& sourceEid);
  void SetCreationTimestamp (const CreationTimestamp& creationTimestamp);
  BundleEndpointId GetSourceEid () const;
  CreationTimestamp GetCreationTimestamp () const;

  uint32_t GetSerializedSize () const;
  
  void Serialize (uint8_t *buffer) const;
  static GlobalBundleIdentifier Deserialize (uint8_t const*buffer);
  
  bool operator == (const GlobalBundleIdentifier& other) const;
  bool operator != (const GlobalBundleIdentifier& other) const;
  bool operator < (const GlobalBundleIdentifier& other) const;
   
 private:
  BundleEndpointId m_eid;
  CreationTimestamp m_creationTimestamp;
  
  friend ostream& operator<< (ostream& os, const GlobalBundleIdentifier& gbid);

};
ostream& operator<< (ostream& os, const GlobalBundleIdentifier& gbid);

}} // namespace bundleProtocol, ns3

#endif /* BP_GLOBAL_BUNDLE_IDENTIFIER_H */
