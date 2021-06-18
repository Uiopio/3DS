#include <iostream>
#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace std;


string path = "C://Users//User//source//repos//Lab5//Lab5//inputImage";
string path2 = "C://Users//User//source//repos//Lab5//Lab5//inputImage2";


void drawCube(Mat inputImage, Mat cameraMatrix, Mat distCoeffs, Vec3d rvecs, Vec3d tvecs, float len)
{
	vector<Point2f> imagePoints;

	//определение вершин куба из его высоты
	vector<Point3f> pointCube(8, Point3d(0, 0, 0));
	pointCube[0] = Point3d(len / 2, len / 2, 0);
	pointCube[1] = Point3d(-len / 2, len / 2, 0);
	pointCube[2] = Point3d(-len / 2, -len / 2, 0);
	pointCube[3] = Point3d(len / 2, -len / 2, 0);

	pointCube[4] = Point3d(len / 2, len / 2, len);
	pointCube[5] = Point3d(-len / 2, len / 2, len);
	pointCube[6] = Point3d(-len / 2, -len / 2, len);
	pointCube[7] = Point3d(len / 2, -len / 2, len);

	projectPoints(pointCube, rvecs, tvecs, cameraMatrix, distCoeffs, imagePoints);

	//// красные линии 
	for (int i = 0; i < 8; i += 2)
	{
		line(inputImage, imagePoints[i], imagePoints[i + 1], Scalar(0, 0, 255), 4);
	}

	// зеленые линии
	line(inputImage, imagePoints[1], imagePoints[2], Scalar(0, 255, 0), 4);
	line(inputImage, imagePoints[3], imagePoints[0], Scalar(0, 255, 0), 4);
	line(inputImage, imagePoints[5], imagePoints[6], Scalar(0, 255, 0), 4);
	line(inputImage, imagePoints[7], imagePoints[4], Scalar(0, 255, 0), 4);

	// синии линии
	for (int i = 0; i < 4; i++)
	{
		line(inputImage, imagePoints[i], imagePoints[i + 4], Scalar(255, 0, 0), 4);
	}

}







// считывание изображений
vector<Mat> readImgs(string filePath)
{
	vector<cv::String> fn;
	glob(filePath, fn, true);
	vector<Mat> imgs(fn.size());
	for (uint i = 0; i < fn.size(); i++)
	{
		imgs[i] = imread(fn[i]);
	}
	return imgs;
}


