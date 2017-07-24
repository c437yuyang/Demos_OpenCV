// SLIC.cpp: implementation of the SLIC class.
//===========================================================================
// This code implements the zero parameter superpixel segmentation technique
// described in:
//
//
//
// "SLIC Superpixels Compared to State-of-the-art Superpixel Methods"
//
// Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua,
// and Sabine Susstrunk,
//
// IEEE TPAMI, Volume 34, Issue 11, Pages 2274-2282, November 2012.
//
//
//===========================================================================
// Copyright (c) 2013 Radhakrishna Achanta.
//
// For commercial use please contact the author:
//
// Email: firstname.lastname@epfl.ch
//===========================================================================
#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */
#include "SLIC.h"

// For superpixels
const int dx4[4] = { -1,  0,  1,  0 };
const int dy4[4] = { 0, -1,  0,  1 };

//const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
//const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

// For supervoxels
const int dx10[10] = { -1,  0,  1,  0, -1,  1,  1, -1,  0, 0 };
const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0 };
const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLIC::SLIC()
{
	m_lvec = NULL;
	m_avec = NULL;
	m_bvec = NULL;

	m_lvecvec = NULL;
	m_avecvec = NULL;
	m_bvecvec = NULL;
}

SLIC::~SLIC()
{
	if (m_lvec) delete[] m_lvec;
	if (m_avec) delete[] m_avec;
	if (m_bvec) delete[] m_bvec;


	if (m_lvecvec)
	{
		for (int d = 0; d < m_depth; d++) delete[] m_lvecvec[d];
		delete[] m_lvecvec;
	}
	if (m_avecvec)
	{
		for (int d = 0; d < m_depth; d++) delete[] m_avecvec[d];
		delete[] m_avecvec;
	}
	if (m_bvecvec)
	{
		for (int d = 0; d < m_depth; d++) delete[] m_bvecvec[d];
		delete[] m_bvecvec;
	}
}

//==============================================================================
///	RGB2XYZ
///
/// sRGB (D65 illuninant assumption) to XYZ conversion
//==============================================================================
void SLIC::RGB2XYZ(
	const int&		sR,
	const int&		sG,
	const int&		sB,
	double&			X,
	double&			Y,
	double&			Z)
{
	double R = sR / 255.0;
	double G = sG / 255.0;
	double B = sB / 255.0;

	double r, g, b;

	if (R <= 0.04045)	r = R / 12.92;
	else				r = pow((R + 0.055) / 1.055, 2.4);
	if (G <= 0.04045)	g = G / 12.92;
	else				g = pow((G + 0.055) / 1.055, 2.4);
	if (B <= 0.04045)	b = B / 12.92;
	else				b = pow((B + 0.055) / 1.055, 2.4);

	X = r*0.4124564 + g*0.3575761 + b*0.1804375;
	Y = r*0.2126729 + g*0.7151522 + b*0.0721750;
	Z = r*0.0193339 + g*0.1191920 + b*0.9503041;
}

//===========================================================================
///	RGB2LAB
//===========================================================================
void SLIC::RGB2LAB(const int& sR, const int& sG, const int& sB, double& lval, double& aval, double& bval)
{
	//------------------------
	// sRGB to XYZ conversion
	//------------------------
	double X, Y, Z;
	RGB2XYZ(sR, sG, sB, X, Y, Z);

	//------------------------
	// XYZ to LAB conversion
	//------------------------
	double epsilon = 0.008856;	//actual CIE standard
	double kappa = 903.3;		//actual CIE standard

	double Xr = 0.950456;	//reference white
	double Yr = 1.0;		//reference white
	double Zr = 1.088754;	//reference white

	double xr = X / Xr;
	double yr = Y / Yr;
	double zr = Z / Zr;

	double fx, fy, fz;
	if (xr > epsilon)	fx = pow(xr, 1.0 / 3.0);
	else				fx = (kappa*xr + 16.0) / 116.0;
	if (yr > epsilon)	fy = pow(yr, 1.0 / 3.0);
	else				fy = (kappa*yr + 16.0) / 116.0;
	if (zr > epsilon)	fz = pow(zr, 1.0 / 3.0);
	else				fz = (kappa*zr + 16.0) / 116.0;

	lval = 116.0*fy - 16.0;
	aval = 500.0*(fx - fy);
	bval = 200.0*(fy - fz);
}

