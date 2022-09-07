#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "dataType.h"
#include "SwiftTracker.h"

using namespace cv;
using namespace std;

#define CNUM 20

int main()
{
    SwiftTracker tracker;

    vector<TrackBox> data_det;
    vector<vector<TrackBox>> data_det_sort;

    const string seq_name = "TUD-Campus";
    const string data_root_dir = "/media/manu/intem/sort/2DMOT2015/train/";
    const string data_img_dir = data_root_dir + seq_name + "/img1/";
    const string data_det_path = data_root_dir + seq_name + "/det/det.txt";

    bool flag_display = true;
    int num_frame = 0;

    RNG rng(0xFFFFFFFF);
    Scalar_<int> colors[CNUM];

    cout << "Swift Tracker Algorithm" << endl;

    cout << "processing " << seq_name << " ..." << endl;

    for (int i = 0; i < CNUM; i++)
    {
        rng.fill(colors[i], RNG::UNIFORM, 0, 255);
    }

    if (flag_display && (access(data_img_dir.c_str(), 0) == -1))
    {
        cerr << "can not find dir " << data_img_dir << endl;
        flag_display = false;
    }

    {
        ifstream fh_det;
        string line;
        istringstream ss;

        char sps;
        float x, y, w, h;

        fh_det.open(data_det_path);
        if (!fh_det.is_open())
        {
            cerr << "can not find file " << data_det_path << endl;
            return -1;
        }

        while (getline(fh_det, line))
        {
            TrackBox db;

            ss.str(line);
            ss >> db.frame_id >> sps >> db.track_id >> sps;
            ss >> x >> sps >> y >> sps >> w >> sps >> h;
            ss.str("");

            db.bbox = Rect_<float>(Point_<float>(x, y), Point_<float>(x + w, y + h));
            data_det.push_back(db);
        }
        fh_det.close();
    }

    for (auto db : data_det)
    {
        if (num_frame < db.frame_id)
            num_frame = db.frame_id;
    }

    {
        vector<TrackBox> vec_tmp;
        for (int i = 1; i <= num_frame; i++)
        {
            for (auto db : data_det)
                if (db.frame_id == i)
                    vec_tmp.push_back(db);
            data_det_sort.push_back(vec_tmp);
            vec_tmp.clear();
        }
    }

    for (int i = 1; i <= num_frame; i++)
    {
        // display detection bboxes
        if (flag_display)
        {
            ostringstream oss;
            oss << data_img_dir << setw(6) << setfill('0') << i;
            Mat img = imread(oss.str() + ".jpg");

            for (auto db : data_det_sort[i - 1])
            {
                assert(db.frame_id == i);
                cv::rectangle(img, db.bbox, colors[(db.track_id + 1) % CNUM], 2, 8, 0); // db.track_id + 1 prevent -1 % cnum
                // cout << "db.track_id -> " << db.track_id << " color idx -> " << (db.track_id + 1) % CNUM << endl;
            }
            imshow(seq_name, img);
        }

        tracker.update(data_det_sort[i - 1]);

        // display tracking bboxes

        if (flag_display)
            waitKey(40);
    }

    if (flag_display)
        destroyAllWindows();

    return 0;
}
