/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-sdnv.h"

#include <iostream>
using namespace std;

namespace ns3 {
namespace bundleProtocol {

Sdnv::Sdnv () 
{}
  
Sdnv::~Sdnv () 
{}

// Encode the value to a SDNV and add it to the buffer
void
Sdnv::Encode (uint64_t val, Buffer::Iterator& iter) 
{
  int length = EncodingLength (val);
  uint64_t mask = 0x7F;
  mask = mask << ( (length - 1) * 7);
  
  for (int i = 0; i < length; ++i)
    {
      if(i == length-1) // The last octet have MSB set to 0
        {
          iter.WriteU8 (((val & mask) >> (((length - 1) - i) * 7)));
        }
      else // All other have MSB set to 1
        {
          iter.WriteU8 ( 0x80 | ((val & mask) >> (((length - 1) - i) * 7)));
        }
      mask = mask >> 7;
    }
}

void
Sdnv::Encode (uint64_t val, uint8_t *buffer) 
{
  int length = EncodingLength (val);

  uint64_t mask = 0x7F;
  mask = mask << ((length - 1) * 7);

  for (int i = 0; i < length; ++i)
    {
      if(i == length-1) // The last octet have MSB set to 0
        {
          buffer[i] =  ((val & mask) >> (((length - 1) - i) * 7));
        }
      else // All other have MSB set to 1
        {
          buffer[i] = ( 0x80 | ((val & mask) >> (((length - 1) - i) * 7)));
        }
      mask = mask >> 7;
    }
}

// Decode one SDNV from the buffer and return it.
uint64_t 
Sdnv::Decode (Buffer::Iterator& iter)
{
  uint64_t res = 0;
  uint8_t octet;
  do 
    {
      res = res << 7;
      octet = iter.ReadU8 ();
      res |= 0x7F & octet;
    }
  while ((octet & 0x80) == 0x80);
  return res;
}

uint64_t 
Sdnv::Decode (uint8_t const*buffer)
{
  uint64_t res = 0;
  uint8_t octet;
  uint64_t i = 0;
  do 
    {
      res = res << 7;
      octet = buffer[i];
      res |= 0x7F & octet;
      ++i;
    }
  while ((octet & 0x80) == 0x80);
  
  return res;
}


uint32_t 
Sdnv::EncodingLength (uint64_t val)
{
  uint32_t len (0);
  uint64_t temp (0);
  temp = val;
  do 
    {
      temp = temp >> 7;
      ++len;
    } while (temp != 0);
  return len;
}

}} // namespace bundleProtocol, ns3


#ifdef RUN_SELF_TESTS


#include "ns3/test.h"
#include "ns3/packet.h"
#include "ns3/buffer.h"
#include <math.h>
#include <iostream>

#include "bp-sdnv.h"

using namespace std;

namespace ns3 {

class SdnvTest : public ns3::Test {
private:
public:
  SdnvTest ();
  virtual bool RunTests (void);

};

  SdnvTest::SdnvTest ()
    : ns3::Test ("Sdnv")
  {}

bool
SdnvTest::RunTests (void)
{
  bool result = true;
  /*
  Buffer buffer;

  uint64_t val1 = 0xABC;
  uint64_t res1 = 0x953C;
  uint64_t elen1 = 2;
  uint64_t dlen1 = 2;
 
  uint64_t val2 = 0x1234;
  uint64_t res2 = 0xA434;
  uint64_t elen2 = 2;
  uint64_t dlen2 = 2;
 
  uint64_t val3 = 0x4234;
  uint64_t res3 = 0x818434;
  uint64_t elen3 = 3;
  uint64_t dlen3 = 2;
  
  uint64_t val4 = 0x7F;
  uint64_t res4 = 0x7F;
  uint64_t elen4 = 1;
  uint64_t dlen4 = 1;
  
  uint64_t tmp;
  
  buffer = Buffer (2);
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::EncodingLength (val1), elen1);
  cout << "Hit?" << endl;
  Buffer::Iterator iter = buffer.Begin ();
  cout << "Sedan hit?" << endl;
  bundleProtocol::Sdnv::Encode ((uint64_t) 2748, iter);
  cout << "Men ej hit?" << endl;
  iter = buffer.Begin ();
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::Length (iter), dlen1);
  tmp = bundleProtocol::Sdnv::Decode (iter);
  NS_TEST_ASSERT_EQUAL (tmp, val1);


  buffer = Buffer (2);
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::EncodingLength (val2), elen2);
  iter = buffer.Begin ();
  bundleProtocol::Sdnv::Encode (val2, iter);
  iter = buffer.Begin ();
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::Length (iter), dlen2);
  tmp = bundleProtocol::Sdnv::Decode (iter);
  NS_TEST_ASSERT_EQUAL (tmp, val2);

  buffer = Buffer (3);
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::EncodingLength (val3), elen3);
  iter = buffer.Begin ();
  bundleProtocol::Sdnv::Encode (val1, iter);
  iter = buffer.Begin ();
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::Length (iter), dlen3);
  tmp = bundleProtocol::Sdnv::Decode (iter);
  NS_TEST_ASSERT_EQUAL (tmp, val3);

  buffer = Buffer (1);
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::EncodingLength (val4), elen4);
  iter = buffer.Begin ();
  bundleProtocol::Sdnv::Encode (val4, iter);
  iter = buffer.Begin ();
  NS_TEST_ASSERT_EQUAL (bundleProtocol::Sdnv::Length (iter), dlen4);
  tmp = bundleProtocol::Sdnv::Decode (iter);
  NS_TEST_ASSERT_EQUAL (tmp, val4);
  */
  return result;
}

static SdnvTest g_SdnvTest;

} // namespace ns3

#endif /* RUN_SELF_TESTS */
