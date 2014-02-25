/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_FORWARDING_LOG_H
#define BP_FORWARDING_LOG_H

#include <map>
#include <deque>
#include <functional>

#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"

#include "bp-bundle.h"
#include "bp-global-bundle-identifier.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-link.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

/**
 * \ingroup bundleRouter
 *
 * \brief A log entry for the ForwardLog.
 *
 *
 */

class ForwardLogEntry 
{
 public:
  ForwardLogEntry ();
  ForwardLogEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo);
  ForwardLogEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl);
  ForwardLogEntry (const ForwardLogEntry& entry);
  virtual ~ForwardLogEntry ();
   
  GlobalBundleIdentifier GetBundleId () const;
  BundleEndpointId GetBundleEndpoint () const;
  Time GetTimeToLive () const;
  Time GetCreationTime () const;

  bool operator== (const ForwardLogEntry& other) const;
  bool operator< (const ForwardLogEntry & other) const;

 private:
  GlobalBundleIdentifier m_gbid;
  BundleEndpointId m_forwardedTo;
  Time m_ttl;
  Time m_creationTime;
  friend ostream& operator<< (ostream& os, const ForwardLogEntry& entry);
};


typedef deque<ForwardLogEntry> ForwardLogEntries;

/**
 * \ingroup bundleRouter
 *
 * \brief A forward log holding information on what bundles have been sent to which node.
 *
 *
 */
class ForwardLog 
{
typedef map<GlobalBundleIdentifier, ForwardLogEntries> ForwardLogList;

 public:
  ForwardLog ();
  ~ForwardLog ();

  uint32_t GetNEntries () const;

  void AddEntry (const ForwardLogEntry& entry);
  void AddEntry (Ptr<Bundle> bundle, Ptr<Link> link);
  void AddEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo);
  void AddEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl);

  bool HasEntry (const ForwardLogEntry& entry) const;
  bool HasEntry (Ptr<Bundle> bundle, Ptr<Link> link) const;
  bool HasEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo) const;
  bool HasEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo) const;

  ForwardLogEntry GetEntry (const ForwardLogEntry& entry) const;
  ForwardLogEntry GetEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo) const;
  ForwardLogEntry GetEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl) const;

  ForwardLogEntries GetAllEntries () const;
  ForwardLogEntries GetEntries (const BundleEndpointId& forwardedTo) const;
  ForwardLogEntries GetEntries (const GlobalBundleIdentifier& gbid) const;

  void RemoveEntry (const ForwardLogEntry& entry);
  void RemoveEntry (Ptr<Bundle> bundle, const BundleEndpointId& forwardedTo);
  void RemoveEntry (const GlobalBundleIdentifier& gbid, const BundleEndpointId& forwardedTo, Time ttl);
  void RemoveEntriesFor (const GlobalBundleIdentifier& gbid);

  void ClearLog ();
  
  void RemoveExpiredEntries ();
 private:

  ForwardLogList m_forwardLog;

  struct EntryExpired : public unary_function <ForwardLogEntry, bool>
  {
    bool operator () (ForwardLogEntry entry)
    {
      return (Simulator::Now () > (entry.GetBundleId ().GetCreationTimestamp ().GetTime () + entry.GetTimeToLive ()));
    }
  };

  friend ostream& operator<< (ostream& os, const ForwardLog& log);
};

ostream& operator<< (ostream& os, const ForwardLogEntry& entry);
ostream& operator<< (ostream& os, const ForwardLog& log);
}} // namespace bundleProtocol, ns3

#endif /* BP_FORWARDING_LOG_H */
