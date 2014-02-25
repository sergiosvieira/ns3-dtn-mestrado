/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_DICTIONARY_H
#define BP_DICTIONARY_H

#include "bp-bundle-endpoint-id.h"
#include "bp-sdnv.h"
#include "bp-utility.h"
#include <vector>
#include <set>
#include <functional>

using namespace std;

namespace ns3 {
namespace bundleProtocol {
 
/**
 * \ingroup bundle
 *
 * \brief Bundle Endpoint Id container used by the PrimaryBundleHeader.
 *
 *
 */

class Dictionary
{
public:
  Dictionary ();
  Dictionary (const Dictionary& dic);
  ~Dictionary();

  void SetDestinationEndpoint (const BundleEndpointId& eid);
  void SetSourceEndpoint (const BundleEndpointId& eid);
  //  void SetReportToEndpoint (const BundleEndpointId& eid);
  void SetCustodianEndpoint (const BundleEndpointId& eid);

  BundleEndpointId GetDestinationEndpoint () const;
  BundleEndpointId GetSourceEndpoint () const;
  //  BundleEndpointId GetReportToEndpoint () const;
  BundleEndpointId GetCustodianEndpoint () const;

  void RemoveDestinationEndpoint ();
  void RemoveSourceEndpoint ();
  //  void RemoveReportToEndpoint ();
  void RemoveCustodianEndpoint ();

  uint64_t GetSerializedSize () const;
  uint64_t Serialize (uint8_t *buffer) const;
  static Dictionary Deserialize (uint8_t const*buffer);

  bool operator== (const Dictionary& other) const;
  bool operator!= (const Dictionary& other) const;
  
  
private:
  Dictionary (const BundleEndpointId& dest, const BundleEndpointId& source, const BundleEndpointId& custodian);
  set<BundleEndpointId> GetUnique () const;
  BundleEndpointId m_destination;
  BundleEndpointId m_source;
  //BundleEndpointId m_reportTo;
  BundleEndpointId m_custodian;
  friend ostream& operator<< (std::ostream& os, const Dictionary& dictionary);
};
ostream& operator<< (std::ostream& os, const Dictionary& dictionary);

}} // namespace bundleProtocol, ns3


#endif /* BP_DICTIONARY_H */
