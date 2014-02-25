/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-registration-factory.h"
#include "bp-bundle-protocol-agent.h"
#include "ns3/socket.h"
#include "ns3/assert.h"

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (RegistrationFactory);

TypeId 
RegistrationFactory::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::RegistrationFactory")
    .SetParent<Object> ()
    .AddConstructor<RegistrationFactory> ()
  ;
  return tid;
}

RegistrationFactory::RegistrationFactory ()
  : m_bpa (0)
{}

RegistrationFactory::~RegistrationFactory ()
{
  NS_ASSERT (m_bpa == 0);
}
  
void 
RegistrationFactory::SetBpa (Ptr<BundleProtocolAgent> bpa)
{
  m_bpa = bpa;
}
  
Ptr<Registration>
RegistrationFactory::CreateRegistration (void)
{
  return m_bpa->CreateRegistration ();
}

void
RegistrationFactory::DoDispose (void)
{
  m_bpa = 0;  
}

}} // namespace bundleProtocol, ns3 
