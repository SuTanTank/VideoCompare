#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String keys =

    "{help h usage ?  |      | print this message   }"
    "{@1              |<none>| path to 1st video}"
    "{@2              |<none>| path to 2nd video}"
    "{view v          |3     | initial view}";

constexpr auto ABOUT =
    "Video Compare Tool by Tan SU           \n"
    "--------- USER MANUAL -------------    \n"
    "                                       \n"
    "Play                                   \n"
    "    space - pause / resume             \n"
    "    esc - quit                         \n"
    "    left arrow(play) - 3 second back   \n"
    "    left arrow(pause) - 1 frame back   \n"
    "    right arrow(play) - 3 second ahead \n"
    "    right arrow(pause) - 1 frame ahead \n"
    "View                                   \n"
    "    q - zoom out                       \n"
    "    e - zoom in                        \n"
    "    wasd - pan all video               \n"
    //"    WASD - pan the first video in pixel\n"
    "    1 - switch to #1 video             \n"
    "    2 - switch to #2 video             \n"
    "    3 - switch to horitonzal view      \n"
    "    4 - switch to vertical view        \n"
    "    f - switch between scale and crop  \n"
    "        (when video size are different)\n"
    "Speed                                  \n"
    "    z - speed down                     \n"
    "    x - speed reset                    \n"
    "    c - speed up                       \n";

constexpr auto WINDOW_NAME = "Video Compare by Tan SU";
constexpr auto NORMAL_DELAY = 5;
constexpr auto ESC_KEY = 27;

enum class View { Unknown = 0, V1 = 1, V2 = 2, Horizontal = 3, Vertial = 4 };
enum class Status { Stop = 0, Play = 1 };
enum class Mode { Unknown = 0, Scale = 1, Crop = 2 };

void PausePlay(Status &s) {
    s = s == Status::Stop ? Status::Play : Status::Stop;
}

void CropScale(Mode &m) {
    m = m == Mode::Crop ? Mode::Scale : Mode::Crop;
}

class Timer {
public:
    Timer() { tick = static_cast<double>(cv::getTickCount()); }
    double Pass() {
        auto pass = (static_cast<double>(cv::getTickCount()) - tick) * 1000 / cv::getTickFrequency();
        return pass;
    }
    void Reset() { tick = static_cast<double>(cv::getTickCount()); }

private:
    double tick = 0;
};

string MakeTitle(const string &prefix, const string &file1, const string &file2) {
    string title = prefix + " - [1]" + file1 + "  [2]" + file2;
    return title;
}

