/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author: Sérgio Vieira
 * MACC - Mestrado Acadêmico em Ciência da Computação - 2012
 */

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
#include "bp-rt-epidemic.h"
#include "bp-header.h"
#include "bp-contact.h"
#include "bp-link-manager.h"
#include "bp-neighbourhood-detection-agent.h"

NS_LOG_COMPONENT_DEFINE ("RTEpidemic");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (RTEpidemic);

TypeId RTEpidemic::GetTypeId(void) {
	static TypeId tid = TypeId ("ns3::bundleProtocol::RTEpidemic")
		.SetParent<BundleRouter> ()
		.AddConstructor<RTEpidemic> ()
		.AddAttribute ("AlwaysSendHello",
				"Sets if the router always should send hellos or only when it have something to send.",
				BooleanValue (true),
				MakeBooleanAccessor (&RTEpidemic::m_alwaysSendHello),
				MakeBooleanChecker ())
		.AddTraceSource ("RedundantRelay", "A message already held in the buffer has been received.",
				MakeTraceSourceAccessor (&RTEpidemic::m_redundantRelayLogger));

	return tid;
}

RTEpidemic::RTEpidemic() : BundleRouter(), m_send_timer((Timer::CANCEL_ON_DESTROY))
{

}

RTEpidemic::~RTEpidemic()
{
}

void RTEpidemic::DoInit()
{
	//cout<<"Init node("<<this->m_node->GetId()<<")"<<m_alwaysSendHello<<"\n";
	if (m_alwaysSendHello) {
		m_nda->Start();
	}
}

void RTEpidemic::DoDispose()
{
	BundleRouter::DoDispose();
}

void RTEpidemic::DoLinkClosed(Ptr<Link> link)
{
	if (link->GetContact() != 0) {
		BundleList bundles = link->GetContact()->GetQueuedBundles();
		for (BundleList::iterator iter = bundles.begin(); iter != bundles.end(); ++iter) {
			link->GetContact()->DequeueBundle(*iter);
			CancelTransmission(*iter, link);
		}
	}

	RemoveRouterSpecificBundles(link);
}

void RTEpidemic::RemoveRouterSpecificBundles(Ptr<Link> link)
{
}

void RTEpidemic::PauseLink(Ptr<Link> link)
{
	if (link->GetState() == LINK_CONNECTED) {
		BundleList bundles = link->GetContact()->GetQueuedBundles();

		for (BundleList::iterator iter = bundles.begin(); iter != bundles.end(); ++iter) {
			link->GetContact()->DequeueBundle(*iter);
			CancelTransmission(*iter, link);
		}

		link->ChangeState(LINK_PAUSED);

		Ptr<Contact> c = link->GetContact();

		if (c->GetRetransmissions() >= m_maxRetries) {
			m_linkManager->CloseLink(link);
		} else {
			c->IncreaseRetransmissions();
			Simulator::Schedule(m_pauseTime,
					&RTEpidemic::UnPauseLink, this, link);
		}
	}
	Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
}

void RTEpidemic::UnPauseLink(Ptr<Link> link)
{
	if (link->GetState() == LINK_PAUSED) {
		link->ChangeState(LINK_CONNECTED);
		Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
	}
}

void RTEpidemic::DoLinkDiscovered(Ptr<Link> link)
{
	m_linkManager->OpenLink(link);
	Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
}

void RTEpidemic::DoBundleReceived(Ptr<Bundle> bundle)
{
        SetBundleReceived("RTEpidemic.out",bundle);
	Ptr<Link> link = m_linkManager->FindLink(
			bundle->GetReceivedFrom().front().GetEndpoint());
	if (link != 0) {
		link->UpdateLastHeardFrom();
	}
}

Ptr<Bundle> RTEpidemic::DoSendBundle(Ptr<Link> link, Ptr<Bundle> bundle)
{
	link->GetContact()->EnqueueBundle(bundle);
	Ptr<Bundle> send = bundle->Copy();
	//send->SetLifetime(200);
	return send;
}

