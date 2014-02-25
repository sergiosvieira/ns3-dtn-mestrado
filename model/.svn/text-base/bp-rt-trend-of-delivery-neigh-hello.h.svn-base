/*
 * bp-rt-trend-of-delivery-neigh-hello.h
 *
 *  Created on: 15/04/2012
 *      Author: Sérgio Vieira
 */

#ifndef BP_RT_TREND_OF_DELIVERY_NEIGH_HELLO_H
#define BP_RT_TREND_OF_DELIVERY_NEIGH_HELLO_H

#include "vector2d.h"
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
//#include "ieee754.h"





namespace ns3 {
namespace bundleProtocol {

class NeighHello: public Header {
public:
        NeighHello();

        static TypeId GetTypeId(void);
        virtual TypeId GetInstanceTypeId(void) const;
        virtual void Print(std::ostream &os) const;
        virtual uint32_t GetSerializedSize(void) const;
        virtual void Serialize(Buffer::Iterator start) const;
        virtual uint32_t Deserialize(Buffer::Iterator start);

        void SetBundleEndpointId (BundleEndpointId eid);
        BundleEndpointId GetBundleEndpointId () const;


        void setPos(double x, double y);
        void setVel(double x, double y);
        Vector2d getPos() const;
        Vector2d getVel() const;

        void setTimeStamp(Time timeStamp);
        Time getTimeStamp() const;

protected:
        BundleEndpointId m_eid;
        Vector2d m_pos; // current position
        Vector2d m_vel; // velocity
        Time m_timeStamp;
};

} /* namespace bundleProtocol */
} /* namespace ns3 */
#endif /* BP_RT_TREND_OF_DELIVERY_NEIGH_HELLO_H_ */
