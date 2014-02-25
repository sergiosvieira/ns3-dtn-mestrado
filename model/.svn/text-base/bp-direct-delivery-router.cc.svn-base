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

#include "bp-direct-delivery-router.h"
#include "bp-header.h"
#include "bp-contact.h"
#include "bp-link-manager.h"
#include "bp-neighbourhood-detection-agent.h"

NS_LOG_COMPONENT_DEFINE ("DirectDeliveryRouter");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (DirectDeliveryRouter);

TypeId
DirectDeliveryRouter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::DirectDeliveryRouter")
    .SetParent<BundleRouter> ()
    .AddConstructor<DirectDeliveryRouter> ()
    .AddAttribute ("AlwaysSendHello",
                   "Sets if the router always should send hellos or only when it have something to send.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&DirectDeliveryRouter::m_alwaysSendHello),
                   MakeBooleanChecker ())
    .AddTraceSource ("RedundantRelay", "A message already held in the buffer has been received.",
                     MakeTraceSourceAccessor (&DirectDeliveryRouter::m_redundantRelayLogger))
    ;
  return tid;
}

DirectDeliveryRouter::DirectDeliveryRouter ()
  : BundleRouter ()
{}

DirectDeliveryRouter::~DirectDeliveryRouter ()
{}

void
DirectDeliveryRouter::DoInit ()
{
  if (m_alwaysSendHello)
    {
      m_nda->Start ();
    }
}

void
DirectDeliveryRouter::DoDispose ()
{
  BundleRouter::DoDispose ();
}

void 
DirectDeliveryRouter::DoLinkClosed (Ptr<Link> link)
{
  if (link->GetContact () != 0)
    {
      BundleList bundles = link->GetContact ()->GetQueuedBundles ();
      for (BundleList::iterator iter = bundles.begin (); iter != bundles.end (); ++iter)
        {
          link->GetContact ()->DequeueBundle (*iter);
          CancelTransmission (*iter, link);
        }
    }
  RemoveRouterSpecificBundles (link);
}

void
DirectDeliveryRouter::RemoveRouterSpecificBundles (Ptr<Link> link)
{}

void 
DirectDeliveryRouter::PauseLink (Ptr<Link> link)
{ 
  if (link->GetState () == LINK_CONNECTED)
    {
      BundleList bundles = link->GetContact ()->GetQueuedBundles ();
      
      for (BundleList::iterator iter = bundles.begin (); iter != bundles.end (); ++iter)
        {
          link->GetContact ()->DequeueBundle (*iter);
          CancelTransmission (*iter, link);
        }

      link->ChangeState (LINK_PAUSED);
      
      Ptr<Contact> c = link->GetContact ();
      
      if (c->GetRetransmissions () >= m_maxRetries)
        {
          m_linkManager->CloseLink (link);
        }
      else
        {
          c->IncreaseRetransmissions ();
          Simulator::Schedule (m_pauseTime, &DirectDeliveryRouter::UnPauseLink, this, link);
        }
    }
  Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
}

void 
DirectDeliveryRouter::UnPauseLink (Ptr<Link> link)
{
  if (link->GetState () == LINK_PAUSED)
    {
      link->ChangeState (LINK_CONNECTED);
      Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
    }
}

void
DirectDeliveryRouter::DoLinkDiscovered (Ptr<Link> link)
{
  m_linkManager->OpenLink (link);
  Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
}

void
DirectDeliveryRouter::DoBundleReceived (Ptr<Bundle> bundle)
{
  Ptr<Link> link = m_linkManager->FindLink (bundle->GetReceivedFrom ().front ().GetEndpoint ());
  if (link != 0)
    {
      link->UpdateLastHeardFrom ();
    }
}

Ptr<Bundle>
DirectDeliveryRouter::DoSendBundle (Ptr<Link> link, Ptr<Bundle> bundle)
{
  link->GetContact ()->EnqueueBundle (bundle);
  Ptr<Bundle> send = bundle->Copy ();
  return send;
}

void
DirectDeliveryRouter::DoBundleSent (const Address& address, const GlobalBundleIdentifier& gbid, bool finalDelivery)
{
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

      m_forwardLog.AddEntry (bundle, link);
      
      if (finalDelivery)
        {
          BundleDelivered (bundle, true);
        }
      Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
    }
  else
    {
      if (finalDelivery)
        {
          // This is a ugly hack utilitzing the fact that i know that 
          // the ttl is "inifinite".
          Ptr<Bundle> bundle = Create<Bundle> ();
          bundle->SetSourceEndpoint (gbid.GetSourceEid ());
          bundle->SetCreationTimestamp (gbid.GetCreationTimestamp ());
          bundle->SetLifetime (43000);
          BundleDelivered (bundle, true);
          Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
        }
    }
}

