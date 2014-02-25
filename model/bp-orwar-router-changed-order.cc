/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <cmath>
#include <algorithm>
#include <limits>
#include <sstream>

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/address.h"
#include "ns3/mac48-address.h"
#include "ns3/mobility-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"

#include "bp-orwar-router-changed-order.h"
#include "bp-header.h"
#include "bp-orwar-contact.h"
#include "bp-link-manager.h"
#include "bp-orwar-link-manager.h"
#include "bp-neighbourhood-detection-agent.h"
#include "bp-link.h"

NS_LOG_COMPONENT_DEFINE ("OrwarRouterChangedOrder");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (OrwarRouterChangedOrder);

TypeId
OrwarRouterChangedOrder::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::OrwarRouterChangedOrder")
    .SetParent<BundleRouter> ()
    .AddConstructor<OrwarRouterChangedOrder> ()
    .AddAttribute ("ReplicationFactor",
                   "Sets the replication factor",
                   UintegerValue (6),
                   MakeUintegerAccessor (&OrwarRouterChangedOrder::m_replicationFactor),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("Delta",
                   "Sets the delta replication factor",
                   UintegerValue (2),
                   MakeUintegerAccessor (&OrwarRouterChangedOrder::m_deltaReplicationFactor),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("PauseTime",
                   "Sets the time a link should be paused",
                   TimeValue (Seconds (0.2)),
                   MakeTimeAccessor (&OrwarRouterChangedOrder::m_pauseTime),
                   MakeTimeChecker ())
    .AddAttribute ("MaxRetries",
                   "Sets the maximum retries before closeing a bundle",
                   UintegerValue (3),
                   MakeUintegerAccessor (&OrwarRouterChangedOrder::m_maxRetries),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("EstimateContactWindow",
                   "Sets if contact window estimation shall be used or not.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&OrwarRouterChangedOrder::m_estimateCw),
                   MakeBooleanChecker ())
    .AddAttribute ("AlwaysSendHello",
                   "Sets if the router always should send hellos or only when it have something to send.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&OrwarRouterChangedOrder::m_alwaysSendHello),
                   MakeBooleanChecker ())
    .AddTraceSource ("CreatedRouterBundle", "A router message have been created.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_createRouterLogger))
    .AddTraceSource ("ContactSetup", "A contact setup has been finished.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_contactSetupLogger))
    .AddTraceSource ("RedundantRelay", "A message already held in the buffer has been received.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_redundantRelayLogger))
    .AddTraceSource ("ContactsOutOfSynch", "A message have been received out of order in the contact seutp.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_outOfSynchLogger))
    .AddTraceSource ("ContactSetupFailed", "A contact setup failed, no ack for router bundle.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_contactSetupFailedLogger))
    .AddTraceSource ("EstimatedCw", "The estimated contact window of a nodes contact.",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_estimatedCwLogger))
    .AddTraceSource ("ContactOpp", ".",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_contactOppLogger))
    .AddTraceSource ("ContactOppBetween", ".",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_contactOppBetweenLogger))
    .AddTraceSource ("DeleteRouter", "A router bundle have been deleted",
                     MakeTraceSourceAccessor (&OrwarRouterChangedOrder::m_routerDeleteLogger))
    
    ;
  return tid;
}

OrwarRouterChangedOrder::OrwarRouterChangedOrder ()
  : BundleRouter (),
    m_kdm ()
{}

OrwarRouterChangedOrder::~OrwarRouterChangedOrder ()
{
  ///cout << "%%%%% OrwarRouterChangedOrder::~OrwarRouterChangedOrder" << endl;
}

void
OrwarRouterChangedOrder::DoInit ()
{
  if (m_alwaysSendHello)
    {
      m_nda->Start ();
    }
}

void
OrwarRouterChangedOrder::DoDispose ()
{
  m_kdm.Clear ();
  BundleRouter::DoDispose ();
}

void 
OrwarRouterChangedOrder::DoLinkClosed (Ptr<Link> link)
{
  /*
  if (m_node->GetId () == 49 && link->GetRemoteEndpointId ().GetId () == 21)
    cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << "OrwarRouterChangedOrder::DoLinkClosed" << endl;
  */
  if (link->GetContact () != 0)
    {
      Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
      
      if (oc->GetState () != READY)
        {
          m_contactSetupFailedLogger (1);
        }
      
      BundleList bundles = link->GetContact ()->GetQueuedBundles ();
      for (BundleList::iterator iter = bundles.begin (); iter != bundles.end (); ++iter)
        {
          link->GetContact ()->DequeueBundle (*iter);
          /*
          if (m_node->GetId () == 49 && link->GetRemoteEndpointId ().GetId () == 21)
            {
              cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << "CancelTransmission anropas av DoLinkClosed" << endl;
            }
          */
          CancelTransmission (*iter, link);
        }
    }

  RemoveRouterSpecificBundles (link);
}

void
OrwarRouterChangedOrder::RemoveRouterSpecificBundles (Ptr<Link> link)
{
  BundleList tmp;
  for (BundleList::iterator iter = m_routerSpecificList.begin (); iter != m_routerSpecificList.end (); ++iter)
    {
      if (link->GetRemoteEndpointId () == (*iter)->GetDestinationEndpoint ())
        {
          tmp.push_back (*iter);
        }
    }
  for (BundleList::iterator iter = tmp.begin (); iter != tmp.end (); ++iter)
    {
      RemoveRouterSpecificBundle (*iter, 1);
    }
}

void 
OrwarRouterChangedOrder::PauseLink (Ptr<Link> link)
{ 
  if (link->GetState () == LINK_CONNECTED)
    {
      BundleList bundles = link->GetContact ()->GetQueuedBundles ();
      
      for (BundleList::iterator iter = bundles.begin (); iter != bundles.end (); ++iter)
        {
          link->GetContact ()->DequeueBundle (*iter);
          /*
          if (m_node->GetId () == 49 && link->GetRemoteEndpointId ().GetId () == 21)
            {
              cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << "CancelTransmission anropas av PauseLink" << endl;
            }
          */
          CancelTransmission (*iter, link);
        }

      link->ChangeState (LINK_PAUSED);
      
      Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
      
      if (oc->GetRetransmissions () >= m_maxRetries)
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") OrwarRouterChangedOrder::PauseLink Closes the link to (" << link->GetRemoteEndpointId ().GetId () << ") due to many retransmission attempts" << endl;
          //Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
    	  Ptr<LinkManager> olm (dynamic_cast<LinkManager *> (PeekPointer (m_linkManager)));
          olm->CloseLink (link);
        }
      else
        {
          oc->IncreaseRetransmissions ();
          Simulator::Schedule (m_pauseTime, &OrwarRouterChangedOrder::UnPauseLink, this, link);
        }
    }
  Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
}

void 
OrwarRouterChangedOrder::UnPauseLink (Ptr<Link> link)
{
  if (link->GetState () == LINK_PAUSED)
    {
      link->ChangeState (LINK_CONNECTED);
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
    }
}

void
OrwarRouterChangedOrder::DoLinkDiscovered (Ptr<Link> link)
{
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") OrwarRouterChangedOrder::DoLinkDiscovered Detected (" << link->GetRemoteEndpointId ().GetId () << ")" << endl;
  m_contactOppLogger (1);
  m_contactOppBetweenLogger (m_node->GetId (), link->GetRemoteEndpointId ().GetId ());
  m_linkManager->OpenLink (link);

  Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
  oc->ChangeState (S_CREATED);
  // Want to send my Kdm to the other node.
  Simulator::ScheduleNow (&OrwarRouterChangedOrder::SendCwi, this, link, true);
}

void
OrwarRouterChangedOrder::DoBundleReceived (Ptr<Bundle> bundle)
{
  Ptr<Link> link = m_linkManager->FindLink (bundle->GetReceivedFrom ().front ().GetEndpoint ());
  if (link != 0)
    {
      link->UpdateLastHeardFrom ();
    }
}

Ptr<Bundle>
OrwarRouterChangedOrder::DoSendBundle (Ptr<Link> link, Ptr<Bundle> bundle)
{
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") OrwarChangedOrder::SendBundle to " << link->GetRemoteEndpointId ().GetId () << endl;

  link->GetContact ()->EnqueueBundle (bundle);

  Ptr<Bundle> send = bundle->Copy ();
  if (!IsRouterSpecific (bundle))
    {
      if (bundle->GetReplicationFactor () > 1)
        {
          send->SetReplicationFactor (ceil ((double) bundle->GetReplicationFactor () / 2.0));
        }
    }
  return send;
}

void
OrwarRouterChangedOrder::DoBundleSent (const Address& address, const GlobalBundleIdentifier& gbid, bool finalDelivery)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoBundleSent");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoBundleSent" << endl;

  Mac48Address mac = Mac48Address::ConvertFrom (address);
  Ptr<Link> link = m_linkManager->FindLink (mac);


  Ptr<Bundle> bundle = GetBundle (gbid);
  if (bundle != 0)
    {
      // I have received an ack for the sent bundle, so i have heard from the 
      // other node.
      link->UpdateLastHeardFrom ();

      if (link->GetState () == LINK_CONNECTED || link->GetState () == LINK_PAUSED)
        {
          link->GetContact ()->DequeueBundle (gbid);
          link->GetContact ()->ResetRetransmissions ();
        }

      if (IsRouterSpecific (bundle))
        {
          SentRouterSpecific (link, bundle);
        }
      else 
        {
          m_forwardLog.AddEntry (bundle, link);
          if (finalDelivery)
            {
              BundleDelivered (bundle, true);
            }
          else
            {
              bundle->SetReplicationFactor (floor ((double)bundle->GetReplicationFactor () / 2.0));
            }
          Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
        }
    }
  else
    {
      // FIXME: Hmm hur ska man ta hand om det h�r fallet...
      // H�r vet jag enbbart gbid, men vet ej vilken typ av bundle det �r
      // eller vad den har f�r ttl.
      if (finalDelivery)
        {
          // This is a ugly hack utilitzing the fact that i know that 
          // the ttl is "inifinite".
          Ptr<Bundle> bundle = Create<Bundle> ();
          bundle->SetSourceEndpoint (gbid.GetSourceEid ());
          bundle->SetCreationTimestamp (gbid.GetCreationTimestamp ());
          bundle->SetLifetime (43000);
          BundleDelivered (bundle, true);
          Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
        }

    }
}

void
OrwarRouterChangedOrder::DoBundleTransmissionFailed (const Address& address, const GlobalBundleIdentifier& gbid)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoBundleTransmissionFailed");
  Mac48Address mac = Mac48Address::ConvertFrom (address);
  Ptr<Link> link = m_linkManager->FindLink (mac);
  if (link->GetContact () != 0)
    {
      //link->GetContact ()->DequeueBundle (gbid);
    }

  PauseLink (link);
}

bool
OrwarRouterChangedOrder::DoAcceptBundle (Ptr<Bundle> bundle, bool fromApplication)
{
  if (HasBundle (bundle))
    {
      m_redundantRelayLogger (bundle);
      Ptr<Bundle> otherBundle = GetBundle (bundle->GetBundleId ());
      otherBundle->AddReceivedFrom (bundle->GetReceivedFrom ().front ());
      return false;
    }
   return CanMakeRoomForBundle (bundle);
}


bool
OrwarRouterChangedOrder::DoCanDeleteBundle (const GlobalBundleIdentifier& gbid)
{
  return true;
}

void
OrwarRouterChangedOrder::DoInsert (Ptr<Bundle> bundle)
{
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoInsert bundle (" << bundle->GetPayload ()->GetSize () << ")" << endl;
  // This is my extra thing, i always set the eid to current node holding the bundle.
  bundle->SetCustodianEndpoint (m_eid);

  m_bundleList.push_back (bundle);
  sort (m_bundleList.begin (), m_bundleList.end (), UtilityPerBitCompare ());
  
  // If this is the first bundle, I now want to begin sending hello messages announcing that
  // I have something to send. If there is more than one bundle in the queue this means that
  // I already have started sending hello messages.
  if (m_nBundles == 1 && !m_alwaysSendHello)
    {
      m_nda->Start ();
    }
  
  Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
}

bool
OrwarRouterChangedOrder::CanMakeRoomForBundle (Ptr<Bundle> bundle)
{
  if (bundle->GetSize () < m_maxBytes)
    {
      return true;
    }
  else
    {
      return false;
    }

}

bool
OrwarRouterChangedOrder::MakeRoomForBundle (Ptr<Bundle> bundle)
{
  if (bundle->GetSize () < m_maxBytes)
    {
      if (bundle->GetSize () < GetFreeBytes ())
        {
          return true;
        }

      for (BundleList::reverse_iterator iter = m_bundleList.rbegin (); iter != m_bundleList.rend ();)
        {
          Ptr<Bundle> currentBundle = *(iter++);

          DeleteBundle (currentBundle,true);
          if (bundle->GetSize () < GetFreeBytes ())
            {
              return true;
            }
        }
    }

  return false;
}

bool
OrwarRouterChangedOrder::DoDelete (const GlobalBundleIdentifier& gbid, bool drop)
{
  // If this is the last bundle in the queue, stop sending Hello messages. 
  if (m_nBundles == 1 && !m_alwaysSendHello)
    {
      m_nda->Stop ();
    }
  return BundleRouter::DoDelete (gbid, drop);
}


void
OrwarRouterChangedOrder::DoCancelTransmission (Ptr<Bundle> bundle, Ptr<Link> link)
{
  /*
  if (m_node->GetId () == 49)
    {
      cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << "DoCancelTransmission anropas p� bundlen" << endl;
      cout << *bundle << endl;
    }
  */
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoCancelTransmission");
}

void
OrwarRouterChangedOrder::DoTransmissionCancelled (const Address& address, const GlobalBundleIdentifier& gbid)
{
  Mac48Address mac = Mac48Address::ConvertFrom (address);
  Ptr<Link> link = m_linkManager->FindLink (mac);

  if (link == 0)
    {
      cout << "This would just be strange" << endl;
    }
  /*
  if (link->GetContact () == 0)
    {
      cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") OrwarRouterChangedOrder::BundleTransmissionCancelled Why does this happen..." << endl;
      cout << *link << endl;
      cout << gbid << endl;
    }
  */
  link->GetContact ()->DequeueBundle (gbid);

  Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
}

uint8_t
OrwarRouterChangedOrder::DoCalculateReplicationFactor (const BundlePriority& priority) const
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoCalculateReplicationFactor");
  switch (priority)
    {
    case BULK:
      return m_replicationFactor - m_deltaReplicationFactor;
      break;
    case NORMAL:
            return m_replicationFactor;
      break;
    case EXPEDITED:
            return m_replicationFactor + m_deltaReplicationFactor;
      break;
    default:
      return 1;
    }
  return 1;
}

