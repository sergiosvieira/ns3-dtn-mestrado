/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
#include <algorithm>
#include <iterator>
#include "ns3/log.h"
#include "ns3/object-vector.h"
#include "bp-link.h"
#include "bp-orwar-link-manager.h"
#include "bp-bundle-router.h"
#include "bp-orwar-router-changed-order.h"
NS_LOG_COMPONENT_DEFINE ("OrwarLinkManager");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (OrwarLinkManager);

TypeId OrwarLinkManager::GetTypeId(void) {
	static TypeId
			tid =
					TypeId("ns3::bundleProtocol::OrwarLinkManager") .SetParent<
							LinkManager> () .AddConstructor<OrwarLinkManager> () .AddAttribute(
							"ContactSetupHoldTime",
							"Sets the time after which a link trying to setup a contact becomes expired, if not updated.",
							TimeValue(Seconds(10)), MakeTimeAccessor(
									&OrwarLinkManager::m_setupHold),
							MakeTimeChecker()) .AddAttribute(
							"CwMaxHoldTime",
							"Sets the time after which a CwMax link becomes expired, if not updated.",
							TimeValue(Seconds(5)), MakeTimeAccessor(
									&OrwarLinkManager::m_cwMaxHold),
							MakeTimeChecker());
	return tid;
}

OrwarLinkManager::OrwarLinkManager() :
	LinkManager() {
}

OrwarLinkManager::~OrwarLinkManager() {
}

void OrwarLinkManager::DoDispose() {
	LinkManager::DoDispose();
}

Time OrwarLinkManager::GetSetupHoldTime() const {
	return m_setupHold;
}

Time OrwarLinkManager::GetCwMaxHoldTime() const {
	return m_cwMaxHold;
}

void OrwarLinkManager::CloseLink(Ptr<Link> link) {
	LinkManager::CloseLink(link);
}

void OrwarLinkManager::DiscoveredLink(Ptr<Packet> hello, Address fromAddress) {
	PacketSocketAddress packetAddress = PacketSocketAddress::ConvertFrom(
			fromAddress);
	Mac48Address peerMac = Mac48Address::ConvertFrom(
			packetAddress.GetPhysicalAddress());

	HelloHeader header;
	hello->PeekHeader(header);

	BundleEndpointId eid = header.GetBundleEndpointId();

	if (HasLink(eid)) {
		Ptr<Link> oldLink = FindLink(eid);
		oldLink->UpdateLastHeardFrom();
		if (oldLink->GetState() == LINK_UNAVAILABLE) {
			oldLink->ChangeState(LINK_AVAILABLE);
			ContactSetup(oldLink, true);
			NotifyLinkIsAvailable(oldLink);
		}
	} else {
		Ptr<Link> link = m_createLinkCb(eid, peerMac);
		link->ChangeState(LINK_AVAILABLE);
		link->UpdateLastHeardFrom();
		LinkTimer lt = make_pair<Ptr<Link> , Timer> (link, Timer(
				Timer::REMOVE_ON_DESTROY));
		m_links.insert(lt);
		//m_linkSet.push_back (link);
		ContactSetup(link, true);
		NotifyLinkIsAvailable(link);
	}
}

void OrwarLinkManager::AddLink(Ptr<Link> link) {
	if (HasLink(link)) {
		Ptr<Link> oldLink = FindLink(link->GetRemoteEndpointId());
		oldLink = link;
		link->UpdateLastHeardFrom();
		ContactSetup(oldLink, false);
	} else {
		link->UpdateLastHeardFrom();
		LinkTimer lt = make_pair<Ptr<Link> , Timer> (link, Timer(
				Timer::REMOVE_ON_DESTROY));
		m_links.insert(lt);
		//m_linkSet.push_back (link);
		ContactSetup(link, false);
	}
}

Ptr<Link> OrwarLinkManager::FindLink(const BundleEndpointId& eid) {
	RecalculateContactWindows();
	return LinkManager::FindLink(eid);
}

