#include "facerecognition.h"

FaceRecognition::FaceRecognition(cv::Mat frame, std::vector<cv::Mat> images,
                                 std::vector<int> labels, QStringList names, cv::Ptr<cv::FaceRecognizer> model):
    QThread()
{
    this->frame = frame;
    this->images = images;
    this->labels = labels;
    this->names = names;
    this->model = model;

    if(!Utilities::loadFaceCascade("../facerecognition/android/assets/haarcascade_frontalface_alt.xml", faceCascade))
    {
        QMessageBox::warning(NULL, "No Haar Cascade", "Unable to load face haar cascade");
    }

    doStop = false;

}

FaceRecognition::~FaceRecognition()
{
    delete model;
}

void FaceRecognition::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop = true;
}
cv::Mat FaceRecognition::getFrame() const
{
    return frame;
}

void FaceRecognition::setFrame(const cv::Mat &value)
{
    frame = value;
}


void FaceRecognition::run()
{

    doStopMutex.lock();
    if(doStop)
    {
        doStop = false;
        doStopMutex.unlock();
    }
    doStopMutex.unlock();

    processingMutex.lock();

    model->set("threshold", 70.0);

    cv::Mat frame_gray;

    cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);

    height = images[0].rows;

    int predictionLabel;
    double confidence;

    QStringList predictedNames;

    //Detect faces
    faceCascade.detectMultiScale(frame_gray, faces, 1.1, 5, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

    for (int i = 0; i < faces.size(); ++i)
    {
        cv::Mat faceROI = frame_gray(faces[i]);
        model->predict(faceROI, predictionLabel, confidence);

        if(predictionLabel != -1)
        {
            predictedNames.append(names.at(predictionLabel - 1));
        }
        else
        {
            predictedNames.append("Unknown");
        }
    }

    processingMutex.unlock();

    //Inform GUI of detected faces
    emit modifiedFrame(faces, predictedNames);
}
