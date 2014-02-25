/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_ORWAR_ROUTER_CHANGED_ORDER_H
#define BP_ORWAR_ROUTER_CHANGED_ORDER_H

#include <deque>

#include "ns3/traced-callback.h"
#include "ns3/timer.h"

#include "bp-bundle-router.h"
#include "bp-known-delivered-messages.h"
#include "bp-contact-window-information.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

/**
 * \ingroup bundleRouter
 *
 * \brief Implements ORWAR routing.
 *
 *
 */

class OrwarRouterChangedOrder : public BundleRouter
{
 public:
  static TypeId GetTypeId (void);
  
  OrwarRouterChangedOrder();
  ~OrwarRouterChangedOrder();
  void ReceiveRouterSpecific (Ptr<Bundle> bundle);
  void SentRouterSpecific (Ptr<Link> link, const GlobalBundleIdentifier& gbid);
protected:
   virtual void DoDispose ();
private:

  // Abstract functions inherited from BundleRouter
  void DoInit ();

  void DoBundleDelivered (Ptr<Bundle> bundle, bool fromAck);
  void DoBundleReceived (Ptr<Bundle> bundle);
  Ptr<Bundle> DoSendBundle (Ptr<Link> link, Ptr<Bundle> bundle);
  void SendRouterSpecific (Ptr<Link> link, Ptr<Bundle> bundle);
  void AddRouterSpecificBundle (Ptr<Bundle> bundle);
  void RemoveRouterSpecificBundle (const GlobalBundleIdentifier& gbid, uint8_t reason);
  void RemoveRouterSpecificBundles (Ptr<Link> link);
  bool HasRouterSpecificBundle (const GlobalBundleIdentifier& gbid);
  Ptr<Bundle> GetRouterSpecificBundle (const GlobalBundleIdentifier& gbid);

  bool DoIsRouterSpecific (Ptr<Bundle> bundle);
  bool DoIsRouterSpecific (const BlockType& block);
  void DoLinkDiscovered (Ptr<Link> link);
  void DoLinkClosed (Ptr<Link> link);

  bool DoAcceptBundle (Ptr<Bundle> bundle, bool fromApplication);
  bool DoCanDeleteBundle (const GlobalBundleIdentifier& gbid);
  void DoInsert (Ptr<Bundle> bundle);
  bool MakeRoomForBundle (Ptr<Bundle> bundle);
  bool DoDelete (const GlobalBundleIdentifier& gbid, bool drop);
  bool CanMakeRoomForBundle (Ptr<Bundle> bundle);
  void DoCancelTransmission (Ptr<Bundle> bundle, Ptr<Link> link);
  void DoTransmissionCancelled (const Address& address, const GlobalBundleIdentifier& gbid);
  uint8_t DoCalculateReplicationFactor (const BundlePriority& priority) const;
  void DoBundleSent (const Address& address, const GlobalBundleIdentifier& gbid, bool);
  void DoBundleTransmissionFailed (const Address& address, const GlobalBundleIdentifier& gbid);  
  
  Ptr<Link> DoCreateLink (const BundleEndpointId& eid, const Address& address);

  LinkBundleList GetAllDeliverableBundles ();
  LinkBundleList GetAllBundlesForLink (Ptr<Link> link);
  LinkBundleList GetAllBundlesToAllLinks ();

  LinkBundle GetNextRouterSpecific ();

  // Orwar specific
  void TryToStartSending ();
  LinkBundle FindNextToSend ();

  uint32_t EstimateNeededBytes (Ptr<Bundle> bundle) const;

  void RemoveDeliveredBundles (const KnownDeliveredMessages& kdm);

  void SendCwi (Ptr<Link> link, bool isInitiator);
  void SendKdm (Ptr<Link> link, bool isInitiator);
 
  void SentCwi (Ptr<Link> link);
  void SentKdm (Ptr<Link> link);

  void HandleCwi (Ptr<Bundle> bundle);
  void HandleKdm (Ptr<Bundle> bundle);

  Ptr<Bundle> GenerateCwi (Ptr<Link> link, bool isInitiator);
  Ptr<Bundle> GenerateKdm (Ptr<Link> link, bool isInitiator);

  void CalculateContactWindow (Ptr<Link> link, const ContactWindowInformation& cwi);

  void PauseLink (Ptr<Link> link);
  void UnPauseLink (Ptr<Link> link);

  uint8_t m_replicationFactor;
  uint8_t m_deltaReplicationFactor;
  bool m_estimateCw;
  bool m_alwaysSendHello;
  Time m_pauseTime;
  uint32_t m_maxRetries;

  KnownDeliveredMessages m_kdm;
    
  TracedCallback<Ptr<const Bundle> > m_createRouterLogger;
  TracedCallback<Ptr<const Bundle> > m_redundantRelayLogger;
  TracedCallback<uint32_t> m_contactSetupLogger;
  TracedCallback<uint32_t> m_outOfSynchLogger;
  TracedCallback<uint32_t> m_contactSetupFailedLogger;
  TracedCallback<uint32_t, uint32_t, Time, bool> m_estimatedCwLogger;

  TracedCallback<uint32_t> m_contactOppLogger;
  TracedCallback<uint32_t,uint32_t> m_contactOppBetweenLogger;
  TracedCallback<Ptr<const Bundle>, uint8_t> m_routerDeleteLogger;
  
struct UtilityPerBitCompare
{
  bool operator() (Ptr<Bundle> left, Ptr<Bundle> right) const
  {
    PrimaryBundleHeader leftHeader = left->GetPrimaryHeader ();
    PrimaryBundleHeader rightHeader = right->GetPrimaryHeader ();
    
    double leftPriority =  leftHeader.GetPriority () / (double) left->GetSize ();
    double rightPriority = rightHeader.GetPriority () / (double) right->GetSize ();
    return leftPriority > rightPriority;
  }
};

struct UtilityPerBitCompare2
{
  bool operator() (const LinkBundle& left, const LinkBundle& right) const
  {
    PrimaryBundleHeader leftHeader = left.GetBundle ()->GetPrimaryHeader ();
    PrimaryBundleHeader rightHeader = right.GetBundle ()->GetPrimaryHeader ();
    
    double leftPriority =  leftHeader.GetPriority () / (double) left.GetBundle ()->GetSize ();
    double rightPriority = rightHeader.GetPriority () / (double) right.GetBundle ()->GetSize ();
    return leftPriority > rightPriority;
  }
};

struct EqualEids : public std::unary_function<Ptr<Bundle>, bool>
{
  BundleEndpointId m_eid;
  
  EqualEids (const BundleEndpointId& eid)      
    : m_eid (eid)
  {}
  
  bool operator() (const Ptr<Bundle>& other)
  {
    BundleEndpointId otherEid = other->GetDestinationEndpoint ();
    return m_eid == otherEid;
  }
};

};


}} // namespace bundleProtocol, ns3

#endif /* BP_ORWAR_ROUTER_CHANGED_ORDER_H */
