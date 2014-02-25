/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "bp-administrative-record.h"
#include "bp-sdnv.h"
#include "bp-header.h"

namespace ns3 {
namespace bundleProtocol {

AdministrativeRecord::AdministrativeRecord (Ptr<Bundle> bundle, AdminRecordType type)
  : m_type (type), m_flag (RECORD_IS_NOT_FOR_A_FRAGMENT)
{
  PrimaryBundleHeader header = bundle->GetPrimaryHeader ();
  if (header.IsFragment ())
    {
      m_flag = RECORD_IS_FOR_A_FRAGMENT;
    }
}

AdministrativeRecord::AdministrativeRecord (AdminRecordType type, AdminRecordFlag flag) 
  : m_type (type), m_flag (flag)
{}

AdministrativeRecord::AdministrativeRecord ()
  : m_type (UNKOWN_TYPE), m_flag (UNKOWN_FLAG)
{}

AdministrativeRecord::~AdministrativeRecord ()
{}

void
AdministrativeRecord::SetRecordType (AdminRecordType type)
{
  m_type = type;
}

AdminRecordType
AdministrativeRecord::GetRecordType () const
{
  return m_type;
}
  
void
AdministrativeRecord::SetAdminRecordFlag (AdminRecordFlag flag)
{
  m_flag = flag;
}

AdminRecordFlag
AdministrativeRecord::GetAdminRecordFlag () const
{
  return m_flag;
}

uint32_t
AdministrativeRecord::GetSerializedSize () const
{
  uint32_t size = 0;
  size += Sdnv::EncodingLength (m_type);
  size += Sdnv::EncodingLength (m_flag);
  return size;
}

void
AdministrativeRecord::Serialize (uint8_t *buffer) const
{
  uint32_t i = 0;
  Sdnv::Encode (m_type, buffer+i);
  i+= Sdnv::EncodingLength (m_type);
  Sdnv::Encode (m_flag, buffer+i);
}

AdministrativeRecord
AdministrativeRecord::Deserialize (uint8_t const*buffer)
{
  uint32_t i = 0;
  AdminRecordType type = (AdminRecordType) Sdnv::Decode (buffer+i);
  i+= Sdnv::EncodingLength (type);
  AdminRecordFlag flag = (AdminRecordFlag) Sdnv::Decode (buffer+i);

  return AdministrativeRecord (type,flag);
}

}} // namespace bundleProtocol, ns3
