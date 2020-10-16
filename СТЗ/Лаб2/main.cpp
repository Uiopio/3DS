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

using namespace std;
using namespace cv;




int main()
{
    //создание класса
    gauss_blur gauss(1);

    //считываем картинку и создаем пустое изображение того же размера 
    Mat image = imread("cub2.bmp");
    Mat test(image.size(), image.type(), Scalar(0, 0, 0));

    //вызов моей функции
    gauss.gaussBlur(image, test, 1);
   
    Mat testImage;
    //вызов библиотечной
    GaussianBlur(image, testImage, Size(5, 5), 1, 1, 0);
    Mat rez;

    //рахница между моей и библиотечной
    absdiff(testImage, test, rez);

    imshow("image", image);
    imshow("test", test);
    imshow("testImage", testImage);
    imshow("rez", rez);
    while (waitKey(25) != 27)
    {
        ;
    }





}

