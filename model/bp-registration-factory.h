/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_REGISTRATION_FACTORY_H
#define BP_REGISTRATION_FACTORY_H

#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3 {
namespace bundleProtocol {

class Registration;
class BundleProtocolAgent;

class RegistrationFactory : public Object
{
public:
  static TypeId GetTypeId (void);

  RegistrationFactory ();
  virtual ~RegistrationFactory ();
  
  void SetBpa (Ptr<BundleProtocolAgent> bpa);
  
  virtual Ptr<Registration> CreateRegistration (void);

 private:
  virtual void DoDispose ();
  Ptr<BundleProtocolAgent> m_bpa;
};

}} // namespace bundleProtocol, ns3 

#endif /* BP_REGISTRATION_FACTORY_H */
