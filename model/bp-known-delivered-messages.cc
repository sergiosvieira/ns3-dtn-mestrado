/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-known-delivered-messages.h"
#include "bp-sdnv.h"
#include "bp-header.h"

#include "ns3/simulator.h"
#include "ns3/log.h"

#include <algorithm>


namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("KnownDeliveredMessages");

KnownDeliveredMessages::KnownDeliveredMessages ()
  : m_kdm (), m_isInitiator (true)
{}

KnownDeliveredMessages::~KnownDeliveredMessages ()
{
  m_kdm.clear ();
}

bool
KnownDeliveredMessages::IsInitiator () const
{
  return m_isInitiator;
}

void
KnownDeliveredMessages::SetIsInitiator (bool isInitiator)
{
  m_isInitiator = isInitiator;
}

void
KnownDeliveredMessages::Insert (Ptr<Bundle> bundle)
{
  PrimaryBundleHeader header = bundle->GetPrimaryHeader ();
  m_kdm.insert (make_pair(bundle->GetBundleId (), header.GetLifetimeSeconds ()));
}

void
KnownDeliveredMessages::Insert (const KdmPair& kp)
{
  m_kdm.insert (kp);
}

bool
KnownDeliveredMessages::Has (const GlobalBundleIdentifier& gbid) const
{ 
  return m_kdm.find (gbid) != m_kdm.end ();
}

KdmList
KnownDeliveredMessages::Get () const
{
  KdmList kdmList;
  for (KdmMap::const_iterator iter = m_kdm.begin (); iter != m_kdm.end (); ++iter)
    {
      kdmList.push_back (iter->first);
    }
  return kdmList;
}

uint32_t
KnownDeliveredMessages::NEntries () const
{
  return m_kdm.size ();
}

void
KnownDeliveredMessages::Merge (const KnownDeliveredMessages& kdm)
{
  m_kdm.insert (kdm.m_kdm.begin (), kdm.m_kdm.end ());
}

void 
KnownDeliveredMessages::RemoveExpiredBundles ()
{
  for (KdmMap::iterator it = m_kdm.begin(); it != m_kdm.end(); )
    {
      if (TimeExpired (*it))
        m_kdm.erase (it++);
      else
        ++it;
    }
}

void
KnownDeliveredMessages::Clear ()
{
  m_kdm.clear ();
}
  

uint32_t
KnownDeliveredMessages::GetSerializedSize () const
{
  KdmMap::const_iterator iter;
  uint64_t size = 1;
  size += Sdnv::EncodingLength (m_kdm.size ());
  for (iter = m_kdm.begin (); iter != m_kdm.end (); ++iter)
    {
      size += GetKdmPairSerializedSize (*iter);
    }
  return size;
}
 
uint32_t
KnownDeliveredMessages::Serialize (uint8_t *buffer) const
{
  uint32_t i = 0;
  buffer[i] = (uint8_t) m_isInitiator;
  i += 1;
  KdmMap::const_iterator iter;
  Sdnv::Encode (m_kdm.size (), buffer+i);
  i += Sdnv::EncodingLength (m_kdm.size ());

  for (iter = m_kdm.begin (); iter != m_kdm.end (); ++iter)
    {
      i += SerializeKdmPair (buffer+i, *iter);
    }

  return i;
}

KnownDeliveredMessages
KnownDeliveredMessages::Deserialize (uint8_t const*buffer)
{
  KnownDeliveredMessages result = KnownDeliveredMessages ();
  uint32_t i = 0;
  bool isInitiator = (bool) buffer[i];
  result.SetIsInitiator (isInitiator);
  i += 1;
  uint64_t size = Sdnv::Decode (buffer+i);
  i += Sdnv::EncodingLength (size);

  for (uint64_t j = 0; j < size; ++j)
    {
      KdmPair pair = DeserializeKdmPair (buffer+i);
      i += GetKdmPairSerializedSize (pair);
      result.Insert (pair);
    }
  return result;
}

uint32_t
KnownDeliveredMessages::GetKdmPairSerializedSize (const KdmPair& pair)
{
  uint32_t size = pair.first.GetSerializedSize ();
  size += Sdnv::EncodingLength (pair.second);
  return size;
}

uint32_t
KnownDeliveredMessages::SerializeKdmPair (uint8_t *buffer, const KdmPair& pair) const
{
  uint32_t i = 0;
  pair.first.Serialize (buffer);
  i += pair.first.GetSerializedSize ();
  Sdnv::Encode (pair.second, buffer+i);
  i += Sdnv::EncodingLength (pair.second);
  return i;
}

KdmPair
KnownDeliveredMessages::DeserializeKdmPair (uint8_t const*buffer)
{
  uint32_t i = 0;

  GlobalBundleIdentifier gbid = GlobalBundleIdentifier::Deserialize (buffer);
  i += gbid.GetSerializedSize ();
  uint64_t ttl = Sdnv::Decode (buffer+i);
  return make_pair<GlobalBundleIdentifier,uint64_t> (gbid,ttl);
}

