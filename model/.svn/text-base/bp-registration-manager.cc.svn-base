/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-registration-manager.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <iostream>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("RegistrationManager");

RegistrationManager::RegistrationManager ()
  : m_endpoints ()
{}

RegistrationManager::~RegistrationManager ()
{
  for (EndpointsI i = m_endpoints.begin (); i != m_endpoints.end (); i++) 
    {
      RegistrationEndpoint *endpoint = *i;
      delete endpoint;
      endpoint = 0;
    }
  m_endpoints.clear ();
}

bool 
RegistrationManager::LookupLocal (BundleEndpointId eid)
{
  for (EndpointsI i = m_endpoints.begin (); i != m_endpoints.end (); i++) 
    {
      if ((*i)->GetBundleEndpointId  () == eid) 
        {
          return true;
        }
    }
  return false;
}

Endpoints
RegistrationManager::Lookup (BundleEndpointId eid)
{
  Endpoints ret;
  for (EndpointsI i = m_endpoints.begin (); i != m_endpoints.end (); i++) 
    {
      if ((*i)->GetBundleEndpointId  () == eid || (*i)->GetBundleEndpointId  () == BundleEndpointId ()) 
        {
          RegistrationEndpoint* endp = *i;
          ret.push_back(endp);
        }
    }
  return ret;
}

RegistrationEndpoint *
RegistrationManager::Allocate (BundleEndpointId eid)
{
  RegistrationEndpoint *endpoint = new RegistrationEndpoint (eid);
  m_endpoints.push_back (endpoint);
  return endpoint;
}

void 
RegistrationManager::DeAllocate (RegistrationEndpoint *endpoint)
{
  for (EndpointsI i = m_endpoints.begin (); i != m_endpoints.end (); i++)
    {
      if (*i == endpoint)
        {
          delete endpoint;
          m_endpoints.erase (i);
          break;
        }
    }
}

}} // namespace bundleProtocol, ns3
