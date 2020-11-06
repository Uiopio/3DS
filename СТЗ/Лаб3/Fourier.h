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
	int spectrum(Mat reAndIm, Mat spec); //вывод спектра картинкой
	void swapSpektr(Mat magI); //обмен четвертей картинки(для вывода спектра)

	Mat expansion(Mat inputImage); //расгирение изображения

	int notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W); //Режекторный фильтр Гаусса
	int inverseTransformFromSpectrePhase(Mat spectr); //Получение картинки из спектра и фазы

	//фильтры
	int laplas(Mat ReAndIm);
	int box(Mat ReAndIm);
	int sobel(Mat ReAndIm, int flag);

	int num(Mat num, Mat symbol); //Исследование номеров

	Mat m_phase;

};

