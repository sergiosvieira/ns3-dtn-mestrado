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
#include "ns3/assert.h"
//#include "ns3/static-waypoint-mobility-model.h"
#include "one-mobility-helper.h"

NS_LOG_COMPONENT_DEFINE ("OneMobilityHelper");

using namespace std;

namespace ns3 {

OneMobilityHelper::OneMobilityHelper(string filename) :
	m_filename(filename) {
}

Ptr<StaticWaypointMobilityModel> OneMobilityHelper::GetMobilityModel(
		uint32_t id, const ObjectStore &store) const {
	Ptr<Object> object = store.Get(id);
	if (object == 0) {
		return 0;
	}
	Ptr<StaticWaypointMobilityModel> model = object->GetObject<
			StaticWaypointMobilityModel> ();
	if (model == 0) {
		model = CreateObject<StaticWaypointMobilityModel> ();
		object->AggregateObject(model);
	}
	return model;
}

// initalLocation 1 100.0 120.0
// newDestination 1 38.765 300.0 700.0 1.2
// waitAtWaypoint 1 70.345

void OneMobilityHelper::LayoutObjectStore(const ObjectStore &store) const {
	ifstream file(m_filename.c_str(), ios::in);
	if (file.is_open()) {
		while (!file.eof()) {
			string line;
			getline(file, line);

			string command;
			uint32_t nodeId;

			istringstream iss(line);
			iss >> command;

			if (iss.fail()) {
				continue;
			}

			iss >> nodeId;

			if (iss.fail()) {
				continue;
			}

			Ptr<StaticWaypointMobilityModel> model = GetMobilityModel(nodeId,
					store);

			if (model == 0) {
				continue;
			}

			if (command == "initalLocation") {
				double x, y;
				iss >> x;
				iss >> y;
				Vector position;
				position.x = x;
				position.y = y;
				position.z = 0;
				model->SetPosition(position);
				model->Pause();
				NS_LOG_DEBUG("Initalize position " << "(" << x << ":" << y
						<< ") for node " << nodeId);
			} else if (command == "initialDestination") {
				double x, y, speed;
				iss >> x;
				iss >> y;
				iss >> speed;
				Vector destination;
				destination.x = x;
				destination.y = y;
				destination.z = 0;
				NS_LOG_DEBUG("Intial destination " << "(" << x << ":" << y
						<< ") " << " for node " << nodeId);
				model->BeginWalk(speed, destination);
			} else if (command == "newDestination") {
				double at, x, y, speed;
				iss >> at;
				iss >> x;
				iss >> y;
				iss >> speed;
				Vector destination;
				destination.x = x;
				destination.y = y;
				destination.z = 0;
				NS_LOG_DEBUG("New destination " << "(" << x << ":" << y << ") "
						<< "for node " << nodeId << " at " << at);
				Simulator::Schedule(Seconds(at),
						&StaticWaypointMobilityModel::BeginWalk, model, speed,
						destination);
			} else if (command == "waitAtWaypoint") {
				double at;
				iss >> at;
				NS_LOG_DEBUG("Node " << nodeId << " should pause at " << at);
				Simulator::Schedule(Seconds(at),
						&StaticWaypointMobilityModel::Pause, model);
			} else if (command == "initialWaitAtWaypoint") {
				NS_LOG_DEBUG("Got initial wait at waypoint for node " << nodeId);
				model->Pause();
			}

		}
		file.close();
	} else {
		cout << "OneTrafficHelper: Could not open the file: " << m_filename
				<< endl;
		NS_ASSERT_MSG(false, "OneTrafficHelper: Could not open the file: "
				<< m_filename);
		NS_LOG_DEBUG("Could not open the file: " << m_filename);
	}
}

void OneMobilityHelper::Install(void) const {
	NS_LOG_DEBUG("OneMobilityHelper::Install");
	Install(NodeList::Begin(), NodeList::End());
}

} // namespace ns3
