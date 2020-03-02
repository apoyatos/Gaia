
#ifndef  UTILITIES
#define UTILITIES
#pragma once

enum Axis { X, Y, Z };

enum SpaceReference{LocalSpace,WorldSpace,ParentSpace};

struct Vector3 {
	double x;
	double y;
	double z;

	Vector3 operator +=(const Vector3& p);
	Vector3 operator -=(const Vector3& p);
	bool operator == (const Vector3& p) const;
	Vector3 operator = (const Vector3& p);
};

Vector3 operator +(const Vector3& p1, const Vector3& p2);


Vector3 operator -(const Vector3& p1, const Vector3& p2);


#endif