void
DirectDeliveryRouter::DoBundleTransmissionFailed (const Address& address, const GlobalBundleIdentifier& gbid)
{
  Mac48Address mac = Mac48Address::ConvertFrom (address);
  Ptr<Link> link = m_linkManager->FindLink (mac);
  PauseLink (link);
}

bool
DirectDeliveryRouter::DoAcceptBundle (Ptr<Bundle> bundle, bool fromApplication)
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
DirectDeliveryRouter::DoCanDeleteBundle (const GlobalBundleIdentifier& gbid)
{
  return true;
}

void
DirectDeliveryRouter::DoInsert (Ptr<Bundle> bundle)
{
  // This is my extra thing, i always set the eid to current node holding the bundle.
  bundle->SetCustodianEndpoint (m_eid);

  m_bundleList.push_back (bundle);
   
  // If this is the first bundle, I now want to begin sending hello messages announcing that
  // I have something to send. If there is more than one bundle in the queue this means that
  // I already have started sending hello messages.
  if (m_nBundles == 1 && !m_alwaysSendHello)
    {
      m_nda->Start ();
    }
  
  Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
}

bool
DirectDeliveryRouter::CanMakeRoomForBundle (Ptr<Bundle> bundle)
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
DirectDeliveryRouter::MakeRoomForBundle (Ptr<Bundle> bundle)
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
DirectDeliveryRouter::DoDelete (const GlobalBundleIdentifier& gbid, bool drop)
{
  // If this is the last bundle in the queue, stop sending Hello messages. 
  if (m_nBundles == 1 && !m_alwaysSendHello)
    {
      m_nda->Stop ();
    }
  return BundleRouter::DoDelete (gbid, drop);
}


void
DirectDeliveryRouter::DoCancelTransmission (Ptr<Bundle> bundle, Ptr<Link> link)
{}

void
DirectDeliveryRouter::DoTransmissionCancelled (const Address& address, const GlobalBundleIdentifier& gbid)
{
  Mac48Address mac = Mac48Address::ConvertFrom (address);
  Ptr<Link> link = m_linkManager->FindLink (mac);
  link->GetContact ()->DequeueBundle (gbid);

  Simulator::ScheduleNow (&DirectDeliveryRouter::TryToStartSending, this);
}

void
DirectDeliveryRouter::TryToStartSending ()
{
	NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  RemoveExpiredBundles (true);
  m_forwardLog.RemoveExpiredEntries ();
  
  if (!IsSending () && (GetNBundles () > 0))
    {
	  NS_LOG_DEBUG("(" << m_node->GetId() << ") Not Sending and NBundle > 0");
      LinkBundle linkBundle = FindNextToSend ();
      if (!linkBundle.IsNull ())
        {
    	  NS_LOG_DEBUG("(" << m_node->GetId() << ") Link IS NOT null");
          SendBundle (linkBundle.GetLink (), linkBundle.GetBundle ());
        } else {
        	NS_LOG_DEBUG("(" << m_node->GetId() << ") Link IS null");
        }
    } else {
    	NS_LOG_DEBUG("(" << m_node->GetId() << ") Sending and NBundle = " << GetNBundles());
    }
}

LinkBundle
DirectDeliveryRouter::FindNextToSend ()
{
  if ((m_linkManager->GetConnectedLinks ().size () > 0) && (GetNBundles () > 0))
    {
	  NS_LOG_DEBUG("(" << m_node->GetId() << ") connected links > 0 and getnbundle > 0");
      LinkBundleList linkBundleList = GetAllDeliverableBundles ();
      
      if (!linkBundleList.empty ())
        {
    	  NS_LOG_DEBUG("(" << m_node->GetId() << ") list not empty");
          return linkBundleList.front ();
        } else {
        	NS_LOG_DEBUG("(" << m_node->GetId() << ") list empty");
        }
    }
  NS_LOG_DEBUG("(" << m_node->GetId() << ") connected links < 0 and getnbundle < 0");
  return LinkBundle (0,0);
}


LinkBundle
DirectDeliveryRouter::GetNextRouterSpecific ()
{
  return LinkBundle (0,0);
}

