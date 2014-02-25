/*
 * ieee754.h
 *
 *  Created on: 15/04/2012
 *      Author: Sérgio Vieira
 */

#ifndef IEEE_754_H
#define IEEE_754_H
#include "ns3/uinteger.h"

//namespace ns3 {
//namespace bundleProtocol {


#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/* IEEE 754 pack unpack */

uint64_t pack754(long double f, unsigned bits, unsigned expbits);
long double unpack754(uint64_t i, unsigned bits, unsigned expbits);
//}
//}


#endif /* IEEE754_H_ */
