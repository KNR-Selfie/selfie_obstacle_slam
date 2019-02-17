/**
 * Copyright (c) 2019 Adam Gotlib
 * This code is licensed under MIT license (see LICENSE for details)
 **/

#include "selfie_obstacle_detection/CornerDetector.h"

using sensor_msgs::LaserScanPtr;
using std::prev;

namespace selfie_obstacle_detection
{

CornerDetector::CornerDetector(IObstacleObservationsExtractor* extractor,
                               ILineHelper* helper,
                               ICornerGenerator* generator)
	: extractor_(extractor),
	  helper_   (helper),
	  generator_(generator)
{ }

CornerArrayPtr CornerDetector::detectCorners(LaserScanPtr scan)
{
	ObstacleObservations observations = extractor_->extractObstacleObservations(scan);

	CornerArrayPtr corners(new CornerArray());
	for (auto observation : observations)
	{
		LinePtr line;
		if (!helper_->fitLineToSegment(observation->begin(), observation->end(), line)) continue;

		PointPtr p1 = helper_->projectPointOntoLine(*observation->begin(), line);
		PointPtr p2 = helper_->projectPointOntoLine(*prev(observation->end()), line);

		CornerPtr c1, c2;
		generator_->generateCorners(p1, p2, c1, c2);

		corners->data.push_back(*c1);
		corners->data.push_back(*c2);
	}

	corners->header = scan->header;
	return corners;
}

} // namespace selfie_obstacle_detection
