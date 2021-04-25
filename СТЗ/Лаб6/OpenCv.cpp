#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"

using namespace cv;
using namespace std;

int alpha_slider_1 = 720;
int alpha_slider_2 = 90;
int t = 180;
int scale = 20;

static void on_trackbar(int, void*) {}

/*функция изображения препятствий на карте*/
Mat draw_lines_on_map(Mat src, Mat map) {
	double K_angle = 74.0 / (double)src.cols;

	double K_scale_y = (double)map.rows / (double)src.rows;
	double K_scale_x = (double)map.cols / (double)src.cols;

	double K_horizontal_angle = (((double)src.cols / (double)src.rows) * 74.0) / (double)src.cols;

	for (int y = src.rows / 2; y < src.rows; y++) 
	{
		for (int x = 0; x < src.cols; x++) 
		{
			if (src.at<uchar>(src.cols * y + x) > 0) 
			{
				/*double angle = ((double)src.cols / 2 - (double)x) * K_angle;
				double angle_x = ((double)src.cols / 2 - (double)x) * K_horizontal_angle;*/

				/*int point_y = ((double)(y) * (double)alpha_slider_1 / 1000.0) * K_scale_y;
				int point_x = map.cols / 2 - ((double)map.rows - point_y) * tan(angle_x * CV_PI / 180.0);*/
				double Cy = (double)src.rows / 2;
				double Cx = (double)src.cols / 2;

				double angle_vertical = 74;
				double angle_horizontal = ((double)src.cols / (double)src.rows) * angle_vertical;
				int h = 200;

				double alpha = (y - Cy) * (angle_vertical / src.rows);
				double beta = (x - Cx) * angle_horizontal / src.cols;

				int d = map.rows - h * (1 / tan(alpha * CV_PI / 180.0)) * ((double)scale / 1000);
				int l = map.cols/2 + h * (1 / tan(alpha * CV_PI / 180.0)) * tan(beta * CV_PI / 180.0) * ((double)scale/1000);

				map.at<Vec3b>(d, l)[0] = 0;
				map.at<Vec3b>(d, l)[1] = 255;
				map.at<Vec3b>(d, l)[2] = 0;


			}
		}
	}
	return map;
}

/*функция создания шаблона карты*/
Mat create_map(void) 
{
	//карта
	Mat map(450, 450, CV_8UC3, Scalar(35, 35, 35));

	//две линии от тележки, угол между которыми равен 74 градуса
	//line(map, Point(map.cols / 2, map.rows), Point(map.cols / 2 + 200, map.rows - 260), Scalar(90, 245, 30), 2);
	//line(map, Point(map.cols / 2, map.rows), Point(map.cols / 2 - 200, map.rows - 260), Scalar(90, 245, 30), 2);

	//горизонтальные линии
	int intensity = 180;
	for (int i = 20; i <= map.rows; i += 20) 
	{
		line(map, Point(0, i), Point(map.cols, i), Scalar(intensity, intensity, intensity), 1);
	}
	//вертикальные линии
	for (int i = 20; i < map.cols; i += 20) 
	{
		line(map, Point(i, 0), Point(i, map.rows), Scalar(intensity, intensity, intensity), 1);
	}
	return map;
}

/*обрезание кадра*/
Mat filter_lines(Mat src) 
{
	Mat clone = src.clone();
	//все, что выше середины кадра, нас не интересует
	Rect deth_zone(0, 0, clone.cols, clone.rows / 2);
	rectangle(clone, deth_zone, Scalar(0, 0, 0), -1, 8, 0);

	return clone;
}

/*функция фильтрации всех линий от лазера*/
Mat find_lines(Mat src) 
{
	Mat src_hsv;
	Mat contours_img;
	cvtColor(src, src_hsv, COLOR_BGR2HSV);

	double mean_value = 0;
	for (int i = 0; i < src_hsv.rows; i++) 
	{
		for (int j = 0; j < src.cols; j++) 
		{
			mean_value += (double)src_hsv.at<Vec3b>(i, j)[2];
		}
		mean_value /= src_hsv.cols;
	}

	inRange(src_hsv.clone(), Scalar(30, 0, 50), Scalar(90, 160, 255), contours_img);

	Mat contours_img_show = contours_img.clone();
	resize(contours_img_show.clone(), contours_img_show, Size(960, 540));
	imshow("find_lines_result", contours_img_show);

	return contours_img;
}

Mat find_lines2(Mat src)
{
	Mat clone_frame = src.clone();
	cvtColor(clone_frame, clone_frame, COLOR_BGR2GRAY);


	//GaussianBlur(clone_frame, clone_frame, Size(1, 1), 2, 2);
	threshold(clone_frame, clone_frame, t, 255, THRESH_BINARY);


	//erode(clone_frame, clone_frame, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)));
	dilate(clone_frame, clone_frame, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
	imshow("1", clone_frame);


	return clone_frame;
}


int main()
{
	//ползунки с настройками
	namedWindow("Settings", WINDOW_AUTOSIZE);

	createTrackbar("distance", "Settings", &alpha_slider_1, 1000, on_trackbar);
	on_trackbar(alpha_slider_1, 0);

	createTrackbar("intensity", "Settings", &alpha_slider_2, 250, on_trackbar);
	on_trackbar(alpha_slider_2, 0);

	createTrackbar("t", "Settings", &t, 255, on_trackbar);
	on_trackbar(t, 0);

	createTrackbar("scale", "Settings", &scale, 1000, on_trackbar);
	on_trackbar(scale, 0);

	char filename[] = "./2.avi";
	VideoCapture cap(filename);

	Mat src;

	Mat map = create_map();

	while (1) {
		cap >> src;
		if (src.empty())
			break;

		src.convertTo(src, -1, (double)alpha_slider_2 / 100.0);

		Mat map_1 = map.clone();
		Mat filter = filter_lines(src);
		map_1 = draw_lines_on_map(find_lines2(filter), map_1);

		imshow("map", map_1);

		imshow("src", src);


		waitKey(10);
	}
}