void RTEpidemic::DoBundleSent(const Address& address,
		const GlobalBundleIdentifier& gbid, bool finalDelivery)
{
	Mac48Address mac = Mac48Address::ConvertFrom(address);
	Ptr<Link> link = m_linkManager->FindLink(mac);

	Ptr<Bundle> bundle = GetBundle(gbid);
	if (bundle != 0) {
		// I have received an ack for the sent bundle, so i have heard from the
		// other node.
		link->UpdateLastHeardFrom();

		if (link->GetState() == LINK_CONNECTED || link->GetState()
				== LINK_PAUSED) {
			link->GetContact()->DequeueBundle(gbid);
			link->GetContact()->ResetRetransmissions();
		}

		m_forwardLog.AddEntry(bundle, link);

		if (finalDelivery) {
			BundleDelivered(bundle, true);
		}

		Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
	} else {
		if (finalDelivery) {
			// This is a ugly hack utilitzing the fact that i know that
			// the ttl is "inifinite".
			Ptr<Bundle> bundle = Create<Bundle> ();
			bundle->SetSourceEndpoint(gbid.GetSourceEid());
			bundle->SetCreationTimestamp(gbid.GetCreationTimestamp());
			bundle->SetLifetime(43000);
			BundleDelivered(bundle, true);

			Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
		}
	}
}

void RTEpidemic::DoBundleTransmissionFailed(const Address& address,
		const GlobalBundleIdentifier& gbid)
{
	Mac48Address mac = Mac48Address::ConvertFrom(address);
	Ptr<Link> link = m_linkManager->FindLink(mac);
	PauseLink(link);
}

bool RTEpidemic::DoAcceptBundle(Ptr<Bundle> bundle,
		bool fromApplication)
{
	if (HasBundle(bundle)) {
		m_redundantRelayLogger(bundle);
		Ptr<Bundle> otherBundle = GetBundle(bundle->GetBundleId());
		otherBundle->AddReceivedFrom(bundle->GetReceivedFrom().front());
		return false;
	}
	return CanMakeRoomForBundle(bundle);
}

bool RTEpidemic::DoCanDeleteBundle(const GlobalBundleIdentifier& gbid)
{
	return true;
}

void RTEpidemic::DoInsert(Ptr<Bundle> bundle)
{
	// This is my extra thing, i always set the eid to current node holding the bundle.
	bundle->SetCustodianEndpoint(m_eid);

	m_bundleList.push_back(bundle);

	// If this is the first bundle, I now want to begin sending hello messages announcing that
	// I have something to send. If there is more than one bundle in the queue this means that
	// I already have started sending hello messages.
	if (m_nBundles == 1 && !m_alwaysSendHello) {
		cout<<"Nao me chame\n";
		m_nda->Start();
	}

	Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
}

bool RTEpidemic::CanMakeRoomForBundle(Ptr<Bundle> bundle)
{
	if (bundle->GetSize() < m_maxBytes) {
		return true;
	} else {
		return false;
	}

}

bool RTEpidemic::MakeRoomForBundle(Ptr<Bundle> bundle)
{
	if (bundle->GetSize() < m_maxBytes) {
		if (bundle->GetSize() < GetFreeBytes()) {
			return true;
		}

		    //for (BundleList::reverse_iterator iter = m_bundleList.rbegin(); iter
                               // != m_bundleList.rend();) {
				/*Politica atual: remove o elemento mais velho da fila para colocar o que chegou*/
                for (BundleList::iterator iter = m_bundleList.begin(); iter
                                                != m_bundleList.end();) {
                        Ptr<Bundle> currentBundle = *(iter++);

                        DeleteBundle(currentBundle, true);
                        if (bundle->GetSize() < GetFreeBytes()) {
                                return true;
                        }
                }
	}

	SetBufferOverFlow("RTEpidemic.buff");

	return false;
}

bool RTEpidemic::DoDelete(const GlobalBundleIdentifier& gbid,
		bool drop)
{
	// If this is the last bundle in the queue, stop sending Hello messages.
	if (m_nBundles == 1 && !m_alwaysSendHello) {
		//m_nda->Stop();
	}
	if (drop)/*drop = true indica que o bundle expirou por isso está sendo apagado*/
	{
                SetBundleExpired("RTEpidemic.expired");
	}
	return BundleRouter::DoDelete(gbid, drop);
}

void RTEpidemic::DoCancelTransmission(Ptr<Bundle> bundle,
		Ptr<Link> link)
{
}

void RTEpidemic::DoTransmissionCancelled(const Address& address,
		const GlobalBundleIdentifier& gbid)
{
	Mac48Address mac = Mac48Address::ConvertFrom(address);
	Ptr<Link> link = m_linkManager->FindLink(mac);

	/*Joao*/
	if(link->GetContact())
	{
	  link->GetContact()->DequeueBundle(gbid);
	}
	/*Joao*/
	Simulator::ScheduleNow(&RTEpidemic::TryToStartSending, this);
}