LinkBundleList
DirectDeliveryRouter::GetAllDeliverableBundles ()
{
  Links links = m_linkManager->GetConnectedLinks ();
  NS_LOG_DEBUG("(" << m_node->GetId() << ") links = " << links.size());
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
DirectDeliveryRouter::GetAllBundlesForLink(Ptr<Link> link) {
	LinkBundleList linkBundleList;
	if (link->GetState() == LINK_CONNECTED) {
		NS_LOG_DEBUG("(" << m_node->GetId() << ") LINK CONNECTED");
		for (BundleList::iterator iter = m_bundleList.begin(); iter
				!= m_bundleList.end(); ++iter) {
			Ptr<Bundle> bundle = *iter;
			NS_LOG_DEBUG("(" << m_node->GetId() << ") HAS RETENTION = " << bundle->HasRetentionConstraint(RC_FORWARDING_PENDING));
			NS_LOG_DEBUG("(" << m_node->GetId() << ") Not HaveBeenReceivedFrom = " << !bundle->HaveBeenReceivedFrom(link));
			NS_LOG_DEBUG("(" << m_node->GetId() << ") Destination = " << (link->GetRemoteEndpointId() == bundle->GetDestinationEndpoint()));
			NS_LOG_DEBUG("(" << m_node->GetId() << ") link->remote_eid = " << link->GetRemoteEndpointId() << " - bundle->dest_eid = " << bundle->GetDestinationEndpoint());
			NS_LOG_DEBUG("(" << m_node->GetId() << ") not has entry = " << !m_forwardLog.HasEntry(bundle, link));
			if (bundle->HasRetentionConstraint(RC_FORWARDING_PENDING)
					&& !bundle->HaveBeenReceivedFrom(link)
					&& (link->GetRemoteEndpointId()
							== bundle->GetDestinationEndpoint())
					&& !m_forwardLog.HasEntry(bundle, link)) {
				NS_LOG_DEBUG("(" << m_node->GetId() << ") HAS RETENTION");
				linkBundleList.push_back(LinkBundle(link, *iter));
			} else {
				NS_LOG_DEBUG("(" << m_node->GetId() << ") NOT HAS RETENTION");
			}
		}

	}
	return linkBundleList;
}

uint8_t
DirectDeliveryRouter::DoCalculateReplicationFactor (const BundlePriority& priority) const
{
  return 1;
}

bool
DirectDeliveryRouter::DoIsRouterSpecific (Ptr<Bundle> bundle)
{
  return false;
}

bool
DirectDeliveryRouter::DoIsRouterSpecific (const BlockType& block)
{
  return false;
}

void
DirectDeliveryRouter::SendRouterSpecific (Ptr<Link> link, Ptr<Bundle> bundle)
{}

void
DirectDeliveryRouter::SentRouterSpecific (Ptr<Link> link, const GlobalBundleIdentifier& gbid)
{}

void
DirectDeliveryRouter::ReceiveRouterSpecific (Ptr<Bundle> bundle)
{}


void
DirectDeliveryRouter::AddRouterSpecificBundle (Ptr<Bundle> bundle)
{}

void
DirectDeliveryRouter::RemoveRouterSpecificBundle (const GlobalBundleIdentifier& gbid, uint8_t reason)
{}

bool
DirectDeliveryRouter::HasRouterSpecificBundle (const GlobalBundleIdentifier& gbid)
{
  return false;
}

Ptr<Bundle>
DirectDeliveryRouter::GetRouterSpecificBundle (const GlobalBundleIdentifier& gbid)
{
  return Create<Bundle> ();
}

void
DirectDeliveryRouter::DoBundleDelivered (Ptr<Bundle> bundle, bool fromAck)
{}

Ptr<Link>
DirectDeliveryRouter::DoCreateLink (const BundleEndpointId& eid, const Address& address)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "DirectDeliveryRouter::DoCreateLink");
  Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent> ()->GetConvergenceLayerAgent ();
  Ptr<Link> link = CreateObject<Link> ();

  link->SetLinkLostCallback (MakeCallback (&ConvergenceLayerAgent::LinkLost, cla));

  link->SetRemoteEndpointId (eid);
  link->SetRemoteAddress (address);
  return link;
}

void DirectDeliveryRouter::DoSendHello(Ptr<Socket> socket, BundleEndpointId eid) {
	NeighHeader header;
	header.SetBundleEndpointId(m_eid);

	/* Direct Link Type */
	TypeTag type(3);

	Ptr<Packet> hello = Create<Packet>();
	hello->AddPacketTag(type);
	hello->AddHeader(header);

	NS_LOG_DEBUG ("(" << m_node->GetId () << ") - eid = " << m_eid);

	socket->Send(hello);
}

void DirectDeliveryRouter::DoHandleHello(Ptr<Socket> socket) {
	Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel> ();
	NS_LOG_DEBUG ("(" << m_node->GetId () << ") - " << mm->GetPosition ());

	Ptr<Packet> receivedHello;
	Address fromAddress;
	receivedHello = socket->RecvFrom(fromAddress);

	NeighHeader header;
	receivedHello->PeekHeader(header);

	NS_LOG_DEBUG ("(" << m_node->GetId () << ") - eid = " << header.GetBundleEndpointId());


	Simulator::ScheduleNow(&NeighbourhoodDetectionAgent::NotifyDiscoveredLink,
			m_nda, receivedHello, fromAddress);

}
;


}} // namespace bundleProtocol, ns3
