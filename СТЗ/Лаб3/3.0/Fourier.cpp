#include "Fourier.h"


//Задаем размер входного и выходного изображения
void Fourier::setSize(Size size)
{
	m_originalSize = size;
	return;
}


//Заполняет фазу (вызывается, если не было вызвано мое прямое преобразование)
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


//Расширение границ
Mat Fourier::expansion(Mat inputImage)
{
	//сохранение изначального размера
	m_originalSize = inputImage.size();

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


//обрезка 
Mat Fourier::clipping(Mat image)
{
	Mat temp(m_originalSize, image.type(), Scalar(0));

	for (int i = 0; i < m_originalSize.height; i++)
	{
		for (int j = 0; j < m_originalSize.width; j++)
		{
			temp.at<float>(i, j) = image.at<float>(i, j);
		}
	}

	return temp;
}



// Прямое преобразование
int Fourier::forwardTransform(Mat inputImage, Mat result)
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
					result.at<Vec2f>(i, j)[0] = result.at<Vec2f>(i, j)[0] + inputImage.at<Vec<float, 1>>(m, n)[0] * cos(CV_2PI * ((double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)));
					//мнимая
					result.at<Vec2f>(i, j)[1] = result.at<Vec2f>(i, j)[1] - inputImage.at<Vec<float, 1>>(m, n)[0] * sin(CV_2PI * ((double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)));
				}
			}
		}
	}

	m_reAndIm = result.clone(); //сохраняем образ
	//result = rezult;

	Mat phase(inputImage.size(), CV_32FC1, Scalar(0, 0));

	for (int x = 0; x < result.rows; x++)
	{
		for (int y = 0; y < result.cols; y++)
		{
			phase.at<Vec<float, 1>>(x, y)[0] = atan2(result.at<Vec2f>(x, y)[1], result.at<Vec2f>(x, y)[0]);
		}
	}

	m_phase = phase.clone(); //сохраняем фазу

	cout << "Конец прямого преобразования" << endl;
	return 0;
}


//вывод спектра
int Fourier::spectrum(Mat ReAndIm, Mat spec)
{
	vector<Mat> temp;
	split(ReAndIm, temp);

	magnitude(temp[0], temp[1], spec);

	swapSpektr(spec);

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

	// обмен квадрантов 0-3
	Mat tmp;  
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	
	// обмен квадрантов 1-2
	q1.copyTo(tmp); 
	q2.copyTo(q1);
	tmp.copyTo(q2);
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
					rezult.at<float>(y, x) += reAndIn.at<Vec2f>(v, u)[0] * a1 - reAndIn.at<Vec2f>(v, u)[1] * a2; //считаем только вещественную часть, тк мнимая = 0 будет
				}
			}
			rezult.at<float>(y, x) = ((float)1 / (reAndIn.cols * reAndIn.rows)) * rezult.at<float>(y, x);
		}
	}

	showImage(rezult, 1, "inverseTransform");

	cout << "Конец обратного преобразования" << endl;
	return 0;
}


//функция для вывода спектра или результата обратного преобраования
// flag == 0 - вывести нужно спектр    flag == чему угодно другому - вывод картинки
int Fourier::showImage(Mat inputImage, int flag, string nameImage)
{
	Mat tempImage = inputImage.clone();

	//Необходимо отобразить спектр
	if (flag == 0)
	{
		tempImage += Scalar::all(1);
		log(tempImage, tempImage); //Натуральный логарифм для каждого элемента
	}

	normalize(tempImage, tempImage, 0, 1, NormTypes::NORM_MINMAX);
	Mat rezult;
	tempImage.convertTo(rezult, CV_8UC1, 255);
	
	imshow(nameImage, rezult);
	while (waitKey(25) != 27)
	{
		;
	}

	return 0;
}