void RTEpidemic::TryToStartSending()
{
	NS_LOG_DEBUG("(" << m_node->GetId () << ")" << "TrySend");
	RemoveExpiredBundles(true);
	m_forwardLog.RemoveExpiredEntries();

	if (!IsSending() && (GetNBundles() > 0)) {
		LinkBundle linkBundle = FindNextToSend();
		if (!linkBundle.IsNull()){
			linkBundle.GetBundle()->SetCustodyTransferRequested(false);
			linkBundle.GetBundle()->AddReceivedFrom(linkBundle.GetLink()->GetRemoteEndpointId().GetId());
			Ptr<MobilityModel> mm = m_node->GetObject<MobilityModel> ();
			//std::cout<<"Node ID:("<<m_node->GetId ()<<") Send Bundle ID: *"<<linkBundle.GetBundle()->GetGlobalId()<<"* To Node ("<<linkBundle.GetLink()->GetRemoteEndpointId()<<") ";
			//std::cout<<"Time: "<< Simulator::Now().GetSeconds()<<" Pos = "<<mm->GetPosition ()<<"\n";
			SendBundle(linkBundle.GetLink(), linkBundle.GetBundle());
			NS_LOG_DEBUG("(" << m_node->GetId () << ")" <<" Enviar" );
			/*Inseri na lista de pacotes que trasnferiram custodia, serve para que ele nao receba novamente o mesmo pacote*/
			//InsertCustodyHistorical(linkBundle.GetBundle()->GetBundleId());
			/*Inseri na lista de pacotes que trasnferiram custodia, mas ainda não esperaram resposta*/
			//InsertCustodyHistoricalPending(linkBundle.GetBundle()->GetBundleId());
			//Simulator::Schedule (Seconds(5.0), &RTEpidemic::EraseCustodyHistoricalPending,this,linkBundle.GetBundle()->GetBundleId());
		} else {
			Simulator::Schedule (Seconds(1.0), &RTEpidemic::TryToStartSending, this);
		}
	}
}

LinkBundle RTEpidemic::FindNextToSend()
{
	NS_LOG_DEBUG("RTEpidemic::FindNextToSend");
	NS_LOG_DEBUG("(" << m_node->GetId() << ") - m_linkManager->GetConnectedLinks().size()= " << m_linkManager->GetConnectedLinks().size() << " GetNBundles() = " << GetNBundles() );
	if ((m_linkManager->GetConnectedLinks().size() > 0) && (GetNBundles() > 0)) {
		LinkBundleList linkBundleList = GetAllDeliverableBundles();
		if (!linkBundleList.empty()) {
			return linkBundleList.front();
		}
	}
	return LinkBundle(0, 0);
}

LinkBundle RTEpidemic::GetNextRouterSpecific()
{
	return LinkBundle(0, 0);
}

LinkBundleList RTEpidemic::GetAllDeliverableBundles()
{
	Links links = m_linkManager->GetConnectedLinks();
	LinkBundleList result;
	for (Links::iterator iter = links.begin(); iter != links.end(); ++iter) {
		Ptr<Link> link = *iter;
		LinkBundleList linkBundleList = GetAllBundlesForLink(link);
		result.insert(result.end(), linkBundleList.begin(),
				linkBundleList.end());
	}
	return result;
}

LinkBundleList RTEpidemic::GetAllBundlesForLink(Ptr<Link> link)
{
	LinkBundleList linkBundleList;
	LinkBundleList direct;
	if (link->GetState() == LINK_CONNECTED) {
		if (link->GetState() == LINK_CONNECTED) {
			for (BundleList::iterator iter = m_bundleList.begin(); iter
					!= m_bundleList.end(); ++iter) {
				Ptr<Bundle> bundle = *iter;
				/*
				if (bundle->HasRetentionConstraint(RC_FORWARDING_PENDING)
						&& !bundle->HaveBeenReceivedFrom(link)
						&& (link->GetRemoteEndpointId()
								== bundle->GetDestinationEndpoint())
						&& !m_forwardLog.HasEntry(bundle, link)) {
					linkBundleList.push_back(LinkBundle(link, *iter));
				}
				*/

				if(!bundle->HaveBeenReceivedFrom(link->GetRemoteEndpointId().GetId()) && link->GetRemoteEndpointId()
						== bundle->GetDestinationEndpoint()){
						direct.push_back(LinkBundle(link,*iter));
						return direct;
				}
				/* agora não tem mais restrição de EID - virou epidêmico */
				else if (!bundle->HaveBeenReceivedFrom(link->GetRemoteEndpointId().GetId())
						&& !m_forwardLog.HasEntry(bundle, link)) {
					linkBundleList.push_back(LinkBundle(link, *iter));
				}

			}
		}
	}
	return linkBundleList;
}

