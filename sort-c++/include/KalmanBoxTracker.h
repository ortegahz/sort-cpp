#pragma once

#include <vector>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"

#include "dataType.h"

using namespace std;
using namespace cv;

class KalmanBoxTracker
{
public:
	KalmanBoxTracker(TrackBox det);
	void update(TrackBox det);
	TrackBox predict();
	Rect_<float> get_state();

    int time_since_update;
    int hit_streak;
    int id;

private:
	void _convert_bbox_to_z(float *bbox);
	void _convert_x_to_bbox(float *bbox);

    static int count;

    KalmanFilter kf;
	Mat measurement;

    vector<TrackBox> history;
    int hits;
    int age;
};