//Режекторный фильтр
int Fourier::notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W)
{
	//Фильтр
	Mat gauss(ReAndIm.size(), CV_32FC1, Scalar(0, 0));

	float x0 = gauss.rows / 2; //центр изображения
	float y0 = gauss.cols / 2;

	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			float D = ((float)x - x0) * ((float)x - x0) + ((float)y - y0) * ((float)y - y0);
			gauss.at<float>(x, y) = 1 - exp(-((D - D0 * D0) / (sqrt(D) * W)) * ((D - D0 * D0) / (sqrt(D) * W)));
		}
	}

	showImage(gauss, 2, "gauss");

	Mat spectrInputImage(ReAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(ReAndIm, spectrInputImage); //квадранты свапнуты

	for (int x = 0; x < gauss.rows; x++)
	{
		for (int y = 0; y < gauss.cols; y++)
		{
			spectrInputImage.at<float>(x, y) = spectrInputImage.at<float>(x, y) * gauss.at<float>(x, y);
		}
	}

	showImage(spectrInputImage, 0, "spectrAfterGauss");

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

	rezult = clipping(rezult);
	showImage(rezult, 1, "inverseTransformFromSpectre");

	return 0;
}


//фильтры
int Fourier::laplas(Mat inputImage)
{
	Mat laplas(inputImage.size(), CV_32FC1, Scalar(0));

	laplas.at<float>(0, 1) = 1;
	laplas.at<float>(1, 0) = 1;
	laplas.at<float>(2, 1) = 1;
	laplas.at<float>(1, 2) = 1;
	laplas.at<float>(1, 1) = -4;


	Mat laplasFur(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(laplas, laplasFur, DFT_COMPLEX_OUTPUT); //Образ фильтра


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //Образ исходного изображения


	Mat result(reAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < reAndIm.rows; x++)
	{
		for (int y = 0; y < reAndIm.cols; y++)
		{
			float a1 = reAndIm.at<Vec2f>(x, y)[0];
			float b1 = reAndIm.at<Vec2f>(x, y)[1];
			float a2 = laplasFur.at<Vec2f>(x, y)[0];
			float b2 = laplasFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	//Спектр фильтра
	Mat specLap(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(laplasFur, specLap);
	showImage(specLap, 0, "specLap");


	//Спектр результата действи фильтра
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterLaplas");


	//Обратное преобразование
	Mat rezultInverse(reAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);
	
	rezultInverse = clipping(rezultInverse);

	showImage(rezultInverse, 5, "outputImageLaplas");

	return 0;
}


int Fourier::box(Mat inputImage)
{
	Mat box(inputImage.size(), CV_32FC1, Scalar(0));

	box.at<float>(0, 0) = 1; box.at<float>(0, 1) = 1; box.at<float>(0, 2) = 1;
	box.at<float>(1, 0) = 1; box.at<float>(1, 1) = 1; box.at<float>(1, 2) = 1;
	box.at<float>(2, 0) = 1; box.at<float>(2, 1) = 1; box.at<float>(2, 2) = 1;


	Mat boxFur(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(box, boxFur);
	dft(box, boxFur, DFT_COMPLEX_OUTPUT); //Образ фильтра


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //Образ исходного изображения


	Mat result(reAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < reAndIm.rows; x++)
	{
		for (int y = 0; y < reAndIm.cols; y++)
		{
			float a1 = reAndIm.at<Vec2f>(x, y)[0];
			float b1 = reAndIm.at<Vec2f>(x, y)[1];
			float a2 = boxFur.at<Vec2f>(x, y)[0];
			float b2 = boxFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	//Спектр фильтра
	Mat specLap(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(boxFur, specLap);
	showImage(specLap, 0, "specBox");


	//Спектр результата действи фильтра
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterBox");


	//Обратное преобразование
	Mat rezultInverse(reAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	rezultInverse = clipping(rezultInverse);

	showImage(rezultInverse, 5, "outputImageBox");

	return 0;
}


int Fourier::sobel(Mat inputImage, int flag)
{
	Mat sobel(inputImage.size(), CV_32FC1, Scalar(0));

	if (flag == 0) //горизонтальный
	{
		sobel.at<float>(0, 0) = 1;  sobel.at<float>(0, 1) = 2;  sobel.at<float>(0, 2) = 1;
		sobel.at<float>(1, 0) = 0;  sobel.at<float>(1, 1) = 0;  sobel.at<float>(1, 2) = 0;
		sobel.at<float>(2, 0) = -1; sobel.at<float>(2, 1) = -2; sobel.at<float>(2, 2) = -1;
	}

	if (flag == 1) //вертиклаьный
	{
		sobel.at<float>(0, 0) = 1; sobel.at<float>(0, 1) = 0; sobel.at<float>(0, 2) = -1;
		sobel.at<float>(1, 0) = 2; sobel.at<float>(1, 1) = 0; sobel.at<float>(1, 2) = -2;
		sobel.at<float>(2, 0) = 1; sobel.at<float>(2, 1) = 0; sobel.at<float>(2, 2) = -1;
	}


	Mat sobelFur(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(sobel, sobelFur);
	dft(sobel, sobelFur, DFT_COMPLEX_OUTPUT); //Образ фильтра


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //Образ исходного изображения


	Mat result(reAndIm.size(), CV_32FC2, Scalar(0, 0));
	for (int x = 0; x < reAndIm.rows; x++)
	{
		for (int y = 0; y < reAndIm.cols; y++)
		{
			float a1 = reAndIm.at<Vec2f>(x, y)[0];
			float b1 = reAndIm.at<Vec2f>(x, y)[1];
			float a2 = sobelFur.at<Vec2f>(x, y)[0];
			float b2 = sobelFur.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}

	//Спектр фильтра
	Mat specSobel(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(sobelFur, specSobel);
	showImage(specSobel, 0, "specSobel");


	//Спектр результата действи фильтра
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterSobel");


	//Обратное преобразование
	Mat rezultInverse(reAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	rezultInverse = clipping(rezultInverse);

	showImage(rezultInverse, 5, "outputImageSobel");

	return 0;
}


//Поиск символа
int Fourier::num(Mat num, Mat symbol)
{

	Mat numForward(num.size(), CV_32FC2, Scalar(0)); //изображение номера
	Mat symbolForward(num.size(), CV_32FC2, Scalar(0)); //изображение символа

	//forwardTransform(num, numForward);
	//forwardTransform(symbol, symbolForward);
	cv::dft(num, numForward, DFT_COMPLEX_OUTPUT);
	cv::dft(symbol, symbolForward, DFT_COMPLEX_OUTPUT);

	//спектр номера
	Mat spec1(num.size(), CV_32FC1, Scalar(0)); 
	spectrum(numForward, spec1);
	showImage(spec1, 0, "specNum");

	//спектр символа
	Mat spec2(num.size(), CV_32FC1, Scalar(0));
	spectrum(symbolForward, spec2);
	showImage(spec2, 0, "specSym");

	//корреляция
	Mat result(num.size(), CV_32FC2, Scalar(0, 0));
	/*for (int x = 0; x < num.rows; x++)
	{
		for (int y = 0; y < num.cols; y++)
		{
			float a1 = numForward.at<Vec2f>(x, y)[0];
			float b1 = numForward.at<Vec2f>(x, y)[1];
			float a2 = symbolForward.at<Vec2f>(x, y)[0];
			float b2 = -symbolForward.at<Vec2f>(x, y)[1];

			result.at<Vec2f>(x, y)[0] = a1 * a2 - b1 * b2;
			result.at<Vec2f>(x, y)[1] = a1 * b2 + a2 * b1;
		}
	}*/

	mulSpectrums(numForward, symbolForward, result, 0, 1);
	

	//вывод спектра после корреляции
	Mat spec(num.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specNumAndSymbol");


	Mat rezultInverse(num.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	cv::dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	showImage(rezultInverse, 1, "rezultInverse");


	//Поиск порогового значения
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

	porog = porog - 0.01;

	//Пороговая фильррация 
	Mat rezultInverseAndFilter(num.size(), CV_32FC1, Scalar(0));
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


	Mat temp1;
	rezultInverseAndFilter.convertTo(temp1, CV_8UC1, 255);

	imshow("num", temp1);

	while (waitKey(25) != 27)
	{
		;
	}

	return 0;
}


//сравнение прямых преоразований
int Fourier::simileForward(Mat inputImage)
{

	Mat resultDFT(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime1 = clock();
	dft(inputImage, resultDFT, DFT_COMPLEX_OUTPUT);
	unsigned int endTime1 = clock();
	unsigned int searchTime1 = endTime1 - startTime1;
	cout << "Время выполнения библиотечной функции " << (double)searchTime1 / CLOCKS_PER_SEC << endl;


	Mat resultForward(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime2 = clock();
	forwardTransform(inputImage, resultForward);
	unsigned int endTime2 = clock();
	unsigned int searchTime2 = endTime2 - startTime2;
	cout << "Время выполнения моего преобразования " << (double)searchTime2 / CLOCKS_PER_SEC << endl;


	Mat rezult(inputImage.size(), CV_32FC2, Scalar(0, 0));

	float temp = 0;
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				temp = resultForward.at<Vec2f>(i, j)[k] - resultDFT.at<Vec2f>(i, j)[k];
				rezult.at<Vec2f>(i, j)[k] = abs(temp);
			}
		}
	}

	float firstCof[2] = { 0,0 };
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
	{ ; }
	return 0;
}


//float mean1 = 0;
	//for (int i = 0; i < inputImage1.rows; i++)
	//{
	//	for (int j = 0; j < inputImage1.cols; j++)
	//	{
	//		mean1 = mean1 + inputImage1.at<float>(i, j);
	//	}
	//}
	//mean1 = mean1 / (inputImage1.rows * inputImage1.cols);
	//cout << "среднее значение для картинки 1: " << mean1 << endl;
	//Mat image1Mean = inputImage1 - mean1;
	////imshow("image1Mean", image1Mean);

	//float mean2 = 0;
	//for (int i = 0; i < inputImage2.rows; i++)
	//{
	//	for (int j = 0; j < inputImage2.cols; j++)
	//	{
	//		mean2 = mean2 + inputImage2.at<float>(i, j);
	//	}
	//}
	//mean2 = mean2 / (inputImage2.rows * inputImage2.cols);
	//cout << "среднее значение для картинки 2: " << mean2 << endl;
	//Mat image2Mean = inputImage2 - mean2;
	//imshow("image2Mean", image2Mean);

int Fourier::corr2(Mat inputImage1, Mat inputImage2)
{
	Mat furie = inputImage1.clone();
	Mat gl = inputImage2.clone();
	//перевод в float
	inputImage1.convertTo(inputImage1, CV_32FC1); //переводим в нужный формат
	inputImage2.convertTo(inputImage2, CV_32FC1); //переводим в нужный формат

	//поиск среднего значения
	Scalar Mean1;
	Mat Std1;
	meanStdDev(inputImage1, Mean1, Std1);

	Scalar Mean2;
	Mat Std2;
	meanStdDev(inputImage2, Mean2, Std2);

	inputImage1 = inputImage1 - Mean1;
	inputImage2 = inputImage2 - Mean2;


	cout << "среднее значение для картинки 1: " << Mean1 << endl;
	cout << "среднее значение для картинки 2: " << Mean2 << endl;


	inputImage1 = expansion(inputImage1);

	Rect rectangl(0, 0, inputImage2.cols, inputImage2.rows);
	Mat Glaza(inputImage1.size(), inputImage1.type(), Scalar(0));
	inputImage2.copyTo(Glaza(rectangl));


	//поиск образов
	Mat FurForward(inputImage1.size(), CV_32FC2, Scalar(0)); //изображение номера
	Mat GlazaForward(inputImage1.size(), CV_32FC2, Scalar(0)); //изображение символа

	cv::dft(inputImage1, FurForward, DFT_COMPLEX_OUTPUT);
	cv::dft(Glaza, GlazaForward, DFT_COMPLEX_OUTPUT);

	//корреляция
	Mat GF;
	mulSpectrums(FurForward, GlazaForward, GF, 0, 1);


	Mat rezultInverse(GF.size(), CV_32FC1, Scalar(0));
	cv::dft(GF, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	cv::normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);

	showImage(rezultInverse, 1, "rezultInverse");

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

	porog = porog - 0.01;

	//Пороговая фильррация 
	Mat rezultInverseAndFilter(inputImage1.size(), CV_32FC1, Scalar(0));
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


	Mat temp1;
	rezultInverseAndFilter.convertTo(temp1, CV_8UC1, 255);

	imshow("num", temp1);

	while (waitKey(25) != 27)
	{
		;
	}

	Point glass(0, 0);
	bool flag = false;
	for (int i = 0; i < temp1.rows; i++)
	{
		for (int j = 0; j < temp1.cols; j++)
		{
			if (temp1.at<uint8_t>(i, j) == 255)
			{
				flag = true;
				glass.x = j;
				glass.y = i;
				break;
			}
		}

		if (flag == true)
		{
			break;
		}

	}
	Point glass2(glass.x + gl.cols , glass.y + gl.rows);
	cv::rectangle(furie, glass, glass2, cv::Scalar(0, 0, 0), 6, 8, 0);

	imshow("furie", furie);

	while (waitKey(25) != 27)
	{
		;
	}


	return 0;
}