int main()
{
	//////////////////////////
	//Калировка по аруко доске
	//////////////////////////

	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_7X7_100); //Объявление словаря
	Ptr<aruco::GridBoard> board = aruco::GridBoard::create(5, 7, 0.08, 0.001, dictionary); //Генерация досик
	Mat arucoBoard;
	board->draw(Size(500, 600), arucoBoard, 10, 2);
	//imwrite("arucoBoard.png", arucoBoard);
	//imshow("arucoBoard", arucoBoard);
	//waitKey();

	Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
	parameters->markerBorderBits = 2;



	// создание и заполнение вектора иображений
	vector<Mat> imgs;
	imgs = readImgs(path);

	// калибровка
	vector<vector<vector<Point2f>>> allCorners;
	vector<vector<int>>allIds;
	Size imgSize;

	//Определение маркеров на фотографиях и их номера
	for (int i = 0; i < imgs.size(); i++)
	{
		vector< int > ids;
		vector< vector< Point2f > > corners, rejected;
		// detect markers
		aruco::detectMarkers(imgs[i], dictionary, corners, ids, parameters, rejected);
		allCorners.push_back(corners);
		allIds.push_back(ids);
		imgSize = imgs[i].size();
	}


	vector<vector<Point2f>> allCornersConcatenated, rejectedCandidates, imgpts;
	vector<int> allIdsConcatenated;
	vector<int> markerCounterPerFrame;
	markerCounterPerFrame.reserve(allCorners.size());

	for (int i = 0; i < allCorners.size(); i++)
	{
		markerCounterPerFrame.push_back((int)allCorners[i].size());
		for (int j = 0; j < allCorners[i].size(); j++)
		{
			allCornersConcatenated.push_back(allCorners[i][j]);
			allIdsConcatenated.push_back(allIds[i][j]);
		}
	}

	Mat cameraMatrix, distCoeffs;
	vector<Mat> rvecs, tvecs; //Вектор поврота и вектор смещения
	double arucoRepErr;
	arucoRepErr = aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated, markerCounterPerFrame, board, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs, 0);

	ofstream resultAruco;
	resultAruco.open("C://Users//User//source//repos//Lab5//Lab5//cameraMatrixAruco.txt", ios::out);
	resultAruco << cameraMatrix;
	resultAruco.close();
	cout << "cameraMatrix : " << cameraMatrix << endl;
	cout << arucoRepErr << endl;
	//cout << distCoeffs << endl;
	waitKey(0);




	//////////////////////////
	//Калировка по шахматной доске
	//////////////////////////


	//// создание и заполнение вектора иображений
	//vector<Mat> imgsChess;
	//imgsChess = readImgs(path2);

	//// Определение размеров шахматной доски
	//int CHECKERBOARD[2]{ 6, 8 };

	//// Создание вектора для хранения векторов трехмерных точек для каждого изображения шахматной доски
	//vector<vector<Point3f> > objpoints;

	//// Создание вектора для хранения векторов 2D точек для каждого изображения шахматной доски
	//vector<vector<Point2f> > imgpoints;

	//// Определение мировых координат для трехмерных точек
	//vector<Point3f> objp;
	//for (int i = 0; i < CHECKERBOARD[1]; i++)
	//{
	//	for (int j = 0; j < CHECKERBOARD[0]; j++)
	//		objp.push_back(Point3f(j, i, 0));
	//}

	//Mat gray;
	//// вектор для хранения пиксельных координат обнаруженных углов шахматной доски 
	//vector<Point2f> corner_pts;
	//bool success;

	//for (int i = 0; i < imgsChess.size(); i++)
	//{
	//	cvtColor(imgsChess[i], gray, COLOR_BGR2GRAY);

	//	// Поиск углов шахматной доски
	//	// Если на изображении найдено желаемое количество углов, тогда success = true.  
	//	success = findChessboardCorners(gray, Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

	//	// При обнаружении желаемого количества углов уточняем координаты пикселей и отображаем их на изображениях шахматной доски.
	//	if (success)
	//	{
	//		TermCriteria criteria(TermCriteria::Type::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);

	//		// уточнение пиксельных координат для заданных 2d точек.
	//		cornerSubPix(gray, corner_pts, Size(11, 11), cv::Size(-1, -1), criteria);

	//		// Отображение обнаруженных угловых точек на шахматной доске
	//		drawChessboardCorners(imgsChess[i], Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

	//		objpoints.push_back(objp);
	//		imgpoints.push_back(corner_pts);
	//	}

	//}

	//destroyAllWindows();

	//Mat cameraMatrixChess, distCoeffsChess, R, T;

	//// Выполнение калибровки камеры путем передачи значения известных 3D-точек (objpoints) и соответствующих пиксельных координат обнаруженных углов (imgpoints)
	//calibrateCamera(objpoints, imgpoints, Size(gray.rows, gray.cols), cameraMatrixChess, distCoeffsChess, R, T);

	//ofstream resultChessBoard;
	////"C://Users//User//source//repos//Lab5//Lab5//cameraMatrixAruco.txt"
	//resultChessBoard.open("C://Users//User//source//repos//Lab5//Lab5//cameraMatrixChessBoard.txt", ios::out);
	//resultChessBoard << cameraMatrixChess;
	//resultChessBoard.close();
	//cout << "cameraMatrix : " << cameraMatrixChess << endl;
	////cout << "distCoeffs : " << distCoeffsChess << endl;
	////cout << "Rotation vector : " << R << endl;
	////cout << "Translation vector : " << T << endl;




	//////////////////////////
	//    отрисовка кубика
	//////////////////////////

	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
	cap.set(CAP_PROP_FOURCC, fourcc);

	Mat input_image;

	while (waitKey(1) != 27)
	{
		cap.read(input_image);
		if (!input_image.empty())
		{
			//Определение маркеров на видео
			vector<vector<Point2f>> markerCorners, rejectedCandidates;
			vector<int> markerIds;
			aruco::detectMarkers(input_image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

			Mat outputImage = input_image.clone();
			// если обнаружен хотя бы один маркер
			if (markerIds.size() > 0) 
			{
				aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds); //Обрисовать найденный маркер
				vector<Vec3d> rvecs1, tvecs1;
				aruco::estimatePoseSingleMarkers(markerCorners, 0.01, cameraMatrix, distCoeffs, rvecs1, tvecs1); //Оценка положения каждого маркера
				// рисуем кубы
				for (int i = 0; i < markerIds.size(); i++)
				{
					drawCube(outputImage, cameraMatrix, distCoeffs, rvecs1[i], tvecs1[i], 0.01);
				}
					
			}
			imshow("output", outputImage);
		}
	}
	return 0;
}