
#include <QSettings>
#include <QCoreApplication>
#include <QFile>

#include "ifiServer.h"

ImageServiceWork::ImageServiceWork()
{
#ifdef MATCH_TOOL
    String modelConfiguration = "D:/Users/sirius/Desktop/yolo/yolov2-tiny-voc.cfg";
    String modelBinary = "D:/Users/sirius/Desktop/yolo/yolov2-tiny-voc_final.weights";

    dnn::Net net = readNetFromDarknet(modelConfiguration, modelBinary);
    if (net.empty())
    {
        printf("Could not load net...\n");
        return;
    }
    vector<string> classNamesVec;
    ifstream classNamesFile("D:/Users/sirius/Desktop/yolo/voc.names");
    if (classNamesFile.is_open())
    {
        string className = "";
        while (std::getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }
#endif
}

ImageServiceWork::~ImageServiceWork()
{

}

bool ImageServiceWork::Work(qtMessage* pMsg)
{
    QString strPath(pMsg->m_data);

    strPath.remove(0, 8);


    QString strSerial = strPath.right(12);
    strSerial = strSerial.left(8);

    qDebug() << strSerial;
    QString strPlate;

#ifdef FIND_PLATE
    if(strSerial.contains("22637684"))
    {
        bool bRes = FindPlate(strPath, strPlate);
        if(bRes)
        {
            qDebug() << "find the plate:" << strPlate;
            //保存文件
        }
        else
        {
            //删除文件
        }
    }
#elif MATCH_TOOL
    matchTool(strPath, strPlate);
#endif

    return true;
}

bool ImageServiceWork::FindPlate(const QString& strPath, QString& strPlate)
{
    cv::Mat image = cv::imread(strPath.toLatin1().data());

    char port;
    vector<char> plate;
    bool IsSuccess;

    IplImage* pImage = &IplImage(image);

    platereco_HSV(pImage, plate, port);
    foreach (char c, plate) {
        strPlate.append(c);
    }

    if(strPlate.isEmpty())
    {
        IsSuccess = false;
    }
    else
    {
        if((int)port == 1 || (int)port == 2 || port == '1' || port == '2')
        {
            strPlate = strPlate.left(strPlate.length() - (int)(port-'0'));
            qDebug() << "Plate:" << strPlate << "success!";
            IsSuccess = true;
        }
    }

//    Mat img_h, img_s, img_v, imghsv;
//    vector<cv::Mat> hsv_vec;
//    cvtColor(image, imghsv, CV_BGR2HSV);
//    // 分割hsv通道
//    split(imghsv, hsv_vec);
//    img_h = hsv_vec[0];
//    int nl = img_h.rows; //行数
//    int nc = img_h.cols * img_h.channels();
//    for (int j = 0; j<nl; j++)
//    {
//        uchar* data = img_h.ptr<uchar>(j);
//        for (int i = 0; i<nc; i++)
//        {
//            if (data[i]>120)
//            {
//                data[i] = 255;
//            }
//            else
//            {
//                data[i] = 0;
//            }

//        }
//    }

//    cv::Mat binaryimage_mat;
//    Mat element_1 = getStructuringElement(MORPH_RECT, Size(10, 5));
//    morphologyEx(img_h,binaryimage_mat, MORPH_DILATE, element_1);
//    cv::imshow("ROI_DILATE_image", binaryimage_mat);
//    //cvShowImage("ROI_DILATE_image", ROI_DILATE_image);


    return true;
}

bool matchTool(const QString& strPath, QString& strName)
{
    cv::Mat image = cv::imread(strPath.toLatin1().data());
#ifdef MATCH_TOOL
    Mat inputBlob = blobFromImage(image, 1 / 255.F, Size(416, 416), Scalar(), true, false);
    net.setInput(inputBlob, "data");

    Mat detectionMat = net.forward("detection_out");
    vector<double> layersTimings;
    double freq = getTickFrequency() / 1000;
    double time = net.getPerfProfile(layersTimings) / freq;
    ostringstream ss;
    ss << "detection time: " << time << " ms";
    putText(frame, ss.str(), Point(20, 20), 0, 0.5, Scalar(0, 0, 255));


    for (int i = 0; i < detectionMat.rows; i++)
    {
        const int probability_index = 5;
        const int probability_size = detectionMat.cols - probability_index;
        float *prob_array_ptr = &detectionMat.at<float>(i, probability_index);
        size_t objectClass = max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
        float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);
        if (confidence > confidenceThreshold)
        {
            float x = detectionMat.at<float>(i, 0);
            float y = detectionMat.at<float>(i, 1);
            float width = detectionMat.at<float>(i, 2);
            float height = detectionMat.at<float>(i, 3);
            int xLeftBottom = static_cast<int>((x - width / 2) * frame.cols);
            int yLeftBottom = static_cast<int>((y - height / 2) * frame.rows);
            int xRightTop = static_cast<int>((x + width / 2) * frame.cols);
            int yRightTop = static_cast<int>((y + height / 2) * frame.rows);
            Rect object(xLeftBottom, yLeftBottom,
                xRightTop - xLeftBottom,
                yRightTop - yLeftBottom);
            rectangle(frame, object, Scalar(0, 0, 255), 2, 8);
            if (objectClass < classNamesVec.size())
            {
                ss.str("");
                ss << confidence;
                String conf(ss.str());
                String label = String(classNamesVec[objectClass]) + ": " + conf;
                int baseLine = 0;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom),
                    Size(labelSize.width, labelSize.height + baseLine)),
                    Scalar(255, 255, 255), CV_FILLED);
                putText(frame, label, Point(xLeftBottom, yLeftBottom + labelSize.height),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
            }
        }
    }
#endif
    return true;
}

CImageService::CImageService(int port)
    :server_(port)
{

}


CImageService::~CImageService(void)
{
}

int CImageService::Start()
{
    m_process.Start(&queue_);

    server_.init(&m_decode, &queue_, &pool_);
    server_.run();

    return 0;
}

int CImageService::Stop()
{
    return 0;
}

