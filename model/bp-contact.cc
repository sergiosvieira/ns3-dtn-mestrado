/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"

#include "bp-contact.h"

#include "bp-link.h"
#include "bp-bundle.h"


namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("Contact");

NS_OBJECT_ENSURE_REGISTERED (Contact);

TypeId
Contact::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::Contact")
    .SetParent<Object> ()
    .AddConstructor<Contact> ()
    ;
  return tid;
}

Contact::Contact ()
  : m_retransmissions (0)
{}

Contact::~Contact ()
{}
  
void
Contact::DoDispose ()
{
  m_queue.clear ();
  Object::DoDispose ();
}

void
Contact::EnqueueBundle (Ptr<Bundle> bundle)
{
  m_queue.push_back (bundle);
}

Ptr<Bundle>
Contact::DequeueBundle ()
{
  Ptr<Bundle> bundle = 0;
  if (!m_queue.empty ())
    {
      bundle = m_queue.front ();
      m_queue.pop_front ();
    }
  return bundle;
}

Ptr<Bundle>
Contact::DequeueBundle (const GlobalBundleIdentifier& gbid)
{
  Ptr<Bundle> bundle = 0;
  if (!m_queue.empty ())
    {
      deque<Ptr<Bundle> >::iterator it = find_if (m_queue.begin (), m_queue.end (), MatchingGbid (gbid));
      if (it != m_queue.end ())
        {
          bundle = *it;
          m_queue.erase (it);
        }
    }
  return bundle;
}

deque<Ptr<Bundle> >
Contact::GetQueuedBundles ()
{
  return m_queue;
}

uint32_t
Contact::GetNumQueuedBundles ()
{
  return m_queue.size ();
}

void
Contact::IncreaseRetransmissions ()
{
  ++m_retransmissions;
}

void
Contact::ResetRetransmissions ()
{
  m_retransmissions = 0;
}

void
Contact::SetRetransmissions (uint32_t rt)
{
  m_retransmissions = rt;
}

uint32_t
Contact::GetRetransmissions () const
{
  return m_retransmissions;
}

}} // namespace bundleProtocol, ns3
