#include "utility.h"

/// <summary>
///	Simple and efficient grayscale algorithm.
/// </summary>
/// <param name="Src">The data structure of the source image to be processed.</param>
/// <param name="Dest">The data structure of the processed image。</param>
///	<remarks>Delta is not carried out in the LAB space. Only in the RGB space, the processing
/// <remarks>speed has little relation with the size of the image, and it has a linear relationship
/// <remarks>with the Level.</remarks>
IP_RET __stdcall ClassicDecolorization(TMatrix *Src, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Depth != Dest->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U || Dest->Depth != IP_DEPTH_8U) return IP_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 3 || (Dest->Channel != 3 && Dest->Channel != 1)) return IP_RET_ERR_ARGUMENTOUTOFRANGE;

	int X, Y, Width = Src->Width, Height = Src->Height;
	unsigned char *LinePS, *LinePD;

	for (Y = 0; Y < Height; Y++)
	{
		LinePS = Src->Data + Y * Src->WidthStep;
		LinePD = Dest->Data + Y * Dest->WidthStep;
		for (X = 0; X < Width; X++)
		{
			LinePD[X] = (LinePS[0] + LinePS[1] + LinePS[1] + LinePS[2]) / 4;
			LinePS += 3;
		}
	}
	return IP_RET_OK;
}

/// <summary>
/// For the matrix data transfer algorithm written in this function, only single channel byte and int type matrix are supported.
/// </summary>
/// <param name="Src">The data structure of the source image to be processed.</param>
/// <param name="Dest">The data structure of the processed image。</param>
/// <remarks> 1: Src and Dest not be the same.</remarks>
IP_RET TransposeMatrix(TMatrix *Src, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Height || Src->Height != Dest->Width || Src->Channel != Dest->Channel || Src->Depth != Dest->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U && Src->Depth != IP_DEPTH_32S) return IP_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 1) return IP_RET_ERR_NOTSUPPORTED;

	unsigned char *LinePS, *LinePD;
	int X, Y, Width, Height, StrideS, StrideD, *LinePSI, *LinePDI;

	Width = Dest->Width, Height = Dest->Height, StrideS = Src->WidthStep;

	if (Src->Depth == IP_DEPTH_8U)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y;
			LinePD = Dest->Data + Y * Dest->WidthStep;
			for (X = 0; X < Width; X++)
			{
				LinePD[X] = LinePS[0];
				LinePS += StrideS;
			}
		}
	}
	else
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePSI = (int *)Src->Data + Y;
			LinePDI = (int *)(Dest->Data + Y * Dest->WidthStep);
			for (X = 0; X < Width; X++)
			{
				LinePDI[X] = LinePSI[0];
				LinePSI += Height;
			}
		}
	}
	return IP_RET_OK;
}

/// <summary>
/// Calculate the local minimum error path along the vertical direction, only supports single-channel data matrix.
/// </summary>
/// <param name="Src">The data that needs to be extracted from the path must be a int type matrix.</param>
/// <param name="Dest">The obtained path, 0 is the reserved part, and 255 means the part that needs to be replaced. It must be byte image data.</param>
/// <remarks> 1:The original algorithm path is optimized.</remarks>
IP_RET MinCutVertical(TMatrix *Src, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_32S || Dest->Depth != IP_DEPTH_8U) return IP_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 1) return IP_RET_ERR_NOTSUPPORTED;
	IP_RET Ret = IP_RET_OK;

	int X, Y, Index, MinValue, Width, Height, *LinePL, *LinePD, *LinePS;
	unsigned char *LinePM;
	TMatrix *Energy = NULL;

	Width = Src->Width, Height = Src->Height;
	Ret = IP_CreateMatrix(Src->Width, Src->Height, IP_DEPTH_32S, 1, &Energy);				//	Assign data
	if (Ret != IP_RET_OK) return Ret;

	memcpy(Energy->Data, Src->Data, Src->WidthStep);										//	The first line of energy is the first line of the original data
	LinePL = (int *)Energy->Data;															//	Energy on the line
	LinePD = (int *)(Energy->Data + Energy->WidthStep);										//	The cumulative energy of the current line (to be calculated)
	for (Y = 1; Y < Height; Y++)															//	for i=2:size(E,1), From the second line
	{
		LinePS = (int *)(Src->Data + Y * Src->WidthStep);									//	The single energy value of the current line
		LinePD[0] = LinePS[0] + min(LinePL[0], LinePL[1]);									//	E(i,1) = X(i,1) + min( E(i-1,1), E(i-1,2) );
		for (X = 1; X < Width - 1; X++)														//	for j=2:size(E,2)-1
		{
			LinePD[X] = LinePS[X] + min(min(LinePL[X - 1], LinePL[X]), LinePL[X + 1]);		//	E(i,j) = X(i,j) + min( [E(i-1,j-1), E(i-1,j), E(i-1,j+1)] );
		}
		LinePD[Width - 1] = LinePS[Width - 1] + min(LinePL[Width - 2], LinePL[Width - 1]);	//	E(i,end) = X(i,end) + min( E(i-1,end-1), E(i-1,end) );
		LinePL += Energy->Width;
		LinePD += Energy->Width;
	}

	MinValue = MaxValueI;
	LinePD = (int *)(Energy->Data + (Height - 1) * Energy->WidthStep);
	for (X = 0; X < Width; X++)
	{
		if (LinePD[X] < MinValue)															//	Find the minimum accumulated energy value in the last line
		{
			Index = X;																		//	[cost, idx] = min(E(end, 1:end));
			MinValue = LinePD[X];
		}
	}
	LinePM = Dest->Data + (Height - 1) * Dest->WidthStep;
	for (X = 0; X < Index; X++) LinePM[X] = 0;												//	C(i, 1:idx-1) = -1;
	for (X = Index; X < Width; X++) LinePM[X] = 255;										//	C(i, idx+1:end) = +1;  	

	for (Y = Height - 2; Y >= 0; Y--)														//	for i=size(E,1)-1:-1:1,
	{
		LinePD = (int *)(Energy->Data + Y * Energy->WidthStep);								//	Start from the last line to search for the local minimum of its 8 domain, which is actually only the three elements of the top row.
		if (Index > 0)
		{
			MinValue = min(LinePD[Index - 1], LinePD[Index]);								//	 if( idx > 1 && E(i,idx-1) == min(E(i,idx-1:min(idx+1,size(E,2))) ) )
			if (Index + 1 < Width) MinValue = min(MinValue, LinePD[Index + 1]);
			if (MinValue == LinePD[Index - 1])
				Index--;																	//	idx = idx-1;
			else if (MinValue == LinePD[Index + 1])
				Index++;
		}
		else
		{
			if (LinePD[Index] > LinePD[Index + 1]) Index++;
		}

		LinePM = Dest->Data + Y * Dest->WidthStep;
		for (X = 0; X < Index; X++) LinePM[X] = 0;											//	The assignment is 0 for the part that does not need to be changed, and 255 means that the color of the new block is needed instead.
		for (X = Index; X < Width; X++) LinePM[X] = 255; ;
	}

	IP_FreeMatrix(&Energy);
	return IP_RET_OK;
}

