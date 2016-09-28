#include "CImg.h"
#include "kernel.h"
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <queue>
#include <string>
#include "dirent.h"
using namespace std;
using namespace cimg_library;

class point
{
public:
	long int X;
	long int Y;
	long int XY;
};

float gaussOne(float X, float sigma, float mu)
{
	return std::exp((-1*(X-mu)*(X-mu))/(2*sigma*sigma));

}
float findInVector(vector<point> Q, point e)
{
	float returnVal = 0;
	for (int i = 0; i < Q.size(); i++)
	{
		if (Q.at(i).XY == e.XY)
		{
			returnVal = i;
		}
	}

	return returnVal;
}

float minValue(float x, float y)
{
	if (x > y)
	{
		return y;
	}
	else
	{
		return x;
	}
}


float getMean(int xSize, int ySize, std::vector<float> resultVector)
{
	float count = 0;
	float sum = 0;
	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			sum = sum + resultVector[x + y*ySize];

			count++;
		}
	}
	//cout << sum << " " << count << " " << sum / count << endl;
	return sum / count;

}
float getVariance(int xSize, int ySize, std::vector<float> resultVector, float meanValue)
{

	float variance = 0;
	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			variance = variance + (resultVector[x + y*ySize] - meanValue)*(resultVector[x + y*ySize] - meanValue);

			
		}
	}
	//cout << "!! " << variance / (xSize*ySize) << " !! " << endl;
	return variance/(xSize*ySize);
}

