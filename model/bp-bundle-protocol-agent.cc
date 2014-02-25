/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <sstream>
#include <algorithm>

#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "ns3/log.h"
#include "ns3/trace-source-accessor.h"

#include "bp-bundle-protocol-agent.h"
#include "bp-sdnv.h"

NS_LOG_COMPONENT_DEFINE ("BundleProtocolAgent");
namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED (BundleProtocolAgent);

TypeId
BundleProtocolAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::BundleProtocolAgent")
    .SetParent<Object> ()
    .AddConstructor<BundleProtocolAgent> ()
    .AddAttribute ("BundleEndpointId",
                   "Sets the default bundle endpoint id.",
                   BundleEndpointIdValue (BundleEndpointId ()),
                   MakeBundleEndpointIdAccessor (&BundleProtocolAgent::m_eid),
                   MakeBundleEndpointIdChecker ())
    .AddTraceSource ("Created", "Created a new bundle",
                     MakeTraceSourceAccessor (&BundleProtocolAgent::m_createLogger))
    .AddTraceSource ("Relayed", "A relayed message have been received.",
                     MakeTraceSourceAccessor (&BundleProtocolAgent::m_relayLogger))
    .AddTraceSource ("Delivered", "A received message have been delivered",
                     MakeTraceSourceAccessor (&BundleProtocolAgent::m_deliveryLogger))
    .AddTraceSource ("RouterDelivered", "A router message have been delivered",
                     MakeTraceSourceAccessor (&BundleProtocolAgent::m_routerDeliveryLogger))
    ;
  return tid;
}

BundleProtocolAgent::BundleProtocolAgent ()
  : m_node (),
    m_cla (),
    m_bundleRouter (),
    m_registrationManager (new RegistrationManager ())
{}


BundleProtocolAgent::BundleProtocolAgent (BundleEndpointId defaultEndpointId, 
                                          Ptr<Node> node,
                                          Ptr<ConvergenceLayerAgent> cla,
                                          Ptr<BundleRouter> bundleRouter)
  : m_node (node), 
    m_cla (cla),
    m_bundleRouter (bundleRouter),
    m_eid (defaultEndpointId),
    m_registrationManager (new RegistrationManager ())
{}

BundleProtocolAgent::~BundleProtocolAgent ()
{}

void
BundleProtocolAgent::DoDispose ()
{
  m_cla = 0;
  m_bundleRouter = 0;
  if (m_registrationManager != 0)
    {
      delete m_registrationManager;
      m_registrationManager = 0;
    }
  m_node = 0;
  Object::DoDispose ();
}

void 
BundleProtocolAgent::Init ()
{
  m_cla->SetBundleReceivedCallback (MakeCallback (&BundleProtocolAgent::BundleReceivedFromConvergenceLayer, this));
  m_cla->SetBundleSentOkCallback (MakeCallback (&BundleProtocolAgent::BundleSentOk, this));
  m_cla->SetBundleSentFailedCallback (MakeCallback (&BundleProtocolAgent::BundleSentFailed, this));
  m_cla->SetTransmissionCancelledCallback (MakeCallback (&BundleProtocolAgent::TransmissionCancelled, this));
  m_bundleRouter->SetBundleSendCallback (MakeCallback (&BundleProtocolAgent::SendBundle, this));
  m_bundleRouter->SetCancelTransmisisonCallback (MakeCallback (&BundleProtocolAgent::CancelTransmission, this));
}


void BundleProtocolAgent::SetNumNode(int num){
        m_num_nodes = num;
}

void
BundleProtocolAgent::SetBundleEndpointId (BundleEndpointId eid)
{
  m_eid = eid;
}

BundleEndpointId
BundleProtocolAgent::GetBundleEndpointId () const
{
  return m_eid;
}

void
BundleProtocolAgent::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node>
BundleProtocolAgent::GetNode () const
{
  return m_node;
}

void
BundleProtocolAgent::SetConvergenceLayer (Ptr<ConvergenceLayerAgent> cla)
{
  m_cla = cla;
}

Ptr<ConvergenceLayerAgent>
BundleProtocolAgent::GetConvergenceLayerAgent () const
{
  return m_cla;
}

void
BundleProtocolAgent::SetBundleRouter (Ptr<BundleRouter> bundleRouter)
{
  m_bundleRouter = bundleRouter;
}

