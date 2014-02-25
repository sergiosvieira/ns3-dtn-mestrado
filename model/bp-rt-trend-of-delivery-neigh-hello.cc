/*
 * bp-rt-trend-of-delivery-neigh-hello.cc
 *
 *  Created on: 15/04/2012
 *      Author: sergio
 */

#include "bp-rt-trend-of-delivery-neigh-hello.h"
#include "ieee754.h"


/* IEEE 754 pack unpack */



NS_LOG_COMPONENT_DEFINE ("NeighHello");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED(NeighHello);


NeighHello::NeighHello() {
}

void NeighHello::SetBundleEndpointId(BundleEndpointId eid) {
        m_eid = eid;
}

BundleEndpointId NeighHello::GetBundleEndpointId() const {
        return m_eid;
}

TypeId NeighHello::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::bundleProtocol::NeighHello")
                        .SetParent<Header>()
                        .AddConstructor<NeighHello>();

        return tid;
}

TypeId NeighHello::GetInstanceTypeId(void) const {
        return GetTypeId();
}

void NeighHello::Print(std::ostream &os) const {
        os << "NeighHello: eid = " << m_eid << " position = (" << m_pos.x << ", " << m_pos.y << ")  velocity = (" << m_vel.x << ", " << m_vel.y << ") time stamp = " << m_timeStamp.GetSeconds();
}

uint32_t NeighHello::GetSerializedSize(void) const {
        return m_eid.GetSerializedSize() + 4 + 32 + 8;
}



void NeighHello::Serialize(Buffer::Iterator start) const {
        int length = m_eid.GetSerializedSize();
        Buffer::Iterator i = start;
        i.WriteHtonU32(length);
        uint8_t buf[length];
        m_eid.Serialize(buf);
        i.Write(buf, length);

        i.WriteU64(pack754_64(m_pos.x));
        i.WriteU64(pack754_64(m_pos.y));

        i.WriteU64(pack754_64(m_vel.x));
        i.WriteU64(pack754_64(m_vel.y));

        i.WriteU64(pack754_64(m_timeStamp.GetSeconds()));
}

uint32_t NeighHello::Deserialize(Buffer::Iterator start) {
        Buffer::Iterator i = start;
        uint32_t length = i.ReadNtohU32();
        uint8_t buf[length];
        i.Read(buf, length);
        m_eid = BundleEndpointId::Deserialize(buf);

        m_pos.x = unpack754_64(i.ReadU64());
        m_pos.y = unpack754_64(i.ReadU64());

        m_vel.x = unpack754_64(i.ReadU64());
        m_vel.y = unpack754_64(i.ReadU64());

        m_timeStamp = Seconds(unpack754_64(i.ReadU64()));

        return GetSerializedSize();
}

void NeighHello::setPos(double x, double y) {
        m_pos.x = x;
        m_pos.y = y;
}

void NeighHello::setVel(double x, double y) {
        m_vel.x = x;
        m_vel.y = y;
}

Vector2d NeighHello::getPos() const {
        return m_pos;
}

Vector2d NeighHello::getVel() const{
        return m_vel;
}

void NeighHello::setTimeStamp(Time timeStamp) {
        m_timeStamp = timeStamp;
}

Time NeighHello::getTimeStamp() const {
        return m_timeStamp;
}

} /* namespace bundleProtocol */
} /* namespace ns3 */
