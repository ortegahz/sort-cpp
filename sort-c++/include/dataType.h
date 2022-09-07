#pragma once

#include "opencv2/core.hpp"

using namespace cv;

typedef struct TrackBox
{
    int frame_id;
    int track_id;
    Rect_<float> bbox;
} TrackBox;