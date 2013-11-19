#include <iostream>
#include <string>
#include <cv.h>
#include <highgui.h>

#include "jpegCompress.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

	if(argc != 3) {

		cout << "error::invalid arguments< 2 needed "<< argc << " provided. >" << endl;
		cout << "format: executable file<space>source image path<space>destination image path" << endl;
		return -1;

	}

	string str = argv[2];

	if (!(str.find(".jpg") != std::string::npos)) {

    	cout << "error::destination path/name invalid" << endl;
        cout << "format: imagePath/imageName.jpg" << endl;
        cout << "*image path is not required for current folder." << endl;
		return -1;

	}

	if(str.length() < 4) {
	
			cout << "error::destination path/name invalid" << endl;
			cout << "format: imagePath/imageName.jpg" << endl;
			cout << "*image path is not required for current folder." << endl;
			return -1;

	}
	
	cout << "Processing.." << endl;
	
	IplImage* sourceImage = cvLoadImage(argv[1], 0);

	if(!sourceImage) {

		cout << "error::image not found." << endl;
		return -1;

	}
	
	int width = sourceImage->width;
	int height = sourceImage->height;
	
	Compress compress;
	IplImage* compressed = compress.jpegCompress(sourceImage);
	
	cvSaveImage(argv[2], compressed);

	cout << "Compression done.." << endl;

	return 0;

}
