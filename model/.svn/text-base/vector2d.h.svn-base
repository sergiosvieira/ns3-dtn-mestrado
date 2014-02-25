/*
 * vector2d.h
 *
 *  Created on: 17/02/2010
 *      Author: Sérgio Vieira - LARCES - UECE
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include <cmath>
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
//#include "ns3/consts.h"
#include "ns3/vector.h"

#define PI_ 3.1415926
#define B_SIZE 64
#define DEGTORAD(Deg) ((Deg * PI_) / 180.0)
#define RADTODEG(Rad) ((180.0 * Rad) / PI_)

namespace ns3 {


class Vector2d {
//protected:


public:
	double x, y;
	Vector2d();
	Vector2d(Vector vector);
	Vector2d(double r, double s);

	/* Convert Vector2d to string */
	std::string toString();
	Vector2d& operator +=(const Vector2d& v);
	Vector2d& operator -=(const Vector2d& v);
	Vector2d& operator *=(double t);
	Vector2d& operator /=(double t);
	Vector2d operator +(const Vector2d& v) const;
	Vector2d operator -(const Vector2d& v) const;
	Vector2d operator *(double t) const;
	Vector2d operator /(double t) const;
	double operator *(const Vector2d& v) const;
	double length();
	Vector2d normalize();
	bool is_null();
	double dotp(Vector2d u);
	double angle(Vector2d u);
	void setFromVector(Vector vector);
	Vector2d normal();
};

std::ostream& operator<< (std::ostream &out, Vector2d& val );

}

#endif /* Vector2d_H_ */
