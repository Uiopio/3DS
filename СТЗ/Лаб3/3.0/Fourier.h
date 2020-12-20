#pragma once
#include <stdio.h>
#include <math.h>
#include <vector>
#include "opencv.hpp"
#include "opencv2\core.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <chrono>


using namespace std;
using namespace cv;


class Fourier
{
public:
	Fourier() = default;
	~Fourier() = default;

	int forwardTransform(Mat inputImage, Mat result);//прямое ДПФ 
	int inverseTransform(Mat reAndIn, Mat rezult);//обратое ДПФ
	int spectrum(Mat ReAndIm, Mat spec);//рассчет спектра
	void swapSpektr(Mat magI);//обмен квадрантов

	int showImage(Mat inputImage, int flag, string nameImage);//вывод спектра или рехультата обратного ДПФ

	Mat expansion(Mat inputImage);//расширение до оптимлаьных размеров
	Mat clipping(Mat image);//обрезка до изначального размера

	int notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W);//Режекторный фильтр Гаусса
	int inverseTransformFromSpectrePhase(Mat spectr); //ОБратное преобразование из спектра и фазы

	int laplas(Mat inputImage);//Лаплас
	int box(Mat inputImage);//прямоугольный фильтр
	int sobel(Mat inputImage, int flag);//Собель

	int num(Mat num, Mat symbol);//отклик на номерах

	int setPhase(Mat inputImage);
	void setSize(Size size);
	
	int simileForward(Mat inputImage);
	int simileInverse(Mat inputImage);
	
	int corr(Mat inputImage1, Mat inputImage2);
	int corr2(Mat inputImage1, Mat inputImage2);
	void normalizedCrossCorrelation(Mat inputImage1, Mat inputImage2);

	Mat m_reAndIm;
	Mat m_phase;
	Size m_originalSize;
};

