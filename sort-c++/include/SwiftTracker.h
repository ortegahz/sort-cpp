#pragma once

#include <vector>
#include <iostream>
#include <set>

#include "opencv2/highgui/highgui.hpp"

#include "KalmanBoxTracker.h"
#include "LinearAssigner.h"
#include "dataType.h"

using namespace std;
using namespace cv;

class SwiftTracker
{
public:
	SwiftTracker();
	void update(vector<TrackBox> dets);
	double calc_iou(Rect_<float> a, Rect_<float> b);

	vector<TrackBox> data_preds;
	vector<TrackBox> data_preds_post;

private:
	int max_age;
	int min_hits;
	float iou_threshold;
	vector<KalmanBoxTracker> trackers;
	int frame_count;
};