// Orwar specific 
void
OrwarRouterChangedOrder::TryToStartSending ()
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::TryToStartSending");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::TryToStartSending" << endl;
  RemoveExpiredBundles (true);
  m_forwardLog.RemoveExpiredEntries ();
  
  if (!IsSending () && ((GetNBundles () > 0) || (m_routerSpecificList.size () > 0)))
    {
      
      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << " Have something to send" << endl;
      LinkBundle linkBundle = FindNextToSend ();
      if (!linkBundle.IsNull ())
        {
          SendBundle (linkBundle.GetLink (), linkBundle.GetBundle ());
        }
    }
  else 
    {
      Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
      ///cout << Simulator::Now ().GetSeconds () << " Could not start sending: " << endl;
      ///cout << Simulator::Now ().GetSeconds () << " !IsSending = " << !IsSending () << endl;
      ///cout << Simulator::Now ().GetSeconds () << " GetNBundles () = " << GetNBundles () << endl;
      ///cout << Simulator::Now ().GetSeconds () << " RouterSpecificBundles = " << m_routerSpecificList.size () << endl;
      ///cout << Simulator::Now ().GetSeconds () << " ReadyLinks = " << olm->GetReadyLinks ().size () << endl;
    }
}

LinkBundle
OrwarRouterChangedOrder::FindNextToSend ()
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::FindNextToSend");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::FindNextToSend" << endl;
 
  if (m_routerSpecificList.size () > 0)
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Want to send a router bundle" << endl;
      LinkBundle lb = GetNextRouterSpecific ();
      if (!lb.IsNull ())
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Found a router bundle to send" << endl;
          return lb;
        }
      else
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Could not find a suitable router bundle" << endl;
        }
    }
  Ptr<LinkManager> olm (dynamic_cast<LinkManager *> (PeekPointer (m_linkManager)));

  //Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));

  //Ptr<OrwarLinkManager> olm ((OrwarLinkManager *) (PeekPointer (m_linkManager)));
  if ((olm->GetReadyLinks ().size () > 0) && (GetNBundles () > 0))
    {
      LinkBundleList linkBundleList = GetAllDeliverableBundles ();
      
      if (linkBundleList.empty ())
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "Want to forward a bundle" << endl;
          linkBundleList = GetAllBundlesToAllLinks ();
        }
      else
        {
          ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Want to delivery a bundle" << endl;
        }
      
      if (!linkBundleList.empty ())
        {
          sort (linkBundleList.begin (), linkBundleList.end (), UtilityPerBitCompare2 ());
          return linkBundleList.front ();
        }
    }
  //cout << "GetNBundles () = " << GetNBundles () << endl;
  //cout << "Doh kom hit utan att skicka n�got" << endl;
  ///cout << Simulator::Now ().GetSeconds () << " Failed to find a suitable bundle: " << endl;
  ///cout << Simulator::Now ().GetSeconds () << " GetNBundles () = " << GetNBundles () << endl;
  ///cout << Simulator::Now ().GetSeconds () << " RouterSpecificBundles = " << m_routerSpecificList.size () << endl;
  ///cout << Simulator::Now ().GetSeconds () << " ReadyLinks = " << olm->GetReadyLinks ().size () << endl;
  return LinkBundle (0,0);
}


