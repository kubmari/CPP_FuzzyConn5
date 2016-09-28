
#ifndef KERNEL_H
#define KERNEL_H

//Struktura zawierajace teksture
struct S_IMG
{
	float *pxMap;
};

extern "C" S_IMG initKernel(S_IMG& _imageVectorHost1, S_IMG& _imageVectorHost2, S_IMG& _imageVectorHost3, S_IMG& _inputImage, S_IMG& _testImage, S_IMG& _J05Host, int _imageSize, size_t _dataSize, int _imageH, int _imageW);
#endif
