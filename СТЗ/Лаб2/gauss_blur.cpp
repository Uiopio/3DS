#include "gauss_blur.h"

gauss_blur::gauss_blur(float sigma)
{
	m_sigma = sigma;
}



int gauss_blur::gaussBlur(Mat inputImage, Mat outputImage, float sigma)
{
	if ( (inputImage.empty() == true) || (inputImage.empty() == true) )
	{
		cout << "image empty" << endl;
		return -1; //пустое изображение
	}

	if (outputImage.size != inputImage.size)
	{
		cout << "Outpuut image error" << endl;
		return -2; //неправильно задано выходное изображение
	}

	//создаем изображение, которое больше оригинала на половину ядра во все стороны
	Mat newInputImage(Size(inputImage.cols + 4, inputImage.rows + 4), inputImage.type());
	//Увеличиваем и обрабатываем края

	borderWrap(newInputImage, inputImage);
	//проходи ядром
	gaussFiltr(newInputImage, outputImage, sigma);
	return 0;
}

int gauss_blur::borderWrap(Mat newImage, Mat inputImage)
{
	//копирование оригинального изображения в центр увеличенной картинки
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = 2;j < newImage.cols - 2; j++)
		{
			for (int channel = 0; channel < inputImage.channels(); channel++)
			{
				newImage.at<Vec<uint8_t, 1 >>(i, j)[channel] = inputImage.at<Vec<uint8_t, 1>>(i - 2, j - 2)[channel];
			}
		}
	}

	
	//заполнение правого края
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = newImage.cols - 2; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < inputImage.channels(); channel++)
			{
				newImage.at<Vec<uint8_t, 1>>(i, j)[channel] = inputImage.at<Vec<uint8_t, 1>>(i - 2, j - (newImage.cols - 2))[channel];
			}
		}
	}
	
	//заполнение левого края 
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int channel = 0; channel < inputImage.channels(); channel++)
			{
				newImage.at<Vec<uint8_t, 1>>(i, j)[channel] = inputImage.at<Vec<uint8_t, 1>>(i - 2, j + inputImage.cols - 2)[channel];
			
			}
		}
	}
	
	//заполнение верха
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < inputImage.channels(); channel++)
			{
				newImage.at<Vec<uint8_t, 1>>(i, j)[channel] = newImage.at<Vec<uint8_t, 1>>(i + newImage.rows - 4, j)[channel];
			}
		}
	}
	
	//заполнение низа
	for (int i = newImage.rows - 2; i < newImage.rows; i++)
	{
		for (int j = 0; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < inputImage.channels(); channel++)
			{
				newImage.at<Vec<uint8_t, 1>>(i, j)[channel] = newImage.at<Vec<uint8_t, 1>>(i - newImage.rows + 4, j)[channel];
			}
		}
	}
	
	return 0;
}

void gauss_blur::gaussFiltr(Mat inputImage, Mat output, float sigma)
{
	//создание ядра 5 на 5
	double matGauss[5][5] = {};
	double sumGauss = 0; //нормирование

	for (int x = -2; x < 3; x++)
	{
		for (int y = -2; y < 3; y++)
		{
			matGauss[x + 2][y + 2] = (1 / (2 * CV_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2 * sigma * sigma));
			//расчет коэф-та для нормирования (чтобы не было переполнения)
			sumGauss = sumGauss + matGauss[x + 2][y + 2];
		}
	}

	int colsImage = inputImage.cols;
	int rowsImage = inputImage.rows;

	double rez = 0; //переменная для записи значения в якорной точке ядра

	//проход ядром
	for (int i = 2; i < (rowsImage - 2); i++) //идем по строчкам
	{
		for (int j = 2; j < (colsImage - 2); j++) //столбцам
		{
			for (int channel = 0; channel < inputImage.channels(); channel++) //каналы пикселя
			{
				for (int z = 0; z < 5; z++) //проходим ядром по пикселям в окресности якорной точки
				{
					for (int v = 0; v < 5; v++)
					{
						rez = rez + matGauss[z][v] * inputImage.at<Vec<uint8_t, 1>>((i - 2) + z, (j - 2) + v)[channel];
					}
				}
				rez = rez / sumGauss;
				output.at<Vec<uint8_t, 1>>(i - 2, j - 2)[channel] = (uint8_t)rez;
				rez = 0;
			}


		}
	}
	return;
}