LinkBundle
OrwarRouterChangedOrder::GetNextRouterSpecific ()
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetNextRouterSpecific");
  //cout << Simulator::Now ().GetSeconds () << "(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetNextRouterSpecific" << endl; 

  for (BundleList::iterator iter = m_routerSpecificList.begin ();
       iter != m_routerSpecificList.end ();
       ++iter)
    {
      Ptr<Bundle> rsBundle = *iter;
      Ptr<Link> link = m_linkManager->FindLink (rsBundle->GetDestinationEndpoint ());

      CanonicalBundleHeader header = rsBundle->GetCanonicalHeaders ().front ();

      if (header.GetBlockType () == KNOWN_DELIVERED_MESSAGES_BLOCK)
        {
          Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
          double txTime = oc->GetDataRate ().CalculateTxTime (EstimateNeededBytes (rsBundle));
          
          if (txTime <= oc->GetContactWindowDuration ().GetSeconds ())
            {
              return LinkBundle (link, rsBundle);
            }
        }
      else 
        {
          return LinkBundle (link, rsBundle);
        }
    }

  return LinkBundle (0,0);
}

LinkBundleList
OrwarRouterChangedOrder::GetAllDeliverableBundles ()
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetAllDeliverableBundles");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetAllDeliverableBundles" << endl;

  //Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
  Ptr<LinkManager> olm (dynamic_cast<LinkManager *> (PeekPointer (m_linkManager)));

  Links links = olm->GetReadyLinks ();
  LinkBundleList result;
  for (Links::iterator iter = links.begin (); iter != links.end (); ++iter)
    {
      Ptr<Link> link = *iter;
      LinkBundleList linkBundleList = GetAllBundlesForLink (link);
      result.insert (result.end (), linkBundleList.begin (), linkBundleList.end ());
    }
  return result;
}

