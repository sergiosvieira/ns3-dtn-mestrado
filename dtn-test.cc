/***************************************************************************
 *   Copyright (C) 2011 by Sergio Vieira                                   *
 *   sergiosvieira@larces.uece.br                                          *
 *   Computer Networks and Security Laboratory (LARCES)                    *
 *   State University of Ceara (UECE/Brazil)                               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*
 * Definitive Nakagami Wifi
 * User Arguments:
 --st: Simulation Time
 --ss: Simulation Seed
 --dn: Distance between nodes
 */

#include <iostream>
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/v4ping-helper.h"
#include <ns3/ipv4-interface-container.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ns2-mobility-helper.h>
#include <ns3/on-off-helper.h>
#include <ns3/inet-socket-address.h>
#include "ns3/stats-module.h"
#include "ns3/aid.h"
#include "ns3/aid-helper.h"
#include "ns3/vector2d.h"
#include "ns3/nstime.h"
#include "ns3/bundle-protocol-helper.h"
#include "ns3/one-traffic-helper.h"

NS_LOG_COMPONENT_DEFINE("AODVTest");

using namespace ns3;
string _mobs;

/*
static void CourseChange(std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility) {
	Ptr<Node> node = mobility->GetObject<Node> ();
	Vector pos = mobility->GetPosition(); // Get position
	Vector vel = mobility->GetVelocity(); // Get velocity


	std::cout.precision(5);

	std::cout << "n -t " << Simulator::Now().GetSeconds() << " -s "
			<< node->GetId() << " -x " << pos.x << " -y " << pos.y << " -z "
			<< pos.z << " -U " << vel.x << " -V " << vel.y << " -T " << 1.0
			<< std::endl;


	//	*os << "n -t " << Simulator::Now().GetSeconds() << " -s "
	//		<< node->GetId() << " -x " << pos.x << " -y " << pos.y << " -z "
	//	<< pos.z << " -U " << vel.x << " -V " << vel.y << " -T " << 1.0
	//<< std::endl;

}
*/

void TxCallback(Ptr<CounterCalculator<uint32_t> > datac, std::string path,
		Ptr<const Packet> packet) {
	NS_LOG_INFO ("Sent frame counted in " << datac->GetKey ());
	datac->Update();
	// end TxCallback
}

/* Class */
class Experiment {
public:
	Experiment();
	void configure(int argc, char **argv);
	void run();
private:
	DataCollector data;
	NodeContainer nodes_;
	NetDeviceContainer devices_;
	Ipv4InterfaceContainer interfaces_;
	double simulation_time_;
	double simulation_seed_;
	double distance_;
	std::string protocol_;
	std::string cenario_;
	std::string traffic_;
	int num_nodes_;
	void createNodes();
	void createDevices();
	void createDevices80211p();
	void createDevices80211a();
	void createMobility();
	void installInternetStack();
	void installApplications();
	void createTraffic();
	void createStats();
};

/* Public Methods */
Experiment::Experiment() {
	simulation_time_ = 1000.0;
	simulation_seed_ = 1978.0;
	distance_ = 100.0;
	protocol_ = "RTEpidemic";
	cenario_ = "s2.tcl";
	traffic_ = "traffic_04";
	num_nodes_ = 9;
}

void Experiment::configure(int argc, char **argv) {
	CommandLine cmd;
	cmd.AddValue("st", "Simulation Time", simulation_time_);
	cmd.AddValue("ss", "Simulation Seed", simulation_seed_);
	cmd.AddValue("dn", "Distance between nodes", distance_);
	cmd.AddValue("pt", "BundleRoter Protocol",protocol_);
	cmd.AddValue("ce", "Tcl of cenario",cenario_);
	cmd.AddValue("tr", "Traffic of cenario",traffic_);
	cmd.AddValue("nn", "Number of Nodes",num_nodes_);
	cmd.Parse(argc, argv);

	std::cout<<"st:"<<simulation_time_<<" ss:"<<simulation_seed_<<" pt:"<<protocol_<<" ce:"<<cenario_<<" tr:"<<traffic_<<" nn:"<<num_nodes_<<"\n";
	SeedManager::SetSeed(simulation_seed_);
}

void Experiment::run() {
	createNodes();
	createMobility();
	createDevices();
	//createDevices80211p();
	installInternetStack();
	installApplications();
	//createTraffic();
	//createStats();
	Simulator::Stop(Seconds(simulation_time_));
	Simulator::Run();
	Simulator::Destroy();
	Ptr<DataOutputInterface> output = 0;
	output = CreateObject<OmnetDataOutput> ();
	output->Output(data);
}

/* Private Methods */

