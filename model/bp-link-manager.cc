/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-link-manager.h"

NS_LOG_COMPONENT_DEFINE ("LinkManager");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (LinkManager);

TypeId
LinkManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::LinkManager")
    .SetParent<Object> ()
    .AddConstructor<LinkManager> ()
    .AddAttribute ("LinkExpirationTime",
                   "Sets the time after which a link becomes expired, if not updated.",
                   TimeValue (Seconds (5)),
                   MakeTimeAccessor (&LinkManager::m_ttl),
                   MakeTimeChecker ())
    ;
  return tid;
}

LinkManager::LinkManager (){

}

LinkManager::~LinkManager ()
{}

void
LinkManager::Doh () const
{}

void 
LinkManager::DoDispose ()
{
  for (LinkMap::iterator iter = m_links.begin (); iter != m_links.end (); ++iter)
    {
      iter->first->m_contact = 0;
      iter->second.SetFunction (&LinkManager::Doh, this);
    }
  m_links.clear ();

  m_createLinkCb = MakeNullCallback<Ptr<Link>, BundleEndpointId, Address> ();
  m_linkAvailableCb = MakeNullCallback<void,Ptr<Link> > ();
  m_closedLinkCb = MakeNullCallback<void,Ptr<Link> > ();
}


void
LinkManager::SetLinkAvailableCallback (Callback<void, Ptr<Link> > linkAvailableCb)
{
  m_linkAvailableCb = linkAvailableCb;
}

void
LinkManager::SetClosedLinkCallback (Callback<void, Ptr<Link> > closedLinkCb)
{
  m_closedLinkCb = closedLinkCb;
}

void
LinkManager::SetCreateLinkCallback (Callback<Ptr<Link>, BundleEndpointId, Address> sayWhatCb)
{
  m_createLinkCb = sayWhatCb;
}

void LinkManager::DiscoveredLink(Ptr<Packet> hello, Address fromAddress) {
	NS_LOG_DEBUG(fromAddress);

	BundleEndpointId eid;
	PacketSocketAddress packetAddress = PacketSocketAddress::ConvertFrom (fromAddress);
	Mac48Address peerMac = Mac48Address::ConvertFrom(
			packetAddress.GetPhysicalAddress());


	TypeTag type;
	hello->PeekPacketTag(type);
	/* Verifico o tipo de pacote que recebi */
	if (type.GetType() == 1) {
		/* Prophet Protocol */
		ProphetHelloHeader header;
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 2) {
		/* used in Trend of Delivery Protocol */
		NeighHello header;
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 3) {
		/* DirectLink */
		NeighHeader header;
		NS_LOG_DEBUG("PeekHeader");
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 4) {
		/* Epidemic */
		NeighHeader header;
		NS_LOG_DEBUG("PeekHeader");
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 5){
		/*SprayAndWait*/
		NeighHeader header;
		NS_LOG_DEBUG("PeekHeader");
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 6){
		/*AntMofo*/
		AntMofoHelloHeader header;
		NS_LOG_DEBUG("PeekHeader");
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	} else if (type.GetType() == 7){
		ProphetTodHelloHeader header;
		NS_LOG_DEBUG("PeekHeader");
		hello->PeekHeader(header);
		eid = header.GetBundleEndpointId();
	}


	/*
	ProphetHelloHeader header;
	hello->PeekHeader(header);
	eid = header.GetBundleEndpointId();
	*/

	if (HasLink(eid)) {
		Ptr<Link> oldLink = FindLink(eid);
		oldLink->UpdateLastHeardFrom();
		if (oldLink->GetState() == LINK_UNAVAILABLE) {
			oldLink->ChangeState(LINK_AVAILABLE);
			SetupTimer(oldLink);
			NotifyLinkIsAvailable(oldLink);
		}
	} else {
		Ptr<Link> link = m_createLinkCb(eid, peerMac);
		link->ChangeState(LINK_AVAILABLE);
		link->UpdateLastHeardFrom();
		LinkTimer lt = make_pair<Ptr<Link> , Timer> (link, Timer(
				Timer::REMOVE_ON_DESTROY));
		m_links.insert(lt);
		SetupTimer(link);
		NotifyLinkIsAvailable(link);
	}
}

void
LinkManager::AddLink (Ptr<Link> link)
{
  if (HasLink (link))
    {
      Ptr<Link> oldLink = FindLink (link);
      oldLink = link;
      link->UpdateLastHeardFrom ();
      SetupTimer (oldLink);
    }
  else
    {
      link->UpdateLastHeardFrom ();
      LinkTimer lt = make_pair<Ptr<Link>,Timer> (link, Timer (Timer::REMOVE_ON_DESTROY));
      m_links.insert (lt);
      //m_linkSet.push_back (link);
      SetupTimer (link);
    }
}

void
LinkManager::RemoveLink (Ptr<Link> link)
{
  /*
  Links::iterator iter = remove (m_linkSet.begin (), m_linkSet.end (), link);
  if (iter != m_linkSet.end ())
    m_linkSet.erase (iter, m_linkSet.end ());
  */
  m_links.erase (link);
}
  
bool
LinkManager::HasLink (const BundleEndpointId& eid)
{
  Links linkSet = GetAllLinks ();

  Links::const_iterator iter;
  for (iter = linkSet.begin (); iter != linkSet.end (); ++iter)
    {
      if ((*iter)->GetRemoteEndpointId () == eid)
        return true;
    }

  return false;
}

bool
LinkManager::HasLink (Ptr<Link> link)
{
  //return HasLink (link->GetRemoteEndpointId ());
  LinkMap::const_iterator iter = m_links.find (link);
  return iter != m_links.end ();
}

Ptr<Link>
LinkManager::FindLink (const BundleEndpointId& eid)
{
  Links linkSet = GetAllLinks ();

  Links::const_iterator iter;
  for (iter = linkSet.begin (); iter != linkSet.end (); ++iter)
    {
      if ((*iter)->GetRemoteEndpointId () == eid)
        return *iter;
    }
  return 0;
}

Ptr<Link>
LinkManager::FindLink (const Mac48Address& address)
{
  Links linkSet = GetAllLinks ();

  Links::const_iterator iter;
  for (iter = linkSet.begin (); iter != linkSet.end (); ++iter)
    {
      Ptr<Link> link = *iter;
      Mac48Address mac = Mac48Address::ConvertFrom (link->GetRemoteAddress ());
      if (address == mac)
        return link;
    }
  return 0;
}

Ptr<Link>
LinkManager::FindLink (Ptr<Link> link)
{
  //return FindLink (link->GetRemoteEndpointId ());
  LinkMap::const_iterator iter = m_links.find (link);
  if (iter != m_links.end ())
    {
      return iter->first;
    }
  else
    {
      return 0;
    }
}

void
LinkManager::OpenLink (Ptr<Link> link)
{
  Ptr<Link> oldLink = FindLink (link);
  if (oldLink != 0)
    {
      oldLink->Open ();
    }
  else
    {
      link->Open ();
    }
}

void
LinkManager::CloseLink (Ptr<Link> link)
{
  Ptr<Link> oldLink = FindLink (link);
  if (oldLink != 0)
    {
      LinkMap::iterator iter = m_links.find (oldLink);
      iter->second.Cancel ();
      iter->second.Remove ();

      NotifyClosedLink (link);
      oldLink->Close ();
    }
  else
    {
      link->Close ();
    }
}

Links LinkManager::GetReadyLinks() {
	//RecalculateContactWindows();

	Links linkSet = GetAllLinks();

	/*
	Links links;
	remove_copy_if(linkSet.begin(), linkSet.end(), back_inserter(links),
			KeepReady());
	*/
	return linkSet;
}


Links
LinkManager::GetAllLinks ()
{
  Links linkSet;
  linkSet.reserve (m_links.size ());
  
  for (LinkMap::const_iterator iter = m_links.begin (); iter != m_links.end (); ++iter)
    {
      linkSet.push_back (iter->first);
    }
  return linkSet;
}

Links
LinkManager::GetConnectedLinks ()
{
  Links linkSet = GetAllLinks ();

  Links links;
  remove_copy_if (linkSet.begin (), linkSet.end (), back_inserter (links), KeepConnected ());
  return links;
}

Links
LinkManager::GetAvailableLinks ()
{
  Links linkSet = GetAllLinks ();

  Links links;
  remove_copy_if (linkSet.begin (), linkSet.end (), back_inserter (links), KeepAvailable ());
  return links;
}

Links
LinkManager::GetUnavailableLinks ()
{
  Links linkSet = GetAllLinks ();

  Links links;
  remove_copy_if (linkSet.begin (), linkSet.end (), back_inserter (links), KeepUnavailable ());
  return links;
}

void
LinkManager::CheckIfExpired (Ptr<Link> link)
{ 
  LinkMap::iterator iter = m_links.find (link);
  
  if (((Simulator::Now () - link->GetLastHeardFrom ()) >= m_ttl))
    {
      CloseLink (link);
    }
  else
    {
      Time delay = m_ttl - (Simulator::Now () - link->GetLastHeardFrom ());
      iter->second.Schedule (delay);
    }
}

void
LinkManager::SetupTimer (Ptr<Link> link)
{
  LinkMap::iterator iter = m_links.find (link);
  //link->m_expirationTimer.SetFunction (&LinkManager::CheckIfExpired, this);
  //link->m_expirationTimer.SetArguments (link);
  //link->m_expirationTimer.Schedule (m_ttl);
  iter->second.SetFunction (&LinkManager::CheckIfExpired, this);
  iter->second.SetArguments (link);
  iter->second.Schedule (m_ttl);
};

void
LinkManager::NotifyLinkIsAvailable (Ptr<Link> link)
{
  if (!m_linkAvailableCb.IsNull ())
    {
      m_linkAvailableCb (link);
    }
}

void
LinkManager::NotifyClosedLink (Ptr<Link> link)
{
  if (!m_closedLinkCb.IsNull ())
    {
      m_closedLinkCb (link);
    }
}

}} // namespace bundleProtocol, ns3