LinkBundleList
OrwarRouterChangedOrder::GetAllBundlesForLink (Ptr<Link> link)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder:: GetAllBundlesForLink");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetAllBundlesForLink (" << link->GetRemoteEndpointId ().GetId () << ")" << endl;
  LinkBundleList linkBundleList;
  if (link->GetState () == LINK_CONNECTED)
    {
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
      if (oc->GetState () == READY)
        {
          ///cout << m_bundleList.size () << endl;
          ///cout << GetNBundles () << endl;
          for (BundleList::iterator iter = m_bundleList.begin (); iter != m_bundleList.end (); ++iter)
            {
              Ptr<Bundle> bundle = *iter;
              double txTime = oc->GetDataRate ().CalculateTxTime (EstimateNeededBytes (bundle));
              /*              
              ///cout << "The bundle: " << endl << *bundle << endl;
              ///cout << boolalpha;   
              ///cout << "Txtime = " << txTime << endl;
              ///cout << "The contact window: " << oc->GetContactWindowDuration ().GetSeconds () << endl;
              ///cout << "Has RC: " << bundle->HasRetentionConstraint (RC_FORWARDING_PENDING) << endl;
              ///cout << "Enough time in contact window: " <<  (txTime <= oc->GetContactWindowDuration ().GetSeconds ()) << endl;
              ///cout << "Not been received from: " << !bundle->HaveBeenReceivedFrom (link) << endl;
              ///cout << "Replication factor > 1: " <<  (bundle->GetReplicationFactor () > 1) << endl;
              ///cout << "Forward log does not have an entry: " << !m_forwardLog.HasEntry (bundle, link) << endl;
              ///cout << noboolalpha;
              */
              if (bundle->HasRetentionConstraint (RC_FORWARDING_PENDING) &&
                  txTime <= oc->GetContactWindowDuration ().GetSeconds () &&
                  !bundle->HaveBeenReceivedFrom (link) &&
                  (link->GetRemoteEndpointId () == bundle->GetDestinationEndpoint ()) &&
                  !m_forwardLog.HasEntry (bundle, link))
                {
                  linkBundleList.push_back (LinkBundle (link, *iter));
                }
            }
        }
    }
  return linkBundleList;
}