Ptr<BundleRouter>
BundleProtocolAgent::GetBundleRouter () const
{
  return m_bundleRouter;
}

// 5.2 Bundle Transmission
int
BundleProtocolAgent::BundleReceivedFromApplication (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority,  Time ttl, const PrimaryProcessingControlFlags& flags)
{
  NS_LOG_DEBUG ( "(" << m_node->GetId () << ") " << "BundleProtocolAgent::BundleReceivedFromApplication ");
  Ptr<Bundle> bundle = GenerateBundle (adu, destination, priority, ttl, flags);

  /*Joao 4 Copias Valor Inicial para SprayAndWait*/
  int num = (m_num_nodes * 0.1) + 1;
  bundle->SetReplicationFactor(num);
  /*Joao*/
  m_createLogger (bundle);
  if (m_bundleRouter->AcceptBundle (bundle, true))
    {
      if (bundle->IsCustodyTransferRequested ())
        {
    	  //NS_LOG_DEBUG("2-> IsCustodyTransferRequested");
          CustodySignalReason reason;
          if (m_bundleRouter->AcceptCustody (bundle, reason))
            {

              CustodyAcceptance (bundle);
            }
          else 
            {
              return -1;
            }
        }
   
      // Step 2:
      bundle->AddRetentionConstraint (RC_DISPATCH_PENDING);

      m_bundleRouter->InsertBundle (bundle);

      BundleForwarding (bundle);
      return 1;
    }
  else
    {
      return -1;
    }
}

