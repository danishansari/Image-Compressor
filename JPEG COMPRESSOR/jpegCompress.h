#ifndef JPEGCOMPRESS_H
#define JPEGCOMPRESS_H
#include <cv.h>

class Compress {

	public:	
		Compress();
		~Compress();

		IplImage* jpegCompress(IplImage*);
		IplImage* jpegDecompress(IplImage*);

	private:
		int width;
        int height;
        int normalizationMatrix[8][8] = { {16, 11, 10, 16, 24, 40, 51, 61},
                                  		{12, 12, 14, 19, 26, 58, 60, 55},
                                  		{14, 13, 16, 24, 40, 57, 69, 56},
                                  		{14, 17, 22, 29, 51, 87, 80, 62},
                                  		{18, 22, 37, 56, 68, 109, 103, 77},
                                  		{24, 35, 55, 64, 81, 104, 113, 92},
                                  		{49, 64, 78, 87, 103, 121, 120, 101},
                                  		{72, 92, 95, 98, 112, 100, 103, 99} };	

		void imageToMatrix(IplImage*, int[][8]);
		void matrixToImage(unsigned long [1600][1200], IplImage*);
		void compute8x8DCT(float[][8], float[][8]);
		void compute8x8IDCT(int[][8],float[][8]);
		void compute8x8Normalization(float[][8], int[][8]);
		void compute8x8Denormalization(int[][8], int[][8]);
		
};

#endif