LinkBundleList
OrwarRouterChangedOrder::GetAllBundlesToAllLinks ()
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder:: GetAllBundlesToAllLinks");
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetAllBundlesToAllLinks" << endl;
  //Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
  Ptr<LinkManager> olm (dynamic_cast<LinkManager *> (PeekPointer (m_linkManager)));
  Links links = olm->GetReadyLinks ();
  LinkBundleList linkBundleList;

  for (Links::iterator iter = links.begin (); iter != links.end (); ++iter)
    {
      Ptr<Link> link = *iter;
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
      //Ptr<Contact> oc = dynamic_cast<Contact *> (PeekPointer (link->GetContact ()));
      for (BundleList::iterator it = m_bundleList.begin (); it != m_bundleList.end (); ++it)
        {
          Ptr<Bundle> bundle = *it;
          double txTime = oc->GetDataRate ().CalculateTxTime (EstimateNeededBytes (bundle));
          /*
          ///cout << "The bundle: " << endl << *bundle << endl;
          ///cout << boolalpha;   
          ///cout << "Txtime = " << txTime << endl;
          ///cout << "The contact window: " << oc->GetContactWindowDuration ().GetSeconds () << endl;
          ///cout << "Has RC: " << bundle->HasRetentionConstraint (RC_FORWARDING_PENDING) << endl;
          ///cout << "Enough time in contact window: " <<  (txTime <= oc->GetContactWindowDuration ().GetSeconds ()) << endl;
          ///cout << "Not been received from: " << !bundle->HaveBeenReceivedFrom (link) << endl;
          ///cout << "Replication factor > 1: " <<  (bundle->GetReplicationFactor () > 1) << endl;
          ///cout << "Forward log does not have an entry: " << !m_forwardLog.HasEntry (bundle, link) << endl;
          ///cout << noboolalpha;
          */
          if (bundle->HasRetentionConstraint (RC_FORWARDING_PENDING) &&
              txTime <= oc->GetContactWindowDuration ().GetSeconds () &&
              !bundle->HaveBeenReceivedFrom (link) &&
               bundle->GetReplicationFactor () > 1 &&
              !m_forwardLog.HasEntry (bundle, link))
            {
              linkBundleList.push_back (LinkBundle (link, bundle));
            }
        }
    }
  return linkBundleList;
}

bool
OrwarRouterChangedOrder::DoIsRouterSpecific (Ptr<Bundle> bundle)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoIsRouterSpecific");
  CanonicalBundleHeader header = bundle->GetCanonicalHeaders ().front ();
  return IsRouterSpecific (header.GetBlockType ());
}

bool
OrwarRouterChangedOrder::DoIsRouterSpecific (const BlockType& block)
{
  switch (block)
    {
    case KNOWN_DELIVERED_MESSAGES_BLOCK:
    case CONTACT_WINDOW_INFORMATION_BLOCK:
      return true;
      break;
    default:
      break;
    }
  return false;
}

void
OrwarRouterChangedOrder::SendRouterSpecific (Ptr<Link> link, Ptr<Bundle> bundle)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::SendRouterSpecific");
}

void
OrwarRouterChangedOrder::SentRouterSpecific (Ptr<Link> link, const GlobalBundleIdentifier& gbid)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::SentRouterSpecific");
  //cout << Simulator::Now ().GetSeconds () << "(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::SentRouterSpecific" << endl;
  Ptr<Bundle> bundle = GetBundle (gbid);
  
  if (bundle != 0)
    {
      RemoveRouterSpecificBundle (bundle,0);
      CanonicalBundleHeader header = bundle->GetCanonicalHeaders ().front ();
      switch (header.GetBlockType ())
        {
        case KNOWN_DELIVERED_MESSAGES_BLOCK:
          SentKdm (link);
          break;
        case CONTACT_WINDOW_INFORMATION_BLOCK:
          SentCwi (link);
          break;
        default:
          break;
        }
    }
  else
    {
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
    }
}

void
OrwarRouterChangedOrder::ReceiveRouterSpecific (Ptr<Bundle> bundle)
{

  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::ReceiveRouterSpecific");
  CanonicalBundleHeader header = bundle->GetCanonicalHeaders ().front ();
  
  switch (header.GetBlockType ())
    {
    case KNOWN_DELIVERED_MESSAGES_BLOCK:
      HandleKdm (bundle);
      break;
    case CONTACT_WINDOW_INFORMATION_BLOCK:
      HandleCwi (bundle);
      break;
    default:
      //cout << Doh this should not happen!! :/";
      break;
    }
}


void
OrwarRouterChangedOrder::AddRouterSpecificBundle (Ptr<Bundle> bundle)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::AddRouterSpecificBundle");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::AddRouterSpecificBundle" << endl;
  m_routerSpecificList.push_back (bundle);
  //cout << "(" << m_node->GetId () << ") " << "======== TryToStartSending anropat fr�n AddRouterSpecific ========" << endl;
  Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
}

void
OrwarRouterChangedOrder::RemoveRouterSpecificBundle (const GlobalBundleIdentifier& gbid, uint8_t reason)
{
  Ptr<Bundle> bundle = GetRouterSpecificBundle (gbid);
  if (bundle != 0)
    {
      m_routerDeleteLogger (bundle, reason);
    }
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::RemoveRouterSpecificBundle");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::RemoveRouterSpecificBundle" << endl;
  BundleList::iterator iter = remove_if (m_routerSpecificList.begin (), m_routerSpecificList.end (), MatchingGbid (gbid));

  if (iter != m_routerSpecificList.end ())
    {
      m_routerSpecificList.erase (iter, m_routerSpecificList.end ());
    }
}

bool
OrwarRouterChangedOrder::HasRouterSpecificBundle (const GlobalBundleIdentifier& gbid)
{
  //NS_LOG_DEBUG' ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::HasRouterSpecificBundle");
  return find_if (m_routerSpecificList.begin (), m_routerSpecificList.end (), MatchingGbid (gbid)) != m_routerSpecificList.end ();
}

Ptr<Bundle>
OrwarRouterChangedOrder::GetRouterSpecificBundle (const GlobalBundleIdentifier& gbid)
{
  //NS_LOG_DEBUG' ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::GetRouterSpecificBundle");
  BundleList::iterator iter = find_if (m_routerSpecificList.begin (), m_routerSpecificList.end (), MatchingGbid (gbid));

  if (iter != m_routerSpecificList.end ())
    {
      return *iter;
    }
  else
    {
      return 0;
    }
}

uint32_t
OrwarRouterChangedOrder::EstimateNeededBytes (Ptr<Bundle> bundle) const
{
  //NS_LOG_DEBUG' ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::EstimateNeededBytes");
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent> ()->GetConvergenceLayerAgent ();
  uint32_t overHeadPerFragment = 40;
  uint16_t neededFragments = ceil ((double) bundle->GetSize () / (double) (cla->GetNetDevice ()->GetMtu () - overHeadPerFragment));

  return bundle->GetSize () + overHeadPerFragment * neededFragments;
}