/// <summary>
/// Calculate the local minimum error path along the horizontal direction, only single-channel data matrix is supported.
/// </summary>
/// <param name="Src">The data from which the path needs to be extracted must be an int type matrix.</param>
/// <param name="Dest">The obtained path, 0 is the reserved part, and 255 means the part that needs to be replaced. It must be byte image data.</param>
/// <remarks> 1:The speed of processing directly using the horizontal direction algorithm is slow, because the calculation address part involves
/// <remarks>   *Width, so it is better to transpose and calculate the vertical direction.</remarks>
IP_RET MinCutHorizontal(TMatrix *Src, TMatrix *Dest)
{
	TMatrix *TransposeToVertical = NULL, *TransposeMaskToVertical = NULL;
	IP_RET Ret = IP_CreateMatrix(Src->Height, Src->Width, IP_DEPTH_32S, 1, &TransposeToVertical);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(Src->Height, Src->Width, IP_DEPTH_8U, 1, &TransposeMaskToVertical);	//	分配数据
	if (Ret != IP_RET_OK) goto Done;
	Ret = TransposeMatrix(Src, TransposeToVertical);
	if (Ret != IP_RET_OK) goto Done;
	Ret = MinCutVertical(TransposeToVertical, TransposeMaskToVertical);
	if (Ret != IP_RET_OK) goto Done;
	Ret = TransposeMatrix(TransposeMaskToVertical, Dest);
	if (Ret != IP_RET_OK) goto Done;
Done:
	IP_FreeMatrix(&TransposeToVertical);
	IP_FreeMatrix(&TransposeMaskToVertical);
	return Ret;
}

