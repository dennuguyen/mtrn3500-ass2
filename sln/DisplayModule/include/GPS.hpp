#ifndef MTRN3500_GPS_HPP_
#define MTRN3500_GPS_HPP_

#include "OEM4.hpp"
#include "SharedMemory.hpp"

class GPS {
   public:
	GPS();
	void draw(double x, double y);

   private:
	void update();

   private:
	sm::FileMappingObject gps;
	uint8_t* numPoints;
	int8_t* head;
	int8_t* tail;
	OEM4* data;
};

#endif  // MTRN3500_GPS_HPP_
