/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <cmath>

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "bp-orwar-contact.h"

namespace ns3 {
namespace bundleProtocol {
 
NS_LOG_COMPONENT_DEFINE ("OrwarContact");

NS_OBJECT_ENSURE_REGISTERED (OrwarContact);

TypeId
OrwarContact::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::OrwarContact")
    .SetParent<Contact> ()
    .AddConstructor<OrwarContact> ()
    ;
  return tid;
}

OrwarContact::OrwarContact ()
  : Contact (),
    m_state (UNKOWN_STATE),
    m_dataRate (0),
    m_contactWindowTime (Seconds (0)),
    m_cwLastUpdated (Simulator::Now ()),
    m_cwMax (false)
{}

OrwarContact::~OrwarContact ()
{}

  
void
OrwarContact::DoDispose ()
{
  Contact::DoDispose ();
}
  
void
OrwarContact::ChangeState (OrwarContactState state)
{
  m_state = state;
}

OrwarContactState
OrwarContact::GetState () const
{
  return m_state;
}

void
OrwarContact::SetContactWindowDuration (Time time)
{
  m_cwLastUpdated = Simulator::Now ();
  m_contactWindowTime = time;
}

bool
OrwarContact::DecreaseContactWindowDuration ()
{
  if (!m_cwMax)
    {
      m_contactWindowTime  = m_contactWindowTime - (Simulator::Now () - m_cwLastUpdated);
      if (m_contactWindowTime.IsNegative () || (m_contactWindowTime < Seconds (0.001)))
        {
          m_contactWindowTime = Seconds (0);
        }
      m_cwLastUpdated = Simulator::Now ();
      return m_contactWindowTime.IsStrictlyPositive ();
    }
  else
    {
      m_cwLastUpdated = Simulator::Now ();
      return true;
    }
}

Time
OrwarContact::GetContactWindowDuration () const
{
  return m_contactWindowTime;
}

Time
OrwarContact::GetLastUpdated () const
{
  return m_cwLastUpdated;
}

void
OrwarContact::SetDataRate (const DataRate& dataRate)
{
  m_dataRate = dataRate;
}

DataRate
OrwarContact::GetDataRate () const
{
  return m_dataRate;
}

bool
OrwarContact::GetCwMax ()
{
  return m_cwMax;
}

void
OrwarContact::SetCwMax (bool cwMax)
{
  m_cwMax = cwMax;
}

string
OrwarContactStateToString (const OrwarContactState& state)
{
  switch (state)
    {
    case S_CREATED:
      return "Sender: Created";
      break;
    case S_WAITING_FOR_KDM:
      return "Sender: Waiting for Kdm";
      break;
    case S_WAITING_FOR_CWI:
      return "Sender: Waiting for Cwi";
      break;
     case R_RECEIVED_KDM:
      return "Receiver: Received Kdm"; 
      break;
    case R_WAITING_FOR_CWI:
      return "Reciver: Waiting for Cwi";
      break;
    case READY:
        return "Ready";
      break;
    default:
      return "Unkown state";
    };
}

ostream&
operator<< (ostream& os, const OrwarContact& oc)
{
  os << "Orwar contact [" << OrwarContactStateToString (oc.m_state) << "]" << endl;
  os << "Data rate: " << oc.m_dataRate << endl;
  os << "Contact window: " << oc.m_contactWindowTime.GetSeconds () << endl;
  os << "Last updated: " << oc.m_cwLastUpdated.GetSeconds () << endl;
  return os;
}

}} // namespace bundleProtocol, ns3
