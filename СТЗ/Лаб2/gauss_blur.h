#pragma once

#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>


using namespace std;
using namespace cv;


class gauss_blur
{
public:
	gauss_blur() = default;
	gauss_blur(float sigma);
	~gauss_blur() = default;

	//основаная функция, реализующая всю логику
	int gaussBlur(Mat inputImage, Mat outputImage, float sigma);

	//Создаем изображение с большей границей
	int borderWrap(Mat newImage, Mat inputImage);

	//прохож ядром гаусса
	void gaussFiltr(Mat inputImage, Mat output, float sigma); //сам фильтр


	float m_sigma;

};

