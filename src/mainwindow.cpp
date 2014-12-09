#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background:lightGray;");


    QApplication::setApplicationName("Face Recognizer");
    ui->centralLayout->setContentsMargins(0, 0, 0, 0);
    ui->addPageLayout->setContentsMargins(0, 0, 0, 0);
    ui->searchLayout->setContentsMargins(0, 0, 0, 0);
    ui->mainPageLayout->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->imagesLayout->setContentsMargins(0, 0, 0, 0);

    /**
     * Action bar
     */

    ui->actionBarLayout->setMargin(0);
    ui->actionBarLayout->setSizeConstraint(QLayout::SetNoConstraint);

    actionBar = new AndroidActionBar(this);
    ui->actionBarLayout->addWidget(actionBar);

    searchLine = new SearchLineEdit(this);
    QMenu *menu = new QMenu(this);
    QWidgetAction *searchBoxAction = new QWidgetAction(this);
    searchBoxAction->setDefaultWidget(searchLine);
    menu->addAction(searchBoxAction);

    QAction *searchAction = new QAction(QIcon(":icons/search"), "Search", this);
    actionBar->addButton(searchAction, menu);

    QAction *settingsAction = new QAction(QIcon(":icons/settings"), "Settings", this);
    actionBar->addButton(settingsAction);

    QAction *addAction = new QAction(QIcon(":icons/add"), "Add Person", this);
    actionBar->addNavigation(addAction);

    QAction *deleteAction = new QAction(QIcon(":icons/discard"), "Delete Person", this);
    actionBar->addNavigation(deleteAction);

    QAction *menuSearchAction = new QAction(QIcon(":icons/search"), "Find Person", this);
    actionBar->addNavigation(menuSearchAction);

    QAction *helpAction = new QAction(QIcon(":icons/help"), "Help", this);
    actionBar->addNavigation(helpAction);

    actionBar->addSeparator(helpAction);

    actionBar->setTitle("Face Recognizer", false);

    //Connect signals for the navigation bar
    connect(actionBar, &AndroidActionBar::up, this, &MainWindow::actionMain);
    connect(menuSearchAction, &QAction::triggered, this, &MainWindow::actionSearch);
    connect(addAction, &QAction::triggered, this, &MainWindow::actionAddPerson);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::actionDeletePerson);
    connect(helpAction, &QAction::triggered, this, &MainWindow::actionHelp);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::actionSettings);

    // Set page as MainPage
    actionMain();

    // Allow QScreen::orientation to trigger orientation changed signal
    screen = QApplication::primaryScreen();
    screen->setOrientationUpdateMask(Qt::PortraitOrientation | Qt::LandscapeOrientation |
                                     Qt::InvertedPortraitOrientation | Qt::InvertedLandscapeOrientation);

    /**
     * Settings page
     */

    ui->verticalLayout->addStretch();
    ui->cameraLayout->addStretch();
    ui->faceDetectionLayout->addStretch();
    ui->faceRecognitionLayout->addStretch();


    /**
     * Add person page
     */

    addFrameLabel = new SelectionWidget(this);
    addFrameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->frameLayout->addWidget(addFrameLabel);


    /**
     * Load data set from Sqlite database
     */


//    QFile dfile("assets:/faces_training_set.sqlite"); - for android
//    if (dfile.exists())
//    {
//        dfile.copy("./faces_training_set.sqlite");
//        QFile::setPermissions("./faces_training_set.sqlite", QFile::WriteOwner | QFile::ReadOwner);
//    }

//    facedb = QSqlDatabase::addDatabase("QSQLITE");
//    facedb.setDatabaseName("./faces_training_set.sqlite");

    facedb = QSqlDatabase::addDatabase("QSQLITE");
    facedb.setDatabaseName("../facerecognition/android/assets/faces_training_set.sqlite");

    future = QtConcurrent::run(this, &MainWindow::loadData);


    /**
     * Camera thread
     */

    cam = new Camera(0, 0, 720, 480);
    cameraThread = new QThread;
    cam->moveToThread(cameraThread);

    //Connect camera with attached thread
    connect(cameraThread, SIGNAL(started()), cam, SLOT(start()));
    connect(cam, SIGNAL(stopped()), cameraThread, SLOT(quit()));
    connect(cam, SIGNAL(stopped()), cam, SLOT(deleteLater()));
    connect(cameraThread, SIGNAL(finished()), cameraThread, SLOT(deleteLater()));

    //Connect camera with main thread
    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(cam, SIGNAL(camFrame(cv::Mat)), this, SLOT(updateFrame(cv::Mat)));

    cameraThread->start();
    cameraThread->setPriority(QThread::TimeCriticalPriority);


    /**
     * Face recognition thread
     */

    faceRecThread = NULL;

    numberImages = 0;

    // Provide search suggestions
    CustomCompleter *completer = new CustomCompleter(names, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(5);
    searchLine->setCompleter(completer);
    connect(searchLine, &QLineEdit::returnPressed, this, &MainWindow::openPage);
}

MainWindow::~MainWindow()
{
    facedb.close();
    delete actionBar;

    future.cancel();
    future.waitForFinished();

    cameraThread->quit();
    cameraThread->wait();
    delete cameraThread;
    delete cam;


    if(faceRecThread != NULL)
    {
        faceRecThread->quit();
        faceRecThread->wait();
    }

    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    int key = ev->key();

    // The menu key has no function by default.
    // But Android users expect that the menu button opens the overflow menu.
    if(key == Qt::Key_Menu)
    {
        ev->accept();
        actionBar->openOverflowMenu();
        return;
    }

    // The Qt Framework closes the app when the back button is pressed.
    // But Android users expect that the back button navigates to the previous page.
    if(key == Qt::Key_Back && page != MainPage)
    {
        ev->accept();
        actionMain();
        return;
    }

    // Let QWidget process all other keys
    QWidget::keyPressEvent(ev);
}

void MainWindow::drawRectangle(cv::Mat img, cv::Rect corners, cv::Scalar color, QString name)
{
    cv::rectangle(img, corners, color, 4, 8, 0);
    cv::putText(img, name.toStdString(), cv::Point(corners.x, corners.y - 15),
                cv::FONT_HERSHEY_DUPLEX, 1, color, 1);
}

void MainWindow::showDetectedFaces(cv::Mat frame)
{
    if(!detectedNames.isEmpty() && detectedFaces.size() > 0)
    {
        cv::RNG rng(12345);
        for (int i = 0; i < detectedNames.size(); ++i)
        {
            cv::Scalar color(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
            drawRectangle(frame, detectedFaces[i], color, detectedNames[i]);
        }

    }
}

void MainWindow::loadData()
{
    facedb.open();
    if(!facedb.isOpen())
    {
        QMessageBox::warning(NULL, "Unable to open database", "Face Recognizer is unable to open the database of training images");
    }
    else
    {
        QString queryText = "SELECT a.name, b.personID, b.image FROM people AS a, training_images AS b WHERE a.personID = b.personID";

        QSqlQuery query;
        query.exec(queryText);

        QString name;

        while (query.next())
        {
            if(name != query.value(0).toString())
            {
                names.append(query.value(0).toString());
                name = query.value(0).toString();
            }
            labels.push_back(query.value(1).toInt());
            cv::Mat image = Utilities::ByteArrayToMat(query.value(2).toByteArray());
            images.push_back(image);

        }

        model = cv::createLBPHFaceRecognizer();


        if(images.size() < 2 || labels.size() < 2)
        {
            recognizeFaces = false;
        }
        else
        {
            model->train(images, labels);
            model->save("model.xml");
            recognizeFaces = true;
        }

        ui->resultsList->addItems(names);
    }
}

void MainWindow::addPerson()
{
    QRect selectionRect = addFrameLabel->getSelectionRect();
    QImage training_image = addFrameLabel->pixmap()->copy(selectionRect).toImage();

    cv::Mat training_image_mat = Utilities::QImageToMat(training_image);
    cv::Mat gray_image;
    cv::cvtColor(training_image_mat, gray_image, CV_BGR2GRAY);

    std::vector<cv::Rect> faces;

    cv::CascadeClassifier cascade;
    bool cascadeLoaded = Utilities::loadFaceCascade("../facerecognition/android/assets/haarcascade_frontalface_alt.xml", cascade);
    if(cascadeLoaded)
    {
        cascade.detectMultiScale(gray_image, faces, 1.1, 5, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
        if(faces.size() == 1)
        {
            cv::Mat face = gray_image(faces[0]);
            cv::Mat face_resized;
            cv::resize(face, face_resized, cv::Size(100, 100));

            QSqlQuery query;

            if(ui->nameLine->text().isEmpty())
            {
                QMessageBox::warning(this, "Name field empty", "The name field is empty. Please enter a name");
                return;
            }

            int newId;
            query.exec("SELECT MAX(personID) FROM people");

            if(numberImages == 0)
            {
                if(query.next())
                {
                    newId = query.value(0).toInt() + 1;
                }
                query.exec(QString("INSERT INTO people VALUES('%1', '%2')").arg(newId).arg(ui->nameLine->text()));
                ui->nameLine->setReadOnly(true);

            }
            else
            {
                if(query.next())
                {
                    newId = query.value(0).toInt();
                }

            }

            newImages.push_back(face_resized);
            newLabels.push_back(newId);

            QByteArray data = Utilities::MatToByteArray(face_resized);

            int imageId;

            query.exec("SELECT MAX(imageID) FROM training_images");
            if(query.next())
            {
                imageId = query.value(0).toInt() + 1;
            }

            query.prepare(QString("INSERT INTO training_images VALUES('%1', '%2', ?)").arg(imageId).arg(newId));
            query.addBindValue(data);
            query.exec();

            numberImages++;
        }
        else if(faces.empty())
        {
            QMessageBox::information(this, "No face detected", "Make sure your face is visible");
        }
        else
        {
            QMessageBox::information(this, "Multiple faces", "More than one face detected");
        }
    }

}

void MainWindow::newFrame(const std::vector<cv::Rect> &faces, const QStringList &labels)
{
    detectedFaces = faces;
    detectedNames = labels;
}

void MainWindow::updateFrame(const cv::Mat &currentFrame)
{
    cv::Mat currentFramePortrait;

    this->currentFrame = currentFrame;

    // Display frame
    QPixmap pixmap;

    int frameWidth;
    int frameHeight;
    if(page == MainPage)
    {
        frameWidth = ui->frameLabel->width();
        frameHeight = ui->frameLabel->height();
    }
    else
    {
        frameWidth = addFrameLabel->width();
        frameHeight = addFrameLabel->height();
    }

    if(recognizeFaces)
    {
        if(faceRecThread == NULL || (faceRecThread != NULL && faceRecThread->isFinished()))
        {
            findFaces();
        }

        if(detectedFaces.size() > 0)
        {
            showDetectedFaces(currentFrame);
        }
    }

    QMatrix rm;

    Qt::ScreenOrientation orientation = screen->orientation();
    switch(orientation)
    {
    case Qt::LandscapeOrientation:
        frame = Utilities::MatToQImage(this->currentFrame);
        pixmap = QPixmap::fromImage(frame).scaled(frameWidth, frameHeight);
        break;
    case Qt::InvertedLandscapeOrientation:
        rm.rotate(180);
        frame = Utilities::MatToQImage(this->currentFrame);
        frame = frame.transformed(rm);
        pixmap = QPixmap::fromImage(frame).scaled(frameWidth, frameHeight);
        break;
    case Qt::PortraitOrientation:
        cv::transpose(this->currentFrame, currentFramePortrait);
        frame = Utilities::MatToQImage(currentFramePortrait);
        pixmap = QPixmap::fromImage(frame).scaled(frameWidth, frameHeight);
        break;
    case Qt::InvertedPortraitOrientation:
        rm.rotate(180);
        cv::transpose(this->currentFrame, currentFramePortrait);
        frame = Utilities::MatToQImage(currentFramePortrait);
        frame = frame.transformed(rm);
        pixmap = QPixmap::fromImage(frame).scaled(frameWidth, frameHeight);
        break;
    }

    if(page == MainPage)
    {
        ui->frameLabel->setPixmap(pixmap);
    }
    else
    {
        addFrameLabel->setPixmap(pixmap);
    }


}

void MainWindow::findFaces()
{
    if(faceRecThread == NULL)
    {
        faceRecThread = new FaceRecognition(currentFrame, images, labels, names, model);

        //Connect face recognition thread with main thread
        qRegisterMetaType<std::vector<cv::Rect>>("std::vector<cv::Rect>");
        qRegisterMetaType<QStringList>("QStringList");
        connect(faceRecThread, SIGNAL(modifiedFrame(std::vector<cv::Rect>, QStringList)),
                this, SLOT(newFrame(std::vector<cv::Rect>, QStringList)));
    }
    else
        faceRecThread->setFrame(currentFrame);

    faceRecThread->start();
}

void MainWindow::actionMain()
{
    page = MainPage;
    if(images.size() > 0)
    {
        recognizeFaces = true; // Only do face detection/recognition if on main page
    }

    // Set the application title and disable up-navigation.
    actionBar->setTitle(QApplication::applicationDisplayName(), false);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
    if(!searchLabels.empty())
    {
        ui->imageList->clear();
        searchLabels.clear();
    }
}

void MainWindow::openPage()
{
    page = PhotosPage;

    QAction *addImageAction = new QAction(QIcon(":icons/add"), "Add Image", this);
    actionBar->addButton(addImageAction, 0, 0);

    if(!searchLabels.empty())
    {
        ui->imageList->clear();
        searchLabels.clear();
    }

    actionBar->setTitle(searchLine->text(), true);
    ui->stackedWidget->setCurrentWidget(ui->photosPage);
    recognizeFaces = false;

    ui->imageList->setViewMode(QListWidget::IconMode);
    ui->imageList->setResizeMode(QListWidget::Adjust);
    ui->imageList->setIconSize(QSize(100, 100));
    ui->imageList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->imageList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    QSqlQuery query;
    QString queryText = "SELECT b.imageID, b.image FROM people AS a, training_images AS b WHERE a.personID = b.personID AND a.name LIKE '%"+ searchLine->text()+"%'";
    query.exec(queryText);
    while(query.next())
    {
        searchLabels.push_back(query.value(0).toInt());
        cv::Mat img = Utilities::ByteArrayToMat(query.value(1).toByteArray());
        QPixmap pixmap = QPixmap::fromImage(Utilities::MatToQImage(img));
        ui->imageList->addItem(new QListWidgetItem(QIcon(pixmap), ""));

    }
}

void MainWindow::actionSearch()
{
    page = SearchPage;
    actionBar->setTitle("Search Person", true);
    ui->stackedWidget->setCurrentWidget(ui->searchPage);

}

void MainWindow::actionAddPerson()
{
    page = AddPersonPage;
    actionBar->setTitle("Add Person", true);
    ui->stackedWidget->setCurrentWidget(ui->addPage);
    recognizeFaces = false;
}

void MainWindow::actionDeletePerson()
{
    page = MainPage;
    actionBar->setTitle("Delete Person", true);
}

void MainWindow::actionHelp()
{
    page = MainPage;
    actionBar->setTitle("Help", true);
    recognizeFaces = false;
}

void MainWindow::actionSettings()
{
    page = SettingsPage;
    actionBar->setTitle("Settings", true);
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
    recognizeFaces = false;

}

void MainWindow::actionDeleteImage(const QPoint &pos)
{
    QListWidgetItem* temp = ui->imageList->itemAt(pos);
    int itemIndex = ui->imageList->row(temp);
    delete temp;

    QSqlQuery query;
    query.exec(QString("DELETE FROM training_images WHERE imageID = %1").arg(searchLabels[itemIndex]));

}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->imageList->mapToGlobal(pos);
    QListWidgetItem* temp = ui->imageList->itemAt(pos);
    if(temp != NULL)
    {
        QMenu menu;
        QAction *deleteImageAction = new QAction(QIcon(":icons/discard"), "Delete image", this);
        menu.addAction(deleteImageAction);
        QAction* selectedItem = menu.exec(globalPos);
        if (selectedItem)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete image", "Are you sure you want to delete this image?");
            if(reply == QMessageBox::Yes)
            {
                actionDeleteImage(pos);
            }

        }
    }
}

void MainWindow::on_captureButton_clicked()
{
    addPerson();
    if(numberImages > 0)
    {
        ui->doneButton->setEnabled(true);
    }
}

void MainWindow::on_doneButton_clicked()
{
    QString name = ui->nameLine->text();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Add person", "Do you wish to add " + name + " ?",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes)
    {
        images.insert(images.end(), newImages.begin(), newImages.end());
        labels.insert(labels.end(), newLabels.begin(), newLabels.end());
        names.push_back(name);
        model->update(newImages, newLabels);

    }
    else
    {
        QSqlQuery query;

        query.exec(QString("DELETE FROM people WHERE personID='%1'").arg(newLabels[0]));
        query.exec(QString("DELETE FROM training_images WHERE personID='%1'").arg(newLabels[0]));

    }

    newImages.clear();
    newLabels.clear();

    numberImages = 0;

    ui->nameLine->setReadOnly(false);
    ui->nameLine->clear();
    ui->doneButton->setEnabled(false);

    page = MainPage;
    ui->stackedWidget->setCurrentWidget(ui->mainPage);

}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->thresholdLine->setText(QString::number(value));
}

void MainWindow::on_thresholdLine_textChanged(const QString &arg1)
{
    int value = arg1.toInt();
    if(value != 0 && value <= ui->horizontalSlider->maximum() && value >= ui->horizontalSlider->minimum())
    {
        ui->horizontalSlider->setValue(arg1.toInt());
    }

}
