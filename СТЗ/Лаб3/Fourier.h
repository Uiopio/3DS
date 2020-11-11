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

	int forwardTransform(Mat inputImage); 
	int inverseTransform(Mat reAndIn, Mat rezult);
	int spectrum(Mat ReAndIm, Mat spec);
	void swapSpektr(Mat magI); 

	int showImage(Mat inputImage, int flag, string nameImage);

	Mat expansion(Mat inputImage); 

	int notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W);
	int inverseTransformFromSpectrePhase(Mat spectr); 


	int laplas(Mat inputImage);
	int box(Mat inputImage);
	int sobel(Mat inputImage, int flag);

	int num(Mat num, Mat symbol); 

	int setPhase(Mat inputImage);

	void setSize(Size size);
	Mat clipping(Mat image);


	Mat m_reAndIm;
	Mat m_phase;
	Size m_originalSize;
};

