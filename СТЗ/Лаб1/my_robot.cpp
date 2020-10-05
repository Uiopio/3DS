#include "my_robot.h"

//œÂÂÏÂÌÌ‡ˇ ‰Îˇ ı‡ÌÂÌËˇ ÔÂ‰˚‰Û˘Ëı ÁÌ‡˜ÂÌËÈ ÍÓÓ‰ËÌ‡Ú ˆÂÌÚ‡
Point2f center(0, 0);


MyRobot::MyRobot(float width, float height, float wheelWidth, float wheelDiameter, float widtgTower, float heigthTower, float speed, float angularSpeed):
	m_width(width),
	m_height(height),
	m_wheelWidth(wheelWidth),
	m_wheelDiameter(wheelDiameter),
	m_widthTower(widtgTower),
	m_heightTower(heigthTower),
	m_speed(speed),
	m_angularSpeed(angularSpeed)
{
	m_angle = 0;
	m_angleTower = 0;
	m_barrier.width = m_width * 2;
	m_barrier.height = m_height * 1.05;
}

//Setters
void MyRobot::setSpeed(float speed)
{
	m_speed = speed;
}

void MyRobot::setAngularSpeed(float angularSpeed)
{
	m_angularSpeed = angularSpeed;
}

void MyRobot::setArea(const Size2i area)
{
	m_area = area;
}

int MyRobot::setArea(Mat image)
{
	if (image.empty())
	{
		return -1;
	}
	m_area.width = image.cols;
	m_area.height = image.rows;
	return 0;
}

int MyRobot::setCenter(Mat image)
{
	//ÔÓ‚ÂÍ‡ ÂÒÚ¸ ÎË ËÁÓ·‡ÊÂÌËÂ
	if (image.empty())
	{
		return -1;
	}
	//
	if ((image.cols / 2 > m_area.width) || (image.rows / 2 > m_area.height))
	{
		return -2;
	}

	m_center.x = image.cols / 2;
	m_center.y = image.rows / 2;

	return 0;
}

int MyRobot::setCenter(float x, float y)
{
	if ((x > m_area.width) ||
		(y > m_area.height))
	{
		return -1;
	}
	m_center.x = x;
	m_center.y = y;

	return 0;
}


//Getters
float MyRobot::getSpeed()
{
	return m_speed;
}

float MyRobot::getAngularSpeed()
{
	return m_angularSpeed;
}



Point2i MyRobot::point—alculation(Point2i point, float angle)
{
	Mat rotateMat = (Mat_<float>(3, 3) <<
	cos(angle), -sin(angle),m_center.x,
	sin(angle), cos(angle), m_center.y,
	    0     ,     0     ,    1     );

	Mat point1Mat = (Mat_<float>(3, 3) <<
		(point.x), 0, 0,
		(point.y), 0, 0,
		    1,     0, 0);

	Mat result;

	result = rotateMat * point1Mat;

	Point2i resultXY(result.at<float>(0, 0), result.at<float>(1, 0));

	return resultXY;
}


