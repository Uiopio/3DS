#include "Fourier.h"

//Расширение границ
Mat Fourier::expansion(Mat inputImage)
{
	//Определение оптимального размера
	Size dftSize;
	dftSize.width = getOptimalDFTSize(inputImage.cols);
	dftSize.height = getOptimalDFTSize(inputImage.rows);

	cout << "оптимальная ширина = " << dftSize.width << endl;
	cout << "оптимальная высота = " << dftSize.height << endl;

	Rect rectangle(0, 0, inputImage.cols, inputImage.rows);
	Mat outputImage(dftSize, inputImage.type(), Scalar(0));

	inputImage.copyTo(outputImage(rectangle));
	return outputImage;
}


void Fourier::setSize(Size size)
{
	m_size = size;
	return;
}

Mat Fourier::clipping(Mat image)
{
	Mat temp(m_size, image.type(), Scalar(0));

	for (int i = 0; i < m_size.height; i++)
	{
		for (int j = 0; j < m_size.width; j++)
		{
			temp.at<float>(i, j) = image.at<float>(i, j);
		}
	}

	return temp;
}



int Fourier::setPhase(Mat inputImage)
{ 
	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));

	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT);

	Mat phase(inputImage.size(), CV_32FC1, Scalar(0, 0));

	for (int x = 0; x < reAndIm.rows; x++)
	{
		for (int y = 0; y < reAndIm.cols; y++)
		{
			phase.at<Vec<float, 1>>(x, y)[0] = atan2(reAndIm.at<Vec2f>(x, y)[1], reAndIm.at<Vec2f>(x, y)[0]);
		}
	}

	m_phase = phase.clone();
	return 0;
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
	/*normalize(rezult, rezult, 0, 1, NormTypes::NORM_MINMAX); 

	Mat temp;
	rezult.convertTo(temp, CV_8UC1, 255);
	imshow("temp", temp);
	while (waitKey(25) != 27)
	{
		;
	}*/
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

	float x0 = gauss.rows / 2; //центр изображения
	float y0 = gauss.cols / 2;

	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			float D = ((float)x - x0) * ((float)x - x0) + ((float)y - y0) *((float)y - y0);
			gauss.at<float>(x, y) = 1 - exp(-((D - D0 * D0) / (sqrt(D) * W)) * ((D - D0 * D0) / (sqrt(D) * W)));
		}
	}

	Mat temp = gauss.clone();
	normalize(temp, temp, 0, 1, NormTypes::NORM_MINMAX);

	Mat rezultgauss;
	temp.convertTo(rezultgauss, CV_8UC1, 255);
	imshow("rezultgauss", rezultgauss);
	while (waitKey(25) != 27)
	{;}

	Mat spectrInputImage(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(ReAndIm, spectrInputImage); //квадранты свапнуты

	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			spectrInputImage.at<float>(x, y) = spectrInputImage.at<float>(x, y) * gauss.at<float>(x, y);
		}
	}

	Mat tempSpec = spectrInputImage.clone();
	tempSpec += Scalar::all(1);
	log(tempSpec, tempSpec); //Натуральный логарифм для каждого элемента
	normalize(tempSpec, tempSpec, 0, 1, NormTypes::NORM_MINMAX);
	Mat rezult;
	tempSpec.convertTo(rezult, CV_8UC1, 255);
	imshow("spectrInputImage", rezult);
	while (waitKey(25) != 27)
	{
		;
	}

	inverseTransformFromSpectrePhase(spectrInputImage);

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
	//inverseTransform(newReAndIm, rezult);
	dft(newReAndIm, rezult, DFT_INVERSE | DFT_REAL_OUTPUT);

	normalize(rezult, rezult, 0, 1, NormTypes::NORM_MINMAX);
	Mat rezult2;
	rezult.convertTo(rezult2, CV_8UC1, 255);
	imshow("imageFilter", rezult2);
	while (waitKey(25) != 27)
	{
		;
	}

	return 0;
}