//===========================================================================
///	DoRGBtoLABConversion
///
///	For whole image: overlaoded floating point version
//===========================================================================
void SLIC::DoRGBtoLABConversion(
	const unsigned int*&		ubuff,
	double*&					lvec,
	double*&					avec,
	double*&					bvec)
{
	int sz = m_width*m_height;
	lvec = new double[sz];
	avec = new double[sz];
	bvec = new double[sz];

#pragma region 我修改成了读入的是三通道的uint图片
	for (int i = 0; i != m_height; ++i)
	{
		for (int j = 0; j != m_width; ++j)
		{
			RGB2LAB(ubuff[i*m_width * 3 + j * 3 + 2], 
					ubuff[i*m_width * 3 + j * 3 + 1], 
					ubuff[i*m_width * 3 + j * 3 + 0], 
					lvec[i*m_width+j], avec[i*m_width + j], bvec[i*m_width + j]);
		}
	}
#pragma endregion


#pragma region 原始版本是从一个通道的UINT图像位移得到rgb
	//for (int j = 0; j < sz; j++)
//{
//	int r = (ubuff[j] >> 16) & 0xFF;
//	int g = (ubuff[j] >> 8) & 0xFF;
//	int b = (ubuff[j]) & 0xFF;

//	RGB2LAB(r, g, b, lvec[j], avec[j], bvec[j]);
//}  
#pragma endregion

}

//===========================================================================
///	DoRGBtoLABConversion
///
/// For whole volume
//===========================================================================
void SLIC::DoRGBtoLABConversion(
	const unsigned int**&		ubuff,
	double**&					lvec,
	double**&					avec,
	double**&					bvec)
{
	int sz = m_width*m_height;
	for (int d = 0; d < m_depth; d++)
	{
		for (int j = 0; j < sz; j++)
		{
			int r = (ubuff[d][j] >> 16) & 0xFF;
			int g = (ubuff[d][j] >> 8) & 0xFF;
			int b = (ubuff[d][j]) & 0xFF;

			RGB2LAB(r, g, b, lvec[d][j], avec[d][j], bvec[d][j]);
		}
	}
}

//=================================================================================
/// DrawContoursAroundSegments
///
/// Internal contour drawing option exists. One only needs to comment the if
/// statement inside the loop that looks at neighbourhood.
//=================================================================================
void SLIC::DrawContoursAroundSegments(
	unsigned int*			ubuff,
	const int*				labels,
	const int&				width,
	const int&				height,
	const unsigned int&		color)
{
	const int dx8[8] = { -1, -1,  0,  1, 1, 1, 0, -1 };
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1 };

	int sz = width*height;

	vector<bool> istaken(sz, false);

	int mainindex(0);
	for (int j = 0; j < height; j++)
	{
		for (int k = 0; k < width; k++)
		{
			int np(0);
			for (int i = 0; i < 8; i++)
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if ((x >= 0 && x < width) && (y >= 0 && y < height))
				{
					int index = y*width + x;

					if (false == istaken[index])//comment this to obtain internal contours
					{
						if (labels[mainindex] != labels[index]) np++;
					}
				}
			}
			if (np > 1)//change to 2 or 3 for thinner lines
			{
				ubuff[mainindex] = color;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}
}

