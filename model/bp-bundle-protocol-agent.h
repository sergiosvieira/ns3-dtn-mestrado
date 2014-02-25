/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BUNDLE_PROTOCOL_AGENT_H
#define BUNDLE_PROTOCOL_AGENT_H

#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"

#include "bp-registration.h"
#include "bp-registration-endpoint.h"
#include "bp-registration-manager.h"
#include "bp-bundle.h"
#include "bp-header.h"
#include "bp-neighbourhood-detection-agent.h"
#include "bp-convergence-layer-agent.h"
#include "bp-bundle-router.h"
#include "bp-link.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {
 
class Registration;
class NeighbourhoodDetectionAgent;

/**
 * \ingroup bundleProtocolAgent
 *
 * \brief The component of a DTN node that offers the bundle protocol services.
 *
 *
 */
class BundleProtocolAgent : public Object 
{
public:
  static TypeId GetTypeId (void);
  
  BundleProtocolAgent ();
  BundleProtocolAgent (BundleEndpointId defaultEndpointId,
                       Ptr<Node> node,
                       Ptr<ConvergenceLayerAgent> cla,
                       Ptr<BundleRouter> bundleRouter);
  virtual ~BundleProtocolAgent ();
  virtual void DoDispose ();

  // Initializes the bpa, setups its callbacks
  /**
   * \brief Initializes the bpa by creating it's callbacks.
   * 
   * This fuction should only be called after the convergence layer and the bundleRouter
   * has been assigned.
   */
  void Init ();
  /**
   * \brief Sets the BundleEndpointId of this node.
   * \param eid The EID to set.
   */
  void SetBundleEndpointId (BundleEndpointId eid);


  BundleEndpointId GetBundleEndpointId () const;

   /**
   * \brief Sets the node the BPA belongs to.
   * \param node The Node to assign.
   */
  void SetNode (Ptr<Node> node);
  
  void SetNumNode(int num);
  
  Ptr<Node> GetNode () const;

   /**
   * \brief Sets the ConvergenceLayerAgent used by this BPA.
   * \param cla The cla to use.
   */
  void SetConvergenceLayer (Ptr<ConvergenceLayerAgent> cla);

  Ptr<ConvergenceLayerAgent>  GetConvergenceLayerAgent () const;

   /**
   * \brief Sets the BundleRouter used by this BPA.
   * \param bundleRouter The BundleRouter to use.
   */
  void SetBundleRouter (Ptr<BundleRouter> bundleRouter);

  Ptr<BundleRouter> GetBundleRouter () const;

   /**
    * \brief A registration uses this method to send a bundle to another i.e insert a bundle in to the BPA.
    * \param adu The Application Data Unit (ADU) to be sent i.e the payload of the bundle.
    * \param destination The destination BundleEndpointId of the ADU.
    * \param reportTo The report to BundleEndpointId of the ADU. Optional and currently not used.
    * \param priority The priority of the ADU.
    * \param ttl The The time to live of the ADU.
    * \param flags The control flags to be used. Optional 
    * \return Returns a 1 if the bundle was 'sent' otherwise returns -1.
    */
  int BundleReceivedFromApplication (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority, Time ttl, const PrimaryProcessingControlFlags& flags);
  
  // Creates a new a registration, i.e a application to bpa connection
 /**
   * \brief Creates a new a registration.
   * \return Returns a new Registration.
   */
  Ptr<Registration> CreateRegistration (void);

  RegistrationEndpoint* Allocate ();
  RegistrationEndpoint* Allocate (const BundleEndpointId& eid);
  void DeAllocate (RegistrationEndpoint *regEndpoint);
 private:  

  // Called whenever an bundle is received
   /**
   * \brief Called by the convergence layer whenever an bundle is received from another node.
   * \param bundle The received bundle.
   */
  void BundleReceivedFromConvergenceLayer (Ptr<Bundle> bundle);

  // Creates an bundle from adu and header information
   /**
   * \brief Creates an bundle from adu and header information
   * \param uri The new URI of this EID.
   * \return Returns if the a valid uri otherwise false.
   */ 
  Ptr<Bundle> GenerateBundle (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority, Time ttl, const PrimaryProcessingControlFlags& flags);
 
 // Creates an custody signal
  void GenerateCustodySignal (Ptr<Bundle> bundle, const CustodySignalReason& reason, bool status);
  bool GenerateCustodySignal (Ptr<Bundle> bundle, bool status);

  // Called when a bundle transmission should be canceled
   /**
   * \brief Called when a bundle transmission should be cancelled.
   * \param bundle The bundle to cancel.
   */
  void CancelTransmission (GlobalBundleIdentifier gbid, BundleEndpointId eid);
  
