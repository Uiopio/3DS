#include "Fourier.h"

//Расширение границ
Mat Fourier::expansion(Mat inputImage)
{
	//Определение оптимального размера
	Size dftSize;
	dftSize.width = getOptimalDFTSize(inputImage.cols);
	dftSize.height = getOptimalDFTSize(inputImage.rows);

	Rect rectangle(0, 0, inputImage.cols, inputImage.rows);
	Mat outputImage(dftSize, inputImage.type(), Scalar(0));

	inputImage.copyTo(outputImage(rectangle));
	return outputImage;
}



// Прямое преобразование
int Fourier::forwardTransform(Mat inputImage, Mat rezult)
{
	cout << "Начало прямого преобразования" << endl;
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

	/*Mat phase(inputImage.size(), CV_32FC1, Scalar(0, 0));

	for (int x = 0; x < rezult.rows; x++)
	{
		for (int y = 0; y < rezult.cols; y++)
		{
			phase.at<Vec<float, 1>>(x, y)[0] = atan2(rezult.at<Vec2f>(x, y)[1], rezult.at<Vec2f>(x, y)[0]);
		}
	}

	m_phase = phase.clone();*/

	cout << "Конец прямого преобразования" << endl;
	return 0;
}

//Обратное
int Fourier::inverseTransform(Mat reAndIn, Mat rezult)
{
	cout << "Начало обартного преобразования" << endl;
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
	while (waitKey(25) != 27)
	{
		;
	}
	cout << "Конец обратного преобразования" << endl;
	return 0;
}

//вывод спектра
int Fourier::spectrum(Mat reAndIm, Mat spec)
{
	cout << "Вывод спектра" << endl;
	//Описание всего этого есть в документе от преподавателя.
	vector<Mat> temp;
	split(reAndIm, temp);

	magnitude(temp[0], temp[1] , spec);

	swapSpektr(spec);

	Mat tempSpec = spec.clone();

	tempSpec += Scalar::all(1);
	log(tempSpec, tempSpec); //Натуральный логарифм для каждого элемента

	normalize(tempSpec, tempSpec, 0, 1, NormTypes::NORM_MINMAX);

	Mat rezult;
	tempSpec.convertTo(rezult, CV_8UC1, 255);
	imshow("outputSpec1", rezult);

	while (waitKey(25) != 27)
	{
		;
	}
	return 0;

}

//обмен четвертей
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

//Режекторный фильтр
int Fourier::notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W)
{

	Mat gauss(ReAndIm.size(), CV_32FC1, Scalar(0, 0));

	float x0 = gauss.rows / 2;
	float y0 = gauss.cols / 2;

	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			float D = ((float)x - x0) * ((float)x - x0) + ((float)y - y0) *((float)y - y0);
			//gauss.at<float>(x, y) = exp(-((D - D0 * D0)/ (sqrt(D) * W)) * ((D - D0 * D0) / (sqrt(D) * W)));
			gauss.at<float>(x, y) = 1 - exp(-((D - D0 * D0) / (sqrt(D) * W)) * ((D - D0 * D0) / (sqrt(D) * W)));
		}
	}

	Mat temp = gauss.clone();
	normalize(temp, temp, 0, 1, NormTypes::NORM_MINMAX);

	Mat rezultgauss;
	temp.convertTo(rezultgauss, CV_8UC1, 255);
	imshow("rezultgauss", rezultgauss);

	while (waitKey(25) != 27)
	{
		;
	}



	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			newReAndIm.at<Vec2f>(x, y)[0] = ReAndIm.at<Vec2f>(x, y)[0] * gauss.at<float>(x, y);
			newReAndIm.at<Vec2f>(x, y)[1] = ReAndIm.at<Vec2f>(x, y)[1] * gauss.at<float>(x, y);
		}
	}

	return 0;
}

//Обратное преобразование из спектра и фазы
int Fourier::inverseTransformFromSpectrePhase(Mat spectr)
{
	Mat newReAndIm(spectr.size(), CV_32FC2, Scalar(0, 0));

	swapSpektr(spectr);


	for (int x = 0; x < spectr.rows; x++)
	{
		for (int y = 0; y < spectr.cols; y++)
		{
			newReAndIm.at<Vec2f>(x, y)[0] = spectr.at<float>(x, y) * cos(m_phase.at<float>(x, y));
			newReAndIm.at<Vec2f>(x, y)[1] = spectr.at<float>(x, y) * sin(m_phase.at<float>(x, y));
		}
	}

	Mat rezult(spectr.size(), CV_32FC1, Scalar(0, 0));
	inverseTransform(newReAndIm, rezult);


	while (waitKey(25) != 27)
	{
		;
	}

	return 0;
}

