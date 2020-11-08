#ifndef MTRN3500_GPS_HPP_
#define MTRN3500_GPS_HPP_

#include "SharedMemory.hpp"

class GPS {
   public:
	GPS();
	static void draw();

   private:
	sm::FileMappingObject gps;
	double* northing;
	double* easting;
	double* height;
};

#endif  // MTRN3500_GPS_HPP_
