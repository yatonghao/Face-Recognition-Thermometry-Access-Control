#include <QApplication>
#include <signal.h>

#include "main.h"
#include "mainwindow.h"
#include "opencv_deal.h"

/* C++ ���� C */
#ifdef __cplusplus
	extern "C" {
#endif

#include "svrsocket.h"

#ifdef __cplusplus
	}
#endif

// ����ȫ�ֹ�����Ϣ
struct mainWorkStru 		gstMainWork;

extern struct socketInfo	svrSocketInfo;
extern FaceRecogn			gFaceRec;
extern struct userMngr_Stru	gstUsrMngr;;


/* �ͷ���Դ */
void main_deInit(void);

/* ��������ص� */
void signal_exit_handler(int sig)
{
	printf("\n--------------- byebye server ---------------\n");
	printf("%s: signal[%d].\n", __FUNCTION__, sig);
	
    main_deInit();

	exit(0);
}

int main(int argc, char *argv[])
{
	printf("--------------- welcome server ---------------\n");
	
	int ret;
	
	if(argc != 2)
	{
		printf("usage: %s <port>\n", argv[0]);
		return -1;
	}

	// ����ȫ�ֹ����ṹ���ʼ��
	gstMainWork.workStatus = 1;
	gstMainWork.tid_Sock = 0;
	gstMainWork.tid_facOpen = 0;
	gstMainWork.tid_facDetRec = 0;
	gstMainWork.tid_addFace = 0;
	gstMainWork.opcvDealReboot = 0;
	
	// ע���쳣�źŻص�
	signal(SIGINT,  signal_exit_handler);
	signal(SIGABRT, signal_exit_handler);

	svrSocketInfo.port = atoi(argv[1]);
	printf("svrSocketInfo.port = %d\n", svrSocketInfo.port);

	// Get the path to your CSV.
	gFaceRec.fn_csv = string(CSV_FILE);
	
	QApplication app(argc, argv);
	MainWindow win;
	win.show();

	userMngr_Init(&gstUsrMngr);	// user manager init, add delete face

	ret = pthread_create(&gstMainWork.tid_Sock, NULL, socket_handle, NULL);
	if(ret < 0)
	{
		printf("pthread_create socket_handle failed.\n");
		return -1;
	}

	ret = pthread_create(&gstMainWork.tid_facOpen, NULL, faceopencv_handle, NULL);
	if(ret < 0)
	{
		printf("faceopencv_handle faceopencv_handle failed.\n");
		return -1;
	}


	return app.exec();
}


void main_deInit(void)
{
	gstMainWork.workStatus = 0; 	// stop working

	usleep(1000);		// 1 ms
	
	opencvDeal_deInit();
	
	svrSocket_deInit(&svrSocketInfo);

	userMngr_DeInit(&gstUsrMngr);

}