int main(int argc, char *argv[]) {
    auto parser = CommandLineParser(argc, argv, keys);
    parser.about(ABOUT);
    parser.printMessage();
    if (!parser.check() || !parser.has("@1") || !parser.has("@2")) {
        printf("press any key to exit.");
        getchar();
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
    Mat v1, v2, v1_o, v2_o;
    auto view = static_cast<View>(parser.get<int>("view"));
    Status status = Status::Play;
    Mode crop_scale = Mode::Scale;
    auto zoom = 1.;
    auto center = Point2f(0.5f, 0.5f);
    Point offset(0, 0);
    string window_title = MakeTitle(WINDOW_NAME, file1, file2);
    namedWindow(window_title, WINDOW_NORMAL);
    Mat frame, frame1, frame2;
    int original_delay = 1000 / cap1.get(CAP_PROP_FPS) + 0.5;
    int delay = original_delay;

    Rect roi;
    int frame_height = 0;
    int frame_width = 0;

    Timer t, t_render;
    while (true) {
        // Read Frame
        if (status == Status::Play) {
            if (!cap1.read(v1_o) || !cap2.read(v2_o)) {
                status = Status::Stop;
                cap1.release();
                cap2.release();
                cap1.open(file1);
                cap2.open(file2);
            }
            else {
                v1 = v1_o;
                v2 = v2_o;
            }
        }
        if (v1.empty() || v2.empty()) {
            perror("failed to read frames. \n");
            break;
        }
        int channel = max(v1.channels(), v2.channels());
        int height, width;

        switch (crop_scale) {
        case Mode::Unknown:
            printf("?");
            break;
        case Mode::Crop:
            height = min(v1.rows, v2.rows);
            width = min(v1.cols, v2.cols);
            v1 = v1(Rect((width - v1.cols) / 2, (height - v1.rows) / 2, v1.cols, v1.rows));
            v2 = v2(Rect((width - v2.cols) / 2, (height - v2.rows) / 2, v2.cols, v2.rows));
            break;
        case Mode::Scale:
        default:
            height = max(v1.rows, v2.rows);
            width = max(v1.cols, v2.cols);
            resize(v1, v1, Size(width, height));
            resize(v2, v2, Size(width, height));
            break;
        }

        frame_height = height;
        frame_width = width;
        roi = Rect(0, 0, frame_width, frame_height);

        // Zoom and pan
        if (zoom > 1.001) {
            frame_height = height / zoom;
            frame_width = width / zoom;
            roi.x = center.x * width - frame_width * 0.5f;
            roi.y = center.y * height - frame_height * 0.5f;
            if (roi.x < 0) {
                center.x = frame_width * 0.5f / width;
                roi.x = 0;
            }
            if (roi.x + frame_width > width) {
                center.x = (width - frame_width * 0.5f) / width;
                roi.x = width - frame_width;
            }
            if (roi.y < 0) {
                center.y = frame_height * 0.5f / height;
                roi.y = 0;
            }
            if (roi.y + frame_height > height) {
                center.y = (height - frame_height * 0.5f) / height;
                roi.y = height - frame_height;
            }
            roi.height = frame_height;
            roi.width = frame_width;
        }

        // Process view
        switch (view) {
        case View::V1:
            frame.create(frame_height, frame_width, CV_8UC(channel));
            resize(v1(roi), frame, frame.size());
            break;
        case View::V2:
            frame.create(frame_height, frame_width, CV_8UC(channel));
            resize(v2(roi), frame, frame.size());
            break;
        case View::Horizontal:
            frame.create(frame_height, frame_width * 2, CV_8UC(channel));
            frame1 = frame(Rect(0, 0, frame_width, frame_height));
            frame2 = frame(Rect(frame_width, 0, frame_width, frame_height));
            resize(v1(roi), frame1, frame1.size());
            resize(v2(roi), frame2, frame2.size());
            break;
        case View::Vertial:
            frame.create(frame_height * 2, frame_width, CV_8UC(channel));
            frame1 = frame(Rect(0, 0, frame_width, frame_height));
            frame2 = frame(Rect(0, frame_height, frame_width, frame_height));
            resize(v1(roi), frame1, frame1.size());
            resize(v2(roi), frame2, frame2.size());
            break;
        case View::Unknown:
        default:
            break;
        }

        // display
        cv::imshow(window_title, frame);
        int key = 0;
        switch (status) {
        case Status::Stop:
            key = cv::waitKeyEx();
            break;
        case Status::Play:
            key = cv::waitKeyEx(max(delay - static_cast<int>(t.Pass() + NORMAL_DELAY), 1));
            t.Reset();
            break;
        default:
            break;
        }
        // handle key
#ifndef NDEBUG
        if (key >= 0) {
            cout << "key: " << key << endl;
        }
#endif
        // pause play
        if (key == ' ') {
            PausePlay(status);
        }
        else if (key == 'f') {
            CropScale(crop_scale);
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
        // zoom
        else if (key == 'q') {
            zoom /= 1.1f;
            if (zoom < 1) {
                zoom = 1.f;
            }
        }
        else if (key == 'e') {
            zoom *= 1.1f;
            if (zoom > 10) {
                zoom = 10.f;
            }
        }
        // pan
        else if (key == 'w') {
            center.y -= 0.05 / zoom;
        }
        else if (key == 's') {
            center.y += 0.05 / zoom;
        }
        else if (key == 'a') {
            center.x -= 0.05 / zoom;
        }
        else if (key == 'd') {
            center.x += 0.05 / zoom;
        }
        else if (key == 'W') {
            offset.y -= 1;
        }
        else if (key == 'S') {
            offset.y += 1;
        }
        else if (key == 'A') {
            offset.x -= 1;
        }
        else if (key == 'D') {
            offset.x += 1;
        }
#if WIN32
        else if (key / 0x10000 == 0x25) {
            if (status == Status::Play) {
                cap1.set(cv::CAP_PROP_POS_MSEC, cap1.get(cv::CAP_PROP_POS_MSEC) - 3000);
                cap2.set(cv::CAP_PROP_POS_MSEC, cap2.get(cv::CAP_PROP_POS_MSEC) - 3000);
            }
            else {
                cap1.set(cv::CAP_PROP_POS_FRAMES, cap1.get(cv::CAP_PROP_POS_FRAMES) - 2);
                cap2.set(cv::CAP_PROP_POS_FRAMES, cap2.get(cv::CAP_PROP_POS_FRAMES) - 2);
                cap1.read(v1);
                cap2.read(v2);
            }
        }
        else if (key / 0x10000 == 0x27) {
            if (status == Status::Play) {
                cap1.set(cv::CAP_PROP_POS_MSEC, cap1.get(cv::CAP_PROP_POS_MSEC) + 3000);
                cap2.set(cv::CAP_PROP_POS_MSEC, cap2.get(cv::CAP_PROP_POS_MSEC) + 3000);
            }
            else {
                if (cap1.read(v1_o) && cap2.read(v2_o)) {
                    v1 = v1_o;
                    v2 = v2_o;
                }
            }
        }
#endif
        // quit
        else if (key == ESC_KEY) {
            break;
        }
#ifndef NDEBUG
        cout << cap1.get(cv::CAP_PROP_POS_FRAMES) << endl;
#endif
        float percent = cap1.get(CAP_PROP_POS_FRAMES) / cap1.get(CAP_PROP_FRAME_COUNT) * 100.0;
        float fps = 1000 / t_render.Pass();
        t_render.Reset();
        printf("\rpercent: %03.2f%%  fps: %f", percent, fps);
    }
}
