/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <algorithm>
#include <iostream>

#include "ns3/log.h"

#include "bp-forwarding-log.h"

#include "ns3/simulator.h"

using namespace std;


namespace ns3 {
namespace bundleProtocol {

// Forward Log Entry

ForwardLogEntry::ForwardLogEntry ()
  : m_gbid (),
    m_forwardedTo (),
    m_ttl (),
    m_creationTime ()
{}

ForwardLogEntry::ForwardLogEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo)
  : m_gbid (bundle->GetBundleId ()), 
    m_forwardedTo (forwardedTo),
    m_ttl (),
    m_creationTime ()
{
  Time lifeTime = bundle->GetLifetime ();
  Time creationTime = bundle->GetCreationTimestamp ().GetTime ();
  m_ttl = lifeTime + creationTime;
}

ForwardLogEntry::ForwardLogEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl)
  : m_gbid (gbid),
    m_forwardedTo (forwardedTo),
    m_ttl (ttl),
    m_creationTime ()
{}

ForwardLogEntry::ForwardLogEntry (const ForwardLogEntry& entry)
  : m_gbid (entry.m_gbid),
    m_forwardedTo (entry.m_forwardedTo),
    m_ttl (entry.m_ttl),
    m_creationTime (entry.m_creationTime)
{}

ForwardLogEntry::~ForwardLogEntry ()
{}

GlobalBundleIdentifier
ForwardLogEntry::GetBundleId () const
{
  return m_gbid;
}

BundleEndpointId   
ForwardLogEntry::GetBundleEndpoint () const
{
  return m_forwardedTo;
}

Time
ForwardLogEntry::GetTimeToLive () const
{
  return m_ttl;
}

Time
ForwardLogEntry::GetCreationTime () const
{
   return m_creationTime;
}

bool
ForwardLogEntry::operator== (const ForwardLogEntry& other) const
{
   return (m_gbid == other.m_gbid) && (m_forwardedTo == other.m_forwardedTo);
}

bool
ForwardLogEntry::operator< (const ForwardLogEntry & other) const
{
  if (m_creationTime == other.m_creationTime)
    {
      return m_gbid < other.m_gbid;      
    }
  else
    {
      return m_creationTime < other.m_creationTime;
    }
}

ostream&
operator<< (ostream& os, const ForwardLogEntry& entry)
{
  os << "Log entry" << endl << "\tCreated " << entry.m_creationTime.GetSeconds () << endl;
  os << "const GlobalBundleIdentifier&: " << endl << entry.m_gbid << endl;
  os << "Forwarded to eid: " << entry.m_forwardedTo << endl;
  os << "TTL: " << entry.m_ttl.GetSeconds () << "\tTime left:" << (entry.m_ttl.GetSeconds () - Simulator::Now ().GetSeconds ());
  return os;
}

// Forward Log

NS_LOG_COMPONENT_DEFINE ("ForwardLog");

ForwardLog::ForwardLog ()
  : m_forwardLog ()
{
}

ForwardLog::~ForwardLog ()
{
  m_forwardLog.clear ();
}
  
uint32_t
ForwardLog::GetNEntries () const
{
  NS_LOG_DEBUG("ForwardLog::GetNEntries");
  return m_forwardLog.size ();
}

void
ForwardLog::AddEntry (const ForwardLogEntry& entry)
{
  NS_LOG_DEBUG("ForwardLog::AddEntry");
  ForwardLogList::iterator iter = m_forwardLog.find (entry.GetBundleId ());
 
  if (iter != m_forwardLog.end ())
    {
      ForwardLogEntries *entries = &(iter->second);
      entries->push_front (entry);
      sort (entries->begin (), entries->end ());
    }
  else 
    {
      ForwardLogEntries entries;
      entries.push_front (entry);
      //m_forwardLog.insert (make_pair<const GlobalBundleIdentifier&, ForwardLogEntries> (entry.GetBundleId (), entries));
      m_forwardLog.insert (make_pair<GlobalBundleIdentifier, ForwardLogEntries> (entry.GetBundleId (), entries));
    }
}

void
ForwardLog::AddEntry (Ptr<Bundle> bundle, Ptr<Link> link)
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           link->GetRemoteEndpointId (),
                                           bundle->GetLifetime ());
  AddEntry (entry);
}

void
ForwardLog::AddEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo)
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           forwardedTo,
                                           bundle->GetLifetime ());
  AddEntry (entry);
}

