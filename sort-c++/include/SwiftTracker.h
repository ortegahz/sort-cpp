#pragma once

#include <vector>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"

#include "dataType.h"

using namespace std;
using namespace cv;

class SwiftTracker
{
public:
	SwiftTracker();
	void update(vector<TrackBox> dets);

private:
	int max_age;
	int min_hits;
	float iou_threshold;
	vector<int> trackers;
	int frame_count;
};
