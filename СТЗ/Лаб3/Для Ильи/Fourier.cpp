int Fourier::corr2(Mat inputImage1, Mat inputImage2)
{
	int mean1 = 0;
	for (int i = 0; i < inputImage1.rows; i++)
	{
		for (int j = 0; j < inputImage1.cols; j++)
		{
			mean1 = mean1 + inputImage1.at<uint8_t>(i, j);
		}
	}
	mean1 = mean1 / (inputImage1.rows * inputImage1.cols);
	cout << "среднее значение для картинки 1: " << mean1 << endl;
	Mat image1Mean = inputImage1 - mean1;
	imshow("image1Mean", image1Mean);

	int mean2 = 0;
	for (int i = 0; i < inputImage2.rows; i++)
	{
		for (int j = 0; j < inputImage2.cols; j++)
		{
			mean2 = mean2 + inputImage2.at<uint8_t>(i, j);
		}
	}
	mean2 = mean2 / (inputImage2.rows * inputImage2.cols);
	cout << "среднее значение для картинки 2: " << mean2 << endl;
	Mat image2Mean = inputImage2 - mean2;
	imshow("image2Mean", image2Mean);

	Mat Mean1;
	Mat Std1;
	meanStdDev(image1Mean, Mean1, Std1);

	Mat Mean2;
	Mat Std2;
	meanStdDev(image2Mean, Mean2, Std2);

	image1Mean = inputImage1 / Std1;
	image2Mean = inputImage2 / Std2;



	image1Mean = expansion(image1Mean);

	Rect rectangl(0, 0, image2Mean.cols, image2Mean.rows);
	Mat Glaza(image1Mean.size(), image1Mean.type(), Scalar(0));
	image2Mean.copyTo(Glaza(rectangl));

	image1Mean.convertTo(image1Mean, CV_32FC1); //переводим в нужный формат
	Glaza.convertTo(Glaza, CV_32FC1); //переводим в нужный формат


	Mat FurForward(image1Mean.size(), CV_32FC2, Scalar(0)); //изображение номера
	Mat GlazaForward(image1Mean.size(), CV_32FC2, Scalar(0)); //изображение символа

	cv::dft(image1Mean, FurForward, DFT_COMPLEX_OUTPUT);
	cv::dft(Glaza, GlazaForward, DFT_COMPLEX_OUTPUT);

	Mat GF;
	mulSpectrums(FurForward, GlazaForward, GF, 0, 1);
	//mulSpectrums(GlazaForward, FurForward, GF, 0, 1);

	




	Mat rezultInverse(GF.size(), CV_32FC1, Scalar(0));
	cv::dft(GF, rezultInverse, DFT_INVERSE | DFT_REAL_OUTPUT);

	cv::normalize(rezultInverse, rezultInverse, 0, 1, NormTypes::NORM_MINMAX);
	showImage(rezultInverse, 0, "rezultInverse");


	return 0;
}