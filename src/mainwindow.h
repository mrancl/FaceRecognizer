#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt
#include <QMainWindow>
#include <QScreen>
#include <QTimer>
#include <QHBoxLayout>
#include <QThread>
#include <QMetaType>
#include <QDesktopWidget>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QWidgetAction>

//OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

//Local
#include "utilities.h"
#include "actionbar.h"
#include "camera.h"
#include "facerecognition.h"
#include "selectionwidget.h"
#include "searchlineedit.h"
#include "customcompleter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *ev);


private:
    Ui::MainWindow *ui;

    AndroidActionBar *actionBar;
    SearchLineEdit *searchLine;

    Camera *cam;
    QThread *cameraThread; //Worker thread for capturing frames from camera
    QImage frame;
    cv::Mat currentFrame;

    FaceRecognition *faceRecThread; //Worker thread for recognizing faces
    QFuture<void> future; //For loading data from database and training face recognition model on separate thread

    /**
     * Variable declaration, needed for face recognition
     */
    QSqlDatabase facedb;
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    QStringList names;

    std::vector<cv::Mat> newImages;
    std::vector<int> newLabels;

    std::vector<int> searchLabels;

    bool recognizeFaces;
    cv::Ptr<cv::FaceRecognizer> model;

    std::vector<cv::Rect> detectedFaces;
    QStringList detectedNames;

    SelectionWidget *addFrameLabel;

    QScreen *screen;

    enum Page { MainPage, AddPersonPage, SearchPage, PhotosPage, SettingsPage };
    Page page;

    /** Used to control the size of icons in menu items. */
    MenuStyle menuStyle;

    int numberImages;

    void drawRectangle(cv::Mat img,
                       cv::Rect corners, cv::Scalar color, QString name);

    void showDetectedFaces(cv::Mat frame);

    void loadData();

    void addPerson();

private slots:

    void newFrame(const std::vector<cv::Rect> &faces, const QStringList &labels);
    void updateFrame(const cv::Mat& currentFrame);
    void openPage();
    void findFaces();
    void actionMain();
    void actionSearch();
    void actionAddPerson();
    void actionDeletePerson();
    void actionHelp();
    void actionSettings();
    void actionDeleteImage(const QPoint &pos);
    void showContextMenu(const QPoint &pos);
    void on_captureButton_clicked();
    void on_doneButton_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_thresholdLine_textChanged(const QString &arg1);
};

#endif // MAINWINDOW_H
