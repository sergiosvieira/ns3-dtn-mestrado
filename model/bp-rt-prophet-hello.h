/*
 * bp-rt-prophet-hello.h
 *
 *  Created on: 12/04/2012
 *      Author: Sérgio Vieira
 */

#ifndef BP_RT_PROPHET_HELLO_H
#define BP_RT_PROPHET_HELLO_H

#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/header.h"
#include "ns3/net-device.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"

#include "bp-bundle-endpoint-id.h"
#include "bp-link-manager.h"
#include "bp-link.h"
#include "bp-bundle-protocol-agent.h"
#include "bp-convergence-layer-agent.h"
//#include "ieee754.h"
#include <iostream>
#include <string>

namespace ns3 {
namespace bundleProtocol {

typedef std::map<BundleEndpointId, double> ProbabilitiesList;

class ProphetHelloHeader: public Header {
public:
	ProphetHelloHeader();
	/* adicionar número do eid e probabilidade dos vizinhos */
	void addProbability(const BundleEndpointId &dst_eid, const double &probability);
	double GetProbability(const BundleEndpointId &dst_eid);
	int GetSize();
	ProbabilitiesList GetNeighList();
	/* fim */

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

	void SetBundleEndpointId(BundleEndpointId eid);
	BundleEndpointId GetBundleEndpointId() const;

protected:
	BundleEndpointId m_eid;
	ProbabilitiesList m_plist; // Usado para enviar a lista de probabilidades dos nós vizinhos
};

}
}
#endif /* BP_RT_PROPHET_HELLO_H */