void 
BundleProtocolAgent::BundleReceivedFromConvergenceLayer (Ptr<Bundle> bundle)
{
  //std::cout<<"Received From: ("<<bundle->GetCustodianEndpoint().GetId()<<")"<<" To me ("<< m_node->GetId ()<<") Bundle Id *"<<bundle->GetGlobalId()<<"*\n";
  if(bundle->GetReplicationFactor() == 20)/*Indica que é uma resposta de custódia*/
  {
	  if(bundle->GetReportCustodyAcceptance()) /*Custódia aceita, pode apagar o bundle*/
	  {
		  NS_LOG_DEBUG("(" << m_node->GetId () << ") " << "-> Custodia Aceita " << bundle->GetBundleId());
		  m_bundleRouter->DeleteBundle(bundle->GetCustody(),false);
	  }
	  else
	  {
		  NS_LOG_DEBUG("(" << m_node->GetId () << ") " << "-> Custodia Não Aceita ");
		  m_bundleRouter->EraseCustodyHistoricalPending(bundle->GetCustody());
	  }
  }
  else if (m_bundleRouter->isBundleDelivery(bundle))/*Nesse caso esse no ja tem o bundle, então ele denvolve apenas um sinal de custodia aceita para no requisitante apagar sua copia*/
  {
	if(bundle->IsCustodyTransferRequested())
	{
		Ptr<Link> link = this->m_bundleRouter->GetLinkManager()->FindLink(bundle->GetCustodianEndpoint());
		if(link)
		{
			Ptr<Bundle> bundleS = bundle->Copy();//GenerateBundle (Create<Packet> (1), bundle->GetCustodianEndpoint(), bundleProtocol::EXPEDITED,Seconds(100), bundleProtocol::PrimaryProcessingControlFlags ());
			bundleS->SetReportCustodyAcceptance(true);
			bundleS->SetReplicationFactor(20);
			bundleS->SetCustody(bundle->GetBundleId());
			m_bundleRouter->SendBundle(link,bundleS);
		}
	}
  }
  else if (!m_bundleRouter->isBundleCustody(bundle))
  {
		if (!m_bundleRouter->IsRouterSpecific (bundle))
		{
			m_relayLogger (bundle);
		}
	    else
		{
		  m_routerDeliveryLogger (bundle);
		}
	    if (m_bundleRouter->AcceptBundle (bundle))
		{
			bundle->AddReceivedFrom(m_node->GetId());
                        m_bundleRouter->AddToList(m_node->GetId(), bundle->GetGlobalId());     
	    	  //std::cout<<"Received From: ("<<bundle->GetCustodianEndpoint().GetId()<<")"<<" To me ("<< m_node->GetId ()<<") Bundle Id *"<<bundle->GetGlobalId()<<"* Aceito "<<Simulator::Now().GetSeconds()<<"\n ";
	    	m_bundleRouter->BundleReceived (bundle);

		  if (m_bundleRouter->IsRouterSpecific (bundle))
		  {
			  m_bundleRouter->ReceiveRouterSpecific (bundle);
			  return;
		  }
		  NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "BundleProtocolAgent::BundleReceivedFromConvergenceLayer Intermediario (" << bundle->GetPayload ()->GetSize () << ") with source eid " << bundle->GetSourceEndpoint () << " and custodian eid: " << bundle->GetCustodianEndpoint () << " and destination eid: " << bundle->GetDestinationEndpoint () << " Bundle ID: " << bundle->GetGlobalId()<<"do");
		  /*Para saber quando chegou no destino*/
		  if(m_node->GetId() == bundle->GetDestinationEndpoint().GetId() )
		  {
			NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "BundleProtocolAgent::BundleReceivedFromConvergenceLayer Destino (" << bundle->GetPayload ()->GetSize () << ") with source eid " << bundle->GetSourceEndpoint () << " and custodian eid: " << bundle->GetCustodianEndpoint () << " and destination eid: " << bundle->GetDestinationEndpoint () << " Bundle ID: "<<bundle->GetGlobalId()<<"do");
			m_bundleRouter->InsertBundleDelivery(bundle);
			NS_LOG_DEBUG("Pacote Mandado para Aplicacao");
			//return;
		  }
		  //cout << Simulator::Now ().GetSeconds () << " " << "(" << m_node->GetId () << ") " << "%%%%%%%%%%%%%%%%%%%%%% BundleProtocolAgent::BundleReceivedFromConvergenceLayer (" << bundle->GetPayload ()->GetSize () << ") with source eid " << bundle->GetSourceEndpoint () << " and custodian eid: " << bundle->GetCustodianEndpoint () << " and destination eid: " << bundle->GetDestinationEndpoint () << endl;
		  if (bundle->IsAdministrativeRecord ())
			{
			//  NS_LOG_DEBUG("-> IsAdministrativeRecord ()");
			  Ptr<Packet> payload;
			  /*Essa linha provavelmente foi esquecida*/
			  payload = bundle->GetPayload();
			  /*****************************************/
			  uint8_t buffer[payload->GetSize ()];
			  payload->CopyData (buffer,payload->GetSize ());
			  AdministrativeRecord adminRecord = AdministrativeRecord::Deserialize (buffer);
			  // The other option for a administrative record is an bundle status report
			  // but these are only of interest for the application?
			  if (adminRecord.GetRecordType () == CUSTODY_SIGNAL)
			   {
				 // NS_LOG_DEBUG("	-> CUSTODY_SIGNAL");
				  CustodySignal custodySignal = CustodySignal::Deserialize (buffer);
				  custodySignal.SetCustodyTransferSucceeded(true);
				  CustodySignalReception (custodySignal);
				  bundle->AddRetentionConstraint(RC_CUSTODY_ACCEPTED);
				  //BundleReception (bundle);
			   }
			  else
			  {
				 // NS_LOG_DEBUG("	-> RECEPTION 1");
				  BundleReception (bundle);
			  }
			}
		  else
			{
			 // NS_LOG_DEBUG("	-> RECEPTION 2");
			  BundleReception (bundle);
			}
		}
  }
}

