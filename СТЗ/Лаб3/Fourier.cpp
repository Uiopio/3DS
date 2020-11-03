#include "Fourier.h"

// Прямое преобразование
int Fourier::forwardTransform(Mat inputImage, Mat rezult)
{
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			for (int m = 0; m < inputImage.rows; m++)
			{
				for (int n = 0; n < inputImage.cols; n++)
				{
					//вещественная часть
					rezult.at<Vec2f>(i, j)[0] = rezult.at<Vec2f>(i, j)[0] + inputImage.at<Vec<float,1>>(m, n)[0] * cos( CV_2PI * ( (double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)) );
					//мнимая
					rezult.at<Vec2f>(i, j)[1] = rezult.at<Vec2f>(i, j)[1] - inputImage.at<Vec<float,1>>(m, n)[0] * sin( CV_2PI * ((double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)));
				}
			}
		}
	}


	return 0;
}


//Обратное
int Fourier::inverseTransform(Mat reAndIn, Mat rezult)
{

	for (int x = 0; x < reAndIn.cols; x++)
	{
		for (int y = 0; y < reAndIn.rows; y++)
		{
			for (int u = 0; u < reAndIn.cols; u++)
			{
				for (int v = 0; v < reAndIn.rows; v++)
				{
					float a1 = cos((float)CV_2PI * (((float)(u * x) / reAndIn.cols) + ((float)(v * y) / reAndIn.rows)));
					float a2 = sin((float)CV_2PI * (((float)(u * x) / reAndIn.cols) + ((float)(v * y) / reAndIn.rows)));
					rezult.at<float>(y, x) += reAndIn.at<Vec2f>(v, u)[0] * a1 -reAndIn.at<Vec2f>(v, u)[1] * a2; //считаем только вещественную часть, тк мнимая = 0 будет
				}
			}
			rezult.at<float>(y, x) = ((float)1 / (reAndIn.cols * reAndIn.rows)) * rezult.at<float>(y, x);
		}
	}

	//переводим в формат, который моэно отображать
	normalize(rezult, rezult, 0, 1, NormTypes::NORM_MINMAX); 

	Mat temp;
	rezult.convertTo(temp, CV_8UC1, 255);
	imshow("temp", temp);

	return 0;
}

//вывод спектра
int Fourier::spectrum(Mat reAndIm)
{
	//Описание всего этого есть в документе от преподавателя.
	vector<Mat> temp;
	split(reAndIm, temp);

	Mat spectrum;
	magnitude(temp[0], temp[1] , spectrum);

	swapSpektr(spectrum);

	spectrum += Scalar::all(1);
	log(spectrum, spectrum); //Натуральный логарифм для каждого элемента

	normalize(spectrum, spectrum, 0, 1, NormTypes::NORM_MINMAX);

	Mat rezult;
	spectrum.convertTo(rezult, CV_8UC1, 255);
	imshow("spectrum", rezult);

	while (waitKey(25) != 27)
	{
		;
	}

}

void Fourier::swapSpektr(Mat magI)
{
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // верхний левый
	Mat q1(magI, Rect(cx, 0, cx, cy));  // верхний правый
	Mat q2(magI, Rect(0, cy, cx, cy));  // нижний левый
	Mat q3(magI, Rect(cx, cy, cx, cy)); // нижний правый

	Mat tmp;                           // обмен квадрантов 0-3
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // обмен квадрантов 1-2
	q2.copyTo(q1);
	tmp.copyTo(q2);
}
