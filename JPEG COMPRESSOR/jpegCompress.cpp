/*
# This is "jpegCompress.cpp" file. It contains the defination of all the function 	#
# declared in the "Compress" class of "jpegCompress.h" file to perform compression 	#
# and decompression on the medical image using JPEG Compression Technique.			#
*/

#include <iostream>
#include <cstring>
#include <cmath>
#include <cv.h>
#include <highgui.h>

#include "jpegCompress.h" // The function prototypes are declared in this header file.

#define PI 3.14159265358979324

using namespace std;
using namespace cv;

Compress::Compress() { }

Compress::~Compress() { } 


// This function takes an IplImage as input, divides the image in 8x8 windows and 
// then get the pixel value of the window. Then is applies DCT, normalization and 
// IDCT functions to remove the redundant data and return back compressed IplImage.

IplImage* Compress::jpegCompress(IplImage *sourceImage) {

	width = sourceImage->width; // getting width of the source image.
	height = sourceImage->height; //  getting height of the source image.
	int widthStep = sourceImage->width; //  getting widthStep of the source image.
	
	// creating an IplImage to store comressed image.
	IplImage* compressedImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);
	
	// matrix to store finale compressed data.
	unsigned long finalMatrix[1600][1200];

	//memset(finalMatrix, 0, sizeof(finalMatrix));

	IplImage* tmpImage;

	int x, y;

	for(x = 0; x+8 <= height; x += 8) { // iterating the loop with increment of window height.

		for(y = 0; y+8 <= width; y += 8) { //iterating the loop with increament of window width.
			
			cvSetImageROI(sourceImage, cvRect(y, x, 8, 8)); // setting image ROI to window size(8x8).

			tmpImage = cvCreateImage(cvSize(8, 8), IPL_DEPTH_8U, 1); // creating the temporary image with the size of ROI.
			cvCopy(sourceImage, tmpImage); // copying the data from source image ROI to temporary image.
			
			cvResetImageROI(sourceImage);
			
			int tmpMatrix[8][8]; // matrix to store DCT data of temporary image.

			imageToMatrix(tmpImage, tmpMatrix); // function to get the pixel value matrix from tmp image.
		
			float shiftedMatrix[8][8]; // matrix to shifted data of 8x8 IDCT matrix

			for(int i = 0;  i < 8; i++) {

				for(int j = 0; j < 8; j++) {

					shiftedMatrix[i][j] = tmpMatrix[i][j]-128; // level shifting by -128

				}

			}

			float dctMatrix[8][8]; // stores 8x8 DCT matrix
			compute8x8DCT(shiftedMatrix, dctMatrix); // function to compute 8x8 DCT matrix
			
			int quantizedMatrix[8][8]; // stores quantized data 
			compute8x8Normalization(dctMatrix, quantizedMatrix); // function to apply quatization on 8x8 matrix

			int dequantizedMatrix[8][8]; // stores de-quantized data 
      		compute8x8Denormalization(quantizedMatrix, dequantizedMatrix); //// function to apply de-quatization on 8x8 matrix

			float idctMatrix[8][8]; // stores IDCT matrix
			compute8x8IDCT(dequantizedMatrix, idctMatrix); // function to compute IDCT matrix

			int compressedMatrix[8][8]; // final 8x8 matrix shifted back by +128
			int val;
			for(int i = 0; i < 8; i++) {
			
				for(int j = 0; j < 8; j++) {

					val = idctMatrix[i][j] + 128; // shifting by 128
					if(val < 0) { val *= -1;; } 
					val += 0.5; // getting closest integer value.

					compressedMatrix[i][j] = (int)val;

				}

			}
			
			// storing 8x8 compressed data to final matrix
			for(int i = x; i < x+8; i++) {

				for(int j = y; j < y+8; j++) {

					finalMatrix[i][j] = compressedMatrix[i-x][j-y];	
					
				}
				
			}

			cvResetImageROI(sourceImage);

		}

		// By the above process only multiple of 8 width and height portion is compressed
		// Now setting the rest value as it is in the final matrix.
		int wd = width - y;
		if(wd) {
			
			cvSetImageROI(sourceImage, cvRect(y, x, wd, 8)); // setting image ROI to window size(8x8).
		
      		tmpImage = cvCreateImage(cvSize(wd, 8), IPL_DEPTH_8U, 1); // creating the temporary image with the size of ROI.
     		cvCopy(sourceImage, tmpImage); // copying the data from source image ROI to temporary image.

        	cvResetImageROI(sourceImage);

        	int tmpMatrix[8][8]; // matrix to store DCT data of temporary image.

       		imageToMatrix(tmpImage, tmpMatrix); // converts image to matrix

			// storing data to final matrix
			for(int i = x; i < x+8; i++) {

        		for(int j = y; j < y+wd; j++) {

            		finalMatrix[i][j] = tmpMatrix[i-x][j-y];
         		}

    		}
		
		}
	}

	int ht = height - x;
	int flag = 0;

	if(ht) {

		for(int y = 0; y < width+8; y++) {

			int wd = 8;

			if(width-y-8 < 8) {

				wd = width-y-8;
				flag = 1;

			}

			
			cvSetImageROI(sourceImage, cvRect(y, x, wd, ht)); // setting image ROI to window size(8x8).

  			tmpImage = cvCreateImage(cvSize(wd, ht), IPL_DEPTH_8U, 1); // creating the temporary image with the size of ROI.
   			cvCopy(sourceImage, tmpImage); // copying the data from source image ROI to temporary image.
    		cvResetImageROI(sourceImage);

    		int tmpMatrix[8][8]; // matrix to store DCT data of temporary image.

    		imageToMatrix(tmpImage, tmpMatrix); // converts image to matrix

			// stroring data to final matrix
    		for(int i = x; i < x+ht; i++) {

     			for(int j = y; j < y+wd; j++) {

          			finalMatrix[i][j] = tmpMatrix[i-x][j-y];
       			}

  			}

			if(flag) {

				break;

			}

		}
	}

	cvReleaseImage(&tmpImage); // releasing the memory held by tempImage

	matrixToImage(finalMatrix, compressedImage); // this function creates back the image from the matrix

	return compressedImage; // returning back the compressed image

}