Ptr<Bundle>
BundleProtocolAgent::GenerateBundle (Ptr<Packet> adu, const BundleEndpointId& destination, BundlePriority priority,  Time ttl, const PrimaryProcessingControlFlags& flags)
{
  PrimaryBundleHeader primaryHeader = PrimaryBundleHeader ();
  CanonicalBundleHeader canonicalHeader = CanonicalBundleHeader (PAYLOAD_BLOCK);

  //cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << " BundleProtocolAgent::GenerateBundle (" << adu->GetSize () << ")" << endl;

  // Setup primary bundle header
  primaryHeader.SetProcessingControlFlags (flags);
  primaryHeader.SetPriority (priority);
  primaryHeader.SetDestinationEndpoint (destination);
  primaryHeader.SetSourceEndpoint (m_eid);
  primaryHeader.SetCustodianEndpoint (m_eid);
  primaryHeader.SetReplicationFactor (m_bundleRouter->CalculateReplicationFactor (priority));
  primaryHeader.SetCreationTimestamp (CreationTimestamp ());
  primaryHeader.SetLifetime (ttl);
  primaryHeader.SetCustodyTransferRequested(false);
  primaryHeader.SetReportCustodyAcceptance(false);
  /* vai ficar com a posição do destino fixa, pra não perder tempo */
  /* sergiosvieira */
  primaryHeader.SetPos(1500.0, 500.0);
  /* sergiosvieira */
  // Setup the canoncial bundle header
  canonicalHeader.SetMustBeReplicated (false);
  canonicalHeader.SetStatusReport (false);
  canonicalHeader.SetDeleteBundle (false);
  canonicalHeader.SetLastBlock (true);
  canonicalHeader.SetDiscardBlock (true);
  canonicalHeader.SetForwarded (false);
  canonicalHeader.SetContainsEid (false);
  canonicalHeader.SetBlockLength (adu->GetSize ());

  Ptr<Bundle> result = Create<Bundle> ();

  result->SetPayload (adu);
  result->SetPrimaryHeader (primaryHeader);
  result->AddCanonicalHeader (canonicalHeader);
  result->GenerateID();
  result->AddReceivedFrom(m_node->GetId());
  m_bundleRouter->AddToList(m_node->GetId(), result->GetGlobalId());            
  return result;
}

void
BundleProtocolAgent::GenerateCustodySignal (Ptr<Bundle> bundle, const CustodySignalReason& reason, bool status)
{}

bool
BundleProtocolAgent::GenerateCustodySignal (Ptr<Bundle> bundle, bool status)
{
	NS_LOG_DEBUG(m_node->GetId() << " GenerateCustodySignal");
	/*Checa se há espaço no buffer para receber o bundle e se o bundle ja foi transferido por custodia*/
	if(m_bundleRouter->GetFreeBytes() > bundle->GetSize() && !m_bundleRouter->isBundleCustody(bundle->GetBundleId()))
	{

			Ptr<Link> link = this->m_bundleRouter->GetLinkManager()->FindLink(bundle->GetCustodianEndpoint());
		if(link)
		{
			Ptr<Bundle> bundleS = bundle->Copy();//GenerateBundle (Create<Packet> (1), bundle->GetCustodianEndpoint(), bundleProtocol::EXPEDITED,Seconds(100), bundleProtocol::PrimaryProcessingControlFlags ());
			bundleS->SetReportCustodyAcceptance(true);
			bundleS->SetReplicationFactor(20);
			bundleS->SetCustody(bundle->GetBundleId());
			NS_LOG_DEBUG("Aceita " << bundleS->GetCustody());
			m_bundleRouter->SendBundle(link,bundleS);
			NS_LOG_DEBUG(m_node->GetId() << " GenerateCustodySignal Positive Enviar");
			return true;
		}
	}
	else
	{
		Ptr<Link> link = this->m_bundleRouter->GetLinkManager()->FindLink(bundle->GetCustodianEndpoint());
		if(link)
		{
			Ptr<Bundle> bundleS = bundle->Copy();//GenerateBundle (Create<Packet> (1), bundle->GetCustodianEndpoint(), bundleProtocol::EXPEDITED,Seconds(100), bundleProtocol::PrimaryProcessingControlFlags ());
			bundleS->SetReportCustodyAcceptance(false);
			bundleS->SetReplicationFactor(20);
			bundleS->SetCustody(bundle->GetBundleId());
			//Simulator::ScheduleNow (&BundleRouter::SendBundle, m_bundleRouter, link, bundleS);
			NS_LOG_DEBUG(m_node->GetId() << " GenerateCustodySignal Negative Enviar");
			m_bundleRouter->SendBundle(link,bundleS);
		}
		return false;
	}
	NS_LOG_DEBUG(m_node->GetId()<<"->" <<bundle->GetCustodianEndpoint()  << " Link quebrado");
	return true;
}

void 
BundleProtocolAgent::CancelTransmission (GlobalBundleIdentifier gbid, BundleEndpointId eid)
{
  m_cla->CancelTransmission (gbid, eid);
}

void 
BundleProtocolAgent::TransmissionCancelled  (const Mac48Address& mac, GlobalBundleIdentifier gbid)
{
  m_bundleRouter->TransmissionCancelled (mac, gbid);
}

