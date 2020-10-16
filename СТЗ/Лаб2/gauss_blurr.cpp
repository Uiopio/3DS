#include "gauss_blur.h"

gauss_blur::gauss_blur(float sigma)
{
	m_sigma = sigma;
}



int gauss_blur::gaussBlur(Mat inputImage, Mat outputImage, float sigma)
{
	//������� �����������, ������� ������ ��������� �� �������� ���� �� ��� �������
	Mat newInputImage(Size(inputImage.cols + 4, inputImage.rows + 4), inputImage.type(), Scalar(0, 0, 0));

	//����������� � ������������ ����
	borderWrap(newInputImage, inputImage);

	//������� �����
	gaussFiltr(newInputImage, outputImage, sigma);

	return 0;
}

int gauss_blur::borderWrap(Mat newImage, Mat inputImage)
{
	//����������� ������������� ����������� � ����� ����������� ��������
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = 2;j < newImage.cols - 2; j++)
		{
			for (int channel = 0; channel < 3; channel++)
			{
				newImage.at<Vec<uint8_t, 3>>(i, j)[channel] = inputImage.at<Vec<uint8_t, 3>>(i - 2, j - 2)[channel];
			}
		}
	}

	//���������� ������� ����
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = newImage.cols - 2; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < 3; channel++)
			{
				newImage.at<Vec<uint8_t, 3>>(i, j)[channel] = inputImage.at<Vec<uint8_t, 3>>(i - 2, j - (newImage.cols - 2))[channel];
			}
		}
	}

	//���������� ������ ���� 
	for (int i = 2; i < newImage.rows - 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int channel = 0; channel < 3; channel++)
			{
				newImage.at<Vec<uint8_t, 3>>(i, j)[channel] = inputImage.at<Vec<uint8_t, 3>>(i - 2, j + inputImage.cols - 2)[channel];
			
			}
		}
	}

	//���������� �����
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < 3; channel++)
			{
				newImage.at<Vec<uint8_t, 3>>(i, j)[channel] = newImage.at<Vec<uint8_t, 3>>(i + newImage.rows - 4, j)[channel];
			}
		}
	}

	//���������� ����
	for (int i = newImage.rows - 2; i < newImage.rows; i++)
	{
		for (int j = 0; j < newImage.cols; j++)
		{
			for (int channel = 0; channel < 3; channel++)
			{
				newImage.at<Vec<uint8_t, 3>>(i, j)[channel] = newImage.at<Vec<uint8_t, 3>>(i - newImage.rows + 4, j)[channel];
			}
		}
	}

	return 0;
}

void gauss_blur::gaussFiltr(Mat inputImage, Mat output, float sigma)
{
	//�������� ���� 5 �� 5
	float matGauss[5][5] = {};
	float sumGauss = 0;

	for (int x = -2; x < 3; x++)
	{
		for (int y = -2; y < 3; y++)
		{
			matGauss[x + 2][y + 2] = (1 / (2 * CV_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2 * sigma * sigma));

			//������ ����-�� ��� ������������ (����� �� ���� ������������)
			sumGauss = sumGauss + matGauss[x + 2][y + 2];
		}
	}



	int colsImage = inputImage.cols;
	int rowsImage = inputImage.rows;

	float rez = 0; //���������� ��� ������ �������� � ������� ����� ����

	//������ �����
	for (int i = 2; i < (rowsImage - 2); i++) //���� �� ��������
	{
		for (int j = 2; j < (colsImage - 2); j++) //��������
		{
			for (int channel = 0; channel < 3; channel++) //������ �������
			{
				for (int z = 0; z < 5; z++) //�������� ����� �� �������� � ���������� ������� �����
				{
					for (int v = 0; v < 5; v++)
					{
						rez = rez + matGauss[z][v] * inputImage.at<Vec<uint8_t, 3>>((i - 2) + z, (j - 2) + v)[channel];
					}
				}
				rez = rez / sumGauss;
				output.at<Vec<uint8_t, 3>>(i - 2, j - 2)[channel] = (uint8_t)rez;
				rez = 0;
			}


		}
	}
	return;
}