#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;



void part3()
{
    // Детекция монет
    Mat inputImage = imread("./money2.jpg", IMREAD_COLOR);

    Mat gray;
    cvtColor(inputImage, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 5);
    vector<Vec3f> circles;
    //HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows/3, 150, 30, 1, 200 );
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows / 7, 100, 30, 20, 200);

    // Создание вектора картинок с найденными монетками
    vector<Mat> money;
    for (size_t i = 0; i < circles.size(); i++)
    {
        //Берем найденный круг и определяем его параметры
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);
        int radius = c[2];

        Size size(radius * 2 + 20, radius * 2 + 20);
        Scalar black(0, 0, 0);

        //Rect rectangle(center.x - radius, center.y - radius, radius * 2, radius * 2);
        Rect rectangle(center.x - radius/2, center.y - radius/2, radius , radius );
        Mat outputImageWithMoney(inputImage, rectangle);

        money.push_back(outputImageWithMoney);
    }

    // перевод монеток и эталонов в хсв
    Mat copper = imread("copper2.jpg");
    Mat nikel = imread("nikel2.jpg");

    vector<Mat> money_HSV;
    for (size_t i = 0; i < money.size(); i++)
    {
        Mat temp_HSV;
        cvtColor(money[i], temp_HSV, COLOR_BGR2HSV);
        money_HSV.push_back(temp_HSV);
    }

    Mat hsv_copper, hsv_nikel;
    cvtColor(copper, hsv_copper, COLOR_BGR2HSV);
    cvtColor(nikel, hsv_nikel, COLOR_BGR2HSV);


    // Параметры для поиска гистограмм
    int h_bins = 180, s_bins = 256, v_bins = 256;
    int histSize[] = { h_bins, s_bins, v_bins };

    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    float v_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges, v_ranges };

    int channels[] = { 0, 1, 2 };

    // Вычисление гистограмм монет
    vector<Mat> money_hist;
    for (size_t i = 0; i < money_HSV.size(); i++)
    {
        Mat temp_hist;
        calcHist(&money_HSV[i], 1, channels, Mat(), temp_hist, 2, histSize, ranges, true, false);
        normalize(temp_hist, temp_hist, 0, 1, NORM_MINMAX, -1, Mat());
        money_hist.push_back(temp_hist);
    }

    Mat hist_copper, hist_nikel;
    calcHist(&hsv_copper, 1, channels, Mat(), hist_copper, 2, histSize, ranges, true, false);
    normalize(hist_copper, hist_copper, 0, 1, NORM_MINMAX, -1, Mat());

    calcHist(&hsv_nikel, 1, channels, Mat(), hist_nikel, 2, histSize, ranges, true, false);
    normalize(hist_nikel, hist_nikel, 0, 1, NORM_MINMAX, -1, Mat());

    // сравнение гистограмм 
    for (size_t i = 0; i < money_hist.size(); i++)
    {
        int met = 1;
        double money_copper = compareHist(money_hist[i], hist_copper, met);
        double money_nikel = compareHist(money_hist[i], hist_nikel, met);

        if (money_copper > money_nikel)
        {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            int radius = c[2];
            circle(inputImage, center, radius, Scalar(0, 0, 120), 3, LINE_AA);
        }
        else
        {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            int radius = c[2];
            circle(inputImage, center, radius, Scalar(120, 0, 0), 3, LINE_AA);
        }
    }

    imshow("output", inputImage);
    waitKey();
}


int transition0to1(Mat img, int x, int y)
{
    int transition = 0;
    if (img.at<uint8_t>(x, y - 1) == 0 && img.at<uint8_t>(x + 1, y - 1) == 255)
        transition++;
    if (img.at<uint8_t>(x + 1, y - 1) == 0 && img.at<uint8_t>(x + 1, y) == 255)
        transition++;
    if (img.at<uint8_t>(x + 1, y) == 0 && img.at<uint8_t>(x + 1, y + 1) == 255)
        transition++;
    if (img.at<uint8_t>(x + 1, y + 1) == 0 && img.at<uint8_t>(x, y + 1) == 255)
        transition++;
    if (img.at<uint8_t>(x, y + 1) == 0 && img.at<uint8_t>(x - 1, y + 1) == 255)
        transition++;
    if (img.at<uint8_t>(x - 1, y + 1) == 0 && img.at<uint8_t>(x - 1, y) == 255)
        transition++;
    if (img.at<uint8_t>(x - 1, y) == 0 && img.at<uint8_t>(x - 1, y - 1) == 255)
        transition++;
    if (img.at<uint8_t>(x - 1, y - 1) == 0 && img.at<uint8_t>(x, y - 1) == 255)
        transition++;
    return transition;
}

