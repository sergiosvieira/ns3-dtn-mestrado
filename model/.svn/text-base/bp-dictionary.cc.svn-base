/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <algorithm>
#include <set>
#include "ns3/log.h"
#include "bp-dictionary.h"
#include "ns3/simulator.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("Dictionary");

Dictionary::Dictionary ()
  : m_destination (),
    m_source (),
    m_custodian ()
{}

Dictionary::Dictionary (const BundleEndpointId& dest, const BundleEndpointId& source, const BundleEndpointId& custodian)
  : m_destination (dest),
    m_source (source),
    m_custodian (custodian)
{}

Dictionary::Dictionary (const Dictionary& dic)
  : m_destination (dic.m_destination),
    m_source (dic.m_source),
    m_custodian (dic.m_custodian)
{}

Dictionary::~Dictionary ()
{}

void 
Dictionary::SetDestinationEndpoint (const BundleEndpointId& eid)
{
  m_destination = eid;
}

void
Dictionary::SetSourceEndpoint (const BundleEndpointId& eid)
{
  m_source = eid;
}

void
Dictionary::SetCustodianEndpoint (const BundleEndpointId& eid)
{
  m_custodian = eid;
}

void
Dictionary::RemoveDestinationEndpoint ()
{
  m_destination = BundleEndpointId::GetAnyBundleEndpointId ();
}

void
Dictionary::RemoveSourceEndpoint ()
{
  m_source = BundleEndpointId::GetAnyBundleEndpointId ();
}

void
Dictionary::RemoveCustodianEndpoint ()
{
  m_custodian = BundleEndpointId::GetAnyBundleEndpointId ();
}

BundleEndpointId
Dictionary::GetDestinationEndpoint () const
{
  return m_destination;
}

BundleEndpointId
Dictionary::GetSourceEndpoint () const
{
  return m_source;
}

BundleEndpointId
Dictionary::GetCustodianEndpoint () const
{
  return m_custodian;
}

set<BundleEndpointId> 
Dictionary::GetUnique () const
{
  set<BundleEndpointId> temp;
  temp.insert (m_destination);
  temp.insert (m_source);
  temp.insert (m_custodian);
  return temp;
}

uint64_t
Dictionary::GetSerializedSize () const
{
  set<BundleEndpointId> unique = GetUnique ();
  return unique.size () * 4 + 1; 
}

uint64_t
Dictionary::Serialize (uint8_t *buffer) const
{ 

  uint64_t special = 0;

  // Serialize the endpoints

  uint64_t i = 1;
  m_source.Serialize (buffer+i);
  i += m_source.GetSerializedSize ();  

  if (m_custodian == m_source)
    {
      if (m_destination == m_source)
        {
          special = 0;
        }
      else
        {
          m_destination.Serialize (buffer+i);
          i += m_destination.GetSerializedSize ();
          special = 1;
        }
    }
  else 
    {
      m_custodian.Serialize (buffer+i);
      i += m_custodian.GetSerializedSize ();
      if (m_destination == m_custodian)
        {
          special = 2;
        }
      else if (m_destination == m_source)
        {
          special = 3;
        }
      else
        {
          m_destination.Serialize (buffer+i);
          i += m_destination.GetSerializedSize ();
          special = 4;
        }
    }

  // The 'offsets' in the integer are put first in the byte buffer.
  
  buffer[0] = special & 0xff;

  return i;
}

Dictionary
Dictionary::Deserialize (uint8_t const*buffer)
{
  uint64_t i = 0;
  uint64_t special = buffer[0];
  i += 1;
  BundleEndpointId source = BundleEndpointId::Deserialize (buffer+i);
  BundleEndpointId custodian; 
  BundleEndpointId destination;

  if (special == 0)
    {
      // This is the case where all three EIDs are the same.
      custodian = BundleEndpointId::Deserialize (buffer+i);
      destination = BundleEndpointId::Deserialize (buffer+i);
    }
  else if (special == 1)
    {
      // This is the case where source and custodian have the same EID
      // and destination have another value.
      custodian = BundleEndpointId::Deserialize (buffer+i);
      i += source.GetSerializedSize ();
      destination = BundleEndpointId::Deserialize (buffer+i);
       }
  else if (special == 2)
    {
      // This is the case where custodian and destination have the same EID
      // and source have another value.
      i += source.GetSerializedSize ();
      custodian = BundleEndpointId::Deserialize (buffer+i);
      destination = BundleEndpointId::Deserialize (buffer+i);
    }
  else if (special == 3)
    {
      // This is when source and destination have the same EID and 
      // custodian have another value.
      destination = BundleEndpointId::Deserialize (buffer+i);
      i += destination.GetSerializedSize ();
      custodian = BundleEndpointId::Deserialize (buffer+i);
    }
  else
    {
      // This is the where source, custodian and destination all have different 
      // EIDs
      i += source.GetSerializedSize ();
      custodian = BundleEndpointId::Deserialize (buffer+i);
      i += custodian.GetSerializedSize ();
      destination = BundleEndpointId::Deserialize (buffer+i);
    }

  return Dictionary (destination, source, custodian);
}

