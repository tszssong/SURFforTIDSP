#define SAMPLE_STEP0			1		//��ʼ�����������������ֵ������ӿ�����������ٶȣ����ǻ������������ȡ��ò��ȶ�
#define TOTAL_OCTAVE 			5		//SURF�㷨�߶ȿռ�����
#define SURF_HESSIAN_TH			20		//Hessian����ֵ
#define MATCH_TH 				0.05f	//������ƥ����ֵ������С�ڴ�����ʾƥ����

#define MAX_IPOINTS 			200		//����������������

#define DESCRIPTOR_LENTH 		64		//��������������

#define Responses				109		//����������ʱ-6,6Բ�������ڵ����ص����

#define SURF_ORI_SEARCH_INC 	0.15f		//��������ÿ��ת���ĽǶȣ�matlab���Ǵ��8�ȶ�

#define Max_TOTAL_LAYERS	20			//��˹����������

#define IMAGE_WIDTH  	256	//ͼ���ȣ���Ӧ��������cols
#define IMAGE_HEIGHT 	256
#define IMAGE_SIZE		IMAGE_WIDTH*IMAGE_HEIGHT

#define INPUT_IMAGE_ADDRESS 		(0x88000000) 						//����ͼ��ĵ�ַ��Ϊddr2�׵�ַ
#define INTEGRAL_IMAGE_ADDRESS 		(0x88100000)
#define DET_IMAGE_ADDRESS 			(0x88200000)
#define TRACE_IMAGE_ADDRESS 		(0x88800000)

#define WIN_ADDRESS 		(0x88e00000)

#define DET_SIZE					sizeof(double)*IMAGE_SIZE			//��һ����Ӧֵ�洢�ռ��С
typedef int img_pix ;//������������
