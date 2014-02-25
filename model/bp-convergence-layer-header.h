#ifndef BP_CONVERGENCE_LAYER_HEADER_H
#define BP_CONVERGENCE_LAYER_HEADER_H

#include <functional>
#include <algorithm>

#include "ns3/packet.h"
#include "ns3/header.h"

#include "bp-global-bundle-identifier.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {


enum ClaHeaderType 
{
  CLA_ACK,
  CLA_SEGMENT
};

enum AckResponse 
{
  ACK_SUCCEEDED,
  ACK_FAILED,
  FINAL_DELIVERY_SUCCEEDED,
  FINAL_DELIVERY_FAILED
};

/**
 * \ingroup convergenceLayer
 *
 * \brief header used when sending bundle segments to enable reassembly.
 *
 *
 */

class ConvergenceLayerHeader : public Header
{
public:
  ConvergenceLayerHeader ();
  ConvergenceLayerHeader (ClaHeaderType type);
  ~ConvergenceLayerHeader ();
  
  void SetType (ClaHeaderType type);
  ClaHeaderType GetType () const;


  void SetStartFlag (bool val);
  bool GetStartFlag () const;

  void SetEndFlag (bool val);
  bool GetEndFlag () const;
  
  void SetNumberOfSegments (uint16_t nSegments);
  uint16_t GetNumberOfSegments () const;

  void SetSegmentNumber (uint16_t segmentNumber);
  uint16_t GetSegmentNumber () const;

  void SetSequenceNumber (uint16_t sequenceNumber);
  uint16_t GetSequenceNumber () const;

  void SetResponse (AckResponse response);
  AckResponse GetResponse () const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  
private:
  ClaHeaderType m_type; // 1

  uint16_t m_nSegments; // 2
  uint16_t m_segmentNumber; // 2
  uint16_t m_sequenceNumber; // 2
  bool m_startFlag; // 0
  bool m_endFlag; // 0

  AckResponse m_response; // 1
};
}} // namespace bundleProtocol, ns3

#endif /* BP_CONVERGENCE_LAYER_HEADER_H */