void Experiment::createStats() {
	data.DescribeRun("1", "1", "1", "1");

	// Add any information we wish to record about this run.
	data.AddMetadata("author", "sergiosvieira");
	Ptr<CounterCalculator<uint32_t> > totalTx = CreateObject<CounterCalculator<uint32_t> > ();
	totalTx->SetKey("wifi-tx-frames");
	totalTx->SetContext("nodes");
	Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
			MakeBoundCallback(&TxCallback, totalTx));
	data.AddDataCalculator(totalTx);

	Ptr<PacketCounterCalculator> totalRx =
			CreateObject<PacketCounterCalculator> ();
	totalRx->SetKey("wifi-rx-frames");
	totalRx->SetContext("nodes");
	Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
			MakeCallback(&PacketCounterCalculator::PacketUpdate, totalRx));
	data.AddDataCalculator(totalRx);

	Ptr<PacketCounterCalculator> appTx = CreateObject<PacketCounterCalculator> ();
	appTx->SetKey("sender-tx-packets");
	appTx->SetContext ("nodes");
	Config::Connect("/NodeList/*/ApplicationList/*/ns3::UdpSocketFactory/Tx",
	MakeCallback(&PacketCounterCalculator::PacketUpdate, appTx));
	data.AddDataCalculator(appTx);

	/*
	 Ptr<CounterCalculator<> > appRx =
	 CreateObject<CounterCalculator<> >();
	 appRx->SetKey("receiver-rx-packets");
	 //appRx->SetContext("node[1]");
	 receiver->SetCounter(appRx);
	 data.AddDataCalculator(appRx);
	 */
}

void Experiment::createNodes() {
	NS_LOG_INFO("creating nodes...");
	nodes_.Create(num_nodes_);
}

void Experiment::createDevices() {
	NS_LOG_INFO("creating devices...");
	// Channel
	YansWifiChannelHelper chn;// = YansWifiChannelHelper::Default();
	chn.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	chn.AddPropagationLoss("ns3::FriisPropagationLossModel");
	chn.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
	// Physical
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
	phy.Set("TxPowerStart", DoubleValue(16.0));
	phy.Set("TxPowerEnd", DoubleValue(16.0));
	phy.Set("TxPowerLevels", UintegerValue(1));
	double threshold = -79.0779;
	/*Valores João, valores tirados diretamente da simulação*/
	//threshold = -60.6779; // até 100m
	//threshold =  -69.0779; // ate 300m
	//threshold = -73.4001;   // 500m
	//threshold = -79.3573;   // até 1000m
	/*Valores Sergio*/
	//100m threshold = -69.6779;
	/// 200m threshold = -75.6985;
	// 300m threshold = -79.2203;
	// 500m threshold = -83.6573;
	// 800m threshold = -87.7397
	// 1000m threshold = -89.6779
	phy.Set("EnergyDetectionThreshold", DoubleValue(threshold));
	phy.Set("CcaMode1Threshold", DoubleValue(threshold + 3.0));
	phy.Set("RxNoiseFigure", DoubleValue(4.0));
	phy.SetChannel(chn.Create());
	/* Creating MAC Layer */
	NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
	mac.SetType("ns3::AdhocWifiMac");
	/* Wifi */
	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211p_CCH);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager");
	devices_ = wifi.Install(phy, mac, nodes_);
}

void Experiment::createDevices80211p() {
	NS_LOG_INFO("creating devices 802.11p...");
	/* Creating Physical Layer */
	// Channel
	YansWifiChannelHelper chn;// = YansWifiChannelHelper::Default();
	chn.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	chn.AddPropagationLoss("ns3::FriisPropagationLossModel");
	chn.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
	// Physical
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
	phy.Set("TxPowerStart", DoubleValue(21.5));
	phy.Set("TxPowerEnd", DoubleValue(21.5));
	phy.Set("TxPowerLevels", UintegerValue(1));
	phy.Set("TxGain", DoubleValue(0));
	phy.Set("RxGain", DoubleValue(0));
	double threshold = -101.0;
	phy.Set("EnergyDetectionThreshold", DoubleValue(threshold));
	phy.Set("CcaMode1Threshold", DoubleValue(threshold + 3.0));
	//phy.Set("RxNoiseFigure", DoubleValue(4.0));
	//phy.Set("RxNoiseFigure", DoubleValue(-99.0));
	phy.SetChannel(chn.Create());
	/* Creating MAC Layer */
	NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
	mac.SetType("ns3::AdhocWifiMac");
	/* Wifi */
	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211p_CCH);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager");
	devices_ = wifi.Install(phy, mac, nodes_);
}

void Experiment::createDevices80211a() {
	NS_LOG_INFO("creating devices 802.11a...");

	YansWifiChannelHelper wifiChannel;// = YansWifiChannelHelper::Default ();
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");


	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
	wifiPhy.SetChannel (wifiChannel.Create ());

	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
	wifiMac.SetType ("ns3::AdhocWifiMac");

	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
	//wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager");
	wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate54Mbps"));

	devices_ = wifi.Install(wifiPhy, wifiMac, nodes_);


}


