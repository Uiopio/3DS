#pragma once
#include <stdio.h>
#include <math.h>
#include <vector>
#include "opencv.hpp"
#include "opencv2\core.hpp"


using namespace std;
using namespace cv;


class Fourier
{
public:

	Fourier() = default;
	~Fourier() = default;

	int forwardTransform(Mat inputImage, Mat rezult); //прямиое преобразование
	int inverseTransform(Mat reAndIn, Mat rezult); //обратное
	int spectrum(Mat reAndIm); //вывод спектра картинкой
	void swapSpektr(Mat magI); //обмен четвертей картинки(для вывода спектра)

};

