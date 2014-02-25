/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/log.h"
#include "ns3/trace-source-accessor.h"

#include "bp-link.h"
#include "bp-orwar-contact.h"

NS_LOG_COMPONENT_DEFINE ("Link");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (Link);

TypeId
Link::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::Link")
    .SetParent<Object> ()
    .AddConstructor<Link> ()
    ;
  return tid;
}

Link::Link ()
  : m_remoteEid (),
    m_remoteAddress (),
    m_state (LINK_UNAVAILABLE), 
    m_lastHeard (Simulator::Now ()),
    m_contact (0)
{}
  
Link::Link (const BundleEndpointId& eid, const Address& address)
  : m_remoteEid (eid),
    m_remoteAddress (address),
    m_state (LINK_UNAVAILABLE), 
    m_lastHeard (Simulator::Now ()),
    m_contact (0)
{}

Link::Link (const EidAddress& ea)
  : m_remoteEid (ea.GetEndpoint ()),
    m_remoteAddress (ea.GetAddress ()),
    m_state (LINK_UNAVAILABLE), 
    m_lastHeard (Simulator::Now ()),
    m_contact (0)
{}
  
Link::~Link ()
{}

void
Link::DoDispose ()
{
  Close ();
  m_linkLostCb = MakeNullCallback<void,Address> ();
  Object::DoDispose ();
}

void
Link::SetLinkLostCallback (Callback<void, Address > linkLostCb)
{
  m_linkLostCb = linkLostCb;
}

void
Link::SetRemoteEndpointId (const BundleEndpointId& eid)
{
  m_remoteEid = eid;
}

BundleEndpointId
Link::GetRemoteEndpointId () const
{
  return m_remoteEid;
}

void
Link::SetContact (Ptr<Contact> contact)
{
  m_contact = contact;
}

Ptr<Contact>
Link::GetContact () const
{
  return m_contact;
}

Time
Link::GetLastHeardFrom () const
{
   return m_lastHeard;
}

void 
Link::UpdateLastHeardFrom ()
{
  m_lastHeard = Simulator::Now ();
}

void
Link::SetRemoteAddress (const Address& address)
{
  m_remoteAddress = address;
}

Address
Link::GetRemoteAddress () const
{
  return m_remoteAddress;
}

LinkState
Link::GetState () const
{
  return m_state;
}

void
Link::Open ()
{
  m_contact = CreateObject<Contact> ();
  ChangeState (LINK_CONNECTED);
}

void
Link::Close ()
{
  if (m_contact != 0)
    {  
      m_contact = 0;
      ChangeState (LINK_UNAVAILABLE);
      if (!m_linkLostCb.IsNull ())
        {
          m_linkLostCb (m_remoteAddress);
        }
    }
  //m_expirationTimer.Cancel (); 
  //m_expirationTimer.Remove (); 
}

void
Link::ChangeState (LinkState state)
{
  m_state = state;
}


Link::operator BundleEndpointId () const
{
  return ToEid ();
}

Link::operator Address () const
{
  return ToAddress ();
}


BundleEndpointId
Link::ToEid () const
{
  return m_remoteEid;
}

Address
Link::ToAddress () const
{
  return m_remoteAddress;
}

bool 
Link::operator == (const Link& other)
{
  return m_remoteEid == other.m_remoteEid && m_remoteAddress == other.m_remoteAddress;
}

bool 
Link::operator != (const Link& other)
{
  return ! (*this == other);
}

bool 
Link::operator < (const Link& other)
{
  if (m_remoteEid == other.m_remoteEid)
    {
      return m_remoteAddress < other.m_remoteAddress;
    }
  else
    {
      return m_remoteEid < other.m_remoteEid;
    }
}


/* sergiosvieira */
// Prophet
void Link::SetProbability(const float &p)
{
	m_p = p;
}

double Link::GetProbability()
{
	return m_p;
}

void Link::AddNeigh(uint32_t eid, double probability) {
	m_nlist[eid] = probability;
}

double Link::GetNeigh(uint32_t eid) {
	return m_nlist[eid];
}

int Link::GetSize() {
	return m_nlist.size();
}

/* sergiosvieira */
/*
NS_OBJECT_ENSURE_REGISTERED (OrwarLink);

TypeId
OrwarLink::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::OrwarLink")
    .SetParent<Link> ()
    .AddConstructor<OrwarLink> ()
    ;
  return tid;
}

OrwarLink::OrwarLink ()
  : Link ()//, m_setupHold (Seconds (0)), m_cwMaxHold (Seconds (0))
{}

OrwarLink::OrwarLink (const BundleEndpointId& remoteEid, const Address& remoteAddress)
  : Link (remoteEid, remoteAddress)//, m_setupHold (Seconds (0)), m_cwMaxHold (Seconds (0))
{}

OrwarLink::~OrwarLink ()
{}

void
OrwarLink::DoDispose ()
{
  m_hackFixmeCb = MakeNullCallback<void, uint32_t, Time, bool > ();
  Link::DoDispose ();
}
  
void
OrwarLink::SetHackFixmeCallback (Callback<void, uint32_t, Time, bool > hackFixmeCb)
{
  m_hackFixmeCb = hackFixmeCb;
}

void
OrwarLink::Open ()
{
  m_contact = CreateObject<OrwarContact> ();
  ChangeState (LINK_CONNECTED);
}

void
OrwarLink::Close ()
{
  if (m_contact != 0)
    {      
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (m_contact));
      if (HaveCw ())
        {      
          oc->DecreaseContactWindowDuration ();
          if (!m_hackFixmeCb.IsNull ())
            {
              m_hackFixmeCb (m_remoteEid.GetId (), oc->GetContactWindowDuration (), oc->GetCwMax ());
            }
        }
    }
  Link::Close ();
}

bool
OrwarLink::HaveCw ()
{
  if (m_state == LINK_CONNECTED || m_state == LINK_PAUSED)
    { 
      // I have seutp the enum for OrwarContactState so that all states
      // with the value 4 or above when convereted to uint8_t have an
      // contact window
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (m_contact));
      return (uint8_t) oc->GetState () >= 4;
    }
  else
    {
      return false;
    }
}
*/

string
LinkStateToString (LinkState state)
{
  switch (state)
    {
    case LINK_AVAILABLE:
      return string ("Link available");
      break;
    case LINK_UNAVAILABLE:
      return string ("Link unavailable");
      break;
    case LINK_CONNECTED:
      return string ("Link connected");
    case LINK_PAUSED:
      return string ("Link paused");
    default:
      return string ("No such state");
    }
}

ostream&
operator<< (ostream& os, const Link& link)
{
  os << "Link [" << LinkStateToString (link.m_state) << "]" << endl;
  os << "Last heard from: " << link.m_lastHeard.GetSeconds () << endl;
  os << "Remote endpoint id: " << link.m_remoteEid << endl;
  os << "Remote address: " << Mac48Address::ConvertFrom (link.m_remoteAddress);
  return os;
}

}} // namespace bundleProtocol, ns3
