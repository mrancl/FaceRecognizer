#include "utilities.h"


QImage Utilities::MatToQImage(const cv::Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
        if(mat.type() == CV_8UC1)
        {
            // Set the color table (used to translate colour indexes to qRgb values)
            QVector<QRgb> colorTable;
            for (int i=0; i<256; i++)
                colorTable.push_back(qRgb(i,i,i));
            // Copy input Mat
            const uchar *qImageBuffer = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
            img.setColorTable(colorTable);
            return img;
        }
        // 8-bits unsigned, NO. OF CHANNELS=3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            const uchar *qImageBuffer = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return img.rgbSwapped();
        }
        else
        {
            qDebug() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
}

cv::Mat Utilities::QImageToMat( const QImage &inImage, bool inCloneImageData)
   {
      switch ( inImage.format() )
      {
         // 8-bit, 4 channel
         case QImage::Format_RGB32:
         {
            cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

            return (inCloneImageData ? mat.clone() : mat);
         }

         // 8-bit, 3 channel
         case QImage::Format_RGB888:
         {
            if ( !inCloneImageData )
               qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

            QImage swapped = inImage.rgbSwapped();

            return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
         }

         // 8-bit, 1 channel
         case QImage::Format_Indexed8:
         {
            cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

            return (inCloneImageData ? mat.clone() : mat);
         }

         default:
            qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
            break;
      }

      return cv::Mat();
   }

bool Utilities::loadFaceCascade(const std::string &cascade, cv::CascadeClassifier &faceCascade)
{
    //QFile dfile("assets:/haarcascade_frontalface_alt.xml"); - for android

//    if (dfile.exists())
//    {
//         dfile.copy("haarcascade_frontalface_alt.xml");
//         QFile::setPermissions("haarcascade_frontalface_alt.xml", QFile::WriteOwner | QFile::ReadOwner);
//    }

    if(!faceCascade.load(cascade))
    {
        return false;
    }
    else
    {
        return true;
    }
}


QByteArray Utilities::MatToByteArray(const cv::Mat &image)
{
    QByteArray byteArray;
    QDataStream stream( &byteArray, QIODevice::WriteOnly );
    stream << image.type();
    stream << image.rows;
    stream << image.cols;
    const size_t data_size = image.cols * image.rows * image.elemSize();
    QByteArray data = QByteArray::fromRawData( (const char*)image.ptr(), data_size );
    stream << data;
    return byteArray;
}


cv::Mat Utilities::ByteArrayToMat(const QByteArray &byteArray)
{
    QDataStream stream(byteArray);
    int matType, rows, cols;
    QByteArray data;
    stream >> matType;
    stream >> rows;
    stream >> cols;
    stream >> data;
    cv::Mat mat( rows, cols, matType, (void*)data.data() );
    return mat.clone();

}