Ptr<Link> OrwarLinkManager::FindLink(const Mac48Address& address) {
	RecalculateContactWindows();
	return LinkManager::FindLink(address);
}

Ptr<Link> OrwarLinkManager::FindLink(Ptr<Link> link) {
	RecalculateContactWindows();
	return LinkManager::FindLink(link);
}

Links OrwarLinkManager::GetAllLinks() {
	RecalculateContactWindows();

	return LinkManager::GetAllLinks();
}

Links OrwarLinkManager::GetReadyLinks() {
	RecalculateContactWindows();

	Links linkSet = GetAllLinks();

	Links links;
	remove_copy_if(linkSet.begin(), linkSet.end(), back_inserter(links),
			KeepReady());
	return links;
}

Links OrwarLinkManager::GetConnectedLinks() {
	RecalculateContactWindows();
	return LinkManager::GetConnectedLinks();
}

void OrwarLinkManager::CheckIfExpired(Ptr<Link> l) {
	Ptr<OrwarLink> link = dynamic_cast<OrwarLink *> (PeekPointer(l));

	LinkMap::iterator iter = m_links.find(link);

	if (link->GetState() == LINK_AVAILABLE) {
		CloseLink(link);
	} else if (!link->HaveCw()) {
		CloseLink(link);
	} else {
		Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer(
				link->GetContact()));
		if (!oc->GetCwMax()) {
			CloseLink(link);
		} else {
			if (((Simulator::Now() - link->GetLastHeardFrom()) >= m_cwMaxHold)) {
				CloseLink(link);
			} else {
				Time delay = m_cwMaxHold - (Simulator::Now()
						- link->GetLastHeardFrom());
				iter->second.Schedule(delay);
			}
		}
	}
}

void OrwarLinkManager::ContactSetup(Ptr<Link> l, bool sender) {
	Ptr<OrwarLink> link = dynamic_cast<OrwarLink *> (PeekPointer(l));
	LinkMap::iterator iter = m_links.find(link);
	//link->m_expirationTimer.SetFunction (&OrwarLinkManager::CheckIfExpired, this);
	//link->m_expirationTimer.SetArguments (l);
	iter->second.SetFunction(&OrwarLinkManager::CheckIfExpired, this);
	iter->second.SetArguments(l);
	if (sender) {
		//link->m_expirationTimer.Schedule (m_setupHold);
		iter->second.Schedule(m_setupHold);
	}
	//link->m_setupHold = m_setupHold;
	//link->m_cwMaxHold = m_cwMaxHold;
}

void OrwarLinkManager::FinishedSetup(Ptr<Link> l) {
	Ptr<OrwarLink> link = dynamic_cast<OrwarLink *> (PeekPointer(l));
	Ptr<OrwarContact> oc = dynamic_cast<OrwarContact *> (PeekPointer(
			l->GetContact()));

	LinkMap::iterator iter = m_links.find(link);

	//link->m_expirationTimer.Cancel ();
	//link->m_expirationTimer.Remove ();
	iter->second.Cancel();
	iter->second.Remove();

	if (oc->GetCwMax()) {
		Time holdtime = m_cwMaxHold - (Simulator::Now() - link->m_lastHeard);
		iter->second.Schedule(holdtime);
	} else {
		iter->second.Schedule(oc->GetContactWindowDuration());
	}

}

void OrwarLinkManager::RecalculateContactWindows() {
	Links linkSet;
	linkSet.reserve(m_links.size());

	for (LinkMap::iterator iter = m_links.begin(); iter != m_links.end(); ++iter) {
		linkSet.push_back(iter->first);
	}

	Links links;
	remove_copy_if(linkSet.begin(), linkSet.end(), back_inserter(links),
			KeepHasCw());

	for (Links::iterator iter = links.begin(); iter != links.end(); ++iter) {
		Ptr<Link> l = *iter;
		Ptr<OrwarContact> oc(dynamic_cast<OrwarContact *> (PeekPointer(
				l->GetContact())));

		oc->DecreaseContactWindowDuration();
	}
}

}
} // namespace bundleProtocol, ns3
