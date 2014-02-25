/*
 * bp-neigh-header.h
 *
 *  Created on: 13/04/2012
 *      Author: sergio
 */

#ifndef BP_NEIGH_HEADER_H
#define BP_NEIGH_HEADER_H

#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/random-variable.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "bp-neighbourhood-detection-agent.h"
#include "ns3/mobility-model.h"

namespace ns3 {
namespace bundleProtocol {

class NeighHeader : public Header
{
public:
  NeighHeader ();
  ~NeighHeader ();

  void SetBundleEndpointId (BundleEndpointId eid);
  BundleEndpointId GetBundleEndpointId () const;

protected:
  BundleEndpointId m_eid;

public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
};

}
}


#endif /* BPNEIGHHEADER_H_ */
