#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_world300d.lib")            // opencv_core
#else
//Releaseモードの場合
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_world300.lib") 
#endif

#include <iostream>
#include <vector>
/*OpenCVライブラリ*/
#include <opencv2/opencv.hpp>
//Matとか行列モジュール
#include <opencv2/core/core.hpp>
//GUIモジュール
#include <opencv2/highgui/highgui.hpp>
//キャリブレーションモジュール
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

int main(){
	//入力画像
	Mat image;
	image = imread("C:\\00.jpg");
	//入力画像のグレイスケール
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	
	//画像一枚分のチェスボードのコーナー座標を入れるvector
	vector<Point2f> corners;
	//それを画像の枚数分入れるvector
	vector<vector<Point2f> > image_points;

	//画像からチェスボードのコーナーを検出する
	findChessboardCorners(image, Size(7, 6), corners);
	//コーナー座標のサブピクセル精度を求める（入力はグレースケール画像）
	cornerSubPix(gray, corners, Size(20, 20), Size(-1, -1), TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.01));
	//image_pointsにcornersを追加
	image_points.push_back(corners);
	//求めたコーナー座標を画像に描画して確認する
	drawChessboardCorners(image, Size(7, 6), corners, true);

	//画像一枚分のチェスボードの世界座標を入れるvetor
	vector<Point3f> object_c;
	//それを画像の枚数分入れるvector
	vector<vector<Point3f> > object_points;
	for (int y = 0; y < 6; y++){
		for (int x = 0; x < 7; x++){
			object_c.push_back(Point3f(x * 10, y * 10, 0.0f));
		}
	}
	for (int i = 0; i < 1; i++){
		object_points.push_back(object_c);
	}

	//求めたカメラ行列を入れるMat
	Mat camera_matrix;
	//求めた歪みベクトルを入れるMat
	Mat dist_coeffs;
	//求めたカメラ回転ベクトルを入れるvector
	vector<Mat> rvecs;
	//求めたカメラ並進ベクトルを入れるvector
	vector<Mat> tvecs;
	cout << object_points[0] << endl;

	//カメラ行列と歪みベクトルと回転行列と並進ベクトルを求める
	double rms = calibrateCamera(object_points, image_points, image.size(), camera_matrix, dist_coeffs, rvecs, tvecs);
	
	//xmlファイルに書き出し
	FileStorage cvfsw("C:\\Users\\0133752\\Desktop\\calibrate.xml", FileStorage::WRITE);
	write(cvfsw, "cameraMatrix", camera_matrix);	//	cvfsw << "cameraMatrix" << camera_matrix;
	write(cvfsw, "distCoeffs", dist_coeffs);		//	cvfsw << "distCoeffs" << dist_coeffs;
	cvfsw.release();
	//xmlファイルから読み込み
	FileStorage cvfsr("C:\\Users\\0133752\\Desktop\\calibrate.xml", FileStorage::READ);
	FileNode node(cvfsr.fs, NULL);
	read(node["cameraMatrix"], camera_matrix);
	read(node["distCoeffs"], dist_coeffs);

	//直接歪み補正（時間がかかる）
	Mat dest_image;
	undistort(image, dest_image, camera_matrix, dist_coeffs);
	imshow("image", dest_image);
	waitKey(0);

	//歪みマップを作る→補正
	Mat remapx, remapy;
	initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(), camera_matrix, image.size(), CV_32FC1, remapx, remapy);
	remap(image, dest_image, remapx, remapy, INTER_LINEAR);
	imshow("remap image", dest_image);
	waitKey(0);


	//魚眼補正
	//cv::Mat fisheye_camera_matrix;
	//cv::Mat fisheye_distortion;
	//double rms_err = cv::fisheye::calibrate(object_points, image_points, image.size(), fisheye_camera_matrix, fisheye_distortion,
	//	cv::noArray(), cv::noArray(), cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC | cv::fisheye::CALIB_CHECK_COND | cv::fisheye::CALIB_FIX_SKEW);
	//Mat dest_fisheye_image;
	//cv::fisheye::undistortImage(image, dest_fisheye_image, fisheye_camera_matrix, fisheye_distortion);
	//imshow("window", dest);
	//waitKey(0);

	return 0;
}