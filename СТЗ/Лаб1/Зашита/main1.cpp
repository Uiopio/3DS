#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "my_robot\my_robot.h"
//#include "windows.h"


using namespace std;
using namespace cv;



int main()
{


	//цвета и размер
	Scalar white(255, 255, 255);
	Scalar black(0, 0, 0);
	Size size(1600, 900);

	//ширина и высота робота
	float width = 60;
	float lenght = 120;
	//Башня
	float widthTower = 10;
	float lenghtTower = 10;

	//ширина и диаметр колес
	float wheelWidth = 10;
	float wheelDiameter = 30;

	//создаем робота
	MyRobot robot(width, lenght, wheelWidth, wheelDiameter, widthTower, lenghtTower, 0, 0);
	robot.setSpeed(10);
	robot.setAngularSpeed(0.03);

	//создаем поле
	Mat background(size, CV_8UC3, white);

	//передаем размеры поля и определяем центр
	robot.setArea(background);
	robot.setCenter(background);


	//координаты для закона движения
	float x[100];
	float y[100];
	float angle = CV_2PI / 100;


	//рассчет для двиэения по фигуре Лиссажу
	for (int i = 0; i < 100; i++)
	{
		x[i] = 200 * cos(i * angle);
		y[i] = 200 * sin(2*i * angle );
		//cout << x[i]<<"\n" << y[i] << "\n";
	}

	int j = 0;
	
	//Передаем координаты в функцию
	while (waitKey(25) != 27)
	{
		j++;
		if (j >= 100)
		{
			j = 0;
			cout << "сброс" << "\n";
		}
		
		
		robot.position(x[j], y[j]);
		Mat backgroundAndRobot2(size, CV_8UC3, white);
		robot.draw(backgroundAndRobot2);
		
		imshow("backgroundAndRobot2", backgroundAndRobot2);

	}




	//проверяем кнопки
	char key = 0;
	while (waitKey(25) != 27)
	{
		key = waitKey(30);
		//каждый раз создаем новое поля для отображения
		Mat backgroundAndRobot(size, CV_8UC3, white);

		if ((key == 'w') || (key == 'W'))
		{
			robot.move(1);
		}
		if ((key == 's') || (key == 'S'))
		{
			robot.move(2);
		}
		if ((key == 'd') || (key == 'D'))
		{
			robot.move(3);
		}
		if ((key == 'a') || (key == 'A'))
		{
			robot.move(4);
		}
		if ((key == 'q') || (key == 'Q'))
		{
			robot.rotate(1);
		}
		if ((key == 'e') || (key == 'E'))
		{
			robot.rotate(2);
		}
		if ((key == 'r') || (key == 'R'))//башня направо
		{
			robot.rotate(3);
		}
		if ((key == 't') || (key == 'T'))//башня налево
		{
			robot.rotate(4);
		}

		//robot.position(x[i], y[i]);
		//рисуем на новом поле
		robot.draw(backgroundAndRobot);

		imshow("backgroundAndRobot", backgroundAndRobot);


	}




}