/*
 * bp-rt-prophet-hello.cc
 *
 *  Created on: 12/04/2012
 *      Author: Sérgio Vieira
 */

#include "bp-rt-prophet-hello.h"
#include "ieee754.h"

NS_LOG_COMPONENT_DEFINE ("ProphetHelloHeader");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (ProphetHelloHeader);

ProphetHelloHeader::ProphetHelloHeader() {

}

/* Adicionar número do eid e probabilidade dos vizinhos */

void ProphetHelloHeader::addProbability(const BundleEndpointId &dst_eid, const double &probability) {
	NS_LOG_DEBUG("dst_eid=" << dst_eid << " probability=" << probability);
	m_plist[dst_eid] = probability;
}

double ProphetHelloHeader::GetProbability(const BundleEndpointId &dst_eid) {
	NS_LOG_DEBUG("dst_eid=" << dst_eid);
	return m_plist[dst_eid];
}

int ProphetHelloHeader::GetSize() {
	NS_LOG_DEBUG("size=" << m_plist.size());
	return m_plist.size();
}

ProbabilitiesList ProphetHelloHeader::GetNeighList() {
	return m_plist;
}


/* Fim */


TypeId
ProphetHelloHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::ProphetHelloHeader")
    .SetParent<Header> ()
    .AddConstructor<ProphetHelloHeader> ();
  return tid;
}

TypeId
ProphetHelloHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ProphetHelloHeader::Print (std::ostream &os) const
{
  os << "ProphetHelloHeader: eid = " << m_eid;
}

uint32_t
ProphetHelloHeader::GetSerializedSize (void) const
{
	uint32_t eid_size = m_eid.GetSerializedSize();
	return eid_size + 4 + sizeof(uint32_t) + (m_plist.size() * (eid_size + 4 + sizeof(double)));
}



void ProphetHelloHeader::Serialize(Buffer::Iterator start) const {
	/* enviando EID, isso é obrigatório pq tá amarrado ao LinkManager */
	int length = m_eid.GetSerializedSize();
	Buffer::Iterator i = start;
	i.WriteU32(length);
	uint8_t buf[length];
	m_eid.Serialize(buf);
	i.Write(buf, length);

	/* novos campos */

	i.WriteU32(m_plist.size()); // quantidade de registros de vizinhos
	ProbabilitiesList::const_iterator it;
	/* Na hora de serializar, a ordem é U64, U32 , mas na hora de deserializar a ordem é U32, U64, não
	 * me pergunte pq!! mas só funciona assim - PROBLEMA CORRIGIDO
	 * O PROBLEMA ERA QUE NO GETTYPE O PARENT TAVA DEFINIDO COMO HEADER, NA VERDADE ERA PRA SER
	 * NEIGHHEADER
	 */
	for (it = m_plist.begin(); it != m_plist.end(); ++it) {
		BundleEndpointId eid = (*it).first;
		double p = (*it).second;
		//NS_LOG_DEBUG("eid = " << eid << " - p = " << p );

		/*i.WriteU32(eid); // n. do eid do vizinho*/

		int length2 = eid.GetSerializedSize();
		/*Buffer::Iterator i = start;*/
		i.WriteU32(length2);
		uint8_t buf2[length];
		eid.Serialize(buf2);
		i.Write(buf2, length2);

		i.WriteU64(pack754_64(p)); // probabilidade de entrega para esse vizinho
	}

}

uint32_t ProphetHelloHeader::Deserialize(Buffer::Iterator start) {
	Buffer::Iterator i = start;
	uint32_t length = i.ReadU32();
	uint8_t buf[length];
	i.Read(buf, length);
	m_eid = BundleEndpointId::Deserialize(buf);

	uint32_t size = i.ReadU32();
	//NS_LOG_DEBUG("size=" << size);
	for (uint32_t j = 0; j < size; j++) {
		/*uint32_t eid_ = i.ReadU32();*/

		uint32_t length2 = i.ReadU32();
		uint8_t buf2[length2];
		i.Read(buf2, length2);
		BundleEndpointId eid_ = BundleEndpointId::Deserialize(buf2);

		double p_ = unpack754_64(i.ReadU64());
		//NS_LOG_DEBUG("eid = " << eid_ << " - p = " << p_ );
		m_plist[eid_] = p_;
	}

	return GetSerializedSize();
}

void
ProphetHelloHeader::SetBundleEndpointId (BundleEndpointId eid)
{
  m_eid = eid;
}

BundleEndpointId
ProphetHelloHeader::GetBundleEndpointId () const
{
  return m_eid;
}


}
}
