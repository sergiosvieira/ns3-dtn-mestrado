/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_LINK_MANAGER_H
#define BP_LINK_MANAGER_H

#include <map>

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"

#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"

#include "ns3/address.h"
#include "ns3/mac48-address.h"
#include "ns3/packet.h"

#include "bp-link.h"
#include "bp-contact.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-neighbourhood-detection-agent.h"

/* sergiosvieira */
#include "bp-type-tag.h"
#include "bp-rt-prophet-hello.h"
#include "bp-rt-trend-of-delivery-neigh-hello.h"
#include "bp-rt-antmofo-hello.h"
#include "bp-rt-prophetTod-hello.h"
/* sergiosvieira */

namespace ns3 {
namespace bundleProtocol {

class BundleRouter;

typedef pair<Ptr<Link>, Timer> LinkTimer;
typedef map<Ptr<Link>,Timer> LinkMap;

/**
 * \ingroup bundleRouter
 *
 * \brief Keeps track of the current local topology.
 *
 *
 */
class LinkManager : public Object
{
public:
  static TypeId GetTypeId (void);

  LinkManager ();
  virtual ~LinkManager ();

  virtual void SetLinkAvailableCallback (Callback<void, Ptr<Link> > linkAvailable);
  virtual void SetClosedLinkCallback (Callback<void, Ptr<Link> > closedLinkCb);
  virtual void SetCreateLinkCallback (Callback<Ptr<Link>, BundleEndpointId, Address> sayWhatCb);

  virtual void DiscoveredLink (Ptr<Packet> hello, Address fromAddress);
  virtual void AddLink (Ptr<Link> link);

  virtual void RemoveLink (Ptr<Link> link);
  
  virtual bool HasLink (const BundleEndpointId& eid);
  virtual bool HasLink (Ptr<Link> link);

  virtual Ptr<Link> FindLink (const BundleEndpointId& eid);
  virtual Ptr<Link> FindLink (const Mac48Address& address);
  virtual Ptr<Link> FindLink (Ptr<Link> link);


  virtual void OpenLink (Ptr<Link> link);
  virtual void CloseLink (Ptr<Link> link);

  virtual Links GetReadyLinks();
  virtual Links GetAllLinks ();
  virtual Links GetConnectedLinks ();
  virtual Links GetAvailableLinks ();
  virtual Links GetUnavailableLinks ();

protected:
  virtual void Doh () const;
  virtual void DoDispose ();
  virtual void CheckIfExpired (Ptr<Link> link);
  virtual void NotifyLinkIsAvailable (Ptr<Link> link);
  virtual void NotifyClosedLink (Ptr<Link> link);
  void SetupTimer (Ptr<Link> link);
  
  LinkMap m_links;
  Time m_ttl;
  Callback<Ptr<Link>, BundleEndpointId, Address> m_createLinkCb;
  Callback<void, Ptr<Link> > m_linkAvailableCb;
  Callback<void, Ptr<Link> > m_closedLinkCb;
 
  
template <typename Pair>
struct SelectKey
{
  typedef Pair argument_type ;
  typedef typename Pair::first_type result_type ;
  
  const result_type &
  operator()(const argument_type &p) const
  {
    return p.first;
  }
};
  
struct KeepConnected : public unary_function <Ptr<Link>, bool>
{
  bool operator() (Ptr<Link> link)
  {
    if (link->GetState () == LINK_CONNECTED)
      return false;
    else
      return true;
  }
};
 
struct KeepAvailable : public unary_function <Ptr<Link>, bool>
{
  bool operator() (Ptr<Link> link)
  {
    if (link->GetState () == LINK_AVAILABLE)
      return false;
    else
      return true;
  }
};

struct KeepUnavailable : public unary_function <Ptr<Link>, bool>
{
  bool operator() (Ptr<Link> link)
  {
    if (link->GetState ()== LINK_UNAVAILABLE)
      return false;
    else
      return true;
  }
};

};

}} // namespace bundleProtocol, ns3

#endif /* BP_LINK_MANAGER_H */
