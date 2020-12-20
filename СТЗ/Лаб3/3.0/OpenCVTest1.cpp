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

    //////* Тест прямого ДПФ, вывода спектра, обратного ДПФ */////////////////////////////////////////////////////////////////////////////////////////


    //Mat inputImage = imread("240x340.jpg", IMREAD_GRAYSCALE); //исходное            
    ////Mat inputImage = imread("100x150.bmp", IMREAD_GRAYSCALE); 
    //fourier.setSize(inputImage.size());

    //inputImage = fourier.expansion(inputImage); //расширение изображения

    //imshow("inputImage", inputImage);
    //while (waitKey(25) != 27)
    //{ ; }

    //inputImage.convertTo(inputImage, CV_32FC1); //переводим в нужный формат

    ///* Прямое преобраование */
    //Mat resultForward(inputImage.size(), CV_32FC2, Scalar(0));
    ////fourier.forwardTransform(inputImage, resultForward);
    //dft(inputImage, resultForward, DFT_COMPLEX_OUTPUT);

    ///* Выврд спектра */
    //Mat specInputImage(inputImage.size(), CV_32FC1, Scalar(0));
    //fourier.spectrum(resultForward, specInputImage);

    ///* Обратное преобраование */
    //Mat resultInverse(inputImage.size(), CV_32FC1, Scalar(0, 0));
    ////fourier.inverseTransform(fourier.m_reAndIm, resultInverse);
    //dft(resultForward, resultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);


    //fourier.simileForward(inputImage); //Нахождение отклонений


    //////* Режекторный фильтр Гаусса */////////////////////////////////////////////////////////////////////////////////////////

    //fourier.setPhase(inputImage);

    //Mat resultNotchFilter(inputImage.size(), CV_32FC2, Scalar(0, 0)); //Вещественна и мнимая часть после умножения на фильтр
    //fourier.notchFilter(resultForward, resultNotchFilter, 25, 10);



    //////* Свертка с фильтрами *///////////////////////////////////////////////////////////////////////////////////////////////


    /*Фильтр Лапласа*/
    //cout << "лаплас" << endl;
    //fourier.laplas(inputImage);

    ///*Прмоугольный фильтр*/
    //cout << "прямоугольный" << endl;
    //fourier.box(inputImage);

    ///*Фильтр Собеля*/
    //cout << "собель" << endl;
    //fourier.sobel(inputImage, 0);
    //fourier.sobel(inputImage, 1);


    //////* Поиск символов на номерах */////////////////////////////////////////////////////////////////////////////////////////

    //Mat num = imread("num1.jpg", IMREAD_GRAYSCALE); //номер
    //num = fourier.expansion(num);
    //imshow("numOrig", num);


    //Mat A = imread("num1A.jpg", IMREAD_GRAYSCALE); //буква
    //Rect rectangle(0, 0, A.cols, A.rows);
    //Mat symbolA(num.size(), num.type(), Scalar(0));
    //A.copyTo(symbolA(rectangle));
    //imshow("symbolA", symbolA);

    //Mat zero = imread("num10.jpg", IMREAD_GRAYSCALE); //цифра 3
    //Rect rectangle2(0, 0, zero.cols, zero.rows);
    //Mat symbol0(num.size(), num.type(), Scalar(0));
    //zero.copyTo(symbol0(rectangle2));
    //imshow("symbol3", symbol0);

    //Mat eight = imread("num19.jpg", IMREAD_GRAYSCALE); //цифра 8
    //Rect rectangle3(0, 0, eight.cols, eight.rows);
    //Mat symbol8(num.size(), num.type(), Scalar(0));
    //eight.copyTo(symbol8(rectangle3));
    //imshow("symbol8", symbol8);


    //num.convertTo(num, CV_32FC1); //переводим в нужный формат
    //symbolA.convertTo(symbolA, CV_32FC1); //переводим в нужный формат
    //symbol0.convertTo(symbol0, CV_32FC1); //переводим в нужный формат
    //symbol8.convertTo(symbol8, CV_32FC1); //переводим в нужный формат

    //fourier.num(num, symbolA);
    //fourier.num(num, symbol0);
    //fourier.num(num, symbol8);





    Mat fur = imread("part4_1.jpg", IMREAD_GRAYSCALE); //номер

    Mat glaza = imread("part4_2.jpg", IMREAD_GRAYSCALE); //буква




    fourier.corr2(fur, glaza);

    fur = fourier.expansion(fur);


    imshow("fur", fur);
    imshow("glaz", glaza);

    Rect rectanglef(0, 0, glaza.cols, glaza.rows);
    Mat symbolGlaza(fur.size(), fur.type(), Scalar(0));
    glaza.copyTo(symbolGlaza(rectanglef));
    imshow("symbolGlaza", symbolGlaza);

    fur.convertTo(fur, CV_32FC1); //переводим в нужный формат
    symbolGlaza.convertTo(symbolGlaza, CV_32FC1); //переводим в нужный формат

    fourier.num(fur, symbolGlaza);

    return 0;

}