  // Called from the convergence layer if a bundle transmission was cancelled
   /**
   * \brief Called from the convergence layer if a bundle transmission was cancelled.
   * \param mac The mac address of the destination node.
   * \param gbid The global bundle identifier of the bundle cancelled.
   * \return Returns if the a valid uri otherwise false.
   */
  void TransmissionCancelled  (const Mac48Address& mac, GlobalBundleIdentifier gbid);

  // Called from BundleReceived when data/status report is received
   /**
   * \brief Called from BundleReceivedFromConvergenceLayer when data/status report is received.
   * \param The bundle received.
   */
  void BundleReception (Ptr<Bundle> bundle);

  // Help wich checks if the Bundle node knowns the extension block format
  /**
   * \brief Checks if the BPA can understand this extention block type.
   * \param uri The block type to check.
   * \return Returns true if the BPA can understand it otherwise false. 
   */
  bool CanProcessBlock (const BlockType& block);

  // Determines if a bundle should be forwarded or delivered locally
   /**
   * \brief Determines if a bundle should be forwarded or delivered locally
   * \param bundle The bundle check
   */
  void BundleDispatching (Ptr<Bundle> bundle);
  
  // Delivers the bundle to the concerned registrations.
   /**
   * \brief Delivers the bundle to the concerned registrations.
   * \param bundle The bundle to deliver.
   */
  void LocalBundleDelivery (Ptr<Bundle> bundle);

  // Help function that performes the forwarding up to the registrations
   /**
   * \brief Help function that performes the forwarding up to the registrations.
   * \param bundle The bundle to forward up.
   */
  void ForwardUp (Ptr<Bundle> bundle);
  
  // Forwards/Sends the bundle to a node

   /**
   * \brief Performce the steps needed before a bundle is ready to be forwarded.
   * \param bundle The bundle to prepare for forward.
   */
  void BundleForwarding (Ptr<Bundle> bundle);

  // Help function checks if forwarding is contradicted.
  void ForwardingContradicted (Ptr<Bundle> bundle, const CustodySignalReason& reason);

  // Help function that checks if the reason for forward contradiction is a forwarding failure
  void ForwardingFailed (Ptr<Bundle> bundle, const CustodySignalReason& reason);

  // Called to delete a bundle
   /**
   * \brief Called to delete a bundle.
   * \param bundle The Bundle to delete
   * \param reason The reason for the deletion.
   */
  void BundleDeletion (Ptr<Bundle> bundle, const CustodySignalReason& reason);

  // Does the necessary things when a bundle is accepted custody 
  void CustodyAcceptance (Ptr<Bundle> bundle);
  
  // Releases custody of a bundle
  void CustodyRelease (Ptr<Bundle> bundle);

  // Called from BundleReceived when a cystody signal is received
  void CustodySignalReception (const CustodySignal& signal);
  
  // Does the necessary things to handle a successfull custody transfer
  void CustodyTransferSuccess (const CustodySignal& signal);
  
  // Does the necessart things to handle a custody transfer failure
  // either from a negative custody signal or a timer timeout
  void CustodyTransferFailure (const CustodySignal& signal, bool timeout);

  // Called by cla when a bundle has been transferred successfully
  /**
   * \brief Called by the convergence layer agent when a bundle has been transferred successfully.
   * \param mac The mac address of the destination node.
   * \param gbid The global bundle identifier of the sent bundle.
   * \return Returns if the a valid uri otherwise false.
   */
  void BundleSentOk (const Mac48Address& address, GlobalBundleIdentifier gbid, bool finalDelivery);
  // Called by cla when a bundle has been transferred unscuccessfully
  /**
   * \brief  Called by convergence layer agent when a bundle transfer fails.
   * \param mac The mac address of the destination node.
   * \param gbid The global bundle identifier of the sent bundle.
   * \return Returns if the a valid uri otherwise false.
   */
  void BundleSentFailed (const Mac48Address& address, GlobalBundleIdentifier gbid);

  // Gives a bundle to the convergence layer to send
  /**
   * \brief Gives a bundle to the convergence layer to send.
   * \param link The link to send the bundle over.
   * \param The bundle to send.
   * \return Returns if the a valid uri otherwise false.
   */
  void SendBundle (Ptr<Link> link, Ptr<Bundle> bundle);

  Ptr<Node> m_node;
  Ptr<ConvergenceLayerAgent> m_cla;
  Ptr<BundleRouter> m_bundleRouter;
  int m_num_nodes;
  BundleEndpointId m_eid;
  RegistrationManager *m_registrationManager;

  TracedCallback<Ptr<const Bundle> > m_createLogger;
  TracedCallback<Ptr<const Bundle> > m_relayLogger;
  TracedCallback<Ptr<const Bundle> > m_deliveryLogger;
  TracedCallback<Ptr<const Bundle> > m_routerDeliveryLogger;
};

}} // namespace bundleProtocol, ns3

#endif /* BUNDLE_PROTOCOL_AGENT_H */
