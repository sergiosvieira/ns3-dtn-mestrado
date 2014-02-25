/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_CONTACT_WINDOW_INFORMATION_H
#define BP_CONTACT_WINDOW_INFORMATION_H

#include "ns3/vector.h"
#include "ns3/data-rate.h"

namespace ns3 {
namespace bundleProtocol {

/**
 * \ingroup bundleRouter
 *
 * \brief Holds the contact information.
 *
 *
 */

class ContactWindowInformation
{
public:
  ContactWindowInformation ();
  ContactWindowInformation (Vector position, Vector speed, uint32_t range, const DataRate& dataRate, bool isInitiator);
  ~ContactWindowInformation ();
  
  bool IsInitiator () const;
  void SetIsInitiator (bool isSender);

  void SetPosition (Vector position);
  Vector GetPosition () const;
  void SetVelocity (Vector speed);
  Vector GetVelocity () const;
  void SetTransmissionRange (uint32_t range);
  uint32_t GetTransmissionRange () const;
  void SetTransmissionSpeed (const DataRate& dataRate);
  DataRate GetTransmissionSpeed () const;

  uint32_t GetSerializedSize () const;
  void Serialize (uint8_t *buffer) const;
  void SerializeXYVector (Vector v, uint8_t *buffer) const;
  static Vector DeserializeXYVector (uint8_t const*buffer);
  static ContactWindowInformation Deserialize (uint8_t const*buffer);
  
private:
  Vector m_position;
  Vector m_speed;
  uint32_t m_range;
  DataRate m_dataRate;
  bool m_isInitiator;
};


}} // namespace bundleProtocol, ns3



#endif /* BP_CONTACT_WINDOW_INFORMATION_H */
