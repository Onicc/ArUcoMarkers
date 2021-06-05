#include <cstdio>
#include <string>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

#define GENMARKER 0

const Mat  cameraMatrix = (Mat_<float>(3, 3)
                               << 1687.63168    , 0         , 945.751409,
                                  0             , 1688.02766, 550.975456,
                                  0             , 0         , 1         );

const Mat  distCoeffs = (Mat_<float>(5, 1) << 0.10288852, 1.11939895, 0.01042932, -0.00966117, -6.64679829);

// const Mat  cameraMatrix = (Mat_<float>(3, 3)
//                                << 562.7828532   , 0             , 319.48935944,
//                                   0             , 562.03428797  , 222.99766471,
//                                   0             , 0         , 1         );

// const Mat  distCoeffs = (Mat_<float>(5, 1) << -0.04760738, 0.85058236, -0.00975579, 0.00247201, -1.27175284);

const Mat  arucodistCoeffs = (Mat_<float>(1, 5) << 0, 0, 0, 0, 0);//矫正后的照片用于检测

/**
 * @brief  生成marker
 * @param  id               图像编号
 * @param  sidePixels       图像大小
 */
void genMarker(int id, int sidePixels) {
    Mat markerImage;
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    aruco::drawMarker(dictionary, id, sidePixels, markerImage, 1);
    imwrite("marker/marker" + to_string(id) + "_" + to_string(sidePixels) + ".png", markerImage);
    // imshow("maker", markerImage);
    // waitKey(0);
}

void detectMarker(Mat inputImage) {
    vector<int> markerIds;
    vector<vector<Point2f>> markerCorners, rejectedCandidates;
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    
    Mat outputImage = inputImage.clone();
    aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);

    vector<Vec3d> rvecs, tvecs;
    aruco::estimatePoseSingleMarkers(markerCorners, 0.123, cameraMatrix, distCoeffs, rvecs, tvecs);
    // inputImage.copyTo(outputImage);
    for (int i = 0; i < rvecs.size(); ++i) {
        auto rvec = rvecs[i];
        auto tvec = tvecs[i];
        aruco::drawAxis(outputImage, cameraMatrix, distCoeffs, rvec, tvec, 0.123/2);

        Mat rotationMatrix;
        Rodrigues(rvec, rotationMatrix);
        double sita_x = atan2(rotationMatrix.at<double>(2, 1), rotationMatrix.at<double>(2, 2)) * 180.0 / M_PI;
        double sita_y = atan2(-rotationMatrix.at<double>(2, 0), sqrt(pow(rotationMatrix.at<double>(2, 1), 2) + pow(rotationMatrix.at<double>(2, 2), 2))) * 180.0 / M_PI;
        double sita_z = atan2(rotationMatrix.at<double>(1, 0), rotationMatrix.at<double>(0, 0)) * 180.0 / M_PI;
        // cout << "id:" << i << ", R:" << rotationMatrix << ", T:" << tvec << endl;
        // cout << "sita_x:" <<  sita_x << endl;
        // cout << "sita_y:" <<  sita_y << endl;
        // cout << "sita_z:" <<  sita_z << endl;
        cout << "t:" << tvec << endl;
    }

    // resize(outputImage, outputImage, Size(960, 540));
    imshow("outputImage", outputImage);
    // waitKey(0);
}

int main()
{
    #if(GENMARKER == 1)
        for(int i = 0; i < 250; i++) {
            genMarker(i, (i%10 + 1)*100);
        }
    #endif

    // vector<String> filenames; 
    // String folder = "Cmarker100-5/*jpg";

    // glob(folder, filenames);
    // for(int i = 0; i < filenames.size(); i++) {
    //     cout << filenames[i] << endl;
    //     Mat inputImage = imread(filenames[i]);
    //     detectMarker(inputImage);
    // }


	VideoCapture capture("data/dark_video/IMG_8789.MOV");
 
	while(true)
	{
		Mat frame;
        capture >> frame;
        detectMarker(frame);
        waitKey(1);
    }

    // VideoCapture inputVideo;
    // inputVideo.open(0);
    // while (inputVideo.grab()) {
    //     cv::Mat image;
    //     inputVideo.retrieve(image);
    //     // imshow("video", image);
    //     detectMarker(image);
    //     char key = (char) cv::waitKey(1);
    //     if (key == 27)
    //         break;
    // }

    return 0;
}