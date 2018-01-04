#include "myPara.h"
char 	sampleStep_map[] = 	{0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4};
int 	filterSize_map[] =	{9,15,21,27,39,51,75,99,147,195,291,387};
char	filter_map[] = 		{0,1,2,3,  1,3,4,5,  3,5,6,7,  5,7,8,9,  7,9,10,11};
#ifndef FLT_MAX
#define FLT_MAX 	3.402823466E+38F   /* LARGEST POSITIVE VALUE    */
#endif
//插值阈值
#define INTERPOLATE_TH 	0.4f
double GAUSS_25[7][7] =  {
      {0.02350693969273f,0.01849121369071f,0.01239503121241f,0.00708015417522f,0.00344628101733f,0.00142945847484f,0.00050524879060f},
      {0.02169964028389f,0.01706954162243f,0.01144205592615f,0.00653580605408f,0.00318131834134f,0.00131955648461f,0.00046640341759f},
      {0.01706954162243f,0.01342737701584f,0.00900063997939f,0.00514124713667f,0.00250251364222f,0.00103799989504f,0.00036688592278f},
      {0.01144205592615f,0.00900063997939f,0.00603330940534f,0.00344628101733f,0.00167748505986f,0.00069579213743f,0.00024593098864f},
      {0.00653580605408f,0.00514124713667f,0.00344628101733f,0.00196854695367f,0.00095819467066f,0.00039744277546f,0.00014047800980f},
      {0.00318131834134f,0.00250251364222f,0.00167748505986f,0.00095819467066f,0.00046640341759f,0.00019345616757f,0.00006837798818f},
      {0.00131955648461f,0.00103799989504f,0.00069579213743f,0.00039744277546f,0.00019345616757f,0.00008024231247f,0.00002836202103f}
    };
double Gaussian(double x, double y, double sig)
{
    return 1.0f / (2.0f * 3.14f * sig * sig) * exp(-(x * x + y * y) / (2.0f * sig * sig));
}
typedef struct SurfMap
{
    int width,height,sampleStep,fliterSize;	//层宽度，高度，采样步长sampleSteps,滤波器大小size
    int length;
    double *det, *trace;
}SurfMap;	//每层的hession响应描述
SurfMap Map[Max_TOTAL_LAYERS];
typedef struct SurfIpt	//特征点描述
{
    double x, y, s, orientation;		//坐标，尺度，主方向方向
    double descriptor[DESCRIPTOR_LENTH];				//64double描述符
}SurfIpt;
SurfIpt Ipt1[MAX_IPOINTS] = {0};
SurfIpt Ipt2[MAX_IPOINTS] = {0};		//特征点
typedef struct PixPoint
{
	double x,y,s;
    char valid;
}PixPoint;
typedef struct MatchedIpt
{
	double x1, y1, x2, y2;		//匹配上的点坐标
	double distance;			//描述符距离
	int id;						//序号
}MatchedIpt;	//每层的hession响应描述
MatchedIpt matchedIpt[MAX_IPOINTS]; 			// 每一层图像 对应的 Hessian响应
/*********************************Timer Reg Addr**************************************/
#define T0_TIM12	*(int *)(0x01c20010)
#define T0_TIM34	*(int *)(0x01c20014)
#define T0_PRD12	*(int *)(0x01c20018)
#define T0_PRD34	*(int *)(0x01c2001C)

#define T0_TCR		*(int *)(0x01c20020)
#define T0_TGCR 	*(int *)(0x01c20024)