void
OrwarRouterChangedOrder::SendKdm (Ptr<Link> link, bool isInitiator)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendKdm " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Kdm>>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ") contains " << m_kdm.NEntries () << " bundles");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendKdm " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Kdm>>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ") contains " << m_kdm.NEntries () << " bundles" << endl;
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendKdm " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Kdm";
  //if (isInitiator)
    {
      ///cout << "*";
    }
  ///cout << ">>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ") contains " << m_kdm.NEntries () << " bundles" << endl;
  Ptr<Bundle> kdmBundle = GenerateKdm (link, isInitiator);
  AddRouterSpecificBundle (kdmBundle);
}

void
OrwarRouterChangedOrder::SendCwi (Ptr<Link> link, bool isInitiator)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendCwi " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Cwi>>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ")");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendCwi " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Cwi>>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ")" << endl;
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SendCwi " << "\t(" << m_node->GetId () << ")"  << " >>>>>>>>>>>>>Cwi";
  //if (isInitiator)
    ///cout << "*";
  ///cout << ">>>>>>>>>>>>> " << "(" << link->GetRemoteEndpointId ().GetSsp () << ")" << endl;
  Ptr<Bundle> cwiBundle = GenerateCwi (link, isInitiator);
  AddRouterSpecificBundle (cwiBundle);
}

void
OrwarRouterChangedOrder::SentKdm (Ptr<Link> link)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentKdm");
 
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentKdm" << endl;
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentKdm to (" << link->GetRemoteEndpointId ().GetId () << ")" << endl;
  
  if (link->GetState () == LINK_CONNECTED)
    {
      Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
      
      if (oc->GetState () == S_RECEIVED_CWI)
        {
          oc->ChangeState (S_WAITING_FOR_KDM);
        }
      else if (oc->GetState () == R_RECEIVED_KDM)
        {
          oc->ChangeState (READY);
        }
      else 
        {
          NS_LOG_DEBUG ("Something is wrong!" << "(" << m_node->GetId () << ") ");
          //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Something is wrong!" << endl;
        }
    }
      //cout << "(" << m_node->GetId () << ") " << "======== TryToStartSending anropat fr�n SentKdm =========" << endl;
 Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);

}

void
OrwarRouterChangedOrder::SentCwi (Ptr<Link> link)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentCwi");
 //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentCwi"  << endl;
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::SentCwi to (" << link->GetRemoteEndpointId ().GetId () << ")" << endl;

  if (link->GetState () == LINK_CONNECTED)
    {
      Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
      
      if (oc->GetState () == S_CREATED)
        {
          //NS_LOG_INFO ("Sender: Sent kdm, waiting for kdm in response");
          
          //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Sender: Sent kdm, waiting for kdm in response" << endl;
          oc->ChangeState (S_WAITING_FOR_CWI);
        }
      else if (oc->GetState () == R_RECEIVED_CWI)
        {
          oc->ChangeState (R_WAITING_FOR_KDM);
         }
      else 
        {
          //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "Something is wrong!" << endl;
          NS_LOG_DEBUG ("Something is wrong!" << "(" << m_node->GetId () << ") ");
        }
    }
      //cout << "(" << m_node->GetId () << ") " << "======== TryToStartSending anropat fr�n SentCwi ========" << endl;
  Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
}

void
OrwarRouterChangedOrder::HandleCwi (Ptr<Bundle> bundle)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::HandleCwi " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Cwi<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") ");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::HandleCwi " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Cwi<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") " << endl;

  Ptr<Packet> serializedCwi = bundle->GetPayload ();
  uint8_t *buffer = new uint8_t [serializedCwi->GetSize ()];
  serializedCwi->CopyData (buffer, serializedCwi->GetSize ());
  ContactWindowInformation cwi = ContactWindowInformation::Deserialize (buffer);
  delete [] buffer;

  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::HandleCwi " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Cwi";
  //  if (cwi.IsInitiator ())
    ///cout << "*";
  ///cout << "<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") " << endl;

  Ptr<Link> link = m_linkManager->FindLink (bundle->GetSourceEndpoint ());

  if (link == 0)
    {
      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") Got a CWI and have no previous link" << endl;
      EidAddress ea = bundle->GetReceivedFrom ().front ();
      link = CreateLink (ea.GetEndpoint (), ea.GetAddress ());
      m_linkManager->AddLink (link);
      m_linkManager->OpenLink (link);
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
      oc->ChangeState (R_RECEIVED_CWI);
      m_contactOppBetweenLogger (m_node->GetId (), link->GetRemoteEndpointId ().GetId ());
    }
  else if (link->GetState () == LINK_UNAVAILABLE)
    {
      //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") Got a CWI and the link is unavailable" << endl;
      m_linkManager->OpenLink (link);
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
      oc->ChangeState (R_RECEIVED_CWI);
      m_contactOppBetweenLogger (m_node->GetId (), link->GetRemoteEndpointId ().GetId ());
    }

  if (link->GetState () == LINK_CONNECTED)
    {
      Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ()));
      
      if (oc->GetState () == S_WAITING_FOR_CWI && !cwi.IsInitiator ())
        {
          //cout << Simulator::Now ().GetSeconds () << " Sender: Got cwi while waiting for cwi " << endl;
          Simulator::ScheduleNow (&OrwarRouterChangedOrder::SendKdm, this, link, true);
          CalculateContactWindow (link, cwi);
          oc->ChangeState (S_RECEIVED_CWI);
        }
      else if (oc->GetState () == R_RECEIVED_CWI && cwi.IsInitiator ()) 
        { 
          //cout << Simulator::Now ().GetSeconds () << " Receiver: Got a cwi* want to send cwi as response" << endl;
          Simulator::ScheduleNow (&OrwarRouterChangedOrder::SendCwi, this, link, false);
          CalculateContactWindow (link, cwi);
        }
      else
        {
           ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") HandleCwi" << " The contact is out of synch! Hmm the link is connected" << endl;
          ///cout << *link << endl;
          m_outOfSynchLogger (1);
          Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
        }
    }
  else
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") HandleCwi" << " The contact is out of synch! Hmm the link is not connected" << endl;
      ///cout << *link << endl;
      m_outOfSynchLogger (1);
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
    }
}
  
