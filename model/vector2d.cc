/*
 * vector2d.cc
 *
 *  Created on: 09/12/2011
 *      Author: sergio
 */

#include "vector2d.h"

namespace ns3 {

Vector2d::Vector2d() {
	x = 0.0;
	y = 0.0;
}

Vector2d::Vector2d(Vector vector) {
	x = vector.x;
	y = vector.y;
}

Vector2d::Vector2d(double r, double s) {
	x = r;
	y = s;
}

/* Convert Vector2d to string */

std::string Vector2d::toString() {
	char result[B_SIZE];
	snprintf(result, B_SIZE, "(%.3f,%.3f)", x, y);
	return std::string(result);
}

Vector2d& Vector2d::operator +=(const Vector2d& v) {
	x += v.x;
	y += v.y;

	return (*this);
}

Vector2d& Vector2d::operator -=(const Vector2d& v) {
	x -= v.x;
	y -= v.y;

	return (*this);
}

Vector2d& Vector2d::operator *=(double t) {
	x *= t;
	y *= t;

	return (*this);
}

Vector2d& Vector2d::operator /=(double t) {
	double f = 1.0F / t;

	x *= f;
	y *= f;

	return (*this);
}

Vector2d Vector2d::operator +(const Vector2d& v) const {
	return (Vector2d(x + v.x, y + v.y));
}

Vector2d Vector2d::operator -(const Vector2d& v) const {
	return (Vector2d(x - v.x, y - v.y));
}

Vector2d Vector2d::operator *(double t) const {
	return (Vector2d(x * t, y * t));
}

Vector2d Vector2d::operator /(double t) const {
	double f = 1.0F / t;

	return (Vector2d(x * f, y * f));
}

double Vector2d::operator *(const Vector2d& v) const {
	return (x * v.x + y * v.y);
}

double Vector2d::length() {
	return sqrt(x * x + y * y);
}

Vector2d Vector2d::normalize() {
	Vector2d u = Vector2d(x, y);
	if (u.length() == 0.0) {
		return Vector2d(0.0, 0.0);
	}
	Vector2d result = u / u.length();
	return result;
}

bool Vector2d::is_null() {
	return x == 0.0 && y == 0.0 ? true : false;
}

double Vector2d::dotp(Vector2d u) {
	return x * u.x + y * u.y;
}

double Vector2d::angle(Vector2d u) {
	double a = dotp(u);
	double b = length() * u.length();

	if (b == 0.0) {
		return 360.0;
	}

	double result = acos(a / b);
	return RADTODEG(result);
}

void Vector2d::setFromVector(Vector vector) {
	x = vector.x;
	y = vector.y;
}

Vector2d Vector2d::normal() {
	return Vector2d(-y, x);
}

std::ostream& operator<< (std::ostream &out, Vector2d& val ) {
	out << "(" << val.x << ", " << val.y << ")";
	return out;
}

/*
double Vector2d::getX() {
	return x;
}

double Vector2d::getY() {
	return y;
}

void Vector2d::setX(double v) {
	x = v;
}

void Vector2d::setY(double v) {
	y = v;
}
*/
}
