#ifndef UTILITIES_H
#define UTILITIES_H

//Qt
#include <QImage>
#include <QDebug>

//OpenCV
#include <opencv2/opencv.hpp>

namespace Utilities
{
    QImage MatToQImage(const cv::Mat &mat);
    cv::Mat QImageToMat( const QImage &inImage, bool inCloneImageData = true );
    QByteArray MatToByteArray(const cv::Mat &image);
    cv::Mat ByteArrayToMat(const QByteArray & byteArray);

    bool loadFaceCascade(const std::string &cascade, cv::CascadeClassifier &faceCascade);
}


#endif // UTILITIES_H
