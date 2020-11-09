#ifndef MTRN3500_GPS_HPP_
#define MTRN3500_GPS_HPP_

#include "SharedMemory.hpp"

class GPS {
   public:
	GPS();
	void draw();

   private:
	void update();

   private:
	sm::FileMappingObject gps;
	uint8_t* numPoints;
	int* head;
	int* tail;
	double* data;
};

#endif  // MTRN3500_GPS_HPP_