uint8_t RTEpidemic::DoCalculateReplicationFactor(
		const BundlePriority& priority) const
{
	return 1;
}

bool RTEpidemic::DoIsRouterSpecific(Ptr<Bundle> bundle)
{
	return false;
}

bool RTEpidemic::DoIsRouterSpecific(const BlockType& block)
{
	return false;
}

void RTEpidemic::SendRouterSpecific(Ptr<Link> link,
		Ptr<Bundle> bundle)
{
}

void RTEpidemic::SentRouterSpecific(Ptr<Link> link,
		const GlobalBundleIdentifier& gbid)
{
}

void RTEpidemic::ReceiveRouterSpecific(Ptr<Bundle> bundle)
{
}

void RTEpidemic::AddRouterSpecificBundle(Ptr<Bundle> bundle)
{
}

void RTEpidemic::RemoveRouterSpecificBundle(
		const GlobalBundleIdentifier& gbid, uint8_t reason)
{
}

bool RTEpidemic::HasRouterSpecificBundle(
		const GlobalBundleIdentifier& gbid)
{
	return false;
}

Ptr<Bundle> RTEpidemic::GetRouterSpecificBundle(
		const GlobalBundleIdentifier& gbid)
{
	return Create<Bundle> ();
}

void RTEpidemic::DoBundleDelivered(Ptr<Bundle> bundle, bool fromAck) {
	NS_LOG_DEBUG("(" << m_node->GetId() << ") - From = " << bundle->GetCustodianEndpoint() << " - fromAck = " << fromAck);

	m_kdm.Insert(bundle);

	BundleList bl;
	for (BundleList::iterator iter = m_bundleList.begin(); iter != m_bundleList.end(); ++iter) {
		NS_LOG_DEBUG("--> BUNDLE EID: " << (*iter)->GetCustodianEndpoint());
		Ptr<Bundle> bundle = *iter;
		if (m_kdm.Has(bundle)) {
			bl.push_back(bundle);
			m_forwardLog.RemoveEntriesFor(bundle->GetBundleId());
		}
	}

	for (BundleList::iterator iter = bl.begin(); iter != bl.end(); ++iter) {
		NS_LOG_DEBUG("--> Delivered EID: " << (*iter)->GetBundleId());
		DeleteBundle(*iter, false);
	}
}

Ptr<Link> RTEpidemic::DoCreateLink(const BundleEndpointId& eid, const Address& address)
{
	//NS_LOG_DEBUG ("(" << m_node->GetId () << ") "  << "RTEpidemic::DoCreateLink");
	Ptr<ConvergenceLayerAgent> cla = m_node->GetObject<BundleProtocolAgent>()->GetConvergenceLayerAgent();
	Ptr<Link> link = CreateObject<Link> ();

	link->SetLinkLostCallback(MakeCallback(&ConvergenceLayerAgent::LinkLost, cla));

	link->SetRemoteEndpointId(eid);
	link->SetRemoteAddress(address);

	return link;
}

/*sergioviera*/
void RTEpidemic::DoSendHello(Ptr<Socket> socket, BundleEndpointId eid)
{
	NeighHeader header;
	header.SetBundleEndpointId(m_eid);
        
        if(Simulator::Now ().GetSeconds () > curTime - 10)
	{
		m_nda->Stop();
		while(!m_bundleList.empty()){m_bundleList.pop_front();}
	}
        else
        {
	        /* Epidemic Type */
	        TypeTag type(4);

	        Ptr<Packet> hello = Create<Packet>();
	        hello->AddPacketTag(type);
	        hello->AddHeader(header);

	        NS_LOG_DEBUG ("(" << m_node->GetId () << ") - eid = " << m_eid);

	        socket->Send(hello);
        }
}
void RTEpidemic::DoHandleHello(Ptr<Socket> socket)
{
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
/*sergioviera*/

}
} // namespace bundleProtocol, ns3
