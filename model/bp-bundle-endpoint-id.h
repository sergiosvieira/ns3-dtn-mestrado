/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BUNDLE_PROTOCOL_EID_H
#define BUNDLE_PROTOCOL_EID_H

#include <iostream>

#include "ns3/object.h"
#include "ns3/address.h"
#include <string>
#include <utility>

using namespace std;

namespace ns3 {
namespace bundleProtocol {

/**
 * \ingroup bundle
 *
 * \brief A address class used in the bundle layer
 *
 * Nodes in the network are members of groups called DTN endpoints. A DTN endpoint is hence a set of nodes,
 * if a DTN endpoint consist of only one node, then the DTN endpoint is called a singleton endpoint.
 *
 * A endpoint identifier (EID) is a name, that identifies a DTN endpoint. In the DTN architecture the
 * EIDs are implemented as Uniform Resource Identifier (URI), which have the general structure
 * < scheme name > : < scheme-specific part> where the two parts consist of strings. 
 *
 * No full implementation of URIs has been made, this class is only a mockup that only understand one simple scheme called 'dtn'.
 * Only singleton endpoints are handled by the bundleProtocolAgent at the moment.
 * So the scheme name used is always 'dtn' and the scheme specific part can be either 'none' representing a null endpoint  or an integer value which constitutes the name of a node. Two exampels are:
 * - dtn:none
 * - dtn:2
 *
 */

class BundleEndpointId
{
public:
  BundleEndpointId ();
  BundleEndpointId (const string& uri);
  BundleEndpointId (const string& scheme, const string& ssp);
  BundleEndpointId (int id);
  virtual ~BundleEndpointId ();

   /**
   * \brief Sets the uri of the EID.
   * \param uri Set the URI of this EID.
   *
   */
  void SetUri (const string& uri);
 
  /**
   * \brief Gets the URI of this EID . 
   * \return The current URI of this EID.
   */
  string GetUri () const;


  /**
   * \brief Gets the scheme name of the URI of this EID . 
   * \return The current scheme name used.
   */
  string GetScheme () const;
  /**
   * \brief Gets the scheme specific part of the URI of this EID . 
   * \return The current scheme specific part used.
   */
  string GetSsp () const;
  /**
   * \brief Gets the scheme specific part of the URI of this EID as an integer. 
   * \return The current scheme speficic part.
   */
  uint32_t GetId () const;

  /**
   * \brief Returns the number of bytes needed to serialize the BundleEndpointId.
   * \return The number of bytes required to serialize the BundleEndpointId.
   */
  uint32_t GetSerializedSize () const;

  /**
   * \brief Returns a null EID.
   * \return A null BundleEndpointId.
   */
  static BundleEndpointId GetAnyBundleEndpointId ();
  /**
   * \brief Check if the BundleEndpointId is an singleton or not.
   * \return Returns true if the BundleEndpointId is an singleton otherwise false.
   */
  bool IsSingleton () const;
  
  bool operator== (const BundleEndpointId& other) const;
  bool operator!= (const BundleEndpointId& other) const;
  bool operator< (const BundleEndpointId& other) const;

  static bool IsMatchingType (const Address &address);
  operator Address () const;
  static BundleEndpointId ConvertFrom (const Address &address);

  /**
   * \brief Serialize this BundleEndpointId to a byte buffer
   * \param buf output buffer that gets written with this BundleEndpointId.
   */
  void Serialize (uint8_t *buf) const;
  /**
   * \brief Deserializes a byte buffer containing a BundleEndpointId.
   * \param buf Buffer to read the BundleEndpointId from.
   * \return Returns the deserialized BundleEndpointId.
   */
  static BundleEndpointId Deserialize (uint8_t const*buf);

private:
  Address ConvertTo () const;
  static uint8_t GetType ();

  string m_scheme;
  string m_ssp;
  uint32_t m_id;
};

/**
 * \class ns3::bundleProtocol::BundleEndpointIdValue
 * \brief hold objects of type ns3::bundleProtocol::BundleEndpointId
 */

ATTRIBUTE_HELPER_HEADER (BundleEndpointId);

ostream& operator<< (ostream& os, const BundleEndpointId& eid);
istream& operator>> (istream& is, BundleEndpointId& eid);


}} // namespace bundleProtocol, ns3


#endif /* BUNDLE_PROTOCOL_EID_H */