//фильтры
int Fourier::laplas(Mat ReAndIm)
{
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

	Mat specLap(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(laplasFur, specLap);

	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	Mat clipRezultInverse = clipping(rezultInverse);

	normalize(clipRezultInverse, clipRezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	clipRezultInverse.convertTo(temp, CV_8UC1, 255);

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

	Mat specBox(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(boxFur, specBox);

	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	Mat clipRezultInverse = clipping(rezultInverse);

	normalize(clipRezultInverse, clipRezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	clipRezultInverse.convertTo(temp, CV_8UC1, 255);

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
	Mat rezSobel;
	Mat box(ReAndIm.size(), CV_32FC1, Scalar(0));

	if (flag == 0)
	{
		box.at<float>(0, 0) = 1;  box.at<float>(0, 1) = 2;  box.at<float>(0, 2) = 1;
		box.at<float>(1, 0) = 0;  box.at<float>(1, 1) = 0;  box.at<float>(1, 2) = 0;
		box.at<float>(2, 0) = -1; box.at<float>(2, 1) = -2; box.at<float>(2, 2) = -1;

		Mat origImage = imread("240x340.jpg", IMREAD_GRAYSCALE);
		Sobel(origImage, rezSobel, -1, 1, 0, 3);

		imshow("rezSobel", rezSobel);
		while (waitKey(25) != 27)
		{
			;
		}
	}
	
	if (flag == 1)
	{
		box.at<float>(0, 0) = 1; box.at<float>(0, 1) = 0; box.at<float>(0, 2) = -1;
		box.at<float>(1, 0) = 2; box.at<float>(1, 1) = 0; box.at<float>(1, 2) = -2;
		box.at<float>(2, 0) = 1; box.at<float>(2, 1) = 0; box.at<float>(2, 2) = -1;

		Mat origImage = imread("240x340.jpg", IMREAD_GRAYSCALE);
		Sobel(origImage, rezSobel, -1, 0, 1);

		imshow("rezSobel", rezSobel);
		while (waitKey(25) != 27)
		{
			;
		}
	}

	Mat SobFur(ReAndIm.size(), CV_32FC2, Scalar(0, 0));

	//forwardTransform(laplas, laplasFur);
	dft(box, SobFur, DFT_COMPLEX_OUTPUT);

	Mat result(ReAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < ReAndIm.rows; x++)
	{
		for (int y = 0; y < ReAndIm.cols; y++)
		{
			float a1 = ReAndIm.at<Vec2f>(x, y)[0];
			float b1 = ReAndIm.at<Vec2f>(x, y)[1];
			float a2 = SobFur.at<Vec2f>(x, y)[0];
			float b2 = SobFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	Mat spec(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);

	Mat specFur(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(SobFur, specFur);

	Mat specRezult(ReAndIm.size(), CV_32FC1, Scalar(0));
	for (int x = 0; x < ReAndIm.rows; x++)
	{
		for (int y = 0; y < ReAndIm.cols; y++)
		{
			specRezult.at<float>(x, y) = spec.at<float>(x, y) * specFur.at<float>(x, y);
		}
	}
	cout << "результат перемножения спектров" << endl;
	inverseTransformFromSpectrePhase(specRezult);
	Mat tempSpecRezult = specRezult.clone();
	specRezult += Scalar::all(1);
	log(specRezult, specRezult); //Натуральный логарифм для каждого элемента
	normalize(specRezult, specRezult, 0, 1, NormTypes::NORM_MINMAX);
	Mat rezultttt;
	specRezult.convertTo(rezultttt, CV_8UC1, 255);
	imshow("specRezult", rezultttt);
	while (waitKey(25) != 27)
	{
		;
	}
	



	Mat rezultInverse(ReAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	Mat clipRezultInverse = clipping(rezultInverse);
	normalize(clipRezultInverse, clipRezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp;
	clipRezultInverse.convertTo(temp, CV_8UC1, 255);

	imshow("sobel", temp);
	
	double firstCof = 0;
	double z = 0;
	for (int i = 0; i < temp.rows; i++)
	{
		for (int j = 0; j < temp.cols; j++)
		{
			z = temp.at<uint8_t>(i, j) - rezSobel.at<uint8_t>(i, j);
			firstCof = firstCof + z*z;

		}
	}
	double sumPixel = temp.rows * temp.cols;
	double S = sqrt(firstCof / (sumPixel));
	cout << "отклонение " << S << endl;

	while (waitKey(25) != 27)
	{
		;
	}
	cout << "Фильтр Лапласа" << endl;
	return 0;
}


int Fourier::num(Mat num, Mat symbol)
{

	Mat numForward(num.size(), CV_32FC2, Scalar(0));
	Mat symbolForward(num.size(), CV_32FC2, Scalar(0));

	//forwardTransform(num, numForward);
	//forwardTransform(symbol, symbolForward);
	cv::dft(num, numForward, DFT_COMPLEX_OUTPUT);
	cv::dft(symbol, symbolForward, DFT_COMPLEX_OUTPUT);

	Mat result(num.size(), CV_32FC2, Scalar(0, 0));

	Mat spec1(num.size(), CV_32FC1, Scalar(0));
	spectrum(numForward, spec1);

	/*for (int k = 0; k < 3; k++)
	{
		for (int x = 0; x < num.rows; x++)
		{
			for (int y = 0; y < num.cols; y++)
			{
				float a1 = numForward.at<Vec2f>(x, y)[0];
				float b1 = numForward.at<Vec2f>(x, y)[1];
				float a2 = symbolForward.at<Vec2f>(x, y)[0];
				float b2 = symbolForward.at<Vec2f>(x, y)[1];

				result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
				result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
			}
		}
		Mat spec(num.size(), CV_32FC1, Scalar(0));
		spectrum(result, spec);
	}*/

	mulSpectrums(numForward, symbolForward, result,0, 1);

	Mat rezultInverse(num.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	cv::dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	cv::normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);
	float porog = 0;
	for (int i = 0; i < rezultInverse.rows; i++)
	{
		for (int j = 0; j < rezultInverse.cols; j++)
		{
			if (rezultInverse.at<float>(i, j) > porog)
			{
				porog = rezultInverse.at<float>(i, j);
			}
		}
	}

	cout << "porog = "<< porog << endl;
	porog = porog - 0.02;
	
	Mat rezultInverseAndFilter(num.size(), CV_32FC1, Scalar(0));
	//threshold(rezultInverse, rezultInverseAndFilter, porog, );

	for (int i = 0; i < rezultInverse.rows; i++)
	{
		for (int j = 0; j < rezultInverse.cols; j++)
		{
			if (rezultInverse.at<float>(i, j) > porog)
			{
				rezultInverseAndFilter.at<float>(i, j) = rezultInverse.at<float>(i, j);
			}
		}
	}


	//normalize(rezultInverseAndFilter, rezultInverseAndFilter, 0, 1, NormTypes::NORM_MINMAX);

	Mat temp1;
	rezultInverseAndFilter.convertTo(temp1, CV_8UC1, 255);

	imshow("num", temp1);

	while (waitKey(25) != 27)
	{
		;
	}

	return 0;
}

int Fourier::simile(Mat inputImage)
{

	Mat Forward2(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime2 = clock();
	dft(inputImage, Forward2, DFT_COMPLEX_OUTPUT);
	unsigned int endTime2 = clock();
	unsigned int searchTimeDFT = endTime2 - startTime2;
	cout << "Время выполнения библиотечной функции " << (double)searchTimeDFT / CLOCKS_PER_SEC << "  " << searchTimeDFT << endl;


	Mat Forward1(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime = clock();
	forwardTransform(inputImage, Forward1);
	unsigned int endTime = clock();
	unsigned int searchTimeMy = endTime - startTime;
	cout << "Время выполнения моего преобразования " << (double)searchTimeMy / CLOCKS_PER_SEC << "  "<< searchTimeMy <<endl;


	Mat rezult(inputImage.size(), CV_32FC2, Scalar(0,0));
	
	float temp = 0;
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				temp = Forward1.at<Vec2f>(i, j)[k] - Forward2.at<Vec2f>(i, j)[k];
				rezult.at<Vec2f>(i, j)[k] = abs(temp);
			}
		}
	}

	double firstCof[2] = {0,0};
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				firstCof[k] = firstCof[k] + (rezult.at<Vec2f>(i, j)[k] * rezult.at<Vec2f>(i, j)[k]);
			}

		}
	}
	double sumPixel = (double)inputImage.rows * (double)inputImage.cols;
	double S[2] = { 0,0 };

	for (int k = 0; k < 2; k++)
	{
		S[k] = sqrt(firstCof[k] / (sumPixel));
	}
	
	cout << "Отклонение Re " << S[0] << endl;
	cout << "Отклонение Im " << S[1] << endl;

	while (waitKey(25) != 27)
	{
		;
	}
	return 0;
}

int Fourier::simileInverse(Mat inputImage)
{
	Mat Forward(inputImage.size(), CV_32FC2, Scalar(0));
	dft(inputImage, Forward, DFT_COMPLEX_OUTPUT);

	Mat rezultInverse1(inputImage.size(), CV_32FC1, Scalar(0));

	unsigned int startTime1 = clock();
	inverseTransform(Forward, rezultInverse1);
	unsigned int endTime1 = clock();
	unsigned int searchTime = endTime1 - startTime1;
	cout << "Время выполнения моей функции " << (double)searchTime / CLOCKS_PER_SEC << "  " << searchTime << endl;


	Mat rezultInverse2(inputImage.size(), CV_32FC1, Scalar(0));

	unsigned int startTime2 = clock();
	dft(Forward, rezultInverse2, DFT_INVERSE | DFT_REAL_OUTPUT);
	unsigned int endTime2 = clock();
	unsigned int searchTimeDFT = endTime2 - startTime2;
	cout << "Время выполнения библиотечной функции " << (double)searchTimeDFT / CLOCKS_PER_SEC << "  " << searchTimeDFT << endl;


	normalize(rezultInverse2, rezultInverse2, 0, 1, NormTypes::NORM_MINMAX);
	Mat temp2;
	rezultInverse2.convertTo(temp2, CV_8UC1, 255);

	normalize(rezultInverse1, rezultInverse1, 0, 1, NormTypes::NORM_MINMAX);
	Mat temp1;
	rezultInverse1.convertTo(temp1, CV_8UC1, 255);

	imshow("rezultInverse2", temp2);
	imshow("rezultInverse1", temp1);
	while (waitKey(25) != 27)
	{
		;
	}

	Mat rezult(inputImage.size(), CV_32FC1, Scalar(0));

	Mat A;
	absdiff(temp1, temp2, A);
	double firstCof = 0;
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			firstCof = firstCof + (A.at<uint8_t>(i, j) * A.at<uint8_t>(i, j));
			//cout << (uint16_t)A.at<uint8_t>(i, j) << " ";
		}
		//cout << endl;
	}

	double sumPixel = (double)inputImage.rows * (double)inputImage.cols;
	double S = 0;


	S = sqrt(firstCof / (sumPixel));

	cout << "Отклонение " << S << endl;
	
	while (waitKey(25) != 27)
	{
		;
	}
	return 0;
}



int Fourier::simileSobel(Mat ReAndIm)
{
	Mat origImage = imread("240x340.jpg", IMREAD_GRAYSCALE);
	Mat rezSobel;
	Sobel(origImage, rezSobel, -1, 1, 0);

	sobel(ReAndIm, 0);
}