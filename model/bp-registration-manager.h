/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_REGISTRATION_ENDPOINT_DEMUX_H
#define BP_REGISTRATION_ENDPOINT_DEMUX_H

#include "bp-registration-endpoint.h"
#include "bp-bundle-endpoint-id.h"
#include <stdint.h>
#include <list>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

typedef list<RegistrationEndpoint *> Endpoints;
typedef list<RegistrationEndpoint *>::iterator EndpointsI;

/**
 * \ingroup bundleProtocolAgent
 *
 * \brief Manages the registrations, demulitplexing bundles to the right application(s).
 *
 *
 */
class RegistrationManager
{
public:
  RegistrationManager ();
  ~RegistrationManager ();

  bool LookupLocal (BundleEndpointId eid);
  Endpoints Lookup (BundleEndpointId eid);
    
  RegistrationEndpoint *Allocate (BundleEndpointId eid);
  void DeAllocate (RegistrationEndpoint *endpoint);

private:
  list<RegistrationEndpoint *> m_endpoints;
};

}} // namespace bundleProtocol, ns3

#endif /* BP_REGISTRATION_ENDPOINT_DEMUX_H */
