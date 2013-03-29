#ifndef _RSTOCK_FUNC_H_
#define _RSTOCK_FUNC_H_
#include "StockInfoItem.h"

#ifndef FLOAT_NAN
#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())
#endif

/*
RStockAnalyst����չ��������
*/


// ��������
enum RStockCircle			//��������
{
	FenShi = 1,				//��ʱģʽ��ֱ�����ߣ�
	Sec10 = 10,				//10��
	Sec30 = 30,				//30��
	Min1 = 60,				//1����
	Min5 = 5*60,			//5����
	Min15 = 15*60,			//15����
	Min30 = 30*60,			//30����
	Min60 = 60*60,			//60����
	MinN,					//N����
	Day,					//����
	DayN,					//N����
	Week,					//����
	Month,					//����
	Month3,					//����
	Year,					//����
};

//��������
enum RWidgetType
{
	WidgetBasic = 0,				//����ͼ
	WidgetKLine,					//K��ͼ
	WidgetMarketTrend,			//�г�����ͼ
	WidgetSColorBlock,		//��Ʊ��ɫ��ͼ
	WidgetBColorBlock,			//����ɫ��ͼ
	WidgetStockInfo,				//��ֻ��Ʊ��������Ϣ
};


//���ڵķ���
enum RWidgetInsert
{
	InsertLeft =1,			//�����
	InsertRight,				//�Ҳ���
	InsertTop,				//�ϲ���
	InsertBottom,			//�²���
	InsertRemove,			//ɾ��
};

// ��������
typedef struct tagRStockData
{
	time_t	tmTime;			//UCT
	float	fOpen;			//���� ע����ʱ�����д����ݱ�ʾ���¼�
	float	fHigh;			//���
	float	fLow;				//���
	float	fClose;			//���� ע����ʱ�����д����ݱ�ʾ��һ�������ļ۸�
	float	fVolume;			//��
	float	fAmount;			//��
	WORD	wAdvance;			//����,��������Ч
	WORD	wDecline;			//����,��������Ч

	tagRStockData()
	{
		memset(&tmTime,0,sizeof(tagRStockData));
	}
	tagRStockData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&tmTime,&p->m_time,sizeof(tagRStockData));
	}
	tagRStockData(const qRcvReportData* p)
	{
		tmTime = QDateTime(QDateTime::fromTime_t(p->tmTime).date()).toTime_t();
		fAmount = p->fAmount;
		fClose = p->fNewPrice;
		fHigh = p->fHigh;
		fLow = p->fLow;
		fOpen = p->fOpen;
		fVolume = p->fVolume;
	}
} RStockData;


// ��չ����,���������ֱʳɽ����ݵ�������
typedef union tagRStockDataEx
{
	struct
	{
		float m_fBuyPrice[5];		//��1--��5��
		float m_fBuyVol[5];			//��1--��5��
		float m_fSellPrice[5];		//��1--��5��        
		float m_fSellVol[5];		//��1--��5�� 
	};
	float m_fDataEx[20];
} RStockDataEx;


// ��Ȩ����
typedef struct tagRSplitData
{
	time_t	m_time;	//ʱ��,UCT
	float	m_fHg;	//���
	float	m_fPg;	//���
	float	m_fPgj;	//��ɼ�
	float	m_fHl;	//����
} RSpliteData;


// ���ýӿ���Ϣ���ݽṹ
typedef struct tagRCalcInfo
{
	DWORD dwVersion;		//�汾
	RStockCircle emCircle;		//��������
	CStockInfoItem* pItem;

	QMap<time_t,RStockData*>* mapData;	//��������,����ΪNULL
	QMap<time_t,RStockDataEx*>* mapDataEx;	//��չ����,�ֱʳɽ�������,ע��:����Ϊ NULL

	tagRCalcInfo()
	{
		memset(&dwVersion,0,sizeof(tagRCalcInfo));
	}
} RCalcInfo;


typedef class QPainter QPainter;
typedef class QRect QRect;
typedef struct tagRDrawInfo
{
	DWORD dwVersion;
	QPainter* pPainter;
	QRectF rtClient;
	int iEndIndex;				//��������������
	float fItemWidth;			//�������ݵĻ��ƿ��

	float fMin;
	float fMax;

	tagRDrawInfo()
	{
		memset(&dwVersion,0,sizeof(tagRDrawInfo));
		fMin = 1.0;
		fMax = -1.0;
	}
} RDrawInfo;

#endif	//_RSTOCK_FUNC_H_