//фильтры
int Fourier::laplas(Mat ReAndIm)
{
	cout << "Фильтр Лапласа" << endl;

	Mat laplas(ReAndIm.size(), CV_32FC1, Scalar(0));
	laplas.at<float>(0, 1) = 1;
	laplas.at<float>(1, 0) = 1;
	laplas.at<float>(2, 1) = 1;
	laplas.at<float>(1, 2) = 1;
	laplas.at<float>(1, 1) = -4;

	Mat laplasFur(ReAndIm.size(), CV_32FC2, Scalar(0, 0));

	//forwardTransform(laplas, laplasFur);
	dft(laplas, laplasFur, DFT_COMPLEX_OUTPUT);

	Mat result(ReAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < ReAndIm.rows; x++)
	{
		for (int y = 0; y < ReAndIm.cols; y++)
		{
			float a1 = ReAndIm.at<Vec2f>(x, y)[0];
			float b1 = ReAndIm.at<Vec2f>(x, y)[1];
			float a2 = laplasFur.at<Vec2f>(x, y)[0];
			float b2 = laplasFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1*a2 - b1* b2;
			result.at<Vec2f>(x, y)[1] = a1*b2 + a2*b1;
		}
	}

	Mat spec(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);



	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	rezultInverse.convertTo(temp, CV_8UC1, 255);

	imshow("Lap", temp);

	while (waitKey(25) != 27)
	{
		;
	}
	cout << "Фильтр Лапласа" << endl;
	return 0;
}

int Fourier::box(Mat ReAndIm)
{
	cout << "Прямоугольный" << endl;

	Mat box(ReAndIm.size(), CV_32FC1, Scalar(0));
	box.at<float>(0, 0) = 1; box.at<float>(0, 1) = 1; box.at<float>(0, 2) = 1;
	box.at<float>(1, 0) = 1; box.at<float>(1, 1) = 1; box.at<float>(1, 2) = 1;
	box.at<float>(2, 0) = 1; box.at<float>(2, 1) = 1; box.at<float>(2, 2) = 1;


	Mat boxFur(ReAndIm.size(), CV_32FC2, Scalar(0, 0));

	//forwardTransform(laplas, laplasFur);
	dft(box, boxFur, DFT_COMPLEX_OUTPUT);

	Mat result(ReAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < ReAndIm.rows; x++)
	{
		for (int y = 0; y < ReAndIm.cols; y++)
		{
			float a1 = ReAndIm.at<Vec2f>(x, y)[0];
			float b1 = ReAndIm.at<Vec2f>(x, y)[1];
			float a2 = boxFur.at<Vec2f>(x, y)[0];
			float b2 = boxFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	Mat spec(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);



	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	rezultInverse.convertTo(temp, CV_8UC1, 255);

	imshow("box", temp);

	while (waitKey(25) != 27)
	{
		;
	}
	cout << "Фильтр Лапласа" << endl;
	return 0;
}

int Fourier::sobel(Mat ReAndIm, int flag)
{
	cout << "Прямоугольный" << endl;

	Mat box(ReAndIm.size(), CV_32FC1, Scalar(0));

	if (flag == 0)
	{
		box.at<float>(0, 0) = 1;  box.at<float>(0, 1) = 2;  box.at<float>(0, 2) = 1;
		box.at<float>(1, 0) = 0;  box.at<float>(1, 1) = 0;  box.at<float>(1, 2) = 0;
		box.at<float>(2, 0) = -1; box.at<float>(2, 1) = -2; box.at<float>(2, 2) = -1;
	}
	
	if (flag == 1)
	{
		box.at<float>(0, 0) = 1; box.at<float>(0, 1) = 0; box.at<float>(0, 2) = -1;
		box.at<float>(1, 0) = 2; box.at<float>(1, 1) = 0; box.at<float>(1, 2) = -2;
		box.at<float>(2, 0) = 1; box.at<float>(2, 1) = 0; box.at<float>(2, 2) = -1;
	}

	Mat boxFur(ReAndIm.size(), CV_32FC2, Scalar(0, 0));

	//forwardTransform(laplas, laplasFur);
	dft(box, boxFur, DFT_COMPLEX_OUTPUT);

	Mat result(ReAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < ReAndIm.rows; x++)
	{
		for (int y = 0; y < ReAndIm.cols; y++)
		{
			float a1 = ReAndIm.at<Vec2f>(x, y)[0];
			float b1 = ReAndIm.at<Vec2f>(x, y)[1];
			float a2 = boxFur.at<Vec2f>(x, y)[0];
			float b2 = boxFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	Mat spec(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);



	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	rezultInverse.convertTo(temp, CV_8UC1, 255);

	imshow("sobel", temp);

	while (waitKey(25) != 27)
	{
		;
	}
	cout << "Фильтр Лапласа" << endl;
	return 0;
}


int Fourier::num(Mat num, Mat symbol)
{
	
}