//=================================================================================
/// DrawContoursAroundSegmentsTwoColors
///
/// Internal contour drawing option exists. One only needs to comment the if
/// statement inside the loop that looks at neighbourhood.
//=================================================================================
void SLIC::DrawContoursAroundSegmentsTwoColors(
	unsigned int*			img,
	const int*				labels,
	const int&				width,
	const int&				height)
{
	const int dx[8] = { -1, -1,  0,  1, 1, 1, 0, -1 };
	const int dy[8] = { 0, -1, -1, -1, 0, 1, 1,  1 };

	int sz = width*height;

	vector<bool> istaken(sz, false);

	vector<int> contourx(sz);
	vector<int> contoury(sz);
	int mainindex(0);
	int cind(0);
	for (int j = 0; j < height; j++)
	{
		for (int k = 0; k < width; k++)
		{
			int np(0);
			for (int i = 0; i < 8; i++)
			{
				int x = k + dx[i];
				int y = j + dy[i];

				if ((x >= 0 && x < width) && (y >= 0 && y < height))
				{
					int index = y*width + x;

					//if( false == istaken[index] )//comment this to obtain internal contours
					{
						if (labels[mainindex] != labels[index]) np++;
					}
				}
			}
			if (np > 1)
			{
				contourx[cind] = k;
				contoury[cind] = j;
				istaken[mainindex] = true;
				//img[mainindex] = color;
				cind++;
			}
			mainindex++;
		}
	}

	int numboundpix = cind;//int(contourx.size());

	for (int j = 0; j < numboundpix; j++)
	{
		int ii = contoury[j] * width + contourx[j];
		img[ii] = 0xffffff;
		//----------------------------------
		// Uncomment this for thicker lines
		//----------------------------------
		for (int n = 0; n < 8; n++)
		{
			int x = contourx[j] + dx[n];
			int y = contoury[j] + dy[n];
			if ((x >= 0 && x < width) && (y >= 0 && y < height))
			{
				int ind = y*width + x;
				if (!istaken[ind]) img[ind] = 0;
			}
		}
	}
}


//==============================================================================
///	DetectLabEdges
//==============================================================================
void SLIC::DetectLabEdges(
	const double*				lvec,
	const double*				avec,
	const double*				bvec,
	const int&					width,
	const int&					height,
	vector<double>&				edges)
{
	int sz = width*height;

	edges.resize(sz, 0);
	for (int j = 1; j < height - 1; j++)
	{
		for (int k = 1; k < width - 1; k++)
		{
			int i = j*width + k;

			double dx = (lvec[i - 1] - lvec[i + 1])*(lvec[i - 1] - lvec[i + 1]) +
				(avec[i - 1] - avec[i + 1])*(avec[i - 1] - avec[i + 1]) +
				(bvec[i - 1] - bvec[i + 1])*(bvec[i - 1] - bvec[i + 1]);

			double dy = (lvec[i - width] - lvec[i + width])*(lvec[i - width] - lvec[i + width]) +
				(avec[i - width] - avec[i + width])*(avec[i - width] - avec[i + width]) +
				(bvec[i - width] - bvec[i + width])*(bvec[i - width] - bvec[i + width]);

			//edges[i] = (sqrt(dx) + sqrt(dy));
			edges[i] = (dx + dy);
		}
	}
}

//===========================================================================
///	PerturbSeeds
//===========================================================================
void SLIC::PerturbSeeds(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	const vector<double>&		edges)
{
	const int dx8[8] = { -1, -1,  0,  1, 1, 1, 0, -1 };
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1 };

	int numseeds = kseedsl.size();

	for (int n = 0; n < numseeds; n++)
	{
		int ox = kseedsx[n];//original x
		int oy = kseedsy[n];//original y
		int oind = oy*m_width + ox;

		int storeind = oind;
		for (int i = 0; i < 8; i++)
		{
			int nx = ox + dx8[i];//new x
			int ny = oy + dy8[i];//new y

			if (nx >= 0 && nx < m_width && ny >= 0 && ny < m_height)
			{
				int nind = ny*m_width + nx;
				if (edges[nind] < edges[storeind])
				{
					storeind = nind;
				}
			}
		}
		if (storeind != oind)
		{
			kseedsx[n] = storeind%m_width;
			kseedsy[n] = storeind / m_width;
			kseedsl[n] = m_lvec[storeind];
			kseedsa[n] = m_avec[storeind];
			kseedsb[n] = m_bvec[storeind];
		}
	}
}


