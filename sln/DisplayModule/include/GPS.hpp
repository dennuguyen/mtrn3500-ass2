#ifndef MTRN3500_GPS_HPP_
#define MTRN3500_GPS_HPP_

#include "OEM4.hpp"
#include "SharedMemory.hpp"
#include "Timer.hpp"

class GPS {
   public:
	GPS();
	void draw(double x, double y);

   private:
	void update();

   private:
	sm::FileMappingObject gps;
	int numPoints;
	int head;
	int tail;
	std::vector<OEM4> data;
	tmr::Timer timer;
};

#endif  // MTRN3500_GPS_HPP_
