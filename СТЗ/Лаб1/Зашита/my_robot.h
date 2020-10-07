#pragma once
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

class MyRobot
{
public:
	MyRobot() = default;
	MyRobot(float width, float height,
		float wheelWidth, float wheelDiameter,
		float m_widtgTower, float m_heightTower,
		float speed, float angularSpeed);

	~MyRobot() = default;

	//сеттеры
	void setSpeed(const float speed);
	void setAngularSpeed(const float angularSpeed);
	void setArea(Size2i area); //
	int32_t setArea(Mat image);
	int32_t setCenter(Mat image);
	int32_t setCenter(float x, float y);

	//геттеры
	float getSpeed();
	float getAngularSpeed();

	int32_t rotate(int8_t buttonNumber);
	void move(int8_t buttonNumber);

	int32_t draw(Mat& outpytImage);
	Point2i pointСalculation(Point2i point, float angle);
	int8_t checkPosition(Point2f point);


	void position(float x, float y);


private:
	Point2f m_center; //к-ты центра прямоугольника
	Size2i m_barrier; //размеры барьера вокруг робота
	float m_width; //ширина
	float m_height; //высота
	float m_widthTower; //ширина башни
	float m_heightTower; //высота башни
	float m_wheelWidth; //ширина колес
	float m_wheelDiameter; //диаметр колес
	float m_speed; //скорость
	float m_angularSpeed; //скорость поворота
	float m_angle; //угол поворота робота
	float m_angleTower; //угол поворота башни
	Size2i m_area; //размер создаваемого поля
	
};