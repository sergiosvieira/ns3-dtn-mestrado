/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_LINK_H
#define BP_LINK_H

#include <vector>

#include "ns3/address.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/timer.h"

#include "bp-contact.h"
#include "bp-convergence-layer-agent.h"
#include "bp-bundle-endpoint-id.h"

#include <map>

using namespace std;

namespace ns3 {
namespace bundleProtocol {



class Contact;
class OrwarContact;
class ConvergenceLayerAgent;
struct EidAddress;

enum LinkState {
  LINK_AVAILABLE,
  LINK_UNAVAILABLE,
  LINK_CONNECTED,
  LINK_PAUSED
};

/**
 * \ingroup utility
 *
 * \brief Reprsents a possible link between two nodes.
 *
 *
 */

class Link : public Object
{
public:
  static TypeId GetTypeId (void);
  Link ();
  Link (const BundleEndpointId& eid, const Address& address);
  Link (const EidAddress& ea);
  virtual ~Link ();

  virtual void SetLinkLostCallback (Callback<void, Address > linkLostCb);
  
  void SetRemoteEndpointId (const BundleEndpointId& eid);
  BundleEndpointId GetRemoteEndpointId () const;

  void SetRemoteAddress (const Address& address);
  Address GetRemoteAddress () const;

  Time GetLastHeardFrom () const;

  virtual void SetContact (Ptr<Contact> contact);
  virtual Ptr<Contact> GetContact () const;
  virtual LinkState GetState () const;
  virtual void ChangeState (LinkState state);
  virtual void Open ();
  void UpdateLastHeardFrom ();
 
  operator BundleEndpointId () const;
  operator Address () const;
  bool operator == (const Link& other);
  bool operator != (const Link& other);
  bool operator < (const Link& other);

  /* sergiosvieira */
  // Prophet
  void SetProbability(const float &p);
  double GetProbability();
  void AddNeigh(uint32_t eid, double probability);
  double GetNeigh(uint32_t eid);
  int GetSize();
  /* sergiosvieira */
protected:
  BundleEndpointId ToEid () const;
  Address ToAddress () const;
  
  virtual void Close ();
  friend class LinkManager;
  friend class OrwarLinkManager;
  friend class Contact;
  friend class OrwarRouterChangedOrder;
  virtual void DoDispose ();

  BundleEndpointId m_remoteEid;
  Address m_remoteAddress;
  LinkState m_state;
  Time m_lastHeard;
  Ptr<Contact> m_contact;
  //Timer m_expirationTimer;

  /* sergiosvieira */
  // Campo criado para o protocolo Prophet - feito para facilitar nossa vida pois não precisamos criar novas classes Link e LinkManager
  double m_p; // Previsibilidade de Entrega
  map<uint32_t, double> m_nlist;
  /* sergiosvieira */

  Callback<void, Address > m_linkLostCb;
  friend ostream& operator<< (ostream& os, const Link& link);
};

/*
class OrwarLink : public Link
{
public:
  static TypeId GetTypeId (void);
  OrwarLink ();
  OrwarLink (const BundleEndpointId& remoteEid, const Address& address);
  ~OrwarLink ();
  
  virtual void SetHackFixmeCallback (Callback<void, uint32_t, Time, bool > hackFixmeCb);
  void Open ();
  bool HaveCw ();
protected:
  void Close ();
  friend class LinkManager;
  friend class OrwarLinkManager;
  friend class OrwarContact;
  friend class OrwarRouterChangedOrder;
  virtual void DoDispose ();
  //Time m_setupHold;
  //Time m_cwMaxHold;

  Callback<void, uint32_t, Time, bool> m_hackFixmeCb;

  TracedCallback<uint32_t, uint32_t, Time, bool> m_contactClosedLogger;
};
*/

typedef std::vector<Ptr<Link> > Links;

ostream& operator<< (ostream& os, const Link& bundle);
}} // namespace bundleProtocol, ns3

#endif /* BP_LINK_H */
