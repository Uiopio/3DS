#include "Fourier.h"


//������ ������ �������� � ��������� �����������
void Fourier::setSize(Size size)
{
	m_originalSize = size;
	return;
}


//��������� ���� (����������, ���� �� ���� ������� ��� ������ ��������������)
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


//���������� ������
Mat Fourier::expansion(Mat inputImage)
{
	//���������� ������������ �������
	m_originalSize = inputImage.size();

	//����������� ������������ �������
	Size dftSize;
	dftSize.width = getOptimalDFTSize(inputImage.cols);
	dftSize.height = getOptimalDFTSize(inputImage.rows);

	cout << "����������� ������ = " << dftSize.width << endl;
	cout << "����������� ������ = " << dftSize.height << endl;

	Rect rectangle(0, 0, inputImage.cols, inputImage.rows);
	Mat outputImage(dftSize, inputImage.type(), Scalar(0));

	inputImage.copyTo(outputImage(rectangle));
	return outputImage;
}


//������� 
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



// ������ ��������������
int Fourier::forwardTransform(Mat inputImage, Mat result)
{
	cout << "������ ������� ��������������" << endl;

	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			for (int m = 0; m < inputImage.rows; m++)
			{
				for (int n = 0; n < inputImage.cols; n++)
				{
					//������������ �����
					result.at<Vec2f>(i, j)[0] = result.at<Vec2f>(i, j)[0] + inputImage.at<Vec<float, 1>>(m, n)[0] * cos(CV_2PI * ((double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)));
					//������
					result.at<Vec2f>(i, j)[1] = result.at<Vec2f>(i, j)[1] - inputImage.at<Vec<float, 1>>(m, n)[0] * sin(CV_2PI * ((double(i * m) / inputImage.rows) + (double(j * n) / inputImage.cols)));
				}
			}
		}
	}

	m_reAndIm = result.clone(); //��������� �����
	//result = rezult;

	Mat phase(inputImage.size(), CV_32FC1, Scalar(0, 0));

	for (int x = 0; x < result.rows; x++)
	{
		for (int y = 0; y < result.cols; y++)
		{
			phase.at<Vec<float, 1>>(x, y)[0] = atan2(result.at<Vec2f>(x, y)[1], result.at<Vec2f>(x, y)[0]);
		}
	}

	m_phase = phase.clone(); //��������� ����

	cout << "����� ������� ��������������" << endl;
	return 0;
}


//����� �������
int Fourier::spectrum(Mat ReAndIm, Mat spec)
{
	vector<Mat> temp;
	split(ReAndIm, temp);

	magnitude(temp[0], temp[1], spec);

	swapSpektr(spec);

	return 0;

}


//����� ���������
void Fourier::swapSpektr(Mat magI)
{
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // ������� �����
	Mat q1(magI, Rect(cx, 0, cx, cy));  // ������� ������
	Mat q2(magI, Rect(0, cy, cx, cy));  // ������ �����
	Mat q3(magI, Rect(cx, cy, cx, cy)); // ������ ������

	// ����� ���������� 0-3
	Mat tmp;  
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	
	// ����� ���������� 1-2
	q1.copyTo(tmp); 
	q2.copyTo(q1);
	tmp.copyTo(q2);
}


//��������
int Fourier::inverseTransform(Mat reAndIn, Mat rezult)
{
	cout << "������ ��������� ��������������" << endl;
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
					rezult.at<float>(y, x) += reAndIn.at<Vec2f>(v, u)[0] * a1 - reAndIn.at<Vec2f>(v, u)[1] * a2; //������� ������ ������������ �����, �� ������ = 0 �����
				}
			}
			rezult.at<float>(y, x) = ((float)1 / (reAndIn.cols * reAndIn.rows)) * rezult.at<float>(y, x);
		}
	}

	showImage(rezult, 1, "inverseTransform");

	cout << "����� ��������� ��������������" << endl;
	return 0;
}