void
ForwardLog::AddEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl)
{
  ForwardLogEntry entry = ForwardLogEntry (gbid,
                                           forwardedTo,
                                           ttl);
  AddEntry (entry);
}

bool
ForwardLog::HasEntry (const ForwardLogEntry& entry) const
{
  NS_LOG_DEBUG("ForwardLog::HasEntry");
  ForwardLogList::const_iterator iter = m_forwardLog.find (entry.GetBundleId ());
  if (iter != m_forwardLog.end ())
    {
      const ForwardLogEntries *entries = &(iter->second);
      return find (entries->begin (), entries->end (), entry) != entries->end ();
    }
  else
    {
      return false;
    }
}

bool
ForwardLog::HasEntry (Ptr<Bundle> bundle, Ptr<Link> link) const
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           link->GetRemoteEndpointId (),
                                           bundle->GetLifetime ());
  return HasEntry (entry);
}

bool
ForwardLog::HasEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo) const
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           forwardedTo,
                                           bundle->GetLifetime ());
  return HasEntry (entry);
}

bool
ForwardLog::HasEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo) const
{
  ForwardLogEntry entry = ForwardLogEntry (gbid,
                                           forwardedTo,
                                           Seconds (0));
  return HasEntry (entry);
}
  
ForwardLogEntry
ForwardLog::GetEntry (const ForwardLogEntry& entry) const
{
  NS_LOG_DEBUG("ForwardLog::GetEntry");
  ForwardLogList::const_iterator iter = m_forwardLog.find (entry.GetBundleId ());
 
  if (iter != m_forwardLog.end ())
    {
      const ForwardLogEntries *entries = &(iter->second);
      ForwardLogEntries::const_iterator iter = find (entries->begin (), entries->end (), entry);
      if (iter != entries->end ())
        {
          return *iter;
        }
    }
  
  return ForwardLogEntry ();
}

ForwardLogEntry
ForwardLog::GetEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo) const
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           forwardedTo,
                                           bundle->GetLifetime ());
  return GetEntry (entry);
}

ForwardLogEntry
ForwardLog::GetEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl) const
{
  ForwardLogEntry entry = ForwardLogEntry (gbid,
                                           forwardedTo,
                                           ttl);
  return GetEntry (entry);
}

ForwardLogEntries
ForwardLog::GetAllEntries () const
{
  NS_LOG_DEBUG("ForwardLog::GetAllEntries");
  ForwardLogEntries logEntries;

  for (ForwardLogList::const_iterator log = m_forwardLog.begin (); 
       log != m_forwardLog.end ();
       ++log)
    {
      logEntries.insert (logEntries.end (), log->second.begin (), log->second.end ());
    }

  return logEntries;
}

ForwardLogEntries
ForwardLog::GetEntries (const BundleEndpointId& forwardedTo) const
{
  NS_LOG_DEBUG("ForwardLog::GetEntries");
  ForwardLogEntries logEntries;

  for (ForwardLogList::const_iterator log = m_forwardLog.begin (); 
       log != m_forwardLog.end ();
       ++log)
    {
      const ForwardLogEntries *entries = &(log->second); 
      for (ForwardLogEntries::const_iterator iter = entries->begin ();
           iter != entries->end (); ++iter)
        {
          if (iter->GetBundleEndpoint () == forwardedTo)
            logEntries.push_back (*iter);
        }
    }

  return logEntries;
}

ForwardLogEntries
ForwardLog::GetEntries (const GlobalBundleIdentifier& gbid) const
{
  NS_LOG_DEBUG("ForwardLog::GetEntries");
  ForwardLogEntries logEntries;

  ForwardLogList::const_iterator iter = m_forwardLog.find (gbid);
  if (iter != m_forwardLog.end ())
    {
      return iter->second;
    }
  else
    {
      return logEntries;
    }
/*
  for (ForwardLogList::const_iterator log = m_forwardLog.begin (); 
  log != m_forwardLog.end ();
       ++log)
    {
        
    const ForwardLogEntries *entries = &(log->second); 
      for (ForwardLogEntries::const_iterator iter = entries->begin ();
           iter != entries->end (); ++iter)
        {
          if (iter->GetBundleId () == gbid)
            logEntries.push_back (*iter);
        }
      
    }
*/
}

void
ForwardLog::ClearLog ()
{
  NS_LOG_DEBUG("ForwardLog::ClearLog");
  m_forwardLog.clear ();
}

