#define SAMPLE_STEP0			1		//初始采样步长，增加这个值，将会加快特征点检测的速度，但是会让特征点的提取变得不稳定
#define TOTAL_OCTAVE 			5		//SURF算法尺度空间组数
#define SURF_HESSIAN_TH			20		//Hessian阵阈值
#define MATCH_TH 				0.05f	//特征点匹配阈值，距离小于此数表示匹配点对

#define MAX_IPOINTS 			200		//最大允许的特征点数

#define DESCRIPTOR_LENTH 		64		//特征描述符长度

#define Responses				109		//计算主方向时-6,6圆形区域内的像素点个数

#define SURF_ORI_SEARCH_INC 	0.15f		//扇形区域每次转过的角度，matlab里是大概8度多

#define Max_TOTAL_LAYERS	20			//高斯金字塔层数

#define IMAGE_WIDTH  	256	//图像宽度，对应矩阵列数cols
#define IMAGE_HEIGHT 	256
#define IMAGE_SIZE		IMAGE_WIDTH*IMAGE_HEIGHT

#define INPUT_IMAGE_ADDRESS 		(0x88000000) 						//保存图像的地址，为ddr2首地址
#define INTEGRAL_IMAGE_ADDRESS 		(0x88100000)
#define DET_IMAGE_ADDRESS 			(0x88200000)
#define TRACE_IMAGE_ADDRESS 		(0x88800000)

#define WIN_ADDRESS 		(0x88e00000)

#define DET_SIZE					sizeof(double)*IMAGE_SIZE			//第一层响应值存储空间大小
typedef int img_pix ;//像素数据类型