int countWhitePix(Mat img, int x, int y)
{
    int count = img.at<uint8_t>(x, y - 1) + img.at<uint8_t>(x + 1, y - 1) +
        img.at<uint8_t>(x + 1, y) + img.at<uint8_t>(x + 1, y + 1) +
        img.at<uint8_t>(x, y + 1) + img.at<uint8_t>(x - 1, y + 1) +
        img.at<uint8_t>(x - 1, y) + img.at<uint8_t>(x - 1, y - 1);
    return count;
}

bool zeros(Mat img)
{
    bool zeros = true;
    for (int i = 1; i < img.rows - 1; i++)
    {
        for (int j = 1; j < img.cols - 1; j++)
        {
            if (img.at<uint8_t>(i, j) != 0)
                return zeros = false;
        }
    }
    return zeros;
}



Mat skelitization(Mat image)
{
    Mat clone = image.clone();
    
    threshold(clone, clone, 150, 255, THRESH_BINARY);
    Mat temp(Size(clone.cols, clone.rows), CV_8U);

    bool stop1 = false;
    bool stop2 = false;

    while (stop1 == false && stop2 == false)
    {
        for (int i = 1; i < clone.rows - 1; i++)
        {
            for (int j = 1; j < clone.cols - 1; j++)
            {
                if (clone.at<uint8_t>(i, j) == 255 && countWhitePix(clone, i, j) >= 2 * 255 && countWhitePix(clone, i, j) <= 6 * 255 && transition0to1(clone, i, j) == 1 &&
                    (clone.at<uint8_t>(i, j - 1) * clone.at<uint8_t>(i + 1, j) * clone.at<uint8_t>(i, j + 1)) == 0 && (clone.at<uint8_t>(i + 1, j) * clone.at<uint8_t>(i, j + 1) * clone.at<uint8_t>(i - 1, j)) == 0)
                    temp.at<uint8_t>(i, j) = 255;
            }
        }

        clone = clone - temp;

        if (zeros(temp) == true)
            stop1 = true;
        temp = 0;
        for (int i = 1; i < clone.rows - 1; i++)
        {
            for (int j = 1; j < clone.cols - 1; j++)
            {
                if (clone.at<uint8_t>(i, j) == 255 && countWhitePix(clone, i, j) >= 2 * 255 && countWhitePix(clone, i, j) <= 6 * 255 && transition0to1(clone, i, j) == 1 &&
                    (clone.at<uint8_t>(i, j - 1) * clone.at<uint8_t>(i + 1, j) * clone.at<uint8_t>(i - 1, j)) == 0 && (clone.at<uint8_t>(i, j - 1) * clone.at<uint8_t>(i, j + 1) * clone.at<uint8_t>(i - 1, j)) == 0)
                    temp.at<uint8_t>(i, j) = 255;
            }
        }
        clone = clone - temp;

        if (zeros(temp) == true)
            stop2 = true;

        temp = 0;
    }
    
    return clone;
}

void video_skelitization(VideoCapture src)
{
    Mat src_frame;
    bool endProgram = false;
    while (waitKey(1) != 27)
    {
        src.read(src_frame);

        Mat clone_frame = src_frame.clone();
        cvtColor(clone_frame, clone_frame, COLOR_BGR2GRAY);

        Rect deth_zone(0, 0, clone_frame.cols, clone_frame.rows / 1.9);
        rectangle(clone_frame, deth_zone, Scalar(0, 0, 0), -1, 8, 0);

        GaussianBlur(clone_frame, clone_frame, Size(5, 5), 2, 2);
        threshold(clone_frame, clone_frame, 70, 255, THRESH_BINARY);
        imshow("threshold", clone_frame);
        erode(clone_frame, clone_frame, getStructuringElement(MORPH_RECT, Size(10, 10)));
        dilate(clone_frame, clone_frame, getStructuringElement(MORPH_RECT, Size(10, 10)));
        imshow("after_erode", clone_frame);

        clone_frame = skelitization(clone_frame);
        imshow("after_skeleton", clone_frame);

        vector<Vec4i> lines;
        HoughLinesP(clone_frame, lines, 1, CV_PI / 180, 50, 30, 50);
        for (int i = 0; i < lines.size(); i++)
        {
            Vec4i l = lines[i];
            line(src_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 2, 8);
        }

        imshow("src_frame", src_frame);

    }
    destroyAllWindows();
    return;
}

int main(int argc, char** argv)
{
    
    // Часть 1 //

    Mat image = imread("csgo.jpg", IMREAD_GRAYSCALE);

    Mat result(Size(image.cols, image.rows), CV_8UC1);
    result = skelitization(image);
    imshow("result", result);
    waitKey();
    
    // Часть 2 //

    VideoCapture src("0.avi");
    video_skelitization(src);
    waitKey();


    // Часть 3 //
    part3();
    
    return 0;
}