// This function takes an 8x8 IplImage pointer and a matrix as input 
// stores the image data in matrix.
void Compress::imageToMatrix(IplImage *img, int matrix[][8]) {

	int width = img->width; 
	int height = img->height;
	int widthStep = img->widthStep;
	
	uchar* data = (uchar*)img->imageData;

	for(int i = 0; i < height; i++) {

		data = (uchar*)(img->imageData + i * widthStep);

		for(int j = 0; j < width; j++) {

			matrix[i][j] = (int)*data++;

		}

	}

}


// This function takes a matrix and a 8x8 IplImage pointer as input 
// stores the data from matrix to image
void Compress::matrixToImage(unsigned long in[][1200], IplImage* compressed) {

	int widthstep = compressed->widthStep;
	int wd = compressed->width;	

	uchar *data = (uchar*)compressed->imageData;

	for(int i = 0; i < height; i++) {

		for(int j = 0; j < width; j++) {

			#if 1
			if(in[i][j] > 255) {
				in[i][j] = 255;
			}
			#endif

			*data = (unsigned char)(in[i][j]);
			data++;

		}
		
		// escaping the padding
		for(int k = 0; k < widthstep-wd; k++) {		
		
			data++;
			
		}

	}

}

// this function computes DCT on 8x8 matix but converting the 8x8 matix from
// intensity domain to frequency domain and stores in out matrix
void Compress::compute8x8DCT(float in[][8], float out[][8]) {

    float Cu, Cv, sum;

    for(int u = 0; u < 8; u++) {

        for(int v = 0; v < 8; v++) {

            if(u == 0) { Cu = 1/sqrt(2); }
            else { Cu = 1; }

            if(v == 0) { Cv = 1/sqrt(2); }
            else { Cv = 1; }

            sum = 0.0;

            for(int i =0; i < 8; i++) {

                for(int j = 0; j < 8; j++) {

					// applying mathematical formula on 8x8 matrix for DCT
                    sum += in[i][j] * cos((2 * i+1) * u *PI/16.0) * cos((2 * j+1) * v * PI/16.0); 

                }

            }

            out[u][v] = (Cu * Cv * sum)/4;

        }

    }

}

// this function computes IDCT on 8x8 matix but converting the 8x8 matix from
// frequency domain to intensity domain and stores in out matrix
void Compress::compute8x8IDCT(int in[8][8], float out[8][8]) {

    float sum, Cu, Cv;

    for (int x = 0; x < 8; x++) {

        for (int y = 0; y < 8; y++) {

            sum = 0;

            for (int u = 0; u < 8; u++) {

                for (int v = 0; v < 8; v++) {

                    if(u == 0) Cu = 1/sqrt(2);
                    else Cu = 1;

                    if(v == 0) Cv = 1/sqrt(2);
                    else Cv = 1;

					// applying mathematical formula for IDCT
                    sum += Cu * Cv * in[u][v] * cos((2 * x + 1) * u * PI / 16) *cos((2 * y + 1) * v * PI / 16);

                    out[x][y] = sum / 4;
                }

            }

        }

    }
}

// this function takes two matrix as inpute apply quatization by dividing in matrix by 
// quatization matrix, round off the result to closest integer ans stores in out matrix
void Compress::compute8x8Normalization(float in[][8], int out[][8]) {

	float pixelValue;

	for(int x = 0; x < 8; x++) {

		for(int y = 0; y < 8; y++) {

			pixelValue = in[x][y] / normalizationMatrix[x][y];

			if(pixelValue < 0) { pixelValue -= 0.5; } // computing closest integer
			else { pixelValue += 0.5; }
			
			out[x][y] = (int)(pixelValue);

		}

	}

}

// this function takes two matrix as inpute apply quatization by multiplying the quantization 
//matrix on in matrix,round off the result to closest integer ans stores in out matrix
void Compress::compute8x8Denormalization(int in[][8], int out[][8]) {

    for(int x = 0; x < 8; x++) {

        for(int y = 0; y < 8; y++) {

            out[x][y] = in[x][y] * normalizationMatrix[x][y];

        }

    }

}
