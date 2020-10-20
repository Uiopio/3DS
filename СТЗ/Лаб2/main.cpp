// OpenCVTest1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//мат формлировка. Распределение для Гаусса. Привезти график и таблицу с рассчетеом коэффициента фильтра
//Завернуть все это в класс с параметрами и функциями
// setImage(Mat image) //проверк допустимого изображения
// getImage
//Mat a = getImage() 
//{im.clome}

//фуккция обработки
//int processImage() //усли функция вернула 0, то все хорошо


//Измерение времени выполнения
//сравнить с функция встроенными в опенсв
//вычитать изображения absdiff
//получить разницу изображений и посчитать сумму всех пикселей и разделить на количество всех пикселей
//работать с полутоновыми
//Нужно будет еще проверить отдельно границу

//в отчете с результатами. Сравнение методов

#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "gauss_blur.h"
#include  <stdlib.h>
#include   <ctime>
#include <math.h>

using namespace std;
using namespace cv;




int main()
{
    setlocale(LC_ALL, "Russian");

    float sigma = 1;

    //создание класса
    gauss_blur gauss(sigma);

    //считываем картинку и создаем пустое изображение того же размера 
    Mat image = imread("1400x1000.jpg", IMREAD_GRAYSCALE);

    cout << image.channels() << endl;
    cout << image.rows << endl;
    cout << image.cols << endl;

    Mat test(image.size(), image.type());

    unsigned int start_time = clock();
    //вызов моей функции
    gauss.gaussBlur(image, test, sigma);
    unsigned int end_time = clock();
    unsigned int search_time = end_time - start_time;

    //вывод времени в секундах
    cout << '\n' << "my function =   " << (float)search_time / CLOCKS_PER_SEC;

    unsigned int start_time2 = clock();
    Mat testImage;
    //вызов библиотечной
    GaussianBlur(image, testImage, Size(5, 5), sigma, sigma, 3);
    unsigned int end_time2 = clock();
    unsigned int search_time2 = end_time2 - start_time2;

    cout << '\n' << "lib function =   " << (float)search_time2 / CLOCKS_PER_SEC << "\n"; 
    Mat rez;


    //разница между моей и библиотечной
    absdiff(testImage, test, rez);

    //Расчет среднеквадратичного отклонения
    double noNull = 0;
    double firstCof = 0;
    for (int i = 0; i < rez.rows; i++)
    {
        for (int j = 0; j < rez.cols; j++)
        {
            noNull = noNull + rez.at<Vec<uint8_t, 1>>(i, j)[0];
        }
    }

    uint32_t rezRows = rez.rows;
    uint32_t rezCols = rez.cols;
    uint32_t sumPixel = rezRows * rezCols;
    uint64_t sumPixel1 = (sumPixel - 1);
    double Xcp = noNull / (sumPixel);

    for (int i = 0; i < rez.rows; i++)
    {
        for (int j = 0; j < rez.cols; j++)
        {

            firstCof = firstCof + (rez.at<Vec<uint8_t, 1>>(i, j)[0] - Xcp) * (rez.at<Vec<uint8_t, 1>>(i, j)[0] - Xcp);
            
        }
    }

    cout << "firstCof  = " << firstCof << endl;

    double S = sqrt(firstCof / (sumPixel1));


    cout << "средняя интенсивность = " << Xcp << endl;
    cout << "Количество всех пикселей = " << rezRows * rezCols << endl;
    cout << "Отклоненние = " << S << endl;

    //imshow("input image", image);
    //imshow("my image", test);
    //imshow("function image", testImage);
    //imshow("absdiff", rez);

    //while (waitKey(25) != 27)
    //{
       // ;
   // }





}