// 5.3 Bundle dispatching
void
BundleProtocolAgent::BundleDispatching (Ptr<Bundle> bundle)
{

  if (m_registrationManager->LookupLocal (bundle->GetDestinationEndpoint ()))
    {
	  NS_LOG_DEBUG("Here");
      LocalBundleDelivery (bundle);
    }
  else
    {
      if (m_bundleRouter->InsertBundle (bundle))
        {
          if (bundle->IsCustodyTransferRequested ())
            {
              CustodyAcceptance (bundle);
            }
          BundleForwarding (bundle);
        }
    }
}

// 5.4 Bundle Forwarding
void 
BundleProtocolAgent::BundleForwarding (Ptr<Bundle> bundle)
{
  // Step 1:
  bundle->AddRetentionConstraint  (RC_FORWARDING_PENDING);
  bundle->RemoveRetentionConstraint (RC_DISPATCH_PENDING);

  // Step 2:
  CustodySignalReason reason;
  if (m_bundleRouter->IsForwardingContradicted (bundle, reason))
    {
      // Step 3:
      ForwardingContradicted (bundle, reason);
      return;
    }
  
  // Step 4:
  if (bundle->IsCustodyTransferRequested ())
    {
      CustodyRelease (bundle);
    }

  // Step 5: Is taken care of by the router.

  // Step 6: Is taken care of in the BundleSent* () functions
}

void
BundleProtocolAgent::BundleSentOk (const Mac48Address& address, GlobalBundleIdentifier gbid, bool finalDelivery)
{
  Ptr<Bundle> bundle = m_bundleRouter->GetBundle (gbid);
  if (bundle != 0)
    {
      //  Step 6: from 5.4 
      if (bundle->GetReportBundleForwarding ())
        {
          // a bundle forwarding status report should be generated
        }
    }
      
  m_bundleRouter->BundleSent (address, gbid, finalDelivery);
}

void
BundleProtocolAgent::BundleSentFailed (const Mac48Address& address, GlobalBundleIdentifier gbid)
{
  m_bundleRouter->BundleTransmissionFailed (address, gbid);
}


// 5.4.1 Forwarding Contradicted
void
BundleProtocolAgent::ForwardingContradicted (Ptr<Bundle> bundle, const CustodySignalReason& reason)
{
  // Step 1:

  // Step 2:
  if (m_bundleRouter->IsForwardingFailure (reason))
    {
      ForwardingFailed (bundle, reason);
    }
  else 
    {
      // At some future time, when forwarding is not contradicted the bpa should try again
    }
}

// 5.4.2 Forwarding Failed
void
BundleProtocolAgent::ForwardingFailed (Ptr<Bundle> bundle, const CustodySignalReason& reason)
{
  // Step 1:
  if (bundle->IsCustodyTransferRequested ())
    {
      GenerateCustodySignal (bundle, reason, false);
    }
  
  if (m_registrationManager->LookupLocal (bundle->GetDestinationEndpoint ()))
    {
      bundle->RemoveRetentionConstraint (RC_FORWARDING_PENDING);
    }
  else
    {
      BundleDeletion (bundle, reason);
    }
}

// 5.6 Bundle Reception
void
BundleProtocolAgent::BundleReception (Ptr<Bundle> bundle)
{
  //NS_LOG_DEBUG("BundleReception");
  if (bundle->IsCustodyTransferRequested ())
    {
		  //NS_LOG_DEBUG("-> IsCustodyTransferRequested ()");
//		  CustodySignalReason reason;
		  //if (m_bundleRouter->AcceptCustody (bundle, reason))
		  	if (bundle->IsCustodyTransferRequested())
			{
			 // NS_LOG_DEBUG("-> Not Accepted Custody ()");
			  if(!GenerateCustodySignal (bundle,false))
				  return;
			}
		  else{
			  //NS_LOG_DEBUG("-> Accepted Custody");
		  }
    } else {
    	//NS_LOG_DEBUG("-> NOT IsCustodyTransferRequested ()");
    }

  // Step 1:
  bundle->AddRetentionConstraint (RC_DISPATCH_PENDING);
          
  // Step 2:
  if (bundle->GetReportBundleReception ())
    {
	 // NS_LOG_DEBUG("-> GetReportBundleReception");
      // Generate a bundle reception status report, with reason code "No additional information"
    }
          
  // Step 3:
  BlockList canonicalBlocks = bundle->GetCanonicalHeaders ();
//  BlockList::iterator block = canonicalBlocks.begin ();
  BlockList tmp;
  
  for (BlockList::iterator block = canonicalBlocks.begin (); block != canonicalBlocks.end (); ++block)
    {
      if (!CanProcessBlock (block->GetBlockType ())) // if the bpa cannot process/understand the extension block type
        {
          if (block->IsStatusReport ())
            {
        	 // NS_LOG_DEBUG("-> IsStatusReport ()");
              //  reception status report with reason code "Block unintelligible"
              //  should be generated
            }
                  
          if (block->IsDeleteBundle ())
            {
        	//  NS_LOG_DEBUG("-> IsDeleteBundle ()");
              BundleDeletion (bundle, CUSTODY_BLOCK_UNINTELLIGIBLE);
              return;          
            }
          else if (block->IsDiscardBlock ())
            {
        	///  NS_LOG_DEBUG("-> IsDiscardBlock ()");
              CanonicalBundleHeader header = *block;
              tmp.push_back (header);
            }
          else 
            {
        	//  NS_LOG_DEBUG("-> block->SetForwarded (true);");
              block->SetForwarded (true);
            }
        }
       // NS_LOG_DEBUG("CanProcessBlock");
    }
  
  bundle->SetCanonicalHeaders (canonicalBlocks);

  // Step 4:
  if (bundle->IsCustodyTransferRequested ())
    {
	 // NS_LOG_DEBUG("bundle->IsCustodyTransferRequested ()");
      if (m_bundleRouter->HasBundle (bundle))
        {
    	 // NS_LOG_DEBUG("m_bundleRouter->HasBundle (bundle)");
          Ptr<Bundle> otherBundle = m_bundleRouter->GetBundle (bundle);
          
          
          if (otherBundle != 0 && otherBundle->HasRetentionConstraint (RC_CUSTODY_ACCEPTED))
            {
        	//  NS_LOG_DEBUG("otherBundle != 0 && otherBundle->HasRetentionConstraint (RC_CUSTODY_ACCEPTED)");
              GenerateCustodySignal (bundle, CUSTODY_REDUNDANT_RECEPTION, false);
              bundle->RemoveRetentionConstraint (RC_DISPATCH_PENDING);
 
              //Simulator::ScheduleNow (&BundleProtocolAgent::BundleDeletion, this, bundle, CUSTODY_REDUNDANT_RECEPTION);
              return;
            }
        }
    }
   // Step 5:
  BundleDispatching (bundle);
 }      


bool 
BundleProtocolAgent::CanProcessBlock (const BlockType& block)
{
  //NS_LOG_DEBUG ( "(" << m_node->GetId () << ") " << "BundleProtocolAgent::CanProcessBlock");
  return block == PAYLOAD_BLOCK || m_bundleRouter->IsRouterSpecific (block);
}

// 5.7 Local Bundle Delivery
void
BundleProtocolAgent::LocalBundleDelivery (Ptr<Bundle> bundle)
{
  // Step 1: No bundle fragmentation implemented making this step, unnecessary

  // Step 2: Registration always in active state
  ForwardUp (bundle);

  // Step 3:
  if (bundle->GetReportBundleDelivery ())
    {
      // Generate a bundle delivery status report
    }
  
  if (bundle->IsCustodyTransferRequested ())
    {
      //GenerateCustodySignal (bundle, true);
    }
}

void
BundleProtocolAgent::ForwardUp (Ptr<Bundle> bundle)
{
  m_deliveryLogger (bundle);
  BundleEndpointId address = bundle->GetSourceEndpoint ();
  Endpoints endpoints = m_registrationManager->Lookup (bundle->GetDestinationEndpoint ());
  Ptr<Packet> adu = bundle->GetPayload (); 
  for (EndpointsI endpoint = endpoints.begin (); endpoint != endpoints.end (); ++endpoint)
    {
      (*endpoint)->ForwardUp (adu->Copy (), address);
    }
  m_bundleRouter->BundleDelivered (bundle, false);
}

