#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>

//OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class Camera : public QObject
{
    Q_OBJECT
public:
    Camera(QObject *parent = 0, int camera = 0, int width = 640, int height = 480);
    ~Camera();

private:

    cv::VideoCapture capture;
    cv::Mat frame;
    QTimer *timer;


signals:
    void camFrame(const cv::Mat& frame);
    void started();
    void stopped();

public slots:
    void start();

};

#endif // CAMERA_H
