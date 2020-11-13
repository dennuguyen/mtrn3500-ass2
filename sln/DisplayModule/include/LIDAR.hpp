#ifndef MTRN3500_LINE_HPP_
#define MTRN3500_LINE_HPP_

#include "SharedMemory.hpp"

constexpr double HEIGHT = 1;

class LIDAR {
   public:
	LIDAR();
	void draw();

   private:
	void update();

   private:
	sm::FileMappingObject laser;
	uint16_t* numPoints;
	std::pair<double, double>* points;
};

#endif  // MTRN3500_LINE_HPP_
