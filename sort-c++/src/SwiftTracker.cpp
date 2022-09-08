#include "SwiftTracker.h"

double SwiftTracker::calc_iou(Rect_<float> a, Rect_<float> b)
{
	float in = (a & b).area();
	float un = a.area() + b.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}

void SwiftTracker::update(vector<TrackBox> dets)
{
	unsigned int n_trk = trackers.size();
	unsigned int n_det = dets.size();
	vector<vector<double>> iou_matrix;
	vector<int> assignment;

	set<int> idx_um_dets;
	set<int> idx_um_trks;
	set<int> idx_all_dets;
	set<int> idx_mts_dets;
	vector<cv::Point> mt_pairs;

	LinearAssigner assigner;

	int idx_det, idx_trk;

	frame_count += 1;

	data_preds.clear();
	data_preds_post.clear();

	if (trackers.size() == 0)
	{
		for (auto it = dets.begin(); it != dets.end(); it++)
		{
			KalmanBoxTracker tracker = KalmanBoxTracker(*it);
			trackers.push_back(tracker);
		}
		goto GEN_RST;
	}

	for (auto it = trackers.begin(); it != trackers.end();)
	{
		TrackBox tbox = (*it).predict();
		if (tbox.bbox.x >= 0 && tbox.bbox.y >= 0 && tbox.bbox.area() > 0)
		{
			data_preds.push_back(tbox);
			it++;
		}
		else
		{
			it = trackers.erase(it);
			n_trk -= 1;
			// cerr << "Box invalid at frame: " << frame_count << endl;
		}
	}

	iou_matrix.resize(n_trk, vector<double>(n_det, 0));

	for (unsigned int i = 0; i < n_trk; i++)
	{
		for (unsigned int j = 0; j < n_det; j++)
		{
			iou_matrix[i][j] = 1 - calc_iou(data_preds[i].bbox, dets[j].bbox);
		}
	}

	assigner.Solve(iou_matrix, assignment);

	if (n_det > n_trk)
	{
		for (int i = 0; i < n_det; i++)
			idx_all_dets.insert(i);

		for (int i = 0; i < n_trk; ++i)
			idx_mts_dets.insert(assignment[i]);

		set_difference(idx_all_dets.begin(), idx_all_dets.end(),
					   idx_mts_dets.begin(), idx_mts_dets.end(),
					   insert_iterator<set<int>>(idx_um_dets, idx_um_dets.begin()));
	}
	else if (n_det < n_trk)
	{
		for (int i = 0; i < n_trk; ++i)
		{
			if (assignment[i] == -1)
				idx_um_trks.insert(i);
		}
	}

	for (int i = 0; i < n_trk; ++i)
	{
		if (assignment[i] == -1)
			continue;
		if (1 - iou_matrix[i][assignment[i]] < iou_threshold)
		{
			idx_um_trks.insert(i);
			idx_um_dets.insert(assignment[i]);
		}
		else
			mt_pairs.push_back(cv::Point(i, assignment[i]));
	}

	for (int i = 0; i < mt_pairs.size(); i++)
	{
		idx_trk = mt_pairs[i].x;
		idx_det = mt_pairs[i].y;
		trackers[idx_trk].update(dets[idx_det]);
	}

	for (auto idx_um_det : idx_um_dets)
	{
		KalmanBoxTracker tracker = KalmanBoxTracker(dets[idx_um_det]);
		trackers.push_back(tracker);
	}

GEN_RST:
	for (auto it = trackers.begin(); it != trackers.end();)
	{
		if (((*it).time_since_update < 1) &&
			((*it).hit_streak >= min_hits || frame_count <= min_hits))
		{
			TrackBox res;
			res.bbox = (*it).get_state();
			res.track_id = (*it).id + 1;
			res.frame_id = frame_count;
			data_preds_post.push_back(res);
		}

		if (it != trackers.end() && (*it).time_since_update > max_age)
		{
			it = trackers.erase(it);
			n_trk -= 1;
		}
		else
		{
			it++;
		}
	}
}

SwiftTracker::SwiftTracker()
{
	max_age = 1;
	min_hits = 3;
	iou_threshold = 0.3;
	frame_count = 0;
}