void
ForwardLog::RemoveEntry (const ForwardLogEntry& entry)
{
  NS_LOG_DEBUG("ForwardLog::RemoveEntry");
  ForwardLogList::iterator iter = m_forwardLog.find (entry.GetBundleId ());
  if (iter != m_forwardLog.end ())
    {
      ForwardLogEntries *entries = &(iter->second);
      ForwardLogEntries::iterator log = remove (entries->begin (), entries->end (), entry);
      entries->erase (log, entries->end ());
    }
}

void
ForwardLog::RemoveEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo)
{
  ForwardLogEntry entry = ForwardLogEntry (bundle->GetBundleId (),
                                           forwardedTo,
                                           bundle->GetLifetime ());
  RemoveEntry (entry);
}

void
ForwardLog::RemoveEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl)
{
  ForwardLogEntry entry = ForwardLogEntry (gbid,
                                           forwardedTo,
                                           ttl);
  RemoveEntry (entry);
}

void
ForwardLog::RemoveEntriesFor (const GlobalBundleIdentifier& gbid)
{
  
  ForwardLogList::iterator iter = m_forwardLog.find (gbid);
  if (iter != m_forwardLog.end ())
    {
      m_forwardLog.erase (iter);
    }
}

void
ForwardLog::RemoveExpiredEntries ()
{
  NS_LOG_DEBUG("ForwardLog::RemoveExpiredEntries");
  for (ForwardLogList::iterator iter = m_forwardLog.begin ();
       iter != m_forwardLog.end ();
       ++iter)
    {
      ForwardLogEntries *entries = &(iter->second);
      ForwardLogEntries::iterator iter2 = remove_if (entries->begin (), entries->end (), EntryExpired ());
      entries->erase (iter2, entries->end ());
    }
}

ostream&
operator<< (ostream& os, const ForwardLog& log)
{
  ForwardLogEntries entries = log.GetAllEntries ();
  os << "#### Forward log ####" << endl;
  for (ForwardLogEntries::iterator iter = entries.begin (); iter != entries.end (); ++iter)
    {
      os <<  *iter << endl << endl;;
    }
  return os;
}

}} // namespace bundleProtocol, ns3


#ifdef RUN_SELF_TESTS

#include "ns3/test.h"


using namespace ns3::bundleProtocol;

namespace ns3 {

class ForwardLogTest : public ns3::Test {
private:
public:
  ForwardLogTest ();
  virtual bool RunTests (void);

};

  ForwardLogTest::ForwardLogTest ()
    : ns3::Test ("ForwardLog")
  {}

bool
ForwardLogTest::RunTests (void)
{
  bool result = true;

  BundleEndpointId eid1 = BundleEndpointId ("dtn","1");
  BundleEndpointId eid2 = BundleEndpointId ("dtn","2");
  BundleEndpointId eid3 = BundleEndpointId ("dtn","3");
  BundleEndpointId eid4 = BundleEndpointId ("dtn","4");
  BundleEndpointId eid5 = BundleEndpointId ("dtn","5");
  BundleEndpointId eid6 = BundleEndpointId ("dtn","6");
  
  Ptr<Packet> p;
  PrimaryBundleHeader pheader = PrimaryBundleHeader ();
  CanonicalBundleHeader cheader = CanonicalBundleHeader (PAYLOAD_BLOCK);
  cheader.SetLastBlock (true);
  
  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid1);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b1 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid2);
  pheader.SetCreationTimestamp (CreationTimestamp (4,6));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b2 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid3);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b3 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid4);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b4 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid5);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b5 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid6);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b6 = Create<Bundle> (p);

  ForwardLog log = ForwardLog ();
  
  log.AddEntry (b1, eid1);
  log.AddEntry (b1, eid2);
  log.AddEntry (b2, eid3);
  log.AddEntry (b3, eid4);

  NS_TEST_ASSERT (log.HasEntry (b1, eid1));
  NS_TEST_ASSERT (log.HasEntry (b1, eid2));
  NS_TEST_ASSERT (log.HasEntry (b1->GetBundleId (), eid2));
  NS_TEST_ASSERT (log.HasEntry (b1->GetBundleId (), eid2));
  NS_TEST_ASSERT (log.HasEntry (b2, eid3));
  NS_TEST_ASSERT (log.HasEntry (b3, eid4));
  NS_TEST_ASSERT (!log.HasEntry (b5, eid1));
  NS_TEST_ASSERT (!log.HasEntry (b2, eid2));
  return result;
}

static ForwardLogTest gForwardLogTest;

} // namespace ns3

#endif
