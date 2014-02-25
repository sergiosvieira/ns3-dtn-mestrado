/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_CONTACT_H
#define BP_CONTACT_H

#include <deque>

#include "ns3/ref-count-base.h"
#include "ns3/ptr.h"
#include "bp-convergence-layer-agent.h"
#include "bp-global-bundle-identifier.h"
#include "bp-bundle.h"

namespace ns3 {
namespace bundleProtocol {

class Link;
class Bundle;


/**
 * \ingroup utility
 *
 * \brief Represents a started contact between to nodes.
 *
 *
 */

class Contact : public Object
{
public:
  static TypeId GetTypeId (void);
  Contact ();
  ~Contact ();
  
  void EnqueueBundle (Ptr<Bundle> bundle);
  Ptr<Bundle> DequeueBundle ();
  Ptr<Bundle> DequeueBundle (const GlobalBundleIdentifier& gbid);

  deque<Ptr<Bundle> > GetQueuedBundles ();
  uint32_t GetNumQueuedBundles ();

  void IncreaseRetransmissions ();
  void ResetRetransmissions ();
  void SetRetransmissions (uint32_t rt);
  uint32_t GetRetransmissions () const;
protected:
  virtual void DoDispose ();
  deque<Ptr<Bundle> > m_queue;
  uint32_t m_retransmissions;

  struct MatchingGbid : public unary_function <Ptr<Bundle>, bool>
  {
    GlobalBundleIdentifier m_gbid;
    
    MatchingGbid (GlobalBundleIdentifier gbid)
      : m_gbid (gbid)
  {}
    
    bool operator () (Ptr<Bundle> bundle)
    {
      return m_gbid == bundle->GetBundleId ();
    }
  };

};

}} // namespace bundleProtocol, ns3

#endif /* BP_CONTACT_H */