bool 
KnownDeliveredMessages::TimeExpired (const KdmPair& p) const
{
  Time lifetime = Seconds (p.second);
  Time creationTime = Seconds (p.first.GetCreationTimestamp ().GetSeconds ());
  return Simulator::Now () > lifetime + creationTime; 
}

}} // namespace bundleProtocol, ns3

#ifdef RUN_SELF_TESTS

#include "ns3/test.h"

using namespace ns3::bundleProtocol;

namespace ns3 {

class KnownDeliveredMessagesTest : public ns3::Test {
private:
public:
  KnownDeliveredMessagesTest ();
  virtual bool RunTests (void);

};

  KnownDeliveredMessagesTest::KnownDeliveredMessagesTest ()
    : ns3::Test ("KnownDeliveredMessages")
  {}

bool
KnownDeliveredMessagesTest::RunTests (void)
{
  bool result = true;
  
  BundleEndpointId eid1 = BundleEndpointId ("dtn","1");
  BundleEndpointId eid2 = BundleEndpointId ("dtn","2");
  BundleEndpointId eid3 = BundleEndpointId ("dtn","3");
  BundleEndpointId eid4 = BundleEndpointId ("dtn","4");
  BundleEndpointId eid5 = BundleEndpointId ("dtn","5");
  BundleEndpointId eid6 = BundleEndpointId ("dtn","6");
  
  Ptr<Packet> p;
  PrimaryBundleHeader pheader = PrimaryBundleHeader ();
  CanonicalBundleHeader cheader = CanonicalBundleHeader (PAYLOAD_BLOCK);
  cheader.SetLastBlock (true);
  
  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid1);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b1 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid2);
  pheader.SetCreationTimestamp (CreationTimestamp (4,6));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b2 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid3);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b3 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid4);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b4 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid5);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b5 = Create<Bundle> (p);

  p = Create<Packet> ();
  pheader.SetSourceEndpoint (eid6);
  pheader.SetCreationTimestamp (CreationTimestamp (3,5));
  p->AddHeader (cheader);
  p->AddHeader (pheader);
  Ptr<Bundle> b6 = Create<Bundle> (p);

  KnownDeliveredMessages kdm1 = KnownDeliveredMessages ();
  kdm1.SetIsInitiator (true);
  KnownDeliveredMessages kdm2 = KnownDeliveredMessages ();
  kdm2.SetIsInitiator (false);
  
  NS_TEST_ASSERT (kdm1.IsInitiator ());
  NS_TEST_ASSERT (!kdm2.IsInitiator ());

  kdm1.Insert (b1);
  kdm1.Insert (b2);
  kdm1.Insert (b3);

  NS_TEST_ASSERT (kdm1.Has (b1));  
  NS_TEST_ASSERT (kdm1.Has (b2));
  NS_TEST_ASSERT (kdm1.Has (b3));

  NS_TEST_ASSERT_EQUAL (kdm1.NEntries (), 3);

  kdm2.Insert (b4);
  kdm2.Insert (b5);
  kdm2.Insert (b6);

  NS_TEST_ASSERT (kdm2.Has (b4));
  NS_TEST_ASSERT (kdm2.Has (b5));
  NS_TEST_ASSERT (kdm2.Has (b6));

  NS_TEST_ASSERT_EQUAL (kdm2.NEntries (), 3);

  NS_TEST_ASSERT (!kdm2.Has (b1));  
  NS_TEST_ASSERT (!kdm2.Has (b2));
  NS_TEST_ASSERT (!kdm2.Has (b3));
  NS_TEST_ASSERT (!kdm1.Has (b4));
  NS_TEST_ASSERT (!kdm1.Has (b5));
  NS_TEST_ASSERT (!kdm1.Has (b6));

  uint8_t buffer [kdm1.GetSerializedSize ()];
  kdm1.Serialize (buffer);
  KnownDeliveredMessages tmp = KnownDeliveredMessages::Deserialize (buffer);
  NS_TEST_ASSERT (tmp.IsInitiator ());
  
  KdmList l1 = kdm1.Get ();
  KdmList l2 = tmp.Get ();
  
  NS_TEST_ASSERT (tmp.Has (b1));
  NS_TEST_ASSERT (tmp.Has (b2));
  NS_TEST_ASSERT (tmp.Has (b3));

  NS_TEST_ASSERT_EQUAL (tmp.NEntries (), 3);

  kdm1.Merge (kdm2);

  NS_TEST_ASSERT (kdm1.Has (b1));
  NS_TEST_ASSERT (kdm1.Has (b2));
  NS_TEST_ASSERT (kdm1.Has (b3));
  NS_TEST_ASSERT (kdm1.Has (b4));
  NS_TEST_ASSERT (kdm1.Has (b5));
  NS_TEST_ASSERT (kdm1.Has (b6));
  
  NS_TEST_ASSERT_EQUAL (kdm1.NEntries (), 6);
  
  kdm2.Clear ();
  NS_TEST_ASSERT_EQUAL (kdm2.NEntries (), 0);

  return result;
}

static KnownDeliveredMessagesTest gKnownDeliveredMessagesTest;

} // namespace ns3

#endif
