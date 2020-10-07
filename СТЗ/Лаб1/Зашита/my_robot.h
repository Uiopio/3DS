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

	//�������
	void setSpeed(const float speed);
	void setAngularSpeed(const float angularSpeed);
	void setArea(Size2i area); //
	int32_t setArea(Mat image);
	int32_t setCenter(Mat image);
	int32_t setCenter(float x, float y);

	//�������
	float getSpeed();
	float getAngularSpeed();

	int32_t rotate(int8_t buttonNumber);
	void move(int8_t buttonNumber);

	int32_t draw(Mat& outpytImage);
	Point2i point�alculation(Point2i point, float angle);
	int8_t checkPosition(Point2f point);


	void position(float x, float y);


private:
	Point2f m_center; //�-�� ������ ��������������
	Size2i m_barrier; //������� ������� ������ ������
	float m_width; //������
	float m_height; //������
	float m_widthTower; //������ �����
	float m_heightTower; //������ �����
	float m_wheelWidth; //������ �����
	float m_wheelDiameter; //������� �����
	float m_speed; //��������
	float m_angularSpeed; //�������� ��������
	float m_angle; //���� �������� ������
	float m_angleTower; //���� �������� �����
	Size2i m_area; //������ ������������ ����
	
};