/// <summary>
/// The local square sum of the image is calculated, the speed has been optimized and 1 and 3 channel images are supported.
/// </summary>
/// <param name="Src">Source image for square sum。</param>
/// <param name="Dest">Square sum data, need to be saved using int type matrix, the size is Src->Width - SizeX + 1, Src->Height - SizeY + 1, and the data is allocated inside the program.</param>
/// <param name="SizeX">The size of the template used in the horizontal direction, if it is a radius mode, corresponds to 2 * Radius + 1.</param>
/// <param name="SizeY">The size of the template used in the vertical direction, if it is a radius mode, corresponds to 2 * Radius + 1. </param>
/// <remarks> 1:An optimization algorithm similar to BoxBlur is used.</remarks>
IP_RET GetLocalSquareSum(TMatrix *Src, TMatrix *Dest, int SizeX, int SizeY)
{
	if (Src == NULL || Dest == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U || Dest->Depth != IP_DEPTH_32S) return IP_RET_ERR_NOTSUPPORTED;
	if (SizeX < 0 || SizeY < 0) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	IP_RET Ret = IP_RET_OK;

	int X, Y, Z, Width, Height, Left, Right, Top, Bottom;
	int SumRed, SumGreen, SumBlue, Sum, LastIndex, NextIndex;
	int *ColSum, *ColSumB, *ColSumG, *ColSumR, *LinePD;
	unsigned char *SamplePS, *LastAddress, *NextAddress;
	TMatrix *Row = NULL, *Col = NULL;

	Width = Src->Width, Height = Src->Height;
	Left = SizeX / 2, Right = SizeX - Left, Top = SizeY / 2, Bottom = SizeY - Top;

	ColSum = (int*)IP_AllocMemory(Width * Src->Channel * sizeof(int), true);
	if (ColSum == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }

	Ret = GetValidCoordinate(Width, Height, Left, Right, Top, Bottom, EdgeMode::Tile, &Row, &Col);				//	Get the coordinate offset
	if (Ret != IP_RET_OK) goto Done;

	if (Src->Channel == 1)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = (int *)(Dest->Data + Y * Dest->WidthStep);
			if (Y == 0)
			{
				for (X = 0; X < Width; X++)
				{
					Sum = 0;
					for (Z = -Top; Z < Bottom; Z++)
					{
						SamplePS = Src->Data + ((int *)Col->Data)[Z + Top] * Src->WidthStep + X;
						Sum += SamplePS[0] * SamplePS[0];
					}
					ColSum[X] = Sum;
				}
			}
			else
			{
				LastAddress = Src->Data + ((int *)Col->Data)[Y - 1] * Src->WidthStep;
				NextAddress = Src->Data + ((int *)Col->Data)[Y + SizeY - 1] * Src->WidthStep;
				for (X = 0; X < Width; X++)
				{
					ColSum[X] -= LastAddress[0] * LastAddress[0] - NextAddress[0] * NextAddress[0];
					LastAddress++;
					NextAddress++;
				}
			}
			for (X = 0; X < Width; X++)
			{
				if (X == 0)
				{
					Sum = 0;
					for (Z = -Left; Z < Right; Z++)
					{
						Sum += ColSum[((int *)Row->Data)[Z + Left]];
					}
				}
				else
				{
					LastIndex = ((int *)Row->Data)[X - 1];
					NextIndex = ((int *)Row->Data)[X + SizeX - 1];
					Sum -= ColSum[LastIndex] - ColSum[NextIndex];
				}
				LinePD[0] = Sum;
				LinePD++;
			}
		}
	}
	else if (Src->Channel == 3)
	{
		ColSumB = ColSum; ColSumG = ColSumB + Width;  ColSumR = ColSumG + Width;
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = (int *)(Dest->Data + Y * Dest->WidthStep);
			if (Y == 0)
			{
				for (X = 0; X < Width; X++)
				{
					SumBlue = 0; SumGreen = 0; SumRed = 0;
					for (Z = -Top; Z < Bottom; Z++)
					{
						SamplePS = Src->Data + ((int *)Col->Data)[Z + Top] * Src->WidthStep + X * 3;
						SumBlue += SamplePS[0] * SamplePS[0];
						SumGreen += SamplePS[1] * SamplePS[1];
						SumRed += SamplePS[2] * SamplePS[2];
					}
					ColSumB[X] = SumBlue;
					ColSumG[X] = SumGreen;
					ColSumR[X] = SumRed;
				}
			}
			else
			{
				LastAddress = Src->Data + ((int *)Col->Data)[Y - 1] * Src->WidthStep;
				NextAddress = Src->Data + ((int *)Col->Data)[Y + SizeY - 1] * Src->WidthStep;
				for (X = 0; X < Width; X++)
				{
					ColSumB[X] -= LastAddress[0] * LastAddress[0] - NextAddress[0] * NextAddress[0];
					ColSumG[X] -= LastAddress[1] * LastAddress[1] - NextAddress[1] * NextAddress[1];
					ColSumR[X] -= LastAddress[2] * LastAddress[2] - NextAddress[2] * NextAddress[2];
					LastAddress += 3;
					NextAddress += 3;
				}
			}
			for (X = 0; X < Width; X++)
			{
				if (X == 0)
				{
					SumBlue = 0; SumGreen = 0; SumRed = 0;			
					for (Z = -Left; Z < Right; Z++)
					{
						SumBlue += ColSumB[((int *)Row->Data)[Z + Left]];
						SumGreen += ColSumG[((int *)Row->Data)[Z + Left]];
						SumRed += ColSumR[((int *)Row->Data)[Z + Left]];
					}
				}
				else
				{
					LastIndex = ((int *)Row->Data)[X - 1];
					NextIndex = ((int *)Row->Data)[X + SizeX - 1];
					SumBlue -= ColSumB[LastIndex] - ColSumB[NextIndex];
					SumGreen -= ColSumG[LastIndex] - ColSumG[NextIndex];
					SumRed -= ColSumR[LastIndex] - ColSumR[NextIndex];
				}
				LinePD[0] = SumBlue;
				LinePD[1] = SumGreen;
				LinePD[2] = SumRed;
				LinePD += 3;
			}
		}
	}