void
OrwarRouterChangedOrder::HandleKdm (Ptr<Bundle> bundle)
{
  Ptr<Packet> serializedKdm = bundle->GetPayload ();
  uint8_t *buffer = new uint8_t[serializedKdm->GetSize ()];  
  serializedKdm->CopyData (buffer, serializedKdm->GetSize ());
  KnownDeliveredMessages kdm = KnownDeliveredMessages::Deserialize (buffer);
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " <<"OrwarRouterChangedOrder::HandleKdm " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Kdm<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") contains " << kdm.NEntries () << " bundles");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::HandleKdm " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Kdm<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") contains " << kdm.NEntries () << " bundles" << endl;
  ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::HandleKdm " << "\t(" << m_node->GetId () << ")" << " <<<<<<<<<<<<<Kdm";
  //if (kdm.IsInitiator ())
    ///cout << "*";
  ///cout << "<<<<<<<<<<<<< " << "(" << bundle->GetSourceEndpoint ().GetSsp () << ") contains " << kdm.NEntries () << " bundles" << endl;

  Ptr<Link> link = m_linkManager->FindLink (bundle->GetSourceEndpoint ());

  if ((link == 0) || (link->GetState () != LINK_CONNECTED))
    {
      m_outOfSynchLogger (1);
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
      return;
    }

  Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
  if (oc->GetState () == S_WAITING_FOR_KDM && !kdm.IsInitiator ())
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& OrwarRouterChangedOrder::HandleKdm The contact between" << " (" << m_node->GetId () << ") and" << " (" << link->GetRemoteEndpointId ().GetId () << ") has been finalized" << endl;
      m_contactSetupLogger (0);
      oc->ChangeState (READY);
    }
  else if (oc->GetState () == R_WAITING_FOR_KDM && kdm.IsInitiator ())
    {
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::SendKdm, this, link, false);
      oc->ChangeState (R_RECEIVED_KDM);
    }
  else 
    {
      ///cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") HandleKdm" << "The contact is out of synch!" << endl;
      m_outOfSynchLogger (1);
      Simulator::ScheduleNow (&OrwarRouterChangedOrder::TryToStartSending, this);
    }
      
  RemoveDeliveredBundles (kdm);
  m_kdm.Merge (kdm);
  delete [] buffer;

}

void 
OrwarRouterChangedOrder::RemoveDeliveredBundles (const KnownDeliveredMessages& kdm)
{
  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::RemoveDelivered");
  BundleList bl;
  for (BundleList::iterator iter = m_bundleList.begin (); iter != m_bundleList.end (); ++iter)
    {
      Ptr<Bundle> bundle = *iter;
      if (kdm.Has (bundle))
        {
          bl.push_back (bundle);
          m_forwardLog.RemoveEntriesFor (bundle->GetBundleId ());
        }
    }
  
  for (BundleList::iterator iter = bl.begin (); iter != bl.end (); ++iter)
    {
      DeleteBundle (*iter, false);
    }
}

Ptr<Bundle>
OrwarRouterChangedOrder::GenerateCwi (Ptr<Link> link, bool isInitiator)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::GenerateCwi");
  Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent> ()->GetConvergenceLayerAgent ();
  ContactWindowInformation cwi = ContactWindowInformation (mobility->GetPosition (), mobility->GetVelocity (), cla->GetTransmissionRange (), cla->GetTransmissionSpeed (), isInitiator);
  uint8_t buffer[cwi.GetSerializedSize ()];
  cwi.Serialize (buffer);

  Ptr<Packet> adu = Create<Packet> (buffer, cwi.GetSerializedSize ());
  PrimaryBundleHeader primaryHeader = PrimaryBundleHeader ();
  primaryHeader.SetDestinationEndpoint (link->GetRemoteEndpointId ());
  primaryHeader.SetSourceEndpoint (m_eid);
  primaryHeader.SetCustodianEndpoint (m_eid);
  primaryHeader.SetReplicationFactor (1);
  primaryHeader.SetCreationTimestamp (CreationTimestamp ());
  primaryHeader.SetLifetime (Seconds (60));

  CanonicalBundleHeader canonicalHeader = CanonicalBundleHeader (CONTACT_WINDOW_INFORMATION_BLOCK);
  canonicalHeader.SetMustBeReplicated (false);
  canonicalHeader.SetStatusReport (false);
  canonicalHeader.SetDeleteBundle (false);
  canonicalHeader.SetLastBlock (true);
  canonicalHeader.SetDiscardBlock (true);
  canonicalHeader.SetForwarded (false);
  canonicalHeader.SetContainsEid (false);
  canonicalHeader.SetBlockLength (adu->GetSize ());
  
  Ptr<Bundle> cwiBundle = Create<Bundle> ();
  cwiBundle->SetPayload (adu);
  cwiBundle->SetPrimaryHeader (primaryHeader);
  cwiBundle->AddCanonicalHeader (canonicalHeader);

  m_createRouterLogger (cwiBundle);

  return cwiBundle;
}

