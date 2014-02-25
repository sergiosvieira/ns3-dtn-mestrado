/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <string>
#include <cstring>
#include <iostream> 
#include <sstream>

#include "ns3/assert.h"
#include "ns3/log.h"

#include "ns3/bp-bundle-endpoint-id.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("BundleEndpointId");

BundleEndpointId::BundleEndpointId ()
  : m_scheme ("dtn"), m_ssp ("none"), m_id (-1)
{
}

BundleEndpointId::BundleEndpointId (const string& uri)
  : m_scheme ("dtn"), m_ssp ("none"), m_id (-1)
{ 
  size_t pos;
  pos = uri.find (":");
  m_scheme = uri.substr (0,pos);
  m_ssp = uri.substr (pos+1);

  stringstream ss (m_ssp);
  ss >> m_id;
}

BundleEndpointId::BundleEndpointId (const string& scheme, const string& ssp)
  : m_scheme (scheme), m_ssp (ssp), m_id (-1)
{
  stringstream ss (ssp);
  ss >> m_id;
}

BundleEndpointId::BundleEndpointId (int id)
  : m_scheme ("dtn"), m_ssp ("none"), m_id (id)
{
  stringstream ss;
  if (id == -1)
    {
      ss << "none";
    }
  else
    {
      ss << id;
    }

  m_ssp = ss.str ();
}
  
BundleEndpointId::~BundleEndpointId ()
{}

void
BundleEndpointId::SetUri (const string& uri)
{ 
  size_t pos;
  pos = uri.find (":");
  m_scheme = uri.substr (0,pos);
  m_ssp = uri.substr (pos+1);
  stringstream ss (m_ssp);
  ss >> m_id;
}

string 
BundleEndpointId::GetUri () const
{ 
  stringstream ss;
  ss << m_scheme << ":" << m_ssp;
  return ss.str();
}

string
BundleEndpointId::GetScheme () const
{ 
  return m_scheme;
}

string
BundleEndpointId::GetSsp () const
{ 
  return m_ssp;
}

uint32_t
BundleEndpointId::GetId () const
{ 
  return m_id;
}

uint32_t
BundleEndpointId::GetSerializedSize () const
{ 
  return 4;
}

bool
BundleEndpointId::IsSingleton () const
{ 
  return true;
}

BundleEndpointId
BundleEndpointId::GetAnyBundleEndpointId ()
{ 
  return BundleEndpointId ();
}


bool 
BundleEndpointId::IsMatchingType (const Address &address)
{ 
  return address.CheckCompatible (GetType (), 15);
}

Address 
BundleEndpointId::ConvertTo (void) const
{ 
  uint8_t buf[4];
  Serialize (buf);
  return Address (GetType (), buf, 4);
}

BundleEndpointId 
BundleEndpointId::ConvertFrom (const Address &address)
{ 
  uint8_t buf[4];
  address.CopyTo (buf);
  return Deserialize (buf);
}

uint8_t
BundleEndpointId::GetType (void)
{ 
  static uint8_t type = Address::Register ();
  return type;
}

void
BundleEndpointId::Serialize (uint8_t *buf) const
{ 
  uint32_t temp = m_id + 1; // Adjust the value one step so that the id is garanteed to be positive.
  buf[0] = temp & 0xff;
  buf[1] = (temp >> 8) & 0xff;
  buf[2] = (temp >> 16) & 0xff;
  buf[3] = (temp >> 24) & 0xff;

}

BundleEndpointId
BundleEndpointId::Deserialize (uint8_t const*buf)
{ 
  uint32_t id = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
  id = id - 1; // Readjust the value 
  return BundleEndpointId (id);
}

bool
BundleEndpointId::operator== (const BundleEndpointId& other) const
{
  return  m_id == other.m_id;
}

bool
BundleEndpointId::operator!= (const BundleEndpointId& other) const
{
  return !(*this == other);
}

bool
BundleEndpointId::operator< (const BundleEndpointId& other) const
{
  return m_id < other.m_id;
}

BundleEndpointId::operator Address () const
{ 
  return ConvertTo ();
}

ATTRIBUTE_HELPER_CPP (BundleEndpointId);

std::ostream& operator<< (std::ostream& os, BundleEndpointId const& eid) 
{ 
  return os << eid.GetUri ();
}

istream&
operator>> (istream &is, BundleEndpointId& eid)
{
  string uri;
  string scheme, ssp;
  is >> uri;
  size_t pos = uri.find (":");
  scheme = uri.substr (0,pos);
  ssp = uri.substr (pos+1);

  stringstream ss (ssp);
  uint32_t id;

  if (scheme != "dtn" || !(ssp == "none" || !(ss >> id).fail ()))
    {
      is.setstate (ios_base::failbit);
    }
  else
    {
      eid.SetUri (uri);
    }
  return is;
}

}} // namespace bundleProtocol, ns3



#ifdef RUN_SELF_TESTS

#include "ns3/test.h"

using namespace ns3::bundleProtocol;

namespace ns3 {

class BundleEndpointIdTest : public ns3::Test {
private:
public:
  BundleEndpointIdTest ();
  virtual bool RunTests (void);

};

  BundleEndpointIdTest::BundleEndpointIdTest ()
    : ns3::Test ("BundleEndpointId")
  {}

bool
BundleEndpointIdTest::RunTests (void)
{
  bool result = true;
  BundleEndpointId eid = BundleEndpointId ();

  NS_TEST_ASSERT_EQUAL (eid.IsSingleton (), true);

  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:none"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (),  string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("none")); 
  NS_TEST_ASSERT_EQUAL (eid.GetId(), -1);

  eid = BundleEndpointId ("dtn","none");

  NS_TEST_ASSERT_EQUAL (eid.IsSingleton (), true);

  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:none"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (),  string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("none")); 
  NS_TEST_ASSERT_EQUAL (eid.GetId(), -1);

  eid.SetUri ("dtn:none");
  NS_TEST_ASSERT_EQUAL (eid.IsSingleton (), true);

  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:none"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (),  string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("none")); 
  NS_TEST_ASSERT_EQUAL (eid.GetId(), -1);


  eid = BundleEndpointId ("dtn","23000");
  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (), string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetId (), 23000);

  eid = BundleEndpointId ("dtn:23000");
  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (), string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetId (), 23000);


  eid.SetUri("dtn:23000");
  NS_TEST_ASSERT_EQUAL (eid.GetUri (), string ("dtn:23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetScheme (), string ("dtn"));
  NS_TEST_ASSERT_EQUAL (eid.GetSsp (), string ("23000"));
  NS_TEST_ASSERT_EQUAL (eid.GetId (), 23000);
  
  uint8_t buffer [eid.GetSerializedSize ()];
  eid.Serialize (buffer);
  BundleEndpointId eid2 = BundleEndpointId::Deserialize (buffer);
  NS_TEST_ASSERT_EQUAL (eid2,eid);

  return result;
}

static BundleEndpointIdTest gBundleEndpointIdTest;

} // namespace ns3

#endif /* RUN_SELF_TESTS */

