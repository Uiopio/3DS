// OpenCVTest1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2\core.hpp"
#include "Fourier.h"



using namespace std;
using namespace cv;



int main()
{
    Fourier fourier;
    setlocale(LC_ALL, "Russian");
    //Mat A; //конвертированное 


    /* Считывание входного изображения */
    Mat inputImage = imread("240x340.jpg", IMREAD_GRAYSCALE); //исходное

    inputImage = fourier.expansion(inputImage);

    imshow("inputImage", inputImage);

    inputImage.convertTo(inputImage, CV_32FC1); //переводим в нужный формат
    
    //Заготовки для сохранения результатов
    Mat rezultInverse(inputImage.size(), CV_32FC1, Scalar(0));
    Mat rezultDFT(inputImage.size(), CV_32FC2, Scalar(0, 0));

    
    /* Прямое преобраование */
    Mat Forward1(inputImage.size(), CV_32FC2, Scalar(0));
    //fourier.forwardTransform(A, Forward1); //Прямое
    dft(inputImage, Forward1, DFT_COMPLEX_OUTPUT);


    /* Выврд спектра */
    Mat Spec1(inputImage.size(), CV_32FC1, Scalar(0)); //спектр без манипуляций. Не годится для вывода
    fourier.spectrum(Forward1, Spec1);


    /*Фильтр Лапласа*/
    fourier.laplas(Forward1);

    /*Прмоугольный фильтр*/
    fourier.box(Forward1);

    /*Фильтр Собеля*/
    cout << "собель" << endl;
    fourier.sobel(Forward1, 0);
    fourier.sobel(Forward1, 1);

    /*Номера*/

    Mat num = imread("num.jpg", IMREAD_GRAYSCALE); //номер
    num = fourier.expansion(num);
    imshow("num", num);

    Mat A = imread("A.jpg", IMREAD_GRAYSCALE); //буква
    Rect rectangle(0, 0, A.cols, A.rows);
    Mat symbol(num.size(), num.type(), Scalar(0));
    A.copyTo(symbol(rectangle));
    imshow("symbol", symbol);



    /*Редекторный фильтр*/

    Mat Forward2(inputImage.size(), CV_32FC2, Scalar(0, 0)); //вВещественна и мнимая часть после умножения на фильтр
    fourier.notchFilter(Forward1, Forward2, 80 , 70);


    Mat Spec2(inputImage.size(), CV_32FC1, Scalar(0));
    fourier.spectrum(Forward2, Spec2);


    Mat Inverse2(inputImage.size(), CV_32FC1, Scalar(0, 0));
    //fourier.inverseTransform(Forward2, Inverse2);
    dft(Forward2, Inverse2, DFT_INVERSE | DFT_REAL_OUTPUT);

    normalize(Inverse2, Inverse2, 0, 1, NormTypes::NORM_MINMAX);

    Mat temp;
    Inverse2.convertTo(temp, CV_8UC1, 255);
    imshow("temp", temp);

    while (waitKey(25) != 27)
    {
        ;
    }

    fourier.inverseTransform(Forward1, rezultInverse);


  



    return 0;

}