Done:
	IP_FreeMatrix(&Row);
	IP_FreeMatrix(&Col);
	IP_FreeMemory(ColSum);
	return Ret;
}

/// <summary>
/// Multiplication of SSE-based byte data.
/// </summary>
/// <param name="Kernel">Matrix that needs convolution</param>
/// <param name="Conv">Convolution matrix.</param>
/// <param name="Length">The length of all elements of the array.</param>
/// <remarks> 1: SSE optimization is used.
///	https://msdn.microsoft.com/en-us/library/t5h7783k(v=vs.90).aspx

int MultiplySSE(unsigned char* Kernel, unsigned char *Conv, int Length)
{
	int Y, Sum;
	__m128i vsum = _mm_set1_epi32(0);
	__m128i vk0 = _mm_set1_epi8(0);
	for (Y = 0; Y <= Length - 16; Y += 16)
	{
		__m128i v0 = _mm_loadu_si128((__m128i*)(Kernel + Y));				// Corresponding to the movdqu instruction, no 16 byte alignment is required
		__m128i v0l = _mm_unpacklo_epi8(v0, vk0);
		__m128i v0h = _mm_unpackhi_epi8(v0, vk0);							//	The purpose of these two lines is to load them into two 128-bit registers for the following 16-bit SSE function call of _mm_madd_epi16 (the role of vk0 is mainly to set the upper 8 bits to 0)				
		__m128i v1 = _mm_loadu_si128((__m128i*)(Conv + Y));
		__m128i v1l = _mm_unpacklo_epi8(v1, vk0);
		__m128i v1h = _mm_unpackhi_epi8(v1, vk0);
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0l, v1l));				//	With _mm_madd_epi16 it is possible to multiply eight 16-bit numbers at a time and then add the results of the two 16 to a 32 number, r0: = (a0 * b0) + (a1 * b1) https://msdn.microsoft.com/en-us/library/yht36sa6(v=vs.90).aspx
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0h, v1h));
	}
	for (; Y <= Length - 8; Y += 8)
	{
		__m128i v0 = _mm_loadl_epi64((__m128i*)(Kernel + Y));
		__m128i v0l = _mm_unpacklo_epi8(v0, vk0);
		__m128i v1 = _mm_loadl_epi64((__m128i*)(Conv + Y));
		__m128i v1l = _mm_unpacklo_epi8(v1, vk0);
		vsum = _mm_add_epi32(vsum, _mm_madd_epi16(v0l, v1l));
	}
	vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 8));
	vsum = _mm_add_epi32(vsum, _mm_srli_si128(vsum, 4));
	Sum = _mm_cvtsi128_si32(vsum);											// MOVD function, Moves the least significant 32 bits of a to a 32-bit integer：   r := a0

	for (; Y < Length; Y++)
	{
		Sum += Kernel[Y] * Conv[Y];
	}
	return Sum;
}

/// <summary>
/// SSE based image convolution algorithm
/// </summary>
/// <param name="Src">The data structure of the source image that needs to be processed</param>
/// <param name="Conv">Convolution matrix, which must be image data. </param>
/// <param name="Dest">The data structure of the saved convolution result must be an int matrix。</param>
/// <remarks> 1: SSE optimization is used.
/// <remarks> 2: The SSE multiplication of the byte array is much faster than the ordinary SSE speed.</remarks>
IP_RET FastConv2(TMatrix *Src, TMatrix *Conv, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL || Conv == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL || Conv->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel) return IP_RET_ERR_PARAMISMATCH;
	if (Conv->Width < 1 || Conv->Height < 1) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	if (Src->Depth != IP_DEPTH_8U || Dest->Depth != IP_DEPTH_32S) return IP_RET_ERR_NOTSUPPORTED;
	IP_RET Ret = IP_RET_OK;

	int IndexD, IndexE, IndexK, ExpHeight, ExpStride, *DestP;
	int X, Y, Width, Height, ConvW, ConvH, Length, Left, Top, Right, Bottom;
	unsigned char *PtExp, *CurKer;;
	TMatrix *Expand = NULL;

	Width = Src->Width, Height = Src->Height, ConvW = Conv->Width, ConvH = Conv->Height, Length = ConvW * ConvH;
	Left = ConvW / 2, Top = ConvH / 2, Right = ConvW - Left - 1, Bottom = ConvH - Top - 1, ExpHeight = Height + ConvH - 1;	//	Note that the core element does not need to be extended, such as the width of the core is 3, as long as a pixel is extended to the left and right.

	Ret = GetExpandImage(Src, &Expand, Left, Right, Top, Bottom, EdgeMode::Tile);								//	The expanded data can be speeded up and easy to program, but a lot of memory is occupied.
	if (Ret != IP_RET_OK) goto Done;

	unsigned char *Conv16 = (unsigned char *)IP_AllocMemory(ConvW * ConvH);					//	Save the convolution matrix to remove potentially useless data in the original Conv, and note that there is no 16 byte alignment here.
	if (Conv16 == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }
	unsigned char *Kernel = (unsigned char *)IP_AllocMemory(ConvW * ExpHeight);				//	Save the 16 byte aligned convolution kernel matrix to facilitate the use of SSE
	if (Kernel == NULL) { Ret = IP_RET_ERR_OUTOFMEMORY; goto Done; }

	PtExp = Expand->Data;  ExpStride = Expand->WidthStep;

	for (Y = 0; Y < ConvH; Y++) 	memcpy(Conv16 + Y * ConvW, Conv->Data + Y * Conv->WidthStep, ConvW);		//	Copy the data of the convolution matrix

	for (Y = 0; Y < ExpHeight; Y++)
	{
		IndexE = Y * ExpStride;
		CurKer = Kernel + Y * ConvW;						//	Computes convolution kernel data for all pixels in the first column to be sampled
		for (X = 0; X < ConvW; X++)
		{
			CurKer[X] = PtExp[IndexE++];
		}
	}
	for (X = 0; X < Width; X++)
	{
		if (X != 0)													//	If it's not the first column, you need to update the data of the convolution kernel
		{
			memcpy(Kernel, Kernel + 1, (ConvW * ExpHeight - 1));	//	Move data forward
			IndexK = ConvW - 1;
			IndexE = IndexK + X;
			for (Y = 0; Y < ExpHeight; Y++)
			{
				Kernel[IndexK] = PtExp[IndexE];						//	Just refresh the next element
				IndexK += ConvW;
				IndexE += ExpStride;
			}
		}

		CurKer = Kernel, IndexD = X, DestP = (int *)Dest->Data;
		for (Y = 0; Y < Height; Y++)								//	Update in the direction of the column
		{
			DestP[IndexD] = MultiplySSE(Conv16, CurKer, Length);
			CurKer += ConvW;
			IndexD += Width;

		}
	}
