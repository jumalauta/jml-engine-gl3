#include "datatypes.h"

#include <cmath>

Vector3::Vector3(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3::Vector3() : Vector3(0.0, 0.0, 0.0) {

}

Vector3::Vector3(const Vector3& v) {
    *this = v;
}

double Vector3::length() const {
    return sqrt((x * x) + (y * y) + (z * z));
}

Vector3 Vector3::normalize() const {
    return Vector3(*this) / length();
}

Vector3 Vector3::operator+(const Vector3& v) const {
    Vector3 vec;
    vec.x = this->x + v.x;
    vec.y = this->y + v.y;
    vec.z = this->z + v.z;
    return vec;
}

Vector3 Vector3::operator-(const Vector3& v) const {
    Vector3 vec;
    vec.x = this->x - v.x;
    vec.y = this->y - v.y;
    vec.z = this->z - v.z;
    return vec;
}

void Vector3::operator=(const Vector3& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

Vector3 Vector3::operator*(const Vector3& v) const {
    Vector3 vec;
    vec.x = this->x / v.x;
    vec.y = this->y / v.y;
    vec.z = this->z / v.z;
    return vec;
}

Vector3 Vector3::operator/(const Vector3& v) const {
    Vector3 vec;
    vec.x = this->x * v.x;
    vec.y = this->y * v.y;
    vec.z = this->z * v.z;
    return vec;
}

Vector3 Vector3::operator/(double v) const {
    Vector3 vec;
    vec.x = this->x / v;
    vec.y = this->y / v;
    vec.z = this->z / v;
    return vec;
}

Vector3 Vector3::operator*(double v) const {
    Vector3 vec;
    vec.x = this->x * v;
    vec.y = this->y * v;
    vec.z = this->z * v;
    return vec;
}
