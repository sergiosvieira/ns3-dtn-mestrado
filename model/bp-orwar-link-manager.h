/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_ORWAR_LINK_MANAGER_H
#define BP_ORWAR_LINK_MANAGER_H

#include <vector>
#include <deque>

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"

#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"

#include "bp-link-manager.h"
#include "bp-link.h"
#include "bp-contact.h"
#include "bp-orwar-contact.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-hello-header.h"

namespace ns3 {
namespace bundleProtocol {

class BundleRouter;

/**
 * \ingroup bundleRouter
 *
 * \brief Keeps track of the current local topology for the ORWAR routing scheme.
 *
 */

class OrwarLinkManager : public LinkManager
{
public:
  static TypeId GetTypeId (void);

  OrwarLinkManager ();
  virtual ~OrwarLinkManager ();

  void DiscoveredLink (Ptr<Packet> hello, Address fromAddress);

  Time GetSetupHoldTime () const;
  Time GetCwMaxHoldTime () const;
  void AddLink (Ptr<Link> link);
  void CloseLink (Ptr<Link> link);

  virtual Ptr<Link> FindLink (const BundleEndpointId& eid);
  virtual Ptr<Link> FindLink (const Mac48Address& address);
  virtual Ptr<Link> FindLink (Ptr<Link> link);

  virtual Links GetAllLinks ();
  virtual Links GetReadyLinks ();
  virtual Links GetConnectedLinks ();

  void ContactSetup (Ptr<Link> link, bool sender);
  void FinishedSetup (Ptr<Link> link);
protected:
  virtual void RecalculateContactWindows ();
  virtual void DoDispose ();
  virtual void CheckIfExpired (Ptr<Link> link);

  Time m_setupHold;
  Time m_cwMaxHold;

struct KeepReady : public unary_function <Ptr<Link>, bool>
{
  bool operator() (Ptr<Link> link)
  {
    if (link->GetState () == LINK_CONNECTED)
      {
        Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
        if (oc->GetState () == READY)
          {
            return false;
          }
        else
          {
            return true;
          }
      }
      else
        {
          return true;
        }
  }
};

struct KeepHasCw : public unary_function <Ptr<Link>, bool>
{
  bool operator() (Ptr<Link> l)
  {
    Ptr<OrwarLink> link = dynamic_cast<OrwarLink *> (PeekPointer (l));
    return !link->HaveCw ();
  }
};

};

}} // namespace bundleProtocol, ns3

#endif /* BP_ORWAR_LINK_MANAGER_H */