bool
Dictionary::operator== (const Dictionary& other) const
{
  return (m_destination == other.m_destination) && (m_source == other.m_source) && (m_custodian == other.m_custodian);
}

bool
Dictionary::operator!= (const Dictionary& other) const
{
  return !(*this == other);
}

ostream&
operator<< (std::ostream& os, const Dictionary& dictionary)
{ 
  os << "Source endpoint: " << dictionary.m_source << endl;
  //  os << "ReportTo endpoint: " << dictionary.m_reportTo << endl;
  os << "Custodian endpoint: " << dictionary.m_custodian << endl;
  os << "Destination endpoint: " << dictionary.m_destination;

  return os;
}
}} // namespace bundleProtocol, ns3
/*
#ifdef RUN_SELF_TESTS
#include <math.h>
#include <iostream>
#include <vector>

#include "ns3/test.h"

#include "bp-dictionary.h"

using namespace std;
using namespace ns3::bundleProtocol;
namespace ns3 {

class DictionaryTest : public ns3::Test {
private:
public:
  DictionaryTest ();
  virtual bool RunTests (void);

};

  DictionaryTest::DictionaryTest ()
    : ns3::Test ("Dictionary")
  {}

bool
DictionaryTest::RunTests (void)
{
  bool result = true;

  Dictionary dic = Dictionary ();
  BundleEndpointId eid1 = BundleEndpointId ("dtn", "1");
  BundleEndpointId eid2 = BundleEndpointId ("dtn", "2");
  BundleEndpointId eid3 = BundleEndpointId ("dtn", "3");
  BundleEndpointId eid4 = BundleEndpointId ("dtn", "4");  

  BundleEndpointId tmp;

  dic.SetDestinationEndpoint (eid1);
  tmp = dic.GetDestinationEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, eid1);

  dic.SetSourceEndpoint (eid2);
  tmp = dic.GetSourceEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, eid2);

  //  dic.SetReportToEndpoint (eid3);
  //tmp = dic.GetReportToEndpoint ().GetBundleEndpoint ();
  // NS_TEST_ASSERT_EQUAL (tmp, eid3);

  dic.SetCustodianEndpoint (eid4);
  tmp = dic.GetCustodianEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, eid4);
  int size = dic.GetEids ().size ();
  NS_TEST_ASSERT_EQUAL (size, 4);

  //  dic.SetReportToEndpoint (eid2);

  tmp = dic.GetSourceEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, eid2);

  //  tmp = dic.GetReportToEndpoint ().GetBundleEndpoint ();
  //NS_TEST_ASSERT_EQUAL (tmp, eid2);

  size = dic.GetEids ().size ();
  NS_TEST_ASSERT_EQUAL (size, 3);
    
  //dic.RemoveReportToEndpoint ();
  tmp = dic.GetSourceEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, eid2);
  
  //  tmp = dic.GetReportToEndpoint ().GetBundleEndpoint ();
  NS_TEST_ASSERT_EQUAL (tmp, BundleEndpointId ("dtn", "none"));

  //  dic.SetReportToEndpoint (eid3);
  uint8_t buffer [dic.GetSerializedSize ()];
  dic.Serialize (buffer);
  
  Dictionary dic2 = Dictionary::Deserialize (buffer);
 
  NS_TEST_ASSERT_EQUAL (dic,dic2);

  return result;
}

static DictionaryTest g_DictionaryTest;

} // namespace ns3
*/
//#endif /* RUN_SELF_TESTS */

