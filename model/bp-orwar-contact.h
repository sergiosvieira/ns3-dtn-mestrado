/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_ORWAR_CONTACT_H
#define BP_ORWAR_CONTACT_H

#include "bp-contact.h"
#include "bp-link.h"

#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"

namespace ns3 {
namespace bundleProtocol {

class Link;
class OrwarLink;


enum OrwarContactState {
  R_WAITING_FOR_CWI = 0, // No longer used
  UNKOWN_STATE = 1,
  S_CREATED = 2,
  S_WAITING_FOR_CWI = 3,
  S_RECEIVED_CWI = 4,
  S_WAITING_FOR_KDM = 5,
  R_RECEIVED_CWI = 6,
  R_WAITING_FOR_KDM = 7,
  R_RECEIVED_KDM = 8,
  READY = 9
};

/**
 * \ingroup bundleRouter
 *
 * \brief Represents a started orwar specific contact between to nodes.
 *
 *
 */

class OrwarContact : public Contact
{
public:
  static TypeId GetTypeId (void);
  OrwarContact ();
  ~OrwarContact ();

  void ChangeState (OrwarContactState state);
  OrwarContactState GetState () const;
  
  void SetContactWindowDuration (Time time);

  bool DecreaseContactWindowDuration ();

  Time GetContactWindowDuration () const;

  Time GetLastUpdated () const;

  void SetDataRate (const DataRate& dataRate);
  DataRate GetDataRate () const;
  bool GetCwMax ();
  void SetCwMax (bool cwMax);

protected:
  virtual void DoDispose ();
private:
  OrwarContactState m_state;
  DataRate m_dataRate;
  Time m_contactWindowTime;
  Time m_cwLastUpdated;
  bool m_cwMax;

  friend ostream& operator<< (ostream& os, const OrwarContact& oc);
};

ostream& operator<< (ostream& os, const OrwarContact& oc);

}} // namespace bundleProtocol, ns3

#endif /* BP_ORWAR_CONTACT_H */