//===========================================================================
///	GetLABXYSeeds_ForGivenStepSize
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetLABXYSeeds_ForGivenStepSize(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	const int&					STEP,
	const bool&					perturbseeds,
	const vector<double>&		edgemag)
{
	int numseeds(0);
	int n(0);

	//int xstrips = m_width/STEP;
	//int ystrips = m_height/STEP;
	int xstrips = (0.5 + double(m_width) / double(STEP));
	int ystrips = (0.5 + double(m_height) / double(STEP));

	int xerr = m_width - STEP*xstrips;
	int yerr = m_height - STEP*ystrips;

	double xerrperstrip = double(xerr) / double(xstrips);
	double yerrperstrip = double(yerr) / double(ystrips);

	int xoff = STEP / 2;
	int yoff = STEP / 2;
	//-------------------------
	numseeds = xstrips*ystrips;
	//-------------------------
	kseedsl.resize(numseeds);
	kseedsa.resize(numseeds);
	kseedsb.resize(numseeds);
	kseedsx.resize(numseeds);
	kseedsy.resize(numseeds);

	for (int y = 0; y < ystrips; y++)
	{
		int ye = y*yerrperstrip;
		for (int x = 0; x < xstrips; x++)
		{
			int xe = x*xerrperstrip;
			int i = (y*STEP + yoff + ye)*m_width + (x*STEP + xoff + xe);

			kseedsl[n] = m_lvec[i];
			kseedsa[n] = m_avec[i];
			kseedsb[n] = m_bvec[i];
			kseedsx[n] = (x*STEP + xoff + xe);
			kseedsy[n] = (y*STEP + yoff + ye);
			n++;
		}
	}


	if (perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
}

//===========================================================================
///	GetLABXYSeeds_ForGivenK
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetLABXYSeeds_ForGivenK(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	const int&					K,
	const bool&					perturbseeds,
	const vector<double>&		edgemag)
{
	int sz = m_width*m_height;
	double step = sqrt(double(sz) / double(K));
	int T = step;
	int xoff = step / 2;
	int yoff = step / 2;

	int n(0); int r(0);
	for (int y = 0; y < m_height; y++)
	{
		int Y = y*step + yoff;
		if (Y > m_height - 1) break;

		for (int x = 0; x < m_width; x++)
		{
			//int X = x*step + xoff;//square grid
			int X = x*step + (xoff << (r & 0x1));//hex grid
			if (X > m_width - 1) break;

			int i = Y*m_width + X;

			//_ASSERT(n < K);

			//kseedsl[n] = m_lvec[i];
			//kseedsa[n] = m_avec[i];
			//kseedsb[n] = m_bvec[i];
			//kseedsx[n] = X;
			//kseedsy[n] = Y;
			kseedsl.push_back(m_lvec[i]);
			kseedsa.push_back(m_avec[i]);
			kseedsb.push_back(m_bvec[i]);
			kseedsx.push_back(X);
			kseedsy.push_back(Y);
			n++;
		}
		r++;
	}

	if (perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
}


//===========================================================================
///	PerformSuperpixelSegmentation_VariableSandM
///
///	Magic SLIC - no parameters
///
///	Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
/// This function picks the maximum value of color distance as compact factor
/// M and maximum pixel distance as grid step size S from each cluster (13 April 2011).
/// So no need to input a constant value of M and S. There are two clear
/// advantages:
///
/// [1] The algorithm now better handles both textured and non-textured regions
/// [2] There is not need to set any parameters!!!
///
/// SLICO (or SLIC Zero) dynamically varies only the compactness factor S,
/// not the step size S.
//===========================================================================
void SLIC::PerformSuperpixelSegmentation_VariableSandM(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	int*						klabels,
	const int&					STEP,
	const int&					NUMITR)
{
	int sz = m_width*m_height;
	const int seedNum = kseedsl.size();
	//double cumerr(99999.9);
	int numitr(0);

	//STEP = sqrt(double(sz) / double(K)) + 2.0;
	//----------------
	int offset = STEP;
	if (STEP < 10) offset = STEP*1.5;
	//----------------

	vector<double> sigmal(seedNum, 0);
	vector<double> sigmaa(seedNum, 0);
	vector<double> sigmab(seedNum, 0);
	vector<double> sigmax(seedNum, 0);
	vector<double> sigmay(seedNum, 0);
	vector<int> clustersize(seedNum, 0);
	vector<double> inv(seedNum, 0);//to store 1/clustersize[k] values
	vector<double> distxy(sz, DBL_MAX);
	vector<double> distlab(sz, DBL_MAX);
	vector<double> distvec(sz, DBL_MAX);

	//最大的lab值做记录
	vector<double> maxlab(seedNum, 10 * 10);//THIS IS THE VARIABLE VALUE OF M, just start with 10
	vector<double> maxxy(seedNum, STEP*STEP);//THIS IS THE VARIABLE VALUE OF M, just start with 10

	//就只是从乘法转换成除法
	double invxywt = 1.0 / (STEP*STEP);//NOTE: this is different from how usual SLIC/LKM works

	while (numitr < NUMITR)
	{
		//------
		//cumerr = 0;
		numitr++;
		//------

		distvec.assign(sz, DBL_MAX);
		//遍历每个超像素块
		for (int n = 0; n < seedNum; n++)
		{
			//这四个点决定搜索范围，其实就是2*step,2*step
			int y1 = max(0, kseedsy[n] - offset);
			int y2 = min(m_height, kseedsy[n] + offset);
			int x1 = max(0, kseedsx[n] - offset);
			int x2 = min(m_width, kseedsx[n] + offset);

			//遍历搜索范围（2*step,2*step），对每个点计算到当前种子点的距离
			for (int y = y1; y < y2; y++)
			{
				for (int x = x1; x < x2; x++)
				{
					int idx = y*m_width + x;
					_ASSERT(y < m_height && x < m_width && y >= 0 && x >= 0);

					double l = m_lvec[idx];
					double a = m_avec[idx];
					double b = m_bvec[idx];

					distlab[idx] = (l - kseedsl[n])*(l - kseedsl[n]) +
						(a - kseedsa[n])*(a - kseedsa[n]) +
						(b - kseedsb[n])*(b - kseedsb[n]);

					distxy[idx] = (x - kseedsx[n])*(x - kseedsx[n]) +
						(y - kseedsy[n])*(y - kseedsy[n]);

					//------------------------------------------------------------------------
					double dist = distlab[idx] / maxlab[n] + distxy[idx] * invxywt;//only varying m, prettier superpixels
					//TODO:更改这里决定使用哪种SLIC方式，S是否固定，S固定，形状更加规则，S不固定产生更加符合边界的
					//double dist = distlab[idx]/maxlab[n] + distxy[idx]/maxxy[n];//varying both m and S
					//------------------------------------------------------------------------

					if (dist < distvec[idx]) //计算完距离后，找到距离最小值，并记录所属的类别，其实就是类似k_means
					{
						distvec[idx] = dist;
						klabels[idx] = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Assign the max color distance for a cluster
		//-----------------------------------------------------------------
		if (0 == numitr) //不执行的
		{
			maxlab.assign(seedNum, 1);
			maxxy.assign(seedNum, 1);
		}

		//这个貌似有问题？最大的lab距离在计算过程中其实因为滑窗存在覆盖，应该在之前循环里面记录，这里只是记录的最后被覆盖过的值
		for (int i = 0; i < sz; i++)
		{
			if (maxlab[klabels[i]] < distlab[i]) maxlab[klabels[i]] = distlab[i];
			if (maxxy[klabels[i]] < distxy[i]) maxxy[klabels[i]] = distxy[i];
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		sigmal.assign(seedNum, 0);
		sigmaa.assign(seedNum, 0);
		sigmab.assign(seedNum, 0);
		sigmax.assign(seedNum, 0);
		sigmay.assign(seedNum, 0);
		clustersize.assign(seedNum, 0);

		//统计每次重新聚类后的新信息，sigmaX分别记录X所属的分量的和，clustersize记录每个超像素的像素含量
		for (int j = 0; j < sz; j++)
		{
			int temp = klabels[j];
			_ASSERT(klabels[j] >= 0);
			sigmal[klabels[j]] += m_lvec[j];
			sigmaa[klabels[j]] += m_avec[j];
			sigmab[klabels[j]] += m_bvec[j];
			sigmax[klabels[j]] += (j%m_width);
			sigmay[klabels[j]] += (j / m_width);
			clustersize[klabels[j]]++;
		}

		for (int k = 0; k < seedNum; k++)
		{
			//_ASSERT(clustersize[k] > 0);
			if (clustersize[k] <= 0) clustersize[k] = 1; //防止出现除以0，要求每个超像素块至少有一个像素
			inv[k] = 1.0 / double(clustersize[k]);//computing inverse now to multiply, than divide later
		}

		//计算新的聚类中心
		for (int k = 0; k < seedNum; k++)
		{
			kseedsl[k] = sigmal[k] * inv[k];
			kseedsa[k] = sigmaa[k] * inv[k];
			kseedsb[k] = sigmab[k] * inv[k];
			kseedsx[k] = sigmax[k] * inv[k];
			kseedsy[k] = sigmay[k] * inv[k];
		}
		//然后继续下一次迭代
	}
	//迭代完成后,klabels里面保存每个像素属于的超像素块的编号(k_means里的类别)
}

//===========================================================================
///	SaveSuperpixelLabels
///
///	Save labels in raster scan order.
//===========================================================================
void SLIC::SaveSuperpixelLabels(
	const int*					labels,
	const int&					width,
	const int&					height,
	const string&				filename,
	const string&				path)
{
	int sz = width*height;

	char fname[_MAX_FNAME];
	char extn[_MAX_FNAME];
	_splitpath(filename.c_str(), NULL, NULL, fname, extn);
	string temp = fname;

	ofstream outfile;
	string finalpath = path + temp + string(".dat");
	outfile.open(finalpath.c_str(), ios::binary);
	for (int i = 0; i < sz; i++)
	{
		outfile.write((const char*)&labels[i], sizeof(int));
	}
	outfile.close();
}

//===========================================================================
///	EnforceLabelConnectivity
///
///		1. finding an adjacent label for each new component at the start
///		2. if a certain component is too small, assigning the previously found
///		    adjacent label to this component, and not incrementing the label.
//===========================================================================
void SLIC::EnforceLabelConnectivity(
	const int*					labels,//input labels that need to be corrected to remove stray labels
	const int&					width,
	const int&					height,
	int*						nlabels,//new labels
	int&						numlabels,//the number of labels changes in the end if segments are removed
	const int&					K) //the number of superpixels desired by the user
{
	//	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	//	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	//四邻域
	const int dx4[4] = { -1,  0,  1,  0 };
	const int dy4[4] = { 0, -1,  0,  1 };

	const int sz = width*height;
	const int SUPSZ = sz / K; //平均每个超像素的面积
	//nlabels.resize(sz, -1);
	for (int i = 0; i < sz; i++) nlabels[i] = -1; //新的标签数组先全部设为-1
	int label(0);
	int* xvec = new int[sz];
	int* yvec = new int[sz];
	int oindex(0);
	int adjlabel(0);//adjacent label
	for (int j = 0; j < height; j++)
	{
		for (int k = 0; k < width; k++)
		{
			if (0 > nlabels[oindex])//如果还未分配(-1)新标签的话
			{
				nlabels[oindex] = label;
				//--------------------
				// Start a new segment
				//--------------------
				xvec[0] = k;
				yvec[0] = j;
				//-------------------------------------------------------
				// Quickly find an adjacent label for use later if needed
				//-------------------------------------------------------
				for (int n = 0; n < 4; n++)
				{
					int x = xvec[0] + dx4[n];
					int y = yvec[0] + dy4[n];
					if ((x >= 0 && x < width) && (y >= 0 && y < height))
					{
						int nindex = y*width + x;
						if (nlabels[nindex] >= 0) adjlabel = nlabels[nindex]; //可以看到这里如果四个点都满足的话，就是最后一个，所以最优先是下面一个点的标签
					}
				}

				int count(1);
				for (int c = 0; c < count; c++)
				{
					for (int n = 0; n < 4; n++)
					{
						int x = xvec[c] + dx4[n];
						int y = yvec[c] + dy4[n];

						if ((x >= 0 && x < width) && (y >= 0 && y < height))
						{
							int nindex = y*width + x;

							if (0 > nlabels[nindex] && labels[oindex] == labels[nindex])
							{
								xvec[count] = x;
								yvec[count] = y;
								nlabels[nindex] = label;
								count++;
							}
						}

					}
				}
				//-------------------------------------------------------
				// If segment size is less then a limit, assign an
				// adjacent label found before, and decrement label count.
				//-------------------------------------------------------
				if (count <= SUPSZ >> 2)
				{
					for (int c = 0; c < count; c++)
					{
						int ind = yvec[c] * width + xvec[c];
						nlabels[ind] = adjlabel;
					}
					label--;
				}
				label++;
			}
			oindex++;
		}
	}
	numlabels = label;

	if (xvec) delete[] xvec;
	if (yvec) delete[] yvec;
}

//===========================================================================
///	PerformSLICO_ForGivenStepSize
///
/// There is option to save the labels if needed.
//===========================================================================
void SLIC::PerformSLICO_ForGivenStepSize(
	const unsigned int*			ubuff,
	const int					width,
	const int					height,
	int*						klabels,
	int&						numlabels,
	const int&					STEP,
	const double&				m)
{
	vector<double> kseedsl(0);
	vector<double> kseedsa(0);
	vector<double> kseedsb(0);
	vector<double> kseedsx(0);
	vector<double> kseedsy(0);

	//--------------------------------------------------
	m_width = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	//klabels = new int[sz];
	for (int s = 0; s < sz; s++) klabels[s] = -1;
	//--------------------------------------------------
	DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
	//--------------------------------------------------

	bool perturbseeds(true);
	vector<double> edgemag(0);
	if (perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, STEP, perturbseeds, edgemag);

	PerformSuperpixelSegmentation_VariableSandM(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, 10);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, double(sz) / double(STEP*STEP));
	{for (int i = 0; i < sz; i++) klabels[i] = nlabels[i]; }
	if (nlabels) delete[] nlabels;
}

//===========================================================================
///	PerformSLICO_ForGivenK
///
/// Zero parameter SLIC algorithm for a given number K of superpixels.
//===========================================================================
void SLIC::PerformSLICO_ForGivenK(
	const unsigned int*			imgBuff,
	const int					width,
	const int					height,
	int*						klabels,
	int&						numlabels,
	const int&					K,//required number of superpixels
	const double&				m)//weight given to spatial distance
{
	vector<double> kseedsl(0);
	vector<double> kseedsa(0);
	vector<double> kseedsb(0);
	vector<double> kseedsx(0);
	vector<double> kseedsy(0);

	//--------------------------------------------------
	m_width = width;
	m_height = height;
	int sz = m_width*m_height;
	//--------------------------------------------------
	//if(0 == klabels) klabels = new int[sz];
	for (int s = 0; s < sz; s++) klabels[s] = -1; //先把所有标签设为-1
	//--------------------------------------------------
	if (1)//LAB
	{
		DoRGBtoLABConversion(imgBuff, m_lvec, m_avec, m_bvec); //函数内部进行了内存的分配
	}
	else //RGB
	{
		m_lvec = new double[sz]; m_avec = new double[sz]; m_bvec = new double[sz];
#pragma region 原始版本
		for (int i = 0; i < sz; i++)
		{
			m_lvec[i] = imgBuff[i] >> 16 & 0xff;
			m_avec[i] = imgBuff[i] >> 8 & 0xff;
			m_bvec[i] = imgBuff[i] & 0xff;
		}
#pragma endregion
#pragma region 我修改的三通道版本
		for (int i = 0; i != m_height; ++i)
		{
			for (int j = 0; j != m_width; ++j)
			{
				m_bvec[i*m_width + j] = imgBuff[i*width * 3 + j * 3 + 0];
				m_avec[i*m_width + j] = imgBuff[i*width * 3 + j * 3 + 1];
				m_lvec[i*m_width + j] = imgBuff[i*width * 3 + j * 3 + 2];
			}
		}
#pragma endregion



	}
	//--------------------------------------------------

	bool perturbseeds(true);
	vector<double> edgemag(0);
	if (perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag); //就是一个简单的左右相减，上下相减检测边缘
	//TODO:这里的边缘检测是否可以改变来改善？

	GetLABXYSeeds_ForGivenK(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, K, perturbseeds, edgemag);

	int STEP = sqrt(double(sz) / double(K)) + 2.0;//adding a small value in the even the STEP size is too small.
	//PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, m);
	PerformSuperpixelSegmentation_VariableSandM(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, 10); //迭代10次
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, K);
	{for (int i = 0; i < sz; i++) klabels[i] = nlabels[i]; }
	if (nlabels) delete[] nlabels;
}