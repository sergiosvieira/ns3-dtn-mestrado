/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_ADMINISTRATIVE_RECORD_H
#define BP_ADMINISTRATIVE_RECORD_H

#include <stdint.h>
#include "bp-bundle.h"

namespace ns3 {
namespace bundleProtocol {

enum AdminRecordType {
    BUNDLE_STATUS_REPORT = 1,
    CUSTODY_SIGNAL = 2,
    UNKOWN_TYPE = 3
};
  
enum AdminRecordFlag {
  RECORD_IS_NOT_FOR_A_FRAGMENT = 0,
  RECORD_IS_FOR_A_FRAGMENT = 1,
  UNKOWN_FLAG = 2
};

class AdministrativeRecord
{
public: 
  AdministrativeRecord (Ptr<Bundle> bundle, AdminRecordType type);
  virtual ~AdministrativeRecord ();
  
  void SetRecordType (AdminRecordType type);
  AdminRecordType GetRecordType () const;
  
  void SetAdminRecordFlag (AdminRecordFlag flag);
  AdminRecordFlag GetAdminRecordFlag () const;

  uint32_t GetSerializedSize () const;
  virtual void Serialize (uint8_t *buffer) const;
  static AdministrativeRecord Deserialize (uint8_t const*buffer);
  
protected:
  AdministrativeRecord (AdminRecordType type, AdminRecordFlag flag);
  AdministrativeRecord ();
  AdminRecordType m_type;
  AdminRecordFlag m_flag;
};

}} // namespace bundleProtocol, ns3

#endif /* BP_ADMINISTRATIVE_RECORD_H  */
