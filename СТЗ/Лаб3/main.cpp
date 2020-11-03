// OpenCVTest1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "opencv2\core.hpp"
#include "Fourier.h"



using namespace std;
using namespace cv;



int main()
{

    Mat A; //конвертированное 

    //Mat inputImage = imread("100x150.bmp", IMREAD_GRAYSCALE); //исходное
    Mat inputImage = imread("260x350.jpg", IMREAD_GRAYSCALE); //исходное

    //Определение оптимального размера
    Size dftSize;
    dftSize.width = getOptimalDFTSize(inputImage.cols);
    dftSize.height = getOptimalDFTSize(inputImage.rows);

    cout << dftSize.width << " " << dftSize.height << endl;

    inputImage.convertTo(A, CV_32FC1);
    
    Mat rezultForward(inputImage.size(), CV_32FC2, Scalar(0, 0));
    Mat rezultInverse(inputImage.size(), CV_32FC1, Scalar(0, 0));
    Mat rezultDFT(inputImage.size(), CV_32FC2, Scalar(0, 0));

    Fourier fourier;

    fourier.forwardTransform(A, rezultForward);

    fourier.spectrum(rezultForward);

    fourier.inverseTransform(rezultForward, rezultInverse);


    dft(A, rezultDFT, DFT_COMPLEX_OUTPUT);




    return 0;

}