Done:
	IP_FreeMemory(Conv16);
	IP_FreeMemory(Kernel);
	IP_FreeMatrix(&Expand);
	return IP_RET_OK;
}

/// <summary>
/// Calculate the cumulative squared error of the image, speed is optimized.
/// </summary>
/// <param name="Src">The source image, our template image, looks for cumulative squared differences in the image.</param>
/// <param name="Template">The template image, its width and height should be smaller than the source image.。</param>
/// <param name="Dest">Save the result image using an int type matrix whose size must be Src-> Width- Template-> Width, Src-> Height- Template-> Height.</param>
/// <remarks> 1:The cumulative mean square error is calculated as the cumulative sum of the squares of the differences in pixels at the positions 
/// <remarks>   corresponding to the two images: (a-b) ^ 2 = a ^ 2 + b ^ 2 - 2ab.
/// <remarks> 2:A (template) of the square is a fixed value, B (a small map source showed) square can be used to achieve fast integral graph, a*b can achieve fast convolution.</remarks>

IP_RET GetSSD(TMatrix *Src, TMatrix *Template, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL || Template == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL || Template->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Width <= Template->Width || Src->Height <= Template->Height || Src->Channel != Template->Channel || Src->Depth != Template->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Dest->Width != Src->Width - Template->Width || Dest->Height != Src->Height - Template->Height || Dest->Channel != 1) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	if (Src->Depth != IP_DEPTH_8U || Dest->Depth != IP_DEPTH_32S) return IP_RET_ERR_NOTSUPPORTED;

	if (Src->Channel == 1)
	{
		IP_RET Ret = IP_RET_OK;
		TMatrix *LocalSquareSum = NULL, *XY = NULL;
		Ret = IP_CreateMatrix(Src->Width, Src->Height, IP_DEPTH_32S, 1, &LocalSquareSum);	//	Assign data
		if (Ret != IP_RET_OK) goto Done8;
		Ret = IP_CreateMatrix(Src->Width, Src->Height, IP_DEPTH_32S, 1, &XY);	//	Assign data
		if (Ret != IP_RET_OK) goto Done8;
		Ret = GetLocalSquareSum(Src, LocalSquareSum, Template->Width, Template->Height);
		if (Ret != IP_RET_OK) goto Done8;
		Ret = FastConv2(Src, Template, XY);
		if (Ret != IP_RET_OK) goto Done8;

		int X, Y, Sum, *LinePP, *LinePXY, *LinePD;
		int Left = Template->Width / 2, Right = Template->Width - Left, Top = Template->Height / 2, Bottom = Template->Height - Top;
		int Width = Dest->Width, Height = Dest->Height;

		for (Y = 0, Sum = 0; Y < Template->Height * Template->WidthStep; Y++)
			Sum += Template->Data[Y] * Template->Data[Y];

		for (Y = 0; Y < Height; Y++)
		{
			LinePP = (int *)(LocalSquareSum->Data + (Y + Top)* LocalSquareSum->WidthStep) + Left;
			LinePXY = (int *)(XY->Data + (Y + Top)* XY->WidthStep) + Left;
			LinePD = (int *)(Dest->Data + Y * Dest->WidthStep);
			for (X = 0; X < Width; X++)
			{
				LinePD[X] = Sum + LinePP[X] - 2 * LinePXY[X];					//	a^2 + b^2 - 2ab
			}
		}
	Done8:
		IP_FreeMatrix(&LocalSquareSum);
		IP_FreeMatrix(&XY);

		return Ret;
	}
	else
	{
		TMatrix *GraySrc = NULL, *GrayTemplate = NULL;
		IP_RET Ret = IP_CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, &GraySrc);
		if (Ret != IP_RET_OK) goto Done24;
		Ret = IP_CreateMatrix(Template->Width, Template->Height, Template->Depth, 1, &GrayTemplate);
		if (Ret != IP_RET_OK) goto Done24;
		Ret = ClassicDecolorization(Src, GraySrc);
		if (Ret != IP_RET_OK) goto Done24;
		Ret = ClassicDecolorization(Template, GrayTemplate);
		if (Ret != IP_RET_OK) goto Done24;
		Ret = GetSSD(GraySrc, GrayTemplate, Dest);
		if (Ret != IP_RET_OK) goto Done24;
	Done24:
		IP_FreeMatrix(&GraySrc);
		IP_FreeMatrix(&GrayTemplate);
		return Ret;

	}
}

