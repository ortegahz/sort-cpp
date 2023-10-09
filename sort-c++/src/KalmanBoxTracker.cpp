#include "KalmanBoxTracker.h"

KalmanBoxTracker::KalmanBoxTracker(TrackBox det)
{
	time_since_update = 0;
	id = count;
	count += 1;
	hits = 0;
	hit_streak = 0;
	age = 0;

	kf = KalmanFilter(7, 4, 0);

	measurement = Mat::zeros(4, 1, CV_32F);

	kf.transitionMatrix = (Mat_<float>(7, 7) << 1, 0, 0, 0, 1, 0, 0,
						   0, 1, 0, 0, 0, 1, 0,
						   0, 0, 1, 0, 0, 0, 1,
						   0, 0, 0, 1, 0, 0, 0,
						   0, 0, 0, 0, 1, 0, 0,
						   0, 0, 0, 0, 0, 1, 0,
						   0, 0, 0, 0, 0, 0, 1);
	kf.measurementMatrix = (Mat_<float>(4, 7) << 1, 0, 0, 0, 0, 0, 0,
							0, 1, 0, 0, 0, 0, 0,
							0, 0, 1, 0, 0, 0, 0,
							0, 0, 0, 1, 0, 0, 0);
	kf.measurementNoiseCov = (Mat_<float>(4, 4) << 1, 0, 0, 0,
							  0, 1, 0, 0,
							  0, 0, 10, 0,
							  0, 0, 0, 10);
	kf.errorCovPost = (Mat_<float>(7, 7) << 10, 0, 0, 0, 0, 0, 0,
					   0, 10, 0, 0, 0, 0, 0,
					   0, 0, 10, 0, 0, 0, 0,
					   0, 0, 0, 10, 0, 0, 0,
					   0, 0, 0, 0, 10000, 0, 0,
					   0, 0, 0, 0, 0, 10000, 0,
					   0, 0, 0, 0, 0, 0, 10000);
	kf.processNoiseCov = (Mat_<float>(7, 7) << 1, 0, 0, 0, 0, 0, 0,
						  0, 1, 0, 0, 0, 0, 0,
						  0, 0, 1, 0, 0, 0, 0,
						  0, 0, 0, 1, 0, 0, 0,
						  0, 0, 0, 0, 0.01, 0, 0,
						  0, 0, 0, 0, 0, 0.01, 0,
						  0, 0, 0, 0, 0, 0, 0.0001);

	// setIdentity(kf.measurementMatrix);
	// setIdentity(kf.processNoiseCov, Scalar::all(1e-2));
	// setIdentity(kf.measurementNoiseCov, Scalar::all(1e-1));
	// setIdentity(kf.errorCovPost, Scalar::all(1));

	{
		float bbox[4];

		bbox[0] = det.bbox.x;
		bbox[1] = det.bbox.y;
		bbox[2] = det.bbox.width;
		bbox[3] = det.bbox.height;

		_convert_bbox_to_z(bbox);

		kf.statePost = (Mat_<float>(7, 1) << bbox[0], bbox[1], bbox[2], bbox[3], 0., 0., 0.);
	}
}

void KalmanBoxTracker::update(TrackBox det)
{
	// Mat measurement = Mat::zeros(4, 1, CV_32F);

	time_since_update = 0;
	history.clear();
	hits += 1;
	hit_streak += 1;
	{
		float bbox[4];

		bbox[0] = det.bbox.x;
		bbox[1] = det.bbox.y;
		bbox[2] = det.bbox.width;
		bbox[3] = det.bbox.height;

		_convert_bbox_to_z(bbox);

		// measurement
		measurement.at<float>(0, 0) = bbox[0];
		measurement.at<float>(1, 0) = bbox[1];
		measurement.at<float>(2, 0) = bbox[2];
		measurement.at<float>(3, 0) = bbox[3];

		// update
		// kf.correct(measurement);
		kf.statePost.at<float>(0, 0) = bbox[0];
		kf.statePost.at<float>(1, 0) = bbox[1];
		kf.statePost.at<float>(2, 0) = bbox[2];
		kf.statePost.at<float>(3, 0) = bbox[3];
	}
}

TrackBox KalmanBoxTracker::predict()
{
	if (kf.statePost.at<float>(6, 0) + kf.statePost.at<float>(2, 0) <= 0)
	{
		kf.statePost.at<float>(6, 0) *= 0;
	}
	// Mat prediction = kf.predict();
	Mat prediction = kf.statePost;
	age += 1;
	if (time_since_update > 0)
		hit_streak = 0;
	time_since_update += 1;

	{
		float bbox[4];
		TrackBox tbox;

		bbox[0] = prediction.at<float>(0, 0);
		bbox[1] = prediction.at<float>(1, 0);
		bbox[2] = prediction.at<float>(2, 0);
		bbox[3] = prediction.at<float>(3, 0);

		_convert_x_to_bbox(bbox);

		tbox.frame_id = -1;
		tbox.track_id = -1;
		tbox.bbox = Rect_<float>(Point_<float>(bbox[0], bbox[1]), Point_<float>(bbox[2], bbox[3]));

		history.push_back(tbox);
	}

	return history.back();
}

Rect_<float> KalmanBoxTracker::get_state()
{
	float bbox[4];

	bbox[0] = kf.statePost.at<float>(0, 0);
	bbox[1] = kf.statePost.at<float>(1, 0);
	bbox[2] = kf.statePost.at<float>(2, 0);
	bbox[3] = kf.statePost.at<float>(3, 0);

	_convert_x_to_bbox(bbox);

	TrackBox tbox;

	tbox.frame_id = -1;
	tbox.track_id = -1;
	tbox.bbox = Rect_<float>(Point_<float>(bbox[0], bbox[1]), Point_<float>(bbox[2], bbox[3]));

	return tbox.bbox;
}

// [u, v, s, r] --> [x1, y1, x2, y2]
void KalmanBoxTracker::_convert_x_to_bbox(float *bbox)
{
	float x1, y1, x2, y2, w, h;
	w = sqrt(bbox[2] * bbox[3]);
	h = bbox[2] / w;
	x1 = bbox[0] - w / 2.;
	y1 = bbox[1] - h / 2.;
	x2 = bbox[0] + w / 2.;
	y2 = bbox[1] + h / 2.;

	if (x1 < 0 && bbox[0] > 0)
		x1 = 0;
	if (y1 < 0 && bbox[1] > 0)
		y1 = 0;

	bbox[0] = x1;
	bbox[1] = y1;
	bbox[2] = x2;
	bbox[3] = y2;
}

// [x, y, w, h] --> [u, v, s, r]
void KalmanBoxTracker::_convert_bbox_to_z(float *bbox)
{
	float u, v, s, r;
	u = bbox[0] + bbox[2] / 2.;
	v = bbox[1] + bbox[3] / 2.;
	s = bbox[2] * bbox[3];
	r = bbox[2] / bbox[3];

	bbox[0] = u;
	bbox[1] = v;
	bbox[2] = s;
	bbox[3] = r;
}