#include <iostream>
#include "ns3/trend-of-delivery.xhf.hpp"
#include "ns3/vector2d.h"

using namespace ns3;

int main(){

	trendofdelivery m_tod;	
	Vector2d pos(10,10);
	m_tod.setFuzzy(my_pos_, my_pos,350);
	double result;
	m_tod.inference(50, 100,20, &result);	
}