int main(){
	//Skonczone na 1896 + na noc 1720
	//char* FILE_NAME = "A_1236_1_RIGHT_CC";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("../../inputIm")) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			string rawfileName = ent->d_name;
			if (rawfileName.length() > 3)
			{
				rawfileName.erase(rawfileName.size() - 10,rawfileName.size());

				printf("\n \n %s \n", rawfileName);
				const char* FILE_NAME = rawfileName.c_str();
	
	
	double tmstart, tmstop, tmtime;
	time_t tstart, tend;

//	CImg<unsigned char> image1("inputIm/A_1620_1_RIGHT_CC.bmp");
//	CImg<unsigned char> image2("inputSeed/A_1620_1_RIGHT_CC.bmp");
	char chFileName[80];
	strcpy(chFileName, "../../inputIm/");
	strcat(chFileName, FILE_NAME);
	strcat(chFileName, "_W_BMP.bmp");
	CImg<unsigned char> image1(chFileName);

	strcpy(chFileName, "../../inputSeed_reczny/");
	strcat(chFileName, FILE_NAME);
	strcat(chFileName, "_M_WSEED_BMP.bmp");
	CImg<unsigned char> image2(chFileName);

	CImg<unsigned char> outputCImg = image1;
	CImg<unsigned char> outputCImg2 = image1;

	const int imageH = image1.height();
	const int imageW = image1.width();
	int imageSize = imageW*imageH;

	S_IMG inputImage;
	S_IMG maskImage;
	S_IMG tempImage;
	S_IMG outputImage;
	S_IMG outputNormalisedImage;
	inputImage.pxMap = new float[imageSize];
	maskImage.pxMap = new float[imageSize];
	tempImage.pxMap = new float[imageSize];
	outputImage.pxMap = new float[imageSize];
	outputNormalisedImage.pxMap = new float[imageSize];
	for (long int i = 0; i < imageH; i++)
	{
		for (long int j = 0; j < imageW; j++)
		{
	

			unsigned char color2[3] = { 0, 0, 0 };
			outputCImg2.draw_point(i, j, 0, color2, 1.0f);

		}
	}
	tmstart = (double)clock() / CLOCKS_PER_SEC;
	tstart = time(0);

	unsigned char c, c2;
	for (long int i = 0; i < imageH; i++)
	{
		for (long int j = 0; j<imageW; j++)
		{


			c = image1(i, j);
			c2 = image2(i, j);
			inputImage.pxMap[j + i*imageW] = c;
			//normalizing mask
			maskImage.pxMap[j + i*imageW] = c2;

			tempImage.pxMap[j + i*imageW] = inputImage.pxMap[j + i*imageW] * maskImage.pxMap[j + i*imageW];
			outputImage.pxMap[j + i*imageW] = 0;
		}
	}

	//LOOKING FOR MAXMIN
	float minVar = 300;
	float maxVar = -100;
	for (int t = 0; t < imageSize; t++)
	{
		if (minVar>maskImage.pxMap[t])
			minVar = maskImage.pxMap[t];

		if (maxVar < maskImage.pxMap[t])
			maxVar = maskImage.pxMap[t];
	}
	
	for (int t = 0; t < imageSize; t++)
	{
		maskImage.pxMap[t] /= maxVar;
	}

	//END

	//building vector of values ignoring 0s, finding central point
	long int cPointX = 0;
	long int cPointY = 0;

	std::vector<float> resultVector;
	float oneCount = 0;
	for (long int i = 0; i<imageH; i++)
	{
		for (long int j = 0; j<imageW; j++)
		{

			if (maskImage.pxMap[j + i*imageW] == 1)
			{
				resultVector.push_back(inputImage.pxMap[j + i*imageW]);
				cPointX = cPointX + i;
				cPointY = cPointY + j;
				oneCount++;
			}

		}
	}

	cPointX = cPointX / oneCount;
	cPointY = cPointY / oneCount;
	//cout << "middlePoint: (" << cPointX << "," << cPointY << ")" << endl;
	//Calculating parameters required for fuzzy conn
	float meanValue = getMean(oneCount, 1, resultVector);
	float variance = getVariance(oneCount, 1, resultVector, meanValue);
	cout << "mean: " << meanValue << " variance: " << variance << endl;
	
	//TRACKING!
	vector<point> Q;
	point cent;
	cent.X = cPointX;
	cent.Y = cPointY;
	cent.XY = cPointY + cPointX*imageW;
	Q.push_back(cent);
	outputImage.pxMap[cent.XY] = 1;
	float maxQueueSize = -1;
	while (Q.size() > 0)
	{
		int maxInd = 0;
		float maxVal = -1;
		//Finding max value and its index
		for (int i = 0; i < Q.size(); i++)
		{
			if (maxVal < inputImage.pxMap[Q.at(i).XY])
			{
				maxVal = inputImage.pxMap[Q.at(i).XY];
				maxInd = i;
			}
			//dla ktorego indeksy Q obraz ma max jasnosc piksela, od tego zacznamy
		}
		//end of finding max value and index		
		point c = Q.at(maxInd);
		float C = inputImage.pxMap[c.XY];
		Q.erase(Q.begin() + maxInd);


		//DIRECTIONAL TRACKING

		//Tracking LEFT
		point e = c;
		e.X -= 1;
		e.XY -= 1;
		if (e.XY >= 0)
		{
			float E = inputImage.pxMap[e.XY];
			float gO = gaussOne(abs(C + E) / 2, variance, meanValue);
			float gT = gaussOne(abs(C - E) / 2, variance, 0);
			float tempNi = sqrt(gO * gT);

			if (tempNi > 0)
			{
				float fmin = minValue(outputImage.pxMap[c.XY], tempNi);
				if (fmin > outputImage.pxMap[e.XY])
				{
					outputImage.pxMap[e.XY] = fmin;

					float location = findInVector(Q, e);
					if (location > 0)
					{
						Q.erase(Q.begin() + location);
						Q.push_back(e);
					}
					else
					{
						Q.push_back(e);
					}
				}
			}



		}


		//Tracking RIGHT
		e = c;
		e.X += 1;
		e.XY += 1;
		if (e.XY < imageSize)
		{
			float E = inputImage.pxMap[e.XY];
			float gO = gaussOne(abs(C + E) / 2, variance, meanValue);
			float gT = gaussOne(abs(C - E) / 2, variance, 0);
			float tempNi = sqrt(gO * gT);

			if (tempNi > 0)
			{
				float fmin = minValue(outputImage.pxMap[c.XY], tempNi);
				if (fmin > outputImage.pxMap[e.XY])
				{
					outputImage.pxMap[e.XY] = fmin;

					float location = findInVector(Q, e);
					if (location > 0)
					{
						Q.erase(Q.begin() + location);
						Q.push_back(e);
					}
					else
					{
						Q.push_back(e);
					}
				}
			}



		}




		//Tracking UP
		e = c;
		e.Y -= 1;
		e.XY -= imageW;
		if (e.XY >= 0)
		{
			float E = inputImage.pxMap[e.XY];
			float gO = gaussOne(abs(C + E) / 2, variance, meanValue);
			float gT = gaussOne(abs(C - E) / 2, variance, 0);
			float tempNi = sqrt(gO * gT);

			if (tempNi > 0)
			{
				float fmin = minValue(outputImage.pxMap[c.XY], tempNi);
				if (fmin > outputImage.pxMap[e.XY])
				{
					outputImage.pxMap[e.XY] = fmin;

					float location = findInVector(Q, e);
					if (location > 0)
					{
						Q.erase(Q.begin() + location);
						Q.push_back(e);
					}
					else
					{
						Q.push_back(e);
					}
				}
			}



		}

		//Tracking DOWN
		e = c;
		e.Y += 1;
		e.XY += imageW;
		if (e.XY < imageSize)
		{
			float E = inputImage.pxMap[e.XY];
			float gO = gaussOne(abs(C + E) / 2, variance, meanValue);
			float gT = gaussOne(abs(C - E) / 2, variance, 0);
			float tempNi = sqrt(gO * gT);

			if (tempNi > 0)
			{
				float fmin = minValue(outputImage.pxMap[c.XY], tempNi);
				if (fmin > outputImage.pxMap[e.XY])
				{
					outputImage.pxMap[e.XY] = fmin;

					float location = findInVector(Q, e);
					if (location > 0)
					{
						Q.erase(Q.begin() + location);
						Q.push_back(e);
					}
					else
					{
						Q.push_back(e);
					}
				}
			}



		}
		if (maxQueueSize < Q.size())
		{
			maxQueueSize = Q.size();
		}
		//cout << "Queue size: " << Q.size() << endl;
	}
	//END OF TRACKING
	cout << "Max Queue Size found: " << maxQueueSize << endl;
	tend = time(0);
	tmstop = (double)clock() / CLOCKS_PER_SEC;

	tmtime = tmstop - tmstart;
	cout << "CPP CPU execution time " << tmtime << endl;

	float maxImageValue = -1;
	float minImageValue = 300;

	for (int i = 0; i < imageH*imageW; i++)
	{
		if (minImageValue>outputImage.pxMap[i])
			minImageValue = outputImage.pxMap[i];
	}
	
	for (int i = 0; i < imageH*imageW; i++)
	{
		outputNormalisedImage.pxMap[i] = outputImage.pxMap[i] - minImageValue;
	}
	//cout << "Min Val: " << minImageValue << endl;
	for (int i = 0; i < imageH*imageW; i++)
	{
		if (maxImageValue < outputNormalisedImage.pxMap[i])
			maxImageValue = outputNormalisedImage.pxMap[i];
	}

	for (int i = 0; i < imageH*imageW; i++)
	{
		outputNormalisedImage.pxMap[i] /= maxImageValue;
		outputNormalisedImage.pxMap[i] *= 255;
	}
	//Writing output
	for(int i = 0; i < imageH; i++)
	{
		for (int j = 0; j < imageW; j++)
		{
			float cutVal = (outputImage.pxMap[j + i*imageW]);
			cutVal = cutVal * 255;
			unsigned char c2 = cutVal;
			unsigned char color1[3] = { c2, c2, c2 };
			outputCImg.draw_point(i, j, 0, color1, 1.0f);

			c2 = outputNormalisedImage.pxMap[j + i*imageW];
			unsigned char color2[3] = { c2, c2, c2 };
			outputCImg2.draw_point(i, j, 0, color2, 1.0f);

		}
	}
	
	//outputCImg.save("..\\..\\Output\\A_1620_1_RIGHT_CC_FC.bmp");
	//outputCImg2.save("..\\..\\Output\\A_1620_1_RIGHT_CC_FCNORM.bmp");

	strcpy(chFileName, "..\\..\\Output\\");
	strcat(chFileName, FILE_NAME);
	strcat(chFileName, "_FC.bmp");
	//outputCImg.save(chFileName);
	
	strcpy(chFileName, "..\\..\\Output\\");
	strcat(chFileName, FILE_NAME);
	strcat(chFileName, "_WFC.bmp");

	outputCImg2.save(chFileName);
	

		}
		}
		closedir(dir);
	}
	char endelino;
	cin >> endelino;
}
