/*
 * bp-type-tag.h
 *
 *  Created on: 15/04/2012
 *      Author: sergio
 */

#ifndef BP_TYPE_TAG_H
#define BP_TYPE_TAG_H

#include "ns3/uinteger.h"
#include "ns3/tag.h"

namespace ns3 {
namespace bundleProtocol {

struct TypeTag: public Tag {
	/// Positive if output device is fixed in RouteOutput
	int32_t m_type;

	TypeTag(int32_t o = -1) :
			Tag(), m_type(o) {
	}

	static TypeId GetTypeId() {
		static TypeId tid = TypeId("ns3::bundleProtocol::TypeTag")
				.SetParent<Tag>();
		return tid;
	}

	uint32_t GetType() {
		return m_type;
	}

	TypeId GetInstanceTypeId() const {
		return GetTypeId();
	}

	uint32_t GetSerializedSize() const {
		return sizeof(int32_t);
	}

	void Serialize(TagBuffer i) const {
		i.WriteU32(m_type);
	}

	void Deserialize(TagBuffer i) {
		m_type = i.ReadU32();
	}

	void Print(std::ostream &os) const {
		os << "TypeTag: output interface = " << m_type;
	}
};

}
}

#endif /* BP_TYPE_TAG_H_ */
