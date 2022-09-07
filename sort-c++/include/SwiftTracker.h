#include <vector>

#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class SwiftTracker
{
public:
	SwiftTracker();
	void update();

private:
	int max_age;
	int min_hits;
	float iou_threshold;
	vector<int> trackers;
	int frame_count;
};