// 5.10.1 Custody Acceptance
void BundleProtocolAgent::CustodyAcceptance (Ptr<Bundle> bundle)
{
	//NS_LOG_DEBUG ( "CustodyAcceptance() " << m_node->GetId());
	bundle->AddRetentionConstraint(RC_CUSTODY_ACCEPTED);
	if (bundle->GetPrimaryHeader().GetReportCustodyAcceptance()) {
		// Generate a custody acceptance report
		bundle->SetAdministrativeRecord(true);
		bundle->SetReportCustodyAcceptance(true);

	}


	/*Joao*/
	/*
	Ptr<Bundle> bundleSucess = bundle->Copy();

	bundleSucess->SetAdministrativeRecord(true);
	bundleSucess->SetDestinationEndpoint(bundle->GetCustodianEndpoint());

	Ptr<Link> link = this->m_bundleRouter->GetLinkManager()->FindLink(bundle->GetCustodianEndpoint());

	if (link != 0)
	{
		NS_LOG_DEBUG("SendBundleSucess");
		this->SendBundle(link,bundleSucess);
	}
	else
	{
		NS_LOG_DEBUG("SendBundleFail");
	}
	*/
	//bundleSucess->
	/*AdministrativeRecord adm(bundleSucess,CUSTODY_SIGNAL);
	CustodySignal c*/
	/*Joao*/

	//GenerateCustodySignal(bundle, true); /*Sinal Succeded deve ser enviado de volta rfc5050*/

	bundle->SetCustodianEndpoint(m_eid);

	m_bundleRouter->HandleCustodyAcceptance(bundle);
}

// 5.10.2 Custody Release
void
BundleProtocolAgent::CustodyRelease (Ptr<Bundle> bundle)
{
	//NS_LOG_DEBUG("CustodyRelease()");
  //  bundle->RemoveRetentionConstraint (RC_CUSTODY_ACCEPTED);
  // Remove the custody transfer timer that has been established for this bundle
  
  m_bundleRouter->HandleCustodyRelease (bundle);
}

// 5.11 Custody Transfer Success
void 
BundleProtocolAgent::CustodyTransferSuccess (const CustodySignal& signal)
{
  //NS_LOG_DEBUG("CustodyTransferSuccess()");
  GlobalBundleIdentifier gbid (signal.GetSourceBundleEndpointId (), signal.GetCreationTimestamp ());
  Ptr<Bundle> bundle = m_bundleRouter->GetBundle (gbid);

  if (bundle == 0)
    return;

  CustodyRelease (bundle);
}

// 5.12 Custody Transfer Failed
void
BundleProtocolAgent::CustodyTransferFailure (const CustodySignal& signal, bool timeout)
{
 // NS_LOG_DEBUG("CustodyTransferFailure()");
  m_bundleRouter->HandleCustodyTransferFailure (signal,timeout);
}

// 5.13 Bundle Deletion
void 
BundleProtocolAgent::BundleDeletion (Ptr<Bundle> bundle, const CustodySignalReason& reason)
{
	//NS_LOG_DEBUG("BundleDeletion()");
  // Step 1:
  if (bundle->HasRetentionConstraint (RC_CUSTODY_ACCEPTED))
    {
      CustodyRelease (bundle);
      // Generate bundle deletion status report citing the reason for the deletion.
    }
  else if (bundle->GetReportBundleDeletion ())
    {
      // Generate a bundle deletion status report
    }

  // Step 2:
  bundle->RemoveAllRetentionConstraints ();
  m_bundleRouter->DeleteBundle (bundle,true);
}

// 6.3 Reception of Custody Signal
void 
BundleProtocolAgent::CustodySignalReception (const CustodySignal& signal)
{
  if (signal.GetCustodyTransferSucceeded ())
    {
      CustodyTransferSuccess (signal);
    }
  else 
    {
      CustodyTransferFailure (signal, false);
    }
}


void
BundleProtocolAgent::SendBundle (Ptr<Link> link, Ptr<Bundle> bundle)
{
  Simulator::ScheduleNow (&ConvergenceLayerAgent::BundleQueued, m_cla, link, bundle);
}

Ptr<Registration>
BundleProtocolAgent::CreateRegistration (void) 
{
  Ptr<Registration> registration = CreateObject<Registration> ();
  registration->SetBundleProtocol (this);
  return registration;
}

RegistrationEndpoint*
BundleProtocolAgent::Allocate ()
{
  return m_registrationManager->Allocate (m_eid);
}

RegistrationEndpoint*
BundleProtocolAgent::Allocate (const BundleEndpointId& eid)
{
  return m_registrationManager->Allocate (eid);
}

void
BundleProtocolAgent::DeAllocate (RegistrationEndpoint *regEndpoint)
{
  m_registrationManager->DeAllocate (regEndpoint);
}

}} // namespace bundleProtocol, ns3
