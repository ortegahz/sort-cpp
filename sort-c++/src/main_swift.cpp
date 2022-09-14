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

int KalmanBoxTracker::count = 0;

int main()
{
    bool flag_display = false;
    bool flag_save_video = false;

    const string path_video_out = "/home/manu/tmp/tracking.flv";
    const string data_root_dir = "/media/manu/intem/sort/2DMOT2015/train/";
    const string data_root_dir_det = "/media/manu/kingstop/workspace/sort/data/train/";

    // vector<string> name_seqs = {"PETS09-S2L1", "TUD-Campus", "TUD-Stadtmitte", "ETH-Bahnhof", "ETH-Sunnyday", "ETH-Pedcross2", "KITTI-13", "KITTI-17", "ADL-Rundle-6", "ADL-Rundle-8", "Venice-2"};

    vector<string> name_seqs = {"ETH-Pedcross2"};

    for (auto seq_name : name_seqs)
    {
        double total_time = 0.0;
        double cycle_time = 0.;
        double start_time = 0.;

        VideoWriter hd_vw;
        // int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        int codec = VideoWriter::fourcc('F', 'L', 'V', '1');

        SwiftTracker tracker;

        vector<TrackBox> data_det;
        vector<vector<TrackBox>> data_det_sort;

        if (flag_save_video)
        {
            hd_vw.open(path_video_out, codec, 25, Size(1280, 720));
            if (!hd_vw.isOpened())
            {
                cerr << "Could not open the output video file for write\n";
                return -1;
            }
        }

        const string data_img_dir = data_root_dir + seq_name + "/img1/";
        const string data_det_path = data_root_dir_det + seq_name + "/det/det.txt";

        ofstream hd_out_file;
        string name_out_file = "/home/manu/tmp/data/trackers/mot_challenge/MOT15-train/SWIFTTrack/data/" + seq_name + ".txt";
        hd_out_file.open(name_out_file);
        if (!hd_out_file.is_open())
        {
            cerr << "Error: can not create file " << name_out_file << endl;
        }

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
                // cout << db.bbox.x << " " << db.bbox.y << " " << db.bbox.width << " " << db.bbox.height << endl;
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
            Mat img;

            if (flag_display)
            {
                ostringstream oss;
                oss << data_img_dir << setw(6) << setfill('0') << i;
                img = imread(oss.str() + ".jpg");
            }

            // // display detection bboxes
            // if (flag_display)
            // {
            //     Scalar color(0, 0, 255);
            //     for (auto db : data_det_sort[i - 1])
            //     {
            //         assert(db.frame_id == i);
            //         cv::rectangle(img, db.bbox, color, 2, 8, 0);
            //     }
            // }

            start_time = getTickCount();
            tracker.update(data_det_sort[i - 1]);
            cycle_time = (double)(getTickCount() - start_time);
            total_time += cycle_time / getTickFrequency();

            for (auto tb : tracker.data_preds_post)
            {
                hd_out_file << tb.frame_id << "," << tb.track_id << "," << tb.bbox.x << "," << tb.bbox.y << "," << tb.bbox.width << "," << tb.bbox.height << ",1,-1,-1,-1" << endl;
            }

            // display tracking bboxes
            if (flag_display)
            {
                for (auto tb : tracker.data_preds_post)
                {
                    string text = "tid " + to_string(tb.track_id);
                    Point pos;
                    pos.x = tb.bbox.x;
                    pos.y = tb.bbox.y;
                    putText(img, text, pos, FONT_HERSHEY_COMPLEX, 1, colors[tb.track_id % CNUM], 2, 8, 0);
                    rectangle(img, tb.bbox, colors[tb.track_id % CNUM], 2, 8, 0);
                    // cout << tb.track_id << endl;
                }
            }

            // display tracking bboxes

            if (flag_display)
            {
                imshow(seq_name, img);
                waitKey(10);
            }
            if (flag_save_video)
            {
                hd_vw.write(img);
            }
        }

        cout << "total_time " << total_time << " for " << num_frame << " frames" << endl;
        cout << (double(num_frame) / total_time) << " fps" << endl;

        if (flag_display)
            destroyAllWindows();
        if (flag_save_video)
        {
            hd_vw.release();
        }

        hd_out_file.close();
    }

    return 0;
}
