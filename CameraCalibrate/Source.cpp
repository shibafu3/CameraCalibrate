#ifdef _DEBUG
//Debug���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_world300d.lib")            // opencv_core
#else
//Release���[�h�̏ꍇ
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_world300.lib") 
#endif

#include <iostream>
#include <vector>
/*OpenCV���C�u����*/
#include <opencv2/opencv.hpp>
//Mat�Ƃ��s�񃂃W���[��
#include <opencv2/core/core.hpp>
//GUI���W���[��
#include <opencv2/highgui/highgui.hpp>
//�L�����u���[�V�������W���[��
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

int main(){
	//���͉摜
	Mat image;
	image = imread("C:\\00.jpg");
	//���͉摜�̃O���C�X�P�[��
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	
	//�摜�ꖇ���̃`�F�X�{�[�h�̃R�[�i�[���W������vector
	vector<Point2f> corners;
	//������摜�̖����������vector
	vector<vector<Point2f> > image_points;

	//�摜����`�F�X�{�[�h�̃R�[�i�[�����o����
	findChessboardCorners(image, Size(7, 6), corners);
	//�R�[�i�[���W�̃T�u�s�N�Z�����x�����߂�i���͂̓O���[�X�P�[���摜�j
	cornerSubPix(gray, corners, Size(20, 20), Size(-1, -1), TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.01));
	//image_points��corners��ǉ�
	image_points.push_back(corners);
	//���߂��R�[�i�[���W���摜�ɕ`�悵�Ċm�F����
	drawChessboardCorners(image, Size(7, 6), corners, true);

	//�摜�ꖇ���̃`�F�X�{�[�h�̐��E���W������vetor
	vector<Point3f> object_c;
	//������摜�̖����������vector
	vector<vector<Point3f> > object_points;
	for (int y = 0; y < 6; y++){
		for (int x = 0; x < 7; x++){
			object_c.push_back(Point3f(x * 10, y * 10, 0.0f));
		}
	}
	for (int i = 0; i < 1; i++){
		object_points.push_back(object_c);
	}

	//���߂��J�����s�������Mat
	Mat camera_matrix;
	//���߂��c�݃x�N�g��������Mat
	Mat dist_coeffs;
	//���߂��J������]�x�N�g��������vector
	vector<Mat> rvecs;
	//���߂��J�������i�x�N�g��������vector
	vector<Mat> tvecs;
	cout << object_points[0] << endl;

	//�J�����s��Ƙc�݃x�N�g���Ɖ�]�s��ƕ��i�x�N�g�������߂�
	double rms = calibrateCamera(object_points, image_points, image.size(), camera_matrix, dist_coeffs, rvecs, tvecs);
	
	//xml�t�@�C���ɏ����o��
	FileStorage cvfsw("C:\\Users\\0133752\\Desktop\\calibrate.xml", FileStorage::WRITE);
	write(cvfsw, "cameraMatrix", camera_matrix);	//	cvfsw << "cameraMatrix" << camera_matrix;
	write(cvfsw, "distCoeffs", dist_coeffs);		//	cvfsw << "distCoeffs" << dist_coeffs;
	cvfsw.release();
	//xml�t�@�C������ǂݍ���
	FileStorage cvfsr("C:\\Users\\0133752\\Desktop\\calibrate.xml", FileStorage::READ);
	FileNode node(cvfsr.fs, NULL);
	read(node["cameraMatrix"], camera_matrix);
	read(node["distCoeffs"], dist_coeffs);

	//���ژc�ݕ␳�i���Ԃ�������j
	Mat dest_image;
	undistort(image, dest_image, camera_matrix, dist_coeffs);
	imshow("image", dest_image);
	waitKey(0);

	//�c�݃}�b�v����遨�␳
	Mat remapx, remapy;
	initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(), camera_matrix, image.size(), CV_32FC1, remapx, remapy);
	remap(image, dest_image, remapx, remapy, INTER_LINEAR);
	imshow("remap image", dest_image);
	waitKey(0);


	//����␳
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