/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-registration.h"
#include "bp-registration-factory.h"

#include <iostream>

using namespace std;

NS_LOG_COMPONENT_DEFINE ("Registration");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (Registration);

TypeId Registration::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::Registration")
    .SetParent<Object> ()
    .AddConstructor<Registration> ()
    ;
  return tid;
}
  
Registration::Registration ()
  : m_bpa (0),
    m_regEndpoint (),
    m_registered (false),
    m_deliveryQueue ()
{}

Registration::~Registration ()
{}

void
Registration::DoDispose ()
{
  if (m_regEndpoint != 0)
    {
      NS_ASSERT (m_bpa != 0);
      NS_ASSERT (m_regEndpoint != 0);
      m_bpa->DeAllocate (m_regEndpoint);
      NS_ASSERT (m_regEndpoint == 0);
    }
  m_bpa = 0;
  for (deque<Ptr<Packet> >::iterator iter = m_deliveryQueue.begin ();
       iter != m_deliveryQueue.end ();
       ++iter)
    {
      *iter = 0;
    }
  m_deliveryQueue.clear ();
  m_registrationSucceeded = MakeNullCallback<void, Ptr<Registration> > ();
  m_registrationFailed = MakeNullCallback<void, Ptr<Registration> > ();
  m_sendCb = MakeNullCallback<void, Ptr<Registration>, uint32_t> ();
  m_recvCb = MakeNullCallback<void, Ptr<Registration> > ();
}

void
Registration::SetBundleProtocol (Ptr<BundleProtocolAgent> bpa)
{ 
  m_bpa = bpa;
}
 
void 
Registration::Destroy (void)
{ 
  m_regEndpoint = 0;
  m_bpa = 0;
}

void 
Registration::SetRegistrationCallback (Callback<void, Ptr<Registration> > registrationSucceeded,
                                       Callback<void,  Ptr<Registration> > registrationFailed)
{ 
  m_registrationSucceeded = registrationSucceeded;
  m_registrationFailed = registrationFailed;
}

void
Registration::SetSendCallback (Callback<void, Ptr<Registration>, uint32_t> sendCb)
{ 
  m_sendCb = sendCb;
}

void
Registration::SetRecvCallback (Callback<void, Ptr<Registration> > recvCb)
{ 
  m_recvCb = recvCb;
}

int
Registration::DoRegister ()
{
  if (m_regEndpoint == 0) 
    {
      m_registered = false;
      NotifyRegistrationFailed ();
      return -1;
    }
  
  m_regEndpoint->SetRxCallback (MakeCallback (&Registration::ForwardUp, this));
  m_regEndpoint->SetDestroyCallback (MakeCallback (&Registration::Destroy, this));
  m_regEndpoint->SetSendCallback (MakeCallback (&Registration::NotifySend, this));
  NotifyRegistrationSucceeded ();
  m_registered = true;
  return 0;
}
  
int
Registration::Register ()
{
  m_regEndpoint = m_bpa->Allocate ();
  return DoRegister ();
}

int
Registration::Register (BundleEndpointId eid)
{ 
  if (eid == BundleEndpointId::GetAnyBundleEndpointId ()) 
    {
      return -1;
    }
  else 
    {
      m_regEndpoint = m_bpa->Allocate (eid);
    }
  return DoRegister ();
}
  
int
Registration::Unregister ()
{
  m_bpa->DeAllocate (m_regEndpoint);
  m_registered = 0;
  return 0;
}

int
Registration::Send (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority, Time ttl, const PrimaryProcessingControlFlags& flags) 
{
  if (!m_registered)
    {
     return -1;
    }
  m_bpa->BundleReceivedFromApplication (adu, destination, priority, ttl, flags);
  return 0;
}

  /*
int
Registration::SendWithReport (Ptr<Packet> adu, const BundleEndpointId& destination, const BundleEndpointId& reportTo, BundlePriority priority, Time ttl, const PrimaryProcessingControlFlags& flags)
{ 
  if (!m_registered)
    {
      return -1;
    }
  m_bpa->BundleReceivedFromApplication (adu, destination, reportTo, priority, ttl, flags);
  return 0;
}
  */
Ptr<Packet>
Registration::Recv (BundleEndpointId& fromAddress)
{ 
  if (m_deliveryQueue.empty() )
    {
      return 0;
    }

  Ptr<Packet> adu = m_deliveryQueue.front ();
  m_deliveryQueue.pop_front ();
  RegistrationAddressTag tag;
  bool found = adu->PeekPacketTag (tag);
  adu->RemovePacketTag (tag);
  NS_ASSERT (found);
  if (found)
    {
      fromAddress = tag.GetAddress ();
    }
  return adu;
}

void
Registration::ForwardUp (Ptr<Packet> adu, BundleEndpointId address)
{ 
  RegistrationAddressTag tag;
  tag.SetAddress (address);
  adu->AddPacketTag (tag);
  m_deliveryQueue.push_back (adu);
  NotifyRecv ();
}

void
Registration::NotifyRegistrationSucceeded (void)
{ 
  if (!m_registrationSucceeded.IsNull ())
    {
      m_registrationSucceeded (this);
    }
}

void
Registration::NotifyRegistrationFailed (void)
{ 
  if (!m_registrationFailed.IsNull ())
    {
      m_registrationFailed (this);
    }
}

void
Registration::NotifySend (uint32_t freeBytes)
{ 
  if (!m_sendCb.IsNull ())
    {
      m_sendCb (this, freeBytes);
    }
}

void
Registration::NotifyRecv (void)
{ 
  if (!m_recvCb.IsNull ())
    {
      m_recvCb (this);
    }
}

/***************************************************************
 *           Address Tag
 ***************************************************************/

RegistrationAddressTag::RegistrationAddressTag ()  
  : m_address ()
{}

void 
RegistrationAddressTag::SetAddress (BundleEndpointId addr)
{
  m_address = addr;
}

BundleEndpointId
RegistrationAddressTag::GetAddress (void) const
{
  return m_address;
}


TypeId
RegistrationAddressTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::RegistrationAddressTag")
    .SetParent<Tag> ()
    .AddConstructor<RegistrationAddressTag> ()
    ;
  return tid;
}
TypeId
RegistrationAddressTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
RegistrationAddressTag::GetSerializedSize (void) const
{
  return m_address.GetSerializedSize () + 1;
}

void
RegistrationAddressTag::Serialize (TagBuffer i) const
{
  i.WriteU8 ((uint8_t)m_address.GetSerializedSize ());
  uint8_t buffer [m_address.GetSerializedSize ()];
  m_address.Serialize (buffer);
  BundleEndpointId test = BundleEndpointId::Deserialize (buffer);
  i.Write (buffer, m_address.GetSerializedSize ());
}

void
RegistrationAddressTag::Deserialize (TagBuffer i)
{
  uint32_t size = i.ReadU8 ();
  uint8_t buffer [size];
  i.Read (buffer, size);
  m_address = BundleEndpointId::Deserialize (buffer);
}

void
RegistrationAddressTag::Print (std::ostream &os) const
{
  os << "address=" << m_address;
}

}} // namespace bundleProtocol, ns3