int32_t MyRobot::draw(Mat& outpytImage)
{
	if (outpytImage.empty())
	{
		return -1;
	}

	if ((outpytImage.cols != m_area.width) ||
		(outpytImage.rows != m_area.height))
	{
		return -2;
	}


	//ŒÔÂ‰ÂÎˇÂÏ ÔÓÎÓÊÂÌËÂ ÚÓ˜ÂÍ Ó·ÓÚ‡ ÓÚÌÓÒËÚÂÎ¸ÌÓ ˆÂÌÚ‡
	Point2i point1((-m_width / 2), (m_height / 2));
	point1 = point—alculation(point1, m_angle);

	Point2i point2((m_width / 2), (m_height / 2));
	point2 = point—alculation(point2, m_angle);

	Point2i point3((-m_width / 2), (-m_height / 2));
	point3 = point—alculation(point3, m_angle);

	Point2i point4((m_width / 2), (-m_height / 2));
	point4 = point—alculation(point4, m_angle);


	//ŒÔÂ‰ÂÎÂÌËÂ ÍÓÎÂÒ
	Point2i point1Wheel(((-m_width / 2) - m_wheelWidth), (m_height / 2));
	point1Wheel = point—alculation(point1Wheel, m_angle);
	Point2i point2Wheel(((-m_width / 2) - m_wheelWidth), ((m_height / 2) - m_wheelDiameter));
	point2Wheel = point—alculation(point2Wheel, m_angle);
	Point2i point3Wheel(((m_width / 2) + m_wheelWidth), ((m_height / 2) - m_wheelDiameter));
	point3Wheel = point—alculation(point3Wheel, m_angle);
	Point2i point4Wheel(((m_width / 2) + m_wheelWidth), ((m_height / 2)));
	point4Wheel = point—alculation(point4Wheel, m_angle);

	Point2i point1Wheel2(((-m_width / 2) - m_wheelWidth), (-m_height / 2));
	point1Wheel2 = point—alculation(point1Wheel2, m_angle);
	Point2i point2Wheel2(((-m_width / 2) - m_wheelWidth), ((-m_height / 2) + m_wheelDiameter));
	point2Wheel2 = point—alculation(point2Wheel2, m_angle);
	Point2i point3Wheel2(((m_width / 2) + m_wheelWidth), ((-m_height / 2) + m_wheelDiameter));
	point3Wheel2 = point—alculation(point3Wheel2, m_angle);
	Point2i point4Wheel2(((m_width / 2) + m_wheelWidth), ((-m_height / 2)));
	point4Wheel2 = point—alculation(point4Wheel2, m_angle);

	//ŒÔÂ‰ÂÎˇÂÏ ÔÓÎÓÊÂÌËÂ ÚÓ˜ÂÍ ·‡¯ÌË ÓÚÌÓÒËÚÂÎ¸ÌÓ ˆÂÌÚ‡
	Point2i point1Tower((-m_widthTower / 2), (m_heightTower / 2));
	point1Tower = point—alculation(point1Tower, (m_angle + m_angleTower));
	Point2i point2Tower((m_widthTower / 2), (m_heightTower / 2));
	point2Tower = point—alculation(point2Tower, (m_angle + m_angleTower));
	Point2i point3Tower((-m_widthTower / 2), (-m_heightTower / 2));
	point3Tower = point—alculation(point3Tower, (m_angle + m_angleTower));
	Point2i point4Tower((m_widthTower / 2), (-m_heightTower / 2));
	point4Tower = point—alculation(point4Tower, (m_angle + m_angleTower));
	
	//œÛ¯Í‡
	Point2i point5Tower(0, (-m_heightTower / 2));
	point5Tower = point—alculation(point5Tower, (m_angle + m_angleTower));
	Point2i point6Tower(0, -100);
	point6Tower = point—alculation(point6Tower, (m_angle + m_angleTower));

	Scalar black(0, 0, 0);
	Scalar white(255, 255, 255);


	//ÒÓÂ‰ËÌÂÌËÂ ÎËÌËˇÏË
	//ÍÓÔÛÒ
	line(outpytImage, point1, point2, black, 3, 8, 0);
	line(outpytImage, point2, point4, black, 3, 8, 0);
	line(outpytImage, point3, point4, black, 3, 8, 0);
	line(outpytImage, point1, point3, black, 3, 8, 0);

	//ÍÓÎÂÒÓ
	line(outpytImage, point1Wheel, point2Wheel, black, 3, 8, 0);
	line(outpytImage, point2Wheel, point3Wheel, black, 3, 8, 0);
	line(outpytImage, point3Wheel, point4Wheel, black, 3, 8, 0);
	line(outpytImage, point4Wheel, point1Wheel, black, 3, 8, 0);
	//ÍÓÎÂÒÓ
	line(outpytImage, point1Wheel2, point2Wheel2, black, 3, 8, 0);
	line(outpytImage, point2Wheel2, point3Wheel2, black, 3, 8, 0);
	line(outpytImage, point3Wheel2, point4Wheel2, black, 3, 8, 0);
	line(outpytImage, point4Wheel2, point1Wheel2, black, 3, 8, 0);
	
	//·‡¯Ìˇ
	line(outpytImage, point1Tower, point2Tower, black, 3, 8, 0);
	line(outpytImage, point2Tower, point4Tower, black, 3, 8, 0);
	line(outpytImage, point3Tower, point4Tower, black, 3, 8, 0);
	line(outpytImage, point1Tower, point3Tower, black, 3, 8, 0);
	//ÓÛ‰ËÂ
	line(outpytImage, point5Tower, point6Tower, black, 3, 15, 0);


	return 0;
}


int32_t MyRobot::rotate(int8_t buttonNumber)
{
	switch (buttonNumber)
	{
	case 1:
	{
		m_angle = m_angle - m_angularSpeed;
		break;
	}
	case 2:
	{
		m_angle = m_angle + m_angularSpeed;
		break;
	}
	case 3:
	{
		m_angleTower = m_angleTower + m_angularSpeed;
		break;
	}
	case 4:
	{
		m_angleTower = m_angleTower - m_angularSpeed;
		break;
	}
	break;
	}

	return 0;
}


int8_t MyRobot::checkPosition(Point2f point)
{
	if ((point.x > (m_area.width - 100)) || (point.x < 100 ))
	{
		if ((point.y > (m_area.height - 100)) || (point.y < 100))
		{
			m_center = center;
			return 0; //ÕÂÎ¸Áˇ ËÁÏÂÌËÚ¸ ÌË x  ÌË y 
		}

		m_center.x = center.x;
		return 1; //ÌÂÎ¸Áˇ ËıÏÂÌËÚ¸ x
	}
	
	if ((point.y > (m_area.height - 100)) || (point.y < 100))
	{
		m_center.y = center.y;
		return 2; //ÌÂÎ¸Áˇ ËÁÏÂÌËÚ¸ y 
	}

	return 3; //ÏÓÊÌÓ ÏÂÌˇÚ¸ ‚ÒÂ
}


void MyRobot::move(int8_t buttonNumber)
{
	center = m_center;

	if ((buttonNumber == 1))
	{
		m_center.x = m_center.x + m_speed * sin(m_angle);
		m_center.y = m_center.y - m_speed * cos(m_angle);
		checkPosition(m_center);
		return;
	}
	if ((buttonNumber == 2))
	{
		m_center.x = m_center.x - m_speed * sin(m_angle);
		m_center.y = m_center.y + m_speed * cos(m_angle);
		checkPosition(m_center);
		return;
	}
	if ((buttonNumber == 3))
	{
		m_center.x = m_center.x + m_speed * cos(m_angle);
		m_center.y = m_center.y + m_speed * sin(m_angle);
		checkPosition(m_center);
		return;
	}
	if ((buttonNumber == 4))
	{
		m_center.x = m_center.x - m_speed * cos(m_angle);
		m_center.y = m_center.y - m_speed * sin(m_angle);
		checkPosition(m_center);
		return;
	}
	
	
}