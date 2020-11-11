// OpenCVTest1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2\core.hpp"
#include <opencv2\highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace std;
using namespace cv;


int settingCapture(VideoCapture cap)
{
    Mat frame;
    char key = 0;
    int BRIGHTNESS = 4;
    int CONTRAST = 100;
    int SATURATION = 98;

    while (waitKey(25) != 27)
    {
        key = waitKey(30);

        if ((key == 'q') || (key == 'Q')) //Яркость
        {
            BRIGHTNESS = BRIGHTNESS + 2;
            if (BRIGHTNESS > 100)
            {
                BRIGHTNESS = 100;
            }
            cap.set(CAP_PROP_BRIGHTNESS, BRIGHTNESS);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;
        }
        if ((key == 'a') || (key == 'A')) //Яркость
        {
            BRIGHTNESS = BRIGHTNESS - 2;
            if (BRIGHTNESS < 0)
            {
                BRIGHTNESS = 0;
            }
            cap.set(CAP_PROP_BRIGHTNESS, BRIGHTNESS);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;

        }



        if ((key == 'w') || (key == 'W')) //Контрастность
        {
            CONTRAST = CONTRAST + 2;
            if (CONTRAST > 100)
            {
                CONTRAST = 100;
            }
            cap.set(CAP_PROP_CONTRAST, CONTRAST);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;

        }
        if ((key == 's') || (key == 'S')) //Контрастность
        {
            CONTRAST = CONTRAST - 2;
            if (CONTRAST < 0)
            {
                CONTRAST = 0;
            }
            cap.set(CAP_PROP_CONTRAST, CONTRAST);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;

        }



        if ((key == 'e') || (key == 'E')) //Насыщенность
        {
            SATURATION = SATURATION + 2;
            if (SATURATION > 100)
            {
                SATURATION = 100;
            }
            cap.set(CAP_PROP_SATURATION, SATURATION);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;
        }
        if ((key == 'd') || (key == 'D')) //Насыщенность
        {
            SATURATION = SATURATION - 2;
            if (SATURATION < 0)
            {
                SATURATION = 0;
            }
            cap.set(CAP_PROP_SATURATION, SATURATION);
            cout << "Яркость = " << BRIGHTNESS << endl;
            cout << "Контрастность = " << CONTRAST << endl;
            cout << "Насыщенность = " << SATURATION << endl;
        }

        if ((key == 'z') || (key == 'Z')) //Насыщенность
        {

            imwrite("1.jpg", frame);
            cout << "сохранение " << endl;
        }

        cap.read(frame);
        cv::imshow("test", frame);
    }
    return 0;
}

void overlayMask(Mat inputImage, Mat mask, Mat outputImage)
{
    
}

int main()
{
    setlocale(LC_ALL, "Russian");

    Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    cap.open(1, CAP_DSHOW);


    cap.set(CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
    cap.set(CAP_PROP_BRIGHTNESS, 0);
    cap.set(CAP_PROP_CONTRAST, 0);
    cap.set(CAP_PROP_SATURATION, 0);


    int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
    cap.set(CAP_PROP_FOURCC, fourcc);

   //settingCapture(cap);



    //Наложение черной маски





    Mat gemNoMask = imread("1.jpg");

    Mat mask;
    Mat res;
    inRange(gemNoMask, Scalar(140,140,140), Scalar(255,255,255), mask);

    Mat mask1 = mask.clone();
    

    morphologyEx(mask,mask,MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(18,18)));
    bitwise_not(mask,mask);


    imshow("gemNoMask", gemNoMask);
    imshow("Mask1", mask1);
    imshow("Mask2", mask);
    //imshow("res",res );

    while (waitKey(25) != 27)
    {
        ;
    }



    return 0;
}