void Experiment::createMobility() {
	NS_LOG_INFO("creating mobility...");
	//std::string filename = "./examples/mobility/mov_01";
	std::string filename = "./examples/mobility/";
	filename += cenario_;
	_mobs = filename;
	/*
	Ptr<ListPositionAllocator> lpa = CreateObject<ListPositionAllocator> ();
	MobilityHelper mh;
	for (uint32_t i = 0; i < nodes_.GetN(); i++) {
		lpa->Add(Vector(i * (distance_), 0.0, 0.0));
		NS_LOG_INFO("\t(" << i * distance_ << ", 0.0, 0.0)\n");
	}
	mh.SetPositionAllocator(lpa);
	mh.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mh.Install(nodes_);
	*/

	Ns2MobilityHelper ns2 = Ns2MobilityHelper(filename);
	ns2.Install();

	/*
	std::filebuf fb;
	fb.open("aodv-test.tr", ios::out);
	std::ostream os(&fb);
	*/

	//Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback(&CourseChange, &os));

}

void Experiment::installInternetStack() {
	AodvHelper aodv;
	InternetStackHelper stack;
	stack.SetRoutingHelper(aodv);
	stack.Install(nodes_);
	Ipv4AddressHelper address;
	address.SetBase("10.0.0.0", "255.0.0.0");
	interfaces_ = address.Assign(devices_);
}

void Experiment::installApplications() {


	//MobnetHelper appsend(Ipv4Address("10.0.0.2"), 1978, true);
	//appsend.SetAttribute("Remote",Ipv4Address("10.0.0.2"));
	//MobnetHelper appwait(interfaces_.GetAddress(1), 1978, true);

	/*
	 ApplicationContainer app1 = appsend.Install(nodes_.Get(0));
	 appsend.SetAttribute("Remote",Ipv4AddressValue(Ipv4Address("10.0.0.1")));
	 appsend.Install(nodes_.Get(1));
	 app1.Start(Seconds(0));
	 app1.Stop(Seconds(simulation_time_) - Seconds(0.001));
	 */

	/*
	AidHelper helper(Ipv4Address("255.255.255.255"), 1978, true, 300);
	ApplicationContainer app = helper.Install(nodes_);
	app.Start(Seconds(0));
	app.Stop(Seconds(simulation_time_) - Seconds(0.001));
	*/


	BundleProtocolHelper bphelper;
	std::string brouter = "ns3::bundleProtocol::" + protocol_;
	bphelper.SetBundleRouter(brouter);
	bphelper.Install(nodes_,num_nodes_);
	//bphelper.Start();

	//OneTrafficHelper trhelper("examples/mobility/traffic_01", 1, 64);
	std::string filename = "examples/mobility/";
	filename += traffic_;
	OneTrafficHelper trhelper(filename, 1, 300);
	trhelper.Install();


	/*
	 ApplicationContainer app2 = appsend.Install(nodes_.Get(1));
	 app2.Start(Seconds(0));
	 app2.Stop(Seconds(simulation_time_) - Seconds(0.001));
	 */
}

void Experiment::createTraffic() {
	uint16_t port = 21;
	uint16_t sinks = 8;
	OnOffHelper onoff1("ns3::UdpSocketFactory", Address());
	onoff1.SetAttribute("OnTime", RandomVariableValue(ConstantVariable(1)));
	onoff1.SetAttribute("OffTime", RandomVariableValue(ConstantVariable(0)));
	onoff1.SetAttribute("DataRate", StringValue("2Mbps"));
	onoff1.SetAttribute("PacketSize", UintegerValue(2000));
	for (int i = 0; i < sinks; i++) {

		AddressValue remoteAddress(InetSocketAddress(interfaces_.GetAddress(i),
				port));
		onoff1.SetAttribute("Remote", remoteAddress);

		UniformVariable var;
		ApplicationContainer temp = onoff1.Install(nodes_.Get(i + sinks));
		temp.Start(Seconds(var.GetValue(5.0, 50.0)));
		temp.Stop(Seconds(50.0));
	}

}

/* Main Program */
int main(int argc, char **argv) {
	std::cout << "Definitive Nakagami WiFi Simulation starting...\n";
	Experiment ex;
	ex.configure(argc, argv);
	ex.run();
	std::cout << "Definitive Nakagami WiFi Simulation finalized...\n";
	/*
	Vector2d local_pos1(0.0, 0.0), local_vel1(11.0, 0.0);
	Vector2d local_pos2(50.0, 0.0), local_vel2(10.0, 0.0);
	uint16_t m_transmission_range = 300;
	Time t = LinkLifeTime::getExpirationTime(local_pos1,
			local_vel1, local_pos2, local_vel2, m_transmission_range);
	std::cout << t.GetSeconds() << "\n";
	*/
	exit(0);
	return 0;
}