//������� ��� ������ ������� ��� ���������� ��������� �������������
// flag == 0 - ������� ����� ������    flag == ���� ������ ������� - ����� ��������
int Fourier::showImage(Mat inputImage, int flag, string nameImage)
{
	Mat tempImage = inputImage.clone();

	//���������� ���������� ������
	if (flag == 0)
	{
		tempImage += Scalar::all(1);
		log(tempImage, tempImage); //����������� �������� ��� ������� ��������
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


//����������� ������
int Fourier::notchFilter(Mat ReAndIm, Mat newReAndIm, float D0, float W)
{
	//������
	Mat gauss(ReAndIm.size(), CV_32FC1, Scalar(0, 0));

	float x0 = gauss.rows / 2; //����� �����������
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
	spectrum(ReAndIm, spectrInputImage); //��������� ��������

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


//�������� �������������� �� ������� � ����
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


//�������
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
	dft(laplas, laplasFur, DFT_COMPLEX_OUTPUT); //����� �������


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //����� ��������� �����������


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

	//������ �������
	Mat specLap(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(laplasFur, specLap);
	showImage(specLap, 0, "specLap");


	//������ ���������� ������� �������
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterLaplas");


	//�������� ��������������
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
	dft(box, boxFur, DFT_COMPLEX_OUTPUT); //����� �������


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //����� ��������� �����������


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

	//������ �������
	Mat specLap(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(boxFur, specLap);
	showImage(specLap, 0, "specBox");


	//������ ���������� ������� �������
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterBox");


	//�������� ��������������
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

	if (flag == 0) //��������������
	{
		sobel.at<float>(0, 0) = 1;  sobel.at<float>(0, 1) = 2;  sobel.at<float>(0, 2) = 1;
		sobel.at<float>(1, 0) = 0;  sobel.at<float>(1, 1) = 0;  sobel.at<float>(1, 2) = 0;
		sobel.at<float>(2, 0) = -1; sobel.at<float>(2, 1) = -2; sobel.at<float>(2, 2) = -1;
	}

	if (flag == 1) //������������
	{
		sobel.at<float>(0, 0) = 1; sobel.at<float>(0, 1) = 0; sobel.at<float>(0, 2) = -1;
		sobel.at<float>(1, 0) = 2; sobel.at<float>(1, 1) = 0; sobel.at<float>(1, 2) = -2;
		sobel.at<float>(2, 0) = 1; sobel.at<float>(2, 1) = 0; sobel.at<float>(2, 2) = -1;
	}


	Mat sobelFur(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(sobel, sobelFur);
	dft(sobel, sobelFur, DFT_COMPLEX_OUTPUT); //����� �������


	Mat reAndIm(inputImage.size(), CV_32FC2, Scalar(0, 0));
	//forwardTransform(laplas, laplasFur);
	dft(inputImage, reAndIm, DFT_COMPLEX_OUTPUT); //����� ��������� �����������


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

	//������ �������
	Mat specSobel(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(sobelFur, specSobel);
	showImage(specSobel, 0, "specSobel");


	//������ ���������� ������� �������
	Mat spec(reAndIm.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specAfterSobel");


	//�������� ��������������
	Mat rezultInverse(reAndIm.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	rezultInverse = clipping(rezultInverse);

	showImage(rezultInverse, 5, "outputImageSobel");

	return 0;
}


//����� �������
int Fourier::num(Mat num, Mat symbol)
{

	Mat numForward(num.size(), CV_32FC2, Scalar(0)); //����������� ������
	Mat symbolForward(num.size(), CV_32FC2, Scalar(0)); //����������� �������

	//forwardTransform(num, numForward);
	//forwardTransform(symbol, symbolForward);
	cv::dft(num, numForward, DFT_COMPLEX_OUTPUT);
	cv::dft(symbol, symbolForward, DFT_COMPLEX_OUTPUT);

	//������ ������
	Mat spec1(num.size(), CV_32FC1, Scalar(0)); 
	spectrum(numForward, spec1);
	showImage(spec1, 0, "specNum");

	//������ �������
	Mat spec2(num.size(), CV_32FC1, Scalar(0));
	spectrum(symbolForward, spec2);
	showImage(spec2, 0, "specSym");

	//����������
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
	

	//����� ������� ����� ����������
	Mat spec(num.size(), CV_32FC1, Scalar(0));
	spectrum(result, spec);
	showImage(spec, 0, "specNumAndSymbol");


	Mat rezultInverse(num.size(), CV_32FC1, Scalar(0));
	//inverseTransform(result, rezultInverse);
	cv::dft(result, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	showImage(rezultInverse, 1, "rezultInverse");


	//����� ���������� ��������
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

	//��������� ���������� 
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


//��������� ������ �������������
int Fourier::simileForward(Mat inputImage)
{

	Mat resultDFT(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime1 = clock();
	dft(inputImage, resultDFT, DFT_COMPLEX_OUTPUT);
	unsigned int endTime1 = clock();
	unsigned int searchTime1 = endTime1 - startTime1;
	cout << "����� ���������� ������������ ������� " << (double)searchTime1 / CLOCKS_PER_SEC << endl;


	Mat resultForward(inputImage.size(), CV_32FC2, Scalar(0));

	unsigned int startTime2 = clock();
	forwardTransform(inputImage, resultForward);
	unsigned int endTime2 = clock();
	unsigned int searchTime2 = endTime2 - startTime2;
	cout << "����� ���������� ����� �������������� " << (double)searchTime2 / CLOCKS_PER_SEC << endl;


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

	cout << "���������� Re " << S[0] << endl;
	cout << "���������� Im " << S[1] << endl;

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
	//cout << "������� �������� ��� �������� 1: " << mean1 << endl;
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
	//cout << "������� �������� ��� �������� 2: " << mean2 << endl;
	//Mat image2Mean = inputImage2 - mean2;
	//imshow("image2Mean", image2Mean);

int Fourier::corr2(Mat inputImage1, Mat inputImage2)
{
	Mat furie = inputImage1.clone();
	Mat gl = inputImage2.clone();
	//������� � float
	inputImage1.convertTo(inputImage1, CV_32FC1); //��������� � ������ ������
	inputImage2.convertTo(inputImage2, CV_32FC1); //��������� � ������ ������

	//����� �������� ��������
	Scalar Mean1;
	Mat Std1;
	meanStdDev(inputImage1, Mean1, Std1);

	Scalar Mean2;
	Mat Std2;
	meanStdDev(inputImage2, Mean2, Std2);

	inputImage1 = inputImage1 - Mean1;
	inputImage2 = inputImage2 - Mean2;


	cout << "������� �������� ��� �������� 1: " << Mean1 << endl;
	cout << "������� �������� ��� �������� 2: " << Mean2 << endl;


	inputImage1 = expansion(inputImage1);

	Rect rectangl(0, 0, inputImage2.cols, inputImage2.rows);
	Mat Glaza(inputImage1.size(), inputImage1.type(), Scalar(0));
	inputImage2.copyTo(Glaza(rectangl));


	//����� �������
	Mat FurForward(inputImage1.size(), CV_32FC2, Scalar(0)); //����������� ������
	Mat GlazaForward(inputImage1.size(), CV_32FC2, Scalar(0)); //����������� �������

	cv::dft(inputImage1, FurForward, DFT_COMPLEX_OUTPUT);
	cv::dft(Glaza, GlazaForward, DFT_COMPLEX_OUTPUT);

	//����������
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

	//��������� ���������� 
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

