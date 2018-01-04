/*
 * main.c
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "myPara.h"
#include "myData.h"
clock_t start,stop;
extern int Image_data1[];
extern int Image_data2[];
//int mydata1[IMAGE_SIZE], mydata2[IMAGE_SIZE];		//原始图像大小1024时用此数组存放256下采样数据
int mysum[IMAGE_SIZE];								//积分图像数据
/**************************************************************
 * 图像下采样函数:这里是对1024*1024图像下采样，
 * 选择原图像中4*4像素小方块的左上角像素作为采样图像
 * src：原始图像地址，dst：下采样图像地址，w:原图像宽度，h：原图像高度
 *************************************************************/
void Sample(int* src, int* dst, int w, int h)
{
	int i,j;
	int* pDst = (int *)dst;
	int* pSrc = (int *)src;
	for(j=0;j<h;j+=4)
	{
		pSrc = (int *)(src+w*j);
		for(i=0;i<w;i+=4)
		{
			*pDst++ = *pSrc;
			pSrc += 4;
		}
	}
}
/**************************************************************
 * 积分图像计算
 * src：原始图像地址，sum：积分图像地址，w:图像宽度，h：图像高度
 *************************************************************/
void integral( int* src,  int* sum, int w, int h)
{
	int i = 0,j=0;
	int* pSum = (int *)sum;
	int* pSrc = (int *)src;
	int rowsum = 0;
	for (j = 0; j < h; j++)
	{
		rowsum = 0;
		for (i = 0; i < w; i++)
		{
	    	rowsum += *pSrc;
	    	if(j==0)
	    		*pSum = rowsum;
	        else
	        	*pSum = rowsum + *(pSum-w);
	    	pSrc++;pSum++;
		}
	}
}
/**************************************************************
 * 尺度空间构建函数，用于计算每层采样值、盒子滤波器大小、层高
 * 层宽、并分配每层Hessian响应值存储空间
 *
 * showMap=1 打印生成的Map信息
 **************************************************************/
void BuildMap(char showMap)
{
	int j;
	int length = 0;
	for(j=0;j<TOTAL_OCTAVE*2+2;j++)
	{
		Map[j].sampleStep = SAMPLE_STEP0*((int)1<<sampleStep_map[j]);
		Map[j].fliterSize = filterSize_map[j];
		Map[j].width = IMAGE_WIDTH/Map[j].sampleStep;
		Map[j].height = IMAGE_HEIGHT/Map[j].sampleStep;
		Map[j].length = IMAGE_SIZE/Map[j].sampleStep/Map[j].sampleStep;
		Map[j].det = (double *)DET_IMAGE_ADDRESS + length;			//考虑到本程序会使用所有分配给det和trace的空间，
		Map[j].trace = (double *)TRACE_IMAGE_ADDRESS + length;		//这里没有初始化清零
		length += Map[j].length;
	}
	if(showMap)
	{
		for(j=0;j<TOTAL_OCTAVE*2+2;j++)
		{
			printf("octave:%d\n",j+1);
			printf("%d, %d, %d, %d, %d\n",Map[j].sampleStep,Map[j].fliterSize,Map[j].width,Map[j].height,Map[j].length);
			printf("%x, %x\n", Map[j].det, Map[j].trace);
		}
	}
}
/*****************************************************************
 * 盒子滤波器的实现函数，考虑图像边界
 *
 * 参数分别为盒子左上角的行号、列号、盒子高度、盒子宽度、积分图像首地址
 *****************************************************************/
int BoxIntegral(int r1, int c1, int h, int w,  int* src)
{
	int row1, col1, row2, col2;
	int A=0, B=0, C=0, D=0;

	if(r1<0)
		{row1=0;}
	else if(r1>IMAGE_HEIGHT)
		{row1 = IMAGE_HEIGHT-1;}
	else
		{row1 = r1-1;}

	if(c1<0)
		{col1=0;}
	else if(c1>IMAGE_WIDTH)
		{col1 = IMAGE_WIDTH-1;}
	else
		{col1 = c1-1;}

	int r2 = r1+h;
	int c2 = c1+w;

	if(r2>=IMAGE_HEIGHT)
		{row2 = IMAGE_HEIGHT-1;}
	else
		{row2 = r2-1;}

	if(c2>=IMAGE_WIDTH)
		{col2 = IMAGE_WIDTH-1;}
	else
		{col2 = c2-1;}

	if( r1>0 && c1>0) A = src[row1*IMAGE_WIDTH+col1];
	if( r1>0 && c2>0) B = src[row1*IMAGE_WIDTH+col2];
	if( r2>0 && c1>0) C = src[row2*IMAGE_WIDTH+col1];
	if( r2>0 && c2>0) D = src[row2*IMAGE_WIDTH+col2];

	return (A+D-B-C);
}
/*HaarX计算，参数分别为积分图像首地址、盒子左上角行号、列号、盒子边长*/
int HaarX(int* src, int row, int column, int size)
{
	return BoxIntegral(row - size / 2, column, size, size/2, src) - BoxIntegral(row - size / 2, column - size / 2, size, size/2, src);
}
/*HaarY计算，参数分别为积分图像首地址、盒子左上角行号、列号、盒子边长*/
int HaarY(int* src, int row, int column, int size)
{
	return BoxIntegral(row, column - size / 2, size / 2, size, src)- BoxIntegral(row - size / 2, column - size / 2, size / 2, size, src);
}
/**************************************************************
 * Hessian响应计算
 * 为每层尺度空间计算Hessian响应值
 **************************************************************/
void GetResponce(int* sum, char show)
{
	int j;
	int row,col,r,c;
	int b,l,w;
	double inverse_area;
	double Dxx, Dyy, Dxy;
	for(j=0;j<TOTAL_OCTAVE*2+2;j++)
	{
		b = (Map[j].fliterSize - 1) / 2 + 1;        // border for this filter
		l = Map[j].fliterSize / 3;                  // lobe for this filter (filter size / 3)
		w = Map[j].fliterSize;  					// filter size
		inverse_area = 1 / (double)(w*w);

		for(row=0;row<Map[j].height;row++)
		{
			for(col=0;col<Map[j].width;col++)
			{
				r = row*Map[j].sampleStep;
				c = col*Map[j].sampleStep;
				Dxx = BoxIntegral(r-l+1, c-b, 2*l-1, w,sum) - BoxIntegral(r-l+1, c-l/2, 2*l-1, l, sum)*3;
				Dyy = BoxIntegral(r-b, c-l+1, w, 2*l-1,sum) - BoxIntegral(r-l/2, c-l+1, l, 2*l-1, sum)*3;
				Dxy = BoxIntegral(r-l, c+1, l, l,sum) + BoxIntegral(r+1, c-l, l, l,sum) - BoxIntegral(r-l, c-l, l, l,sum) - BoxIntegral(r+1, c+1, l, l,sum);
				Dxx *= inverse_area;
				Dyy *= inverse_area;
				Dxy *= inverse_area;
				Map[j].det[row*Map[j].width+col] = (Dxx*Dyy - 0.81*Dxy* Dxy);
				Map[j].trace[row*Map[j].width+col]= (int)(Dxx + Dyy);
			}
		}
	}
}
/****************************************************************************
 * 插值函数：对获得的兴趣点做3*3*3邻域内插
 * N9[3][9]：兴趣点的3*3*3邻域；
 * dStep:中间层与底层滤波器大小之差；
 * filterSize:本层滤波器大小
 ******************************************************************************/
PixPoint interpolateKeypoint(double N9[3][9], int dStep, int filterSize)
{
	PixPoint InterploatePonit;
	InterploatePonit.valid = 0;
	double dx = -(N9[1][5] - N9[1][3]) / 2;  	// Negative 1st deriv with respect to x
    double dy = -(N9[1][7] - N9[1][1]) / 2;  	// Negative 1st deriv with respect to y
    double ds = -(N9[2][4] - N9[0][4]) / 2; 	// Negative 1st deriv with respect to s

    double dxx = N9[1][3] - 2 * N9[1][4] + N9[1][5];            	// 2nd deriv x, x
    double dxy = (N9[1][8] - N9[1][6] - N9[1][2] + N9[1][0]) / 4; 	// 2nd deriv x, y
    double dxs = (N9[2][5] - N9[2][3] - N9[0][5] + N9[0][3]) / 4; 	// 2nd deriv x, s
    double dyy = N9[1][1] - 2 * N9[1][4] + N9[1][7];            	// 2nd deriv y, y
    double dys = (N9[2][7] - N9[2][1] - N9[0][7] + N9[0][1]) / 4; 	// 2nd deriv y, s
    double dss = N9[0][4] - 2 * N9[1][4] + N9[2][4];           		// 2nd deriv s, s

    double x[3];
    x[0] = ( (dyy*dss-dys*dys)*dx + (dys*dxs -dxy*dss)*dy + (dxy*dys- dxs*dyy)*ds )/(dxx*dyy*dss + 2*dxy*dys*dxs - dxs*dxs*dyy - dys*dys*dxx - dxy*dxy*dss);
    x[1] = ( (dxs*dys - dxy*dss)*dx + (dxx*dss - dxs*dxs)*dy + (dxy*dxs - dxx*dys)*ds )/(dxx*dyy*dss + 2*dxy*dys*dxs - dxs*dxs*dyy - dys*dys*dxx - dxy*dxy*dss);
    x[2] = ( (dxy*dys - dxs*dyy)*dx + (dxy*dxs - dxx*dys)*dy + (dxx*dyy - dxy*dxy)*ds )/(dxx*dyy*dss + 2*dxy*dys*dxs - dxs*dxs*dyy - dys*dys*dxx - dxy*dxy*dss);


    InterploatePonit.valid = ( (x[0] != 0 || x[1] != 0 || x[2] != 0) && (fabs(x[0])<0.5) && (fabs(x[1])<0.5)&& (fabs(x[2])<0.5) );

    if (InterploatePonit.valid==1)
    {
    	InterploatePonit.x = x[0];		//*dx;
    	InterploatePonit.y = x[1];		//*dy;
    	InterploatePonit.s = (double)(filterSize + x[2] * dStep)*0.13333f;
    }
    return InterploatePonit;
}
/**************************************************************************************
 * Search for SURF key points
 * keypoints: store key points; showOctaves: for debug
 * ***********************************************************************************/
int GetIpoints(SurfIpt* keypoints, char showOctaves)
{
	int o, i;
	int r, c;
	int nIpt = 0;
	int b, m, t;
	double val0;
	double N9[3][9];
	int rr, cc;
	PixPoint IptPoint;
	for (o = 0; o<TOTAL_OCTAVE; o++)
	{
		for (i = 0; i <= 1; i++)
		{
			b = filter_map[o * 4 + i];
			m = filter_map[o * 4 + i + 1];
			t = filter_map[o * 4 + i + 2];
			if(showOctaves)
				printf("o:%d, i:%d, b:%d, m:%d, t:%d\n", o, i, b, m, t);
			for (r = 0; r<Map[t].height; r++)
			{
				for (c = 0; c<Map[t].width; c++)
				{
					int layerBorder = (Map[t].fliterSize + 1) / (2 * Map[t].sampleStep);
					if (r <= layerBorder || r >= Map[t].height - layerBorder || c <= layerBorder || c >= Map[t].width - layerBorder)
						continue;
					int s_bt = Map[b].width / Map[t].width;
					int s_mt = Map[m].width / Map[t].width;
					val0 = Map[m].det[r*s_mt*Map[m].width + c*s_mt];
					if (val0 > SURF_HESSIAN_TH)
					{
						for (rr = -1; rr <= 1; rr++)
						{
							for (cc = -1; cc <= 1; cc++)
							{
								N9[0][rr * 3 + cc + 4] = Map[b].det[(r + rr)*s_bt*Map[b].width + (c + cc)*s_bt];
								N9[1][rr * 3 + cc + 4] = Map[m].det[(r + rr)*s_mt*Map[m].width + (c + cc)*s_mt];
								N9[2][rr * 3 + cc + 4] = Map[t].det[(r + rr)*Map[t].width + c + cc];
							}
						}
						if (val0 > N9[0][0] && val0 > N9[0][1] && val0 > N9[0][2] &&
							val0 > N9[0][3] && val0 > N9[0][4] && val0 > N9[0][5] &&
							val0 > N9[0][6] && val0 > N9[0][7] && val0 > N9[0][8] &&
							val0 > N9[1][0] && val0 > N9[1][1] && val0 > N9[1][2] &&
							val0 > N9[1][3] && val0 > N9[1][5] &&
							val0 > N9[1][6] && val0 > N9[1][7] && val0 > N9[1][8] &&
							val0 > N9[2][0] && val0 > N9[2][1] && val0 > N9[2][2] &&
							val0 > N9[2][3] && val0 > N9[2][4] && val0 > N9[2][5] &&
							val0 > N9[2][6] && val0 > N9[2][7] && val0 > N9[2][8])
						{
							IptPoint.valid = 0;
							if (nIpt<MAX_IPOINTS)
								IptPoint = interpolateKeypoint(N9, (Map[m].fliterSize - Map[b].fliterSize), Map[m].fliterSize);
							if (IptPoint.valid == 1)
							{
								keypoints[nIpt].x = (c + IptPoint.x)*Map[t].sampleStep;
								keypoints[nIpt].y = (r + IptPoint.y)*Map[t].sampleStep;
								keypoints[nIpt].s = IptPoint.s;
								nIpt++;
							}
						}
					}
				}
			}
		}
	}
	return nIpt;
}
double GetOrientation(int* sum, SurfIpt ip)
{
	double pi = 3.1415926f;
	double resX[Responses];
	double resY[Responses];
	double Ang[Responses];
	int idx = 0;
	int id[] = { 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6 };

	// Get rounded InterestPoint data
	int X = (int)round(ip.x);
	int Y = (int)round(ip.y);
	int S = (int)round(ip.s);
	int i, j, k;
	// calculate haar responses for points within radius of 6*scale
	for ( i = -6; i <= 6; i++)
	{
		for ( j = -6; j <= 6; j++)
		{
			if (i * i + j * j < 36)
			{
				double gauss = GAUSS_25[ id[i + 6] ][ id[j + 6] ];
				resX[idx] = gauss * (double)HaarX(sum, Y + j * S, X + i * S, 4 * S);
				resY[idx] = gauss * (double)HaarY(sum, Y + j * S, X + i * S, 4 * S);

				if (atan2(resY[idx], resX[idx]) < 0)
					Ang[idx] = atan2(resY[idx], resX[idx]) + 2 * pi;
				else
					Ang[idx] = atan2(resY[idx], resX[idx]);
				++idx;
			}
		}
	}

	// calculate the dominant direction
	double sumX, sumY, max = 0, orientation = 0;
	double ang1, ang2;

	// loop slides pi/3 window around feature point
	for (ang1 = 0; ang1 < 2 * pi; ang1 += SURF_ORI_SEARCH_INC)
	{
		if (ang1 + pi / 3.0f > 2 * pi)
			ang2 = ang1 - 5 * pi / 3.0f;
		else
			ang2 = ang1 + pi / 3.0f;

		sumX = sumY = 0;

		for (k = 0; k < Responses; k++)
		{
			// determine whether the point is within the window
			if (ang1 < ang2 && ang1 < Ang[k] && Ang[k] < ang2)
			{
				sumX += resX[k];
				sumY += resY[k];
			}
			else if (ang2 < ang1 && ((Ang[k] > 0 && Ang[k] < ang2) || (Ang[k] > ang1 && Ang[k] < pi)))
			{
				sumX += resX[k];
				sumY += resY[k];
			}
		}

		if (sumX * sumX + sumY * sumY > max)
		{
			// store largest orientation
			max = sumX * sumX + sumY * sumY;
			//orientation = atan2(sumY, sumX);
			if( atan2(sumY,sumX)<0 )
				orientation = atan2(sumY, sumX)+2*pi;
			else
				orientation = atan2(sumY, sumX);
		}
	}

	// assign orientation of the dominant response vector
	return orientation;
}
void GetDescriptor(int* sum, SurfIpt* keypoints, int nIpt, char upright, char extended)
{
	int sample_x, sample_y, count = 0;
	int i = 0, ix = 0, j = 0, jx = 0, xs = 0, ys = 0, k = 0, l = 0, c = 0, d = 0;
	double dx, dy, mdx, mdy, co, si;
	double gauss_s1 = 0.0f, gauss_s2 = 0.0f;
	double rx = 0.0f, ry = 0.0f, rrx = 0.0f, rry = 0.0f;
	double len = 0.0f, tempLen = 0.0f;

	// Get rounded InterestPoint data
	int X, Y, S;
	for (c = 0; c<nIpt; c++)
	{
		X = (int)round( keypoints[c].x );
		Y = (int)round( keypoints[c].y );
		S = (int)round( keypoints[c].s );
		if(upright)
		{
			co = 1;
			si = 0;
		}
		else
		{
			co = cos(keypoints[c].orientation);
			si = sin(keypoints[c].orientation);
		}
		//Calculate descriptor for this interest point
		count = 0;
		len = 0;
		double cx = -0.5f, cy = 0.0f; 	//Subregion centers for the 4x4 gaussian weighting
		i = -8;
		while (i < 12)
		{
			j = -8;
			i = i - 4;

			cx += 1.0f;
			cy = -0.5f;

			while (j < 12)
			{
				cy += 1.0f;

				j = j - 4;

				ix = i + 5;
				jx = j + 5;

				xs = (int)round(X + (-jx * S * si + ix * S * co));	//四舍五入取整，0表示0位小数点
				ys = (int)round(Y + (jx * S * co + ix * S * si));

				// zero the responses
				dx = dy = mdx = mdy = 0.0f;

				for (k = i; k < i + 9; ++k)
				{
					for (l = j; l < j + 9; ++l)
					{
						//Get coords of sample point on the rotated axis
						sample_x = (int)round(X + (-l * S * si + k * S * co));
						sample_y = (int)round(Y + (l * S * co + k * S * si));

						//Get the gaussian weighted x and y responses
						gauss_s1 = Gaussian(xs - sample_x, ys - sample_y, 2.5f * S);
						rx = (double)HaarX(sum, sample_y, sample_x, 2 * S);
						ry = (double)HaarY(sum, sample_y, sample_x, 2 * S);

						//Get the gaussian weighted x and y responses on rotated axis
						rrx = gauss_s1 * (-rx * si + ry * co);
						rry = gauss_s1 * (rx * co + ry * si);

						dx += rrx;
						dy += rry;
						mdx += fabs(rrx);
						mdy += fabs(rry);

					}
				}

				//Add the values to the descriptor vector
				gauss_s2 = Gaussian(cx - 2.0f, cy - 2.0f, 1.5f);

				keypoints[c].descriptor[count++] = dx * gauss_s2;
				keypoints[c].descriptor[count++] = dy * gauss_s2;
				keypoints[c].descriptor[count++] = mdx * gauss_s2;
				keypoints[c].descriptor[count++] = mdy * gauss_s2;

				tempLen = (dx * dx + dy * dy + mdx * mdx + mdy * mdy) * gauss_s2 * gauss_s2;
				len += tempLen;

				j += 9;
			}
			i += 9;
		}

		//Convert to Unit Vector
		len = sqrt(len);
		if (len > 0)
		{
			for (d = 0; d < DESCRIPTOR_LENTH; ++d)
			{
				keypoints[c].descriptor[d] /= len;
			}
		}

	}//end for(c=0;c<NumOfIpt;c++)
}

double GetDistance(SurfIpt keypoints1, SurfIpt keypoints2)
{
	double sum = 0;
	int i = 0;
	for (i = 0; i < DESCRIPTOR_LENTH; i++)
	{
		sum += (keypoints1.descriptor[i] - keypoints2.descriptor[i]) * (keypoints1.descriptor[i] - keypoints2.descriptor[i]);
	}
	return sum;
}
//寻找double型数组中最小的数，返回该最小值所在的位置
int getMin(double *data, int number)
{
	int i, idx = 0;
	double temp = FLT_MAX;
	for (i = 0; i<number; i++)
	{
		if (data[i]<temp)
		{
			temp = data[i];
			idx = i;
		}
	}
	return idx;
}
//返回特征点少的特征集合特征点数
int getMatches(SurfIpt* keypoints1, SurfIpt* keypoints2, int nI1, int nI2)
{
	int i, j, k, t;
	int nMatched = 0;	//统计大于阈值的个数
	double dist[MAX_IPOINTS] = { FLT_MAX };

		for (i = 0; i < nI1; i++)
		{
			for (j = 0; j < nI2; j++)
			{
				dist[j] = GetDistance(keypoints1[i], keypoints2[j]);
			}
			j = getMin(dist, nI2);
			if(dist[j]<=MATCH_TH) nMatched++;
			for (k = 0; k < i; k++)
			{
				if (dist[j] < matchedIpt[k].distance)
					break;
			}
			for (t = i; t > k; t--)
			{
				matchedIpt[t].id = matchedIpt[t - 1].id;
				matchedIpt[t].x1 = matchedIpt[t - 1].x1;
				matchedIpt[t].x2 = matchedIpt[t - 1].x2;
				matchedIpt[t].y1 = matchedIpt[t - 1].y1;
				matchedIpt[t].y2 = matchedIpt[t - 1].y2;
				matchedIpt[t].distance = matchedIpt[t - 1].distance;
			}
			matchedIpt[t].id = j;	//id记录匹配到的点在特征点多的特征集合中的位置
			matchedIpt[t].x1 = keypoints1[i].x;
			matchedIpt[t].y1 = keypoints1[i].y;
			matchedIpt[t].x2 = keypoints2[j].x;
			matchedIpt[t].y2 = keypoints2[j].y;
			matchedIpt[t].distance = dist[j];
		}
		return nMatched;
}

int main(void) {
	int i=0;
	int nIpt1=0, nIpt2=0, nMatched = 0;
	//若原图像使用1024*1024调用Sample函数下采样
	//Sample((int *)Image_data1, mydata1, 1024, 1024);
	//Sample((int *)Image_data2, mydata2, 1024, 1024);
	printf("OpenSURF test:\n");
		/*T0_TGCR = 0;
		T0_TIM12 = 0;
		T0_TIM34 = 0;
		T0_TGCR = 3;
		T0_PRD12 = 0XFFFFFFFF;
		T0_PRD34 = 0XFFFFFFFF;
		printf("%x,%x\n",T0_TIM34,T0_TIM12);
		T0_TCR = 0x00000040;*/
	start = clock();
	//计算积分图像
	integral(Image_data1, mysum, IMAGE_WIDTH, IMAGE_HEIGHT);
	//参数为0不打印
	BuildMap(0);
	GetResponce(mysum,0);
	nIpt1 = GetIpoints(Ipt1, 0);
	//printf("Time to get ipt1: %d ms\n",T0_TIM12/24000);
	stop=clock();
		    printf("Cycle to get ipt1 is %d\n",stop-start);
	printf("Number of key points in pic1:%d\n",nIpt1);
	for (i = 0; i < nIpt1; i++)
		Ipt1[i].orientation = GetOrientation(mysum, Ipt1[i]);
	GetDescriptor(mysum, Ipt1, nIpt1, 0, 0);
	//printf("Time to describe ipt1: %d ms\n",T0_TIM12/24000);
	stop=clock();
	    printf("Cycle to describe ipt1 is %d\n",stop-start);

	integral(Image_data2, mysum, IMAGE_WIDTH, IMAGE_HEIGHT);
	BuildMap(0);
	GetResponce(mysum,0);
	nIpt2 = GetIpoints(Ipt2, 0);
	//printf("Time to get ipt2: %d ms\n",T0_TIM12/24000);
	printf("Number of key points in pic2:%d\n",nIpt2);
	for (i = 0; i < nIpt2; i++)
		Ipt2[i].orientation = GetOrientation(mysum, Ipt2[i]);
	GetDescriptor(mysum, Ipt2, nIpt2, 0, 0);
	//printf("Time to describe ipt2: %d ms\n",T0_TIM12/24000);
	stop=clock();
	printf("Cycle to get and describe ipt2 is %d\n",stop-start);

	//for(i = 0;i<nIpt1;i++)
	//	printf("Ipt1[%2d] (%6.2f,%6.2f) s: %.2f ori: %.2f\n",i,Ipt1[i].x,Ipt1[i].y,Ipt1[i].s, Ipt1[i].orientation);
	//for(i = 0;i<nIpt2;i++)
	//	printf("Ipt2[%2d] (%6.2f,%6.2f) s: %.2f ori: %.2f\n",i,Ipt2[i].x,Ipt2[i].y,Ipt2[i].s, Ipt2[i].orientation);


	nMatched = getMatches(Ipt1, Ipt2, nIpt1, nIpt2);
	printf("Time to matched: %d ms\n",T0_TIM12/24000);
	stop=clock();
	//printf("Cycle to matched is %d\n",stop-start);
	//nMatched = 30;
	//printf("Matched points: %d\n", nMatched);
	//printf("Matched points: \n");
	for(i=0;i<nIpt1*0.7;i++)		//按照距离从小到大输出匹配特征点
		printf("%2d: matchedIpt[%2d] (%6.2f,%6.2f,%6.2f,%6.2f) d: %.5f\n", i, matchedIpt[i].id, matchedIpt[i].x1, matchedIpt[i].y1, matchedIpt[i].x2, matchedIpt[i].y2, matchedIpt[i].distance);

	printf("OpenSURF end!\n");
	return 0;
}
