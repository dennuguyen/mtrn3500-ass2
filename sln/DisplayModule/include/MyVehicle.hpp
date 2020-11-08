#ifndef __MYVEHICLE_HPP__
#define __MYVEHICLE_HPP__

#include "GPS.hpp"
#include "LIDAR.hpp"
#include "Vehicle.hpp"

class MyVehicle : public Vehicle
{
public:
	MyVehicle();
	virtual void draw();
	LIDAR oem4;
	GPS lms151;
};

#endif