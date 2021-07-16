/*
USER MANUAL
space - pause/resume
q, scroll up   - zoom in
e, scroll down - zoom out
r              - reset zoom
wasd - pan
1 - switch to #1 video
2 - switch to #2 video
3 - switch to horitonzal view
4 - switch to vertical view
z - speed down
x - speed reset
c - speed up
*/

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String keys =

    "{help h usage ?  |      | print this message   }"
    "{@1              |<none>| path to 1st video}"
    "{@2              |<none>| path to 2nd video}"
    "{view v          |3     | initial view}";

constexpr auto WINDOW_NAME = "Video Compare by Tan SU";

enum class View { Unknown = 0, V1 = 1, V2 = 2, Horizontal = 3, Vertial = 4 };
enum class Status { Stop = 0, Play = 1 };

void PausePlay(Status &s) {
    s = s == Status::Stop ? Status::Play : Status::Stop;
}

class Timer {
public:
    Timer() { tick = static_cast<double>(cv::getTickCount()); }
    double Pass() { return (cv::getTickCount() - tick) * 1000 / cv::getTickFrequency(); }
    void Reset() { tick = static_cast<double>(cv::getTickCount()); }

private:
    double tick = 0;
};

int main(int argc, char *argv[]) {
    auto parser = CommandLineParser(argc, argv, keys);
    if (!parser.check() || !parser.has("@1") || !parser.has("@2")) {
        parser.printMessage();
        return -1;
    }
    string file1 = parser.get<string>("@1");
    string file2 = parser.get<string>("@2");
    auto cap1 = VideoCapture(file1);
    auto cap2 = VideoCapture(file2);
    if (!cap1.isOpened() || !cap2.isOpened()) {
        printf("can't open video. ");
        return -1;
    }
    Mat v1, v2;
    auto view = static_cast<View>(parser.get<int>("view"));
    Status status = Status::Play;

    namedWindow(WINDOW_NAME, WINDOW_NORMAL);
    Mat frame, frame1, frame2;
    int original_delay = 1000 / cap1.get(CAP_PROP_FPS);
    int delay = original_delay;

    while (true) {
        // Read Frame
        Timer t;
        if (status == Status::Play) {
            if (!cap1.read(v1)) {
                break;
            }
            if (!cap2.read(v2)) {
                break;
            }
        }
        int height = max(v1.rows, v2.rows);
        int width = max(v1.cols, v2.cols);
        int channel = max(v1.channels(), v2.channels());

        // Zoom

        // Process view
        switch (view) {
        case View::V1:
            frame.create(height, width, CV_8UC(channel));
            resize(v1, frame, frame.size());
            break;
        case View::V2:
            frame.create(height, width, CV_8UC(channel));
            resize(v2, frame, frame.size());
            break;
        case View::Horizontal:
            frame.create(height, width * 2, CV_8UC(channel));
            frame1 = frame(Rect(0, 0, width, height));
            frame2 = frame(Rect(width, 0, width, height));
            resize(v1, frame1, frame1.size());
            resize(v2, frame2, frame2.size());
            break;
        case View::Vertial:
            frame.create(height * 2, width, CV_8UC(channel));
            frame1 = frame(Rect(0, 0, width, height));
            frame2 = frame(Rect(0, height, width, height));
            resize(v1, frame1, frame1.size());
            resize(v2, frame2, frame2.size());
            break;
        case View::Unknown:
        default:
            break;
        }

        // display
        cv::imshow(WINDOW_NAME, frame);
        int key = 0;
        switch (status) {
        case Status::Stop:
            key = cv::waitKey();
            break;
        case Status::Play:
            key = cv::waitKey(max(delay - static_cast<int>(t.Pass()), 1));
            break;
        default:
            break;
        }
        // handle key
        // pause play
        if (key == ' ') {
            PausePlay(status);
        }
        // view
        else if (key == '1') {
            view = View::V1;
        }
        else if (key == '2') {
            view = View::V2;
        }
        else if (key == '3') {
            view = View::Horizontal;
        }
        else if (key == '4') {
            view = View::Vertial;
        }
        // speed
        else if (key == 'z') {
            delay = delay * 2;
        }
        else if (key == 'x') {
            delay = original_delay;
        }
        else if (key == 'c') {
            delay = delay * 0.5;
        }
        // quit
        else if (key == 'q') {
            break;
        }
        float percent = cap1.get(CAP_PROP_POS_FRAMES) / cap1.get(CAP_PROP_FRAME_COUNT) * 100.0;
        float fps = 1000 / t.Pass();
        printf("\rpercent: %03.2f%%  fps: %f", percent, fps);
    }
}
