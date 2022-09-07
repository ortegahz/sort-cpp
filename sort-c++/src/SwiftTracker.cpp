#include "SwiftTracker.h"

void SwiftTracker::update(vector<TrackBox> dets)
{
	cout << "updating " << dets.size() << " bboxes ..." << endl;
	frame_count += 1;
}

SwiftTracker::SwiftTracker()
{
	max_age = 1;
	min_hits = 3;
	iou_threshold = 0.3;
	frame_count = 0;
}