/// <summary>
/// Texture image synthesis algorithm.
/// </summary>
/// <param name="Src">The source image, our template image, looks for cumulative squared differences in the image.</param>
/// <param name="Dest">Composite image.</param>
/// <param name="TileSizeX">The width of the block.</param>
/// <param name="TileSizeY">The height of the block.</param>
/// <param name="OverlapX">The width of the overlap.</param>
/// <param name="OverlapY">The height of the overlap.</param>
/// <remarks> 1:The speed is still very slow, the key is to find another measure of similarity index.</remarks>

IP_RET __stdcall ImageQuilting(TMatrix *Src, TMatrix *Dest, int TileSizeX, int TileSizeY, int OverlapX, int OverlapY)
{
	if (Src == NULL || Dest == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IP_RET_ERR_NULLREFERENCE;
	if (Src->Channel != Dest->Channel || Src->Depth != Dest->Depth) return IP_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IP_DEPTH_8U || Dest->Depth != IP_DEPTH_8U) return IP_RET_ERR_NOTSUPPORTED;
	if (TileSizeX < 0 || TileSizeY < 0 || OverlapX < 0 || OverlapY < 0) return IP_RET_ERR_ARGUMENTOUTOFRANGE;
	if (TileSizeX >= Src->Width || TileSizeY >= Src->Height || OverlapX >= TileSizeX || OverlapY >= TileSizeY) return IP_RET_ERR_ARGUMENTOUTOFRANGE;

	int TileX = (int)ceil((double)(Dest->Width - OverlapX) / (TileSizeX - OverlapX));
	int TileY = (int)ceil((double)(Dest->Height - OverlapY) / (TileSizeY - OverlapY));			//	The size of each piece
	int NewWidth = TileX * TileSizeX - (TileX - 1) * OverlapX;
	int NewHeight = TileY * TileSizeY - (TileY - 1) * OverlapY;			//	To alleviate the trouble of encoding, one way is to extend the width and height of the image to the size of TileX and TileY

	if ((NewWidth != Dest->Width) || (NewHeight != Dest->Height))		//	If the image can't be divided, a pair of images that can be divided is set up so that it makes it more convenient
	{
		TMatrix *Expand = NULL;
		IP_RET Ret = IP_CreateMatrix(NewWidth, NewHeight, Dest->Depth, Dest->Channel, &Expand);		//	Create a blank image
		if (Ret != IP_RET_OK) goto Free;
		Ret = ImageQuilting(Src, Expand, TileSizeX, TileSizeY, OverlapX, OverlapY);
		if (Ret != IP_RET_OK) goto Free;
		for (int Y = 0; Y < Dest->Height; Y++)			//	Copy from the far left
			memcpy(Dest->Data + Y * Dest->WidthStep, Expand->Data + Y * Expand->WidthStep, Dest->WidthStep);
	Free:
		IP_FreeMatrix(&Expand);
		return Ret;
	}

	int X, Y, XX, YY, Width, Height, DistW, DistH;
	int StartX, StartY, Sum, Best, Left, Top, Count, Index, Alpha;
	unsigned char *LinePS, *LinePD, *LinePP, *LinePM;
	int *LinePH, *LinePV, *LinePHV, *LinePT;

	Width = Src->Width, Height = Src->Height;
	DistW = Width - TileSizeX, DistH = Height - TileSizeY;


	TMatrix *Mask = NULL, *MaskX = NULL, *MaskY = NULL;			//	The minimum energy path segmentation corresponding mask map
	TMatrix *TileH = NULL, *TileV = NULL, *TileHV = NULL, *SmallTileH = NULL;
	TMatrix *Distance = NULL, *ImgOverlapX = NULL, *ImgOverlapY = NULL, *DistanceH = NULL, *DistanceV = NULL, *DistanceHV = NULL;

	IP_RET Ret = IP_CreateMatrix(DistW, DistH, IP_DEPTH_32S, 1, &Distance);								//	Assign data
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(TileSizeX, TileSizeY, IP_DEPTH_8U, 1, &Mask);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(OverlapX, TileSizeY, IP_DEPTH_32S, 1, &ImgOverlapX);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(TileSizeX, OverlapY, IP_DEPTH_32S, 1, &ImgOverlapY);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(OverlapX, TileSizeY, IP_DEPTH_8U, 1, &MaskX);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(TileSizeX, OverlapY, IP_DEPTH_8U, 1, &MaskY);
	if (Ret != IP_RET_OK) goto Done;

	Ret = IP_CreateMatrix(TileSizeX, OverlapY, IP_DEPTH_8U, Src->Channel, &TileH);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(OverlapX, TileSizeY, IP_DEPTH_8U, Src->Channel, &TileV);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(OverlapX, OverlapY, IP_DEPTH_8U, Src->Channel, &TileHV);
	if (Ret != IP_RET_OK) goto Done;

	Ret = IP_CreateMatrix(Width - TileSizeX, Height - OverlapY, IP_DEPTH_32S, 1, &DistanceH);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(Width - OverlapX, Height - TileSizeY, IP_DEPTH_32S, 1, &DistanceV);
	if (Ret != IP_RET_OK) goto Done;
	Ret = IP_CreateMatrix(Width - OverlapX, Height - OverlapY, IP_DEPTH_32S, 1, &DistanceHV);
	if (Ret != IP_RET_OK) goto Done;

	memset(Dest->Data, 0, Dest->Height * Dest->WidthStep);

	for (YY = 0; YY < TileY; YY++)
	{
		StartY = YY * TileSizeY - YY * OverlapY;				//	The starting position of the new block
		for (XX = 0; XX < TileX; XX++)
		{
			StartX = XX * TileSizeX - XX * OverlapX;
			if (XX == 0 && YY == 0)								//	For the first block, copy the data of a block randomly from the source image
			{
				Left = Rand() * (Width - TileSizeX);			//	The location of the starting point according to the principle of the left, do not pay attention to cross-border
				Top = Rand() * (Height - TileSizeY);
				for (Y = 0; Y < TileSizeY; Y++)
					memcpy(Dest->Data + Y * Dest->WidthStep, Src->Data + (Y + Top) * Src->WidthStep + Left * Src->Channel, TileSizeX * Src->Channel);
			}
			else
			{

				///	*****************************The following part of the code is used to calculate the most similar block***************************************
				///	1. Note that the blocks here are not, in many cases, square, usually L-shaped
				///	2. If it is implemented with force, attention must be made to determine whether the target has already had data, and if there is no data, 
				///     it can not be included in the ranks of the calculated distance.

				memset(Distance->Data, 0, Distance->Height * Distance->WidthStep);			//	Clear the distance data
				if (YY > 0)										//	There is overlap in the vertical direction
				{
					for (Y = 0; Y < OverlapY; Y++)				//	Copy the data in the vertical direction to a separate TileH data structure, mainly for convenience, size TileSizeX * OverlapY
						memcpy(TileH->Data + Y * TileH->WidthStep, Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel, TileH->Width * TileH->Channel);

					Ret = GetSSD(Src, TileH, DistanceH);		//	Calculate the distance from this part to each block of the original image
					if (Ret != IP_RET_OK) goto Done;
					for (Y = 0; Y < Distance->Height; Y++)		//	Copy the calculated result directly into the distance matrix
						memcpy(Distance->Data + Y * Distance->WidthStep, DistanceH->Data + Y * DistanceH->WidthStep, Distance->WidthStep);
				}
				if (XX > 0)										//	There is a overlap in the horizontal direction
				{
					for (Y = 0; Y < TileSizeY; Y++)				//	Copying horizontally-aligned data into a single TileV data structure is primarily for convenience, size OverlapX * TileSizeY
						memcpy(TileV->Data + Y * TileV->WidthStep, Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel, TileV->Width * TileV->Channel);



					Ret = GetSSD(Src, TileV, DistanceV);
					if (Ret != IP_RET_OK) goto Done;
					if (YY > 0)									//	There must be overlapping parts at this time
					{
						for (Y = 0; Y < OverlapY; Y++)			//	Copy overlapping area data
							memcpy(TileHV->Data + Y * TileHV->WidthStep, Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel, TileHV->Width * TileHV->Channel);

						Ret = GetSSD(Src, TileHV, DistanceHV);
						if (Ret != IP_RET_OK) goto Done;
						for (Y = 0; Y < DistH; Y++)
						{
							LinePT = (int *)(Distance->Data + Y * Distance->WidthStep);
							LinePV = (int *)(DistanceV->Data + Y * DistanceV->WidthStep);
							LinePHV = (int *)(DistanceHV->Data + Y * DistanceHV->WidthStep);
							for (X = 0; X < DistW; X++)	LinePT[X] += LinePV[X] - LinePHV[X];					//	 Need to subtract the overlap
						}
					}
					else
					{
						for (Y = 0; Y < Distance->Height; Y++)													//	Direct copy
							memcpy(Distance->Data + Y * Distance->WidthStep, DistanceV->Data + Y * DistanceV->WidthStep, Distance->WidthStep);
					}
				}
				//	Find the coordinates of the starting point (upper left corner) of the most similar block
				for (Y = 0, Best = MaxValueI; Y < DistH; Y++)
				{
					LinePT = (int *)(Distance->Data + Y * Distance->WidthStep);
					for (X = 0; X < DistW; X++)
					{
						if (LinePT[X] < Best)
						{
							//	Some algorithms suggest adding a tolerance. If the tolerance is less than the best match
							//  the block is randomly selected as a selected patch, which is not a bad idea, but I still 
							//  directly select the best block.
							Best = LinePT[X];		
							Left = X;
							Top = Y;
						}
					}
				}

				//Index = 0;
				/*for(Y = 0; Y < TileSizeY; Y++)
				{
				memcpy(Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel, Src->Data + (Y + Top) * Src->WidthStep + Left * Src->Channel, TileSizeX * Src->Channel) ;
				}*/

				memset(Mask->Data, 255, Mask->Height * Mask->WidthStep);


				if (YY > 0)
				{
					for (Y = 0; Y < OverlapY; Y++)
					{
						LinePT = (int *)(ImgOverlapY->Data + Y * ImgOverlapY->WidthStep);
						LinePS = Src->Data + (Y + Top) * Src->WidthStep + Left * Src->Channel;
						LinePD = Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel;
						for (X = 0; X < TileSizeX; X++)
						{
							LinePT[X] = (LinePD[0] - LinePS[0]) * (LinePD[0] - LinePS[0]) + (LinePD[1] - LinePS[1]) * (LinePD[1] - LinePS[1]) + (LinePD[2] - LinePS[2]) * (LinePD[2] - LinePS[2]);
							LinePD += Dest->Channel;
							LinePS += Src->Channel;
						}
					}
					Ret = MinCutHorizontal(ImgOverlapY, MaskY);
					if (Ret != IP_RET_OK) goto Done;
					for (Y = 0; Y < OverlapY; Y++)
						memcpy(Mask->Data + Y * Mask->WidthStep, MaskY->Data + Y * MaskY->WidthStep, TileSizeX);
				}
				if (XX > 0)
				{
					for (Y = 0; Y < TileSizeY; Y++)
					{
						LinePT = (int *)(ImgOverlapX->Data + Y * ImgOverlapX->WidthStep);
						LinePS = Src->Data + (Y + Top) * Src->WidthStep + Left * Src->Channel;
						LinePD = Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel;
						for (X = 0; X < OverlapX; X++)
						{
							LinePT[X] = (LinePD[0] - LinePS[0]) * (LinePD[0] - LinePS[0]) + (LinePD[1] - LinePS[1]) * (LinePD[1] - LinePS[1]) + (LinePD[2] - LinePS[2]) * (LinePD[2] - LinePS[2]);
							LinePD += Dest->Channel;
							LinePS += Src->Channel;
						}
					}
					Ret = MinCutVertical(ImgOverlapX, MaskX);
					if (Ret != IP_RET_OK) goto Done;
					for (Y = 0; Y < TileSizeY; Y++)
					{
						LinePS = MaskX->Data + Y * MaskX->WidthStep;
						LinePM = Mask->Data + Y * Mask->WidthStep;
						for (X = 0; X < OverlapX; X++)
						{
							LinePM[X] &= LinePS[X];
						}
					}
				}

				for (Y = 0; Y < TileSizeY; Y++)
				{
					LinePS = Src->Data + (Y + Top) * Src->WidthStep + Left * Src->Channel;
					LinePD = Dest->Data + (Y + StartY) * Dest->WidthStep + StartX * Dest->Channel;
					LinePM = Mask->Data + Y * Mask->WidthStep;
					for (X = 0; X < TileSizeX; X++)
					{
						Alpha = LinePM[X];
						if (Alpha == 255)					//	Most of the objects are 255
						{
							LinePD[0] = LinePS[0];
							LinePD[1] = LinePS[1];
							LinePD[2] = LinePS[2];
						}
						LinePD += Dest->Channel;
						LinePS += Src->Channel;
					}
				}
			}
		}

	}
Done:
	IP_FreeMatrix(&Distance);
	IP_FreeMatrix(&Mask);
	IP_FreeMatrix(&ImgOverlapX);
	IP_FreeMatrix(&ImgOverlapY);
	IP_FreeMatrix(&MaskX);
	IP_FreeMatrix(&MaskY);
	IP_FreeMatrix(&TileH);
	IP_FreeMatrix(&TileV);
	IP_FreeMatrix(&TileHV);
	IP_FreeMatrix(&DistanceH);
	IP_FreeMatrix(&DistanceV);
	IP_FreeMatrix(&DistanceHV);
	return Ret;
}
