#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

//Qt
#include <QThread>
#include <QMessageBox>
#include <QImage>
#include <QtSql>
#include <QUrl>
#include <QDebug>
#include <QFile>

//OpenCV
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/opencv.hpp>

//Local
#include "utilities.h"

#include <vector>

class FaceRecognition : public QThread
{
    Q_OBJECT
public:
    FaceRecognition(cv::Mat frame, std::vector<cv::Mat> images,
                             std::vector<int> labels, QStringList names, cv::Ptr<cv::FaceRecognizer> model);
    ~FaceRecognition();

    void stop();

    cv::Mat getFrame() const;
    void setFrame(const cv::Mat &value);

private:

    //Needed for face detection/recognition
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    QStringList names;

    cv::CascadeClassifier faceCascade;
    cv::Ptr<cv::FaceRecognizer> model;

    std::vector<cv::Rect> faces;

    //Current frame captured from camera
    cv::Mat frame;

    int height;

    QMutex processingMutex;

    volatile bool doStop;
    QMutex doStopMutex;


protected:
    void run();

signals:
    void modifiedFrame(const std::vector<cv::Rect> &faces, const QStringList &names);

};

#endif // FACERECOGNITION_H
