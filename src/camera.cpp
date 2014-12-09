#include "camera.h"

Camera::Camera(QObject *parent, int camera, int width, int height) :
    QObject(parent)
{   
    //capture = cv::VideoCapture(CV_CAP_ANDROID + camera);
    capture = cv::VideoCapture(camera);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, width);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(start()));
    timer->start(10);
}

Camera::~Camera()
{
    capture.release();
}

void Camera::start()
{
    if(!capture.grab())
    {
        return;
    }

    capture.retrieve(frame);

    //Inform GUI of new frame
    emit camFrame(frame);
}

