// OpenCVTest1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2\core.hpp"
#include "Fourier.h"
#include  <stdlib.h>
#include   <ctime>



using namespace std;
using namespace cv;



int main()
{
    Fourier fourier;
    setlocale(LC_ALL, "Russian");

    //////* Тест прямого ДПФ, вывода спектра, обратного ДПФ *//////


    Mat inputImage = imread("240x340.jpg", IMREAD_GRAYSCALE); //исходное            
    //Mat inputImage = imread("100x150.bmp", IMREAD_GRAYSCALE); 
    fourier.setSize(inputImage.size());

    inputImage = fourier.expansion(inputImage); //расширение изображения

    imshow("inputImage", inputImage);
    while (waitKey(25) != 27)
    { ; }

    inputImage.convertTo(inputImage, CV_32FC1); //переводим в нужный формат

    /* Прямое преобраование */
    //fourier.forwardTransform(inputImage);
    Mat resultForward(inputImage.size(), CV_32FC2, Scalar(0));
    dft(inputImage, resultForward, DFT_COMPLEX_OUTPUT);

    /* Выврд спектра */
    Mat specInputImage(inputImage.size(), CV_32FC1, Scalar(0));
    fourier.spectrum(resultForward, specInputImage);

    /* Обратное преобраование */
    Mat resultInverse(inputImage.size(), CV_32FC1, Scalar(0, 0));
    //fourier.inverseTransform(fourier.m_reAndIm, resultInverse);
    dft(resultForward, resultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);



    /* Режекторный фильтр Гаусса */

    fourier.setPhase(inputImage);

    Mat resultNotchFilter(inputImage.size(), CV_32FC2, Scalar(0, 0)); //Вещественна и мнимая часть после умножения на фильтр
    fourier.notchFilter(resultForward, resultNotchFilter, 25, 10);



    //////* Свертка с фильтрами *//////


    /*Фильтр Лапласа*/
    cout << "лаплас" << endl;
    fourier.laplas(inputImage);

    /*Прмоугольный фильтр*/
    cout << "прямоугольный" << endl;
    fourier.box(inputImage);

    /*Фильтр Собеля*/
    cout << "собель" << endl;
    fourier.sobel(inputImage, 0);
    fourier.sobel(inputImage, 1);


    //////* Поиск символов на номерах *//////

    Mat num = imread("num.jpg", IMREAD_GRAYSCALE); //номер
    num = fourier.expansion(num);
    imshow("numOrig", num);


    Mat A = imread("A.jpg", IMREAD_GRAYSCALE); //буква
    Rect rectangle(0, 0, A.cols, A.rows);
    Mat symbolA(num.size(), num.type(), Scalar(0));
    A.copyTo(symbolA(rectangle));
    imshow("symbol", symbolA);


    num.convertTo(num, CV_32FC1); //переводим в нужный формат
    symbolA.convertTo(symbolA, CV_32FC1); //переводим в нужный формат

    fourier.num(num, symbolA);

    return 0;

}

