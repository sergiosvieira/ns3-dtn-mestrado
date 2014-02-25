/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "bp-contact-window-information.h"
#include "bp-utility.h"

namespace ns3 {
namespace bundleProtocol {

ContactWindowInformation::ContactWindowInformation ()
  : m_position (), m_speed (), m_range (0), m_dataRate (0), m_isInitiator (true)
{}

ContactWindowInformation::ContactWindowInformation (Vector position, Vector speed, uint32_t range, const DataRate& dataRate, bool isInitiator)
  : m_position (position), m_speed (speed), m_range (range), m_dataRate (dataRate), m_isInitiator (isInitiator)
{}

ContactWindowInformation::~ContactWindowInformation ()
{}
  
bool
ContactWindowInformation::IsInitiator () const
{
  return m_isInitiator;
}

void
ContactWindowInformation::SetIsInitiator (bool isInitiator)
{
  m_isInitiator = isInitiator;
}

void
ContactWindowInformation::SetPosition (Vector position)
{
  m_position = position;
}

Vector
ContactWindowInformation::GetPosition () const
{
  return m_position;
}

void
ContactWindowInformation::SetVelocity (Vector speed)
{
  m_speed = speed;
}

Vector
ContactWindowInformation::GetVelocity () const
{
  return m_speed;
}

void
ContactWindowInformation::SetTransmissionRange (uint32_t range)
{
  m_range = range;
}

uint32_t
ContactWindowInformation::GetTransmissionRange () const
{
  return m_range;
}

void
ContactWindowInformation::SetTransmissionSpeed (const DataRate& dataRate)
{
  m_dataRate = dataRate;
}

DataRate
ContactWindowInformation::GetTransmissionSpeed () const
{
  return m_dataRate;
}
  
uint32_t
ContactWindowInformation::GetSerializedSize () const
{
  return sizeof (double) * 3 * 2 + 4 + 4 + 1;
}

void
ContactWindowInformation::Serialize (uint8_t *buffer) const
{
  SerializeXYVector (m_position, buffer);
  SerializeXYVector (m_speed, buffer+16);
  uint32_t tmp = (uint32_t) m_dataRate.GetBitRate ();
  buffer[32] = m_range & 0xff;
  buffer[33] = (m_range >> 8) & 0xff;
  buffer[34] = (m_range >> 16) & 0xff;
  buffer[35] = (m_range >> 24) & 0xff;
  buffer[36] = tmp & 0xff;
  buffer[37] = (tmp >> 8) & 0xff;
  buffer[38] = (tmp >> 16) & 0xff;
  buffer[39] = (tmp >> 24) & 0xff;
  buffer[40] = (uint8_t) m_isInitiator;
}

void 
ContactWindowInformation::SerializeXYVector (Vector v, uint8_t *buffer) const
{
  SerializeDouble (v.x, buffer);
  SerializeDouble (v.y, buffer+8);
}

Vector 
ContactWindowInformation::DeserializeXYVector (uint8_t const*buffer)
{
  double x = DeserializeDouble (buffer);
  double y = DeserializeDouble (buffer+8);
  return Vector(x,y,0);
}

ContactWindowInformation
ContactWindowInformation::Deserialize (uint8_t const*buffer)
{
  Vector position = DeserializeXYVector (buffer);
  Vector speed = DeserializeXYVector (buffer+16);
  uint32_t range = buffer[32] | (buffer[33] << 8) | (buffer[34] << 16) | (buffer[35] << 24);
  uint32_t tmp = buffer[36] | (buffer[37] << 8) | (buffer[38] << 16) | (buffer[39] << 24);
  DataRate dataRate (tmp);
  bool isInitiator = buffer[40];
  return ContactWindowInformation (position, speed, range, dataRate, isInitiator);
}

}} // namespace bundleProtocol, ns3
