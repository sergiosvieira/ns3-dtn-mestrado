/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "one-traffic-helper.h"

#include "ns3/bp-header.h"
#include "ns3/bp-bundle-endpoint-id.h"
#include "ns3/bp-registration.h"
#include "ns3/bp-registration-factory.h"
#include "ns3/bp-global-bundle-identifier.h"

 NS_LOG_COMPONENT_DEFINE ("OneTrafficHelper");

#include <iostream>
 using namespace std;

namespace ns3 {


OneTrafficHelper::OneTrafficHelper (string filename, uint32_t divider, uint64_t ttl)
  : m_filename (filename), m_divider (divider), m_ttl (ttl)
{}

 Ptr<bundleProtocol::Registration>
 OneTrafficHelper::GetRegistration (uint32_t id, const ObjectStore &store) const
 {
   Ptr<Object> object = store.Get (id);
   
   if (object == 0)
     {
       return 0;
     }
   
   Ptr<bundleProtocol::Registration> reg = object->GetObject<bundleProtocol::Registration> ();
   if (reg == 0)
     {
       Ptr<bundleProtocol::RegistrationFactory> registrationFactory = object->GetObject<bundleProtocol::RegistrationFactory> ();
       reg = registrationFactory->CreateRegistration ();
        if (reg == 0)
         {
           return 0;
         }
       reg->Register ();
       object->AggregateObject (reg);
     }
   return reg;
 }

   // 3600	C	M1	91	37	15850	u2

void 
OneTrafficHelper::LayoutObjectStore (const ObjectStore &store) const
{
  ifstream file (m_filename.c_str ());
  if (file.is_open())
    {
      while (!file.eof() )
        {
          string line;
          getline (file, line);
          double at;
          string command,message,util;
          uint32_t from,to,size;
          
          istringstream iss (line);
          iss >> at;
          iss >> command;
          if (command != "C")
            {
              continue;
            }
          
          iss >> message;
          iss >> from;
          iss >> to;
          iss >> size;
          iss >> util;
          
         
          if (size > 1)
            size = ceil ((double) size / (double) m_divider);
         
          
          // size = 2 * size;
          
          Ptr<bundleProtocol::Registration> reg = GetRegistration (from, store);
          if (reg == 0)
            {
              cout << "%%%%%%%%%%%%%% Nooo" << endl;
              cout << "%%%%%%%%%%%%%% Could not find a registration for node (" << from << ")" << endl;
              continue;
            }
          bundleProtocol::BundlePriority utility = GetUtility (util);

          stringstream ss;
          ss << to;

          bundleProtocol::BundleEndpointId destination = bundleProtocol::BundleEndpointId ("dtn",ss.str ());

          Simulator::Schedule (Seconds (at), &bundleProtocol::Registration::Send, reg, Create<Packet> (size), destination, utility, Seconds (m_ttl), bundleProtocol::PrimaryProcessingControlFlags ());
          NS_LOG_DEBUG ("At " << at << " send message " << message << " from node " << from << " to node " << to << " with utility " << util << " and size " << size);
        }
      file.close();
    }
  else 
    {
      cout << "OneTrafficHelper: Could not open the file: " << m_filename << endl;
      NS_ASSERT_MSG (false, "OneTrafficHelper: Could not open the file: " << m_filename);
      NS_LOG_DEBUG ("Could not open the file: " << m_filename);
    }
}
   
bundleProtocol::BundlePriority
OneTrafficHelper::GetUtility (string utility) const
{
  if (utility == "u1")
    {
      return bundleProtocol::BULK;
    }
  else if (utility == "u2")
    {
      return bundleProtocol::NORMAL;
    }
  else if (utility == "u3")
    {
      return bundleProtocol::EXPEDITED;
    }
  else
    {
      return bundleProtocol::BULK;
    }
}

void 
OneTrafficHelper::Install (void) const
{
  NS_LOG_DEBUG ("OneTrafficHelper::Install");
  Install (NodeList::Begin (), NodeList::End ());
}

} // namespace ns3