Ptr<Bundle>
OrwarRouterChangedOrder::GenerateKdm (Ptr<Link> link, bool isInitiator)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::GenerateKdm");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::GenerateKdm" << endl;
  m_kdm.RemoveExpiredBundles (); // Removes all expired bundles from the kdm, before createing the bundle to send.
  m_kdm.SetIsInitiator (isInitiator);
  uint8_t buffer[m_kdm.GetSerializedSize ()];
  m_kdm.Serialize (buffer);

  KnownDeliveredMessages test = KnownDeliveredMessages::Deserialize (buffer);
  
  Ptr<Packet> adu = Create<Packet> (buffer, m_kdm.GetSerializedSize ());
  PrimaryBundleHeader primaryHeader = PrimaryBundleHeader ();
  primaryHeader.SetDestinationEndpoint (link->GetRemoteEndpointId ());
  primaryHeader.SetSourceEndpoint (m_eid);
  primaryHeader.SetCustodianEndpoint (m_eid);
  primaryHeader.SetReplicationFactor (1);
  primaryHeader.SetCreationTimestamp (CreationTimestamp ());
  primaryHeader.SetLifetime (Seconds (60));

  CanonicalBundleHeader canonicalHeader = CanonicalBundleHeader (KNOWN_DELIVERED_MESSAGES_BLOCK);
  canonicalHeader.SetMustBeReplicated (false);
  canonicalHeader.SetStatusReport (false);
  canonicalHeader.SetDeleteBundle (false);
  canonicalHeader.SetLastBlock (true);
  canonicalHeader.SetDiscardBlock (true);
  canonicalHeader.SetForwarded (false);
  canonicalHeader.SetContainsEid (false);
  canonicalHeader.SetBlockLength (adu->GetSize ());

  Ptr<Bundle> kdm = Create<Bundle> ();
  kdm->SetPayload (adu);
  kdm->SetPrimaryHeader (primaryHeader);
  kdm->AddCanonicalHeader (canonicalHeader);
  m_createRouterLogger (kdm);
  return kdm;
}

void
OrwarRouterChangedOrder::CalculateContactWindow (Ptr<Link> link, const ContactWindowInformation& cwi)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "OrwarRouterChangedOrder::CalculateContactWindow");
      Ptr<OrwarContact> oc (dynamic_cast<OrwarContact *> (PeekPointer (link->GetContact ())));
  Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent> ()->GetConvergenceLayerAgent ();

  Vector p1 = mobility->GetPosition ();
  Vector v1 = mobility->GetVelocity ();
  uint32_t r1 = cla->GetTransmissionRange ();
  
  Vector p2 = cwi.GetPosition ();
  Vector v2 = cwi.GetVelocity ();
  uint32_t r2 = cwi.GetTransmissionRange ();
  

  double minr = min (r1,r2);
  double a = v2.y - v1.y;
  double b = v1.x - v2.x;
  double c = p2.y - p1.y;
  double d = p1.x - p2.x;

  double result;

  if (a == 0 && b == 0)
    {
      result = numeric_limits<uint32_t>::max();
      oc->SetCwMax (true);
    }
  else 
    {
      double e = pow (a,2) + pow (b,2);
      double f = c*b - a*d;
      double s = pow (minr,2) * e - pow (f,2);
      double numerator = -a*c + sqrt (s) - b*d;
      result = numerator / e; 
    }
  
  // This case exist beacuse the nodes transmission range is not exact, sometimes the nodes detects each other from a larger distance.
  // This can result in a negative contact window if they according to their conectivity settings are moving away from each other.
  // One solution could be to set result to zero if the distance between the nodes according to the coordinates is larger than minr,
  // but this solution also sets 
  if (result < 0)
    {
      result = 0;
    }
  
  //debug << "### " << Simulator::Now ().GetSeconds () << " Contact between 0 and " << oc->GetLink ()->GetRemoteEndpointId ().GetId () << endl;
  //debug << "p1=" << p1 << " v1=" << v1 << " r1=" << r1 << endl;
  //debug << "p2=" << p2 << " v2=" << v2 << " r2=" << r2 << endl;
  //debug << "result = " << result << endl;
  //debug << "#############" << endl << endl;
  
  //NS_LOG_INFO ("(" << m_node->GetId () << ") " << "Contact window estimated to: " << result);
  oc->SetContactWindowDuration (Seconds (result));
  oc->SetDataRate (cla->GetTransmissionSpeed ());

  Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
  if (GetPointer(olm) != NULL) {
	  olm->FinishedSetup (link);
  }
  m_estimatedCwLogger (m_node->GetId (), link->GetRemoteEndpointId ().GetId (), oc->GetContactWindowDuration (), oc->GetCwMax ());
}

void
OrwarRouterChangedOrder::DoBundleDelivered (Ptr<Bundle> bundle, bool fromAck)
{
  NS_LOG_DEBUG ("############################# " << "(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoBundleDelivered");
  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") ############################# " << "OrwarRouterChangedOrder::DoBundleDelivered" << endl;
  m_kdm.Insert (bundle);
  if (fromAck)
    {
      RemoveDeliveredBundles (m_kdm);
    }
}

Ptr<Link>
OrwarRouterChangedOrder::DoCreateLink (const BundleEndpointId& eid, const Address& address)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "OrwarRouterChangedOrder::DoCreateLink");
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent> ()->GetConvergenceLayerAgent ();
  Ptr<OrwarLink> link = CreateObject<OrwarLink> ();

  link->SetLinkLostCallback (MakeCallback (&ConvergenceLayerAgent::LinkLost, cla));
  link->SetHackFixmeCallback (MakeCallback (&ConvergenceLayerAgent::HackFixme, cla));

  /*
  Ptr<OrwarLinkManager> olm (dynamic_cast<OrwarLinkManager *> (PeekPointer (m_linkManager)));
  link->m_expirationTimer.SetFunction (&OrwarLinkManager::CheckIfExpired, olm);
  link->m_expirationTimer.SetArguments (link);
  */
  link->SetRemoteEndpointId (eid);
  link->SetRemoteAddress (address);
  return link;
}

}} // namespace bundleProtocol, ns3
