/************************************************************************/
/* �ļ����ƣ�RadarWatcher.h
/* ����ʱ�䣺2013-06-05 19:51
/*
/* ��    �����״�����࣬�����������ݵ��쳣
/************************************************************************/
#ifndef	RADAR_WATCHER_H
#define	RADAR_WATCHER_H
#include <QtCore>
#include "rsd_global.h"

class CStockInfoItem;
class CBlockInfoItem;
class CRadarWatcher;

enum RadarType
{
	BigVolumn = 1,		//��ɽ���
	BigIncrease = 2,	//��ɽ���
	MaxPrice = 4,		//���¸�
	MinPrice = 8,		//���µ�
};

typedef struct tagRRadarData
{
	RadarType tpType;				//�״�����
	time_t tmTime;					//����ʱ��
	CRadarWatcher* pWatcher;		//��Դ�״�
	CStockInfoItem* pStock;			//��Ʊָ��
	QString qsDesc;					//������Ϣ
} RRadarData;

class RSDSHARED_EXPORT CRadarWatcher : public QObject
{
	Q_OBJECT
public:
	/*
		�����״����
		pBlock ���ӵİ��
		_t �״�����
		iSec ���ӵ�ʱ����
		_hold ��ֵ
	*/
	static CRadarWatcher* createRadarWatcher(CBlockInfoItem* pBlock,
		RadarType _t,int iSec,float _hold,int iId=-1);

	/*
		��ȡ���еļ����״�
	*/
	static QList<CRadarWatcher*> getRadarWatchers();

	//���ؼ����״�
	static void loadRadars();
	//��������״�
	static void saveRadars();
	//���������״�
	static void releaseRadars();

public:
	CRadarWatcher(int _id,CBlockInfoItem* pBlock,RadarType _t,int iSec,float _hold);
	~CRadarWatcher(void);

public:
	//��ȡ���״��id
	int getId(){ return m_id; }
	RadarType getType(){return m_type;}
	float getHold(){ return m_fHold; }
	int getSec(){ return m_iSec; }
	CBlockInfoItem* getBlock(){ return m_pWatcherBlock; }

signals:
	void radarAlert(RRadarData* pRadar);
	void watcherDelete(CRadarWatcher* pWatcher);

protected:
	void appendRadar(RRadarData* pRadar);

protected slots:
	virtual void onStockReportComing(CStockInfoItem* pItem) = 0;

protected:
	int m_iSec;									//���Ӽ��
	float m_fHold;								//��ֵ

private:
	QList<RRadarData*> m_listRadar;				//Ԥ���״�����
	int m_id;									//�״�id
	RadarType m_type;							//�״�����
	CBlockInfoItem* m_pWatcherBlock;			//���ӵİ��

private:
	static QMap<int,CRadarWatcher*> m_listWatchers;		//���еļ�����
};


/*�ɽ������ӣ����ӳɽ����ı仯*/
class CVolumnWatcher : public CRadarWatcher
{
	struct tagVolumnData
	{
		time_t tmTime;		//����γ�ʱ��
		float fLastV;		//��һ�εĳɽ���
		float fNewV;		//ʱ������ʾ�ĳɽ���
	};
	Q_OBJECT
public:
	CVolumnWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold);
	~CVolumnWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagVolumnData*> m_mapVolume;		//��һ�ֳɽ���
};

class CIncreaseWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//����γ�ʱ��
		float fMaxPrice;	//�����е���߼۸�
		float fNewPrice;	//ʱ������ʾ�ļ۸�
	};
	Q_OBJECT
public:
	CIncreaseWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold);
	~CIncreaseWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//��һ�ֳɽ���
};

class CMaxPriceWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//����γ�ʱ��
		float fMaxPrice;	//�����е���߼۸�
		float fNewPrice;	//ʱ������ʾ�ļ۸�
	};
	Q_OBJECT
public:
	CMaxPriceWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold);
	~CMaxPriceWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//��һ�ֳɽ���
};

class CMinPriceWatcher : public CRadarWatcher
{
	struct tagPriceData
	{
		time_t tmTime;		//����γ�ʱ��
		float fMinPrice;	//�����е���߼۸�
		float fNewPrice;	//ʱ������ʾ�ļ۸�
	};
	Q_OBJECT
public:
	CMinPriceWatcher(int _id,CBlockInfoItem* pBlock,int iSec,float _hold);
	~CMinPriceWatcher(void);
protected slots:
	void onStockReportComing(CStockInfoItem* pItem);

private:
	QMap<CStockInfoItem*,tagPriceData*> m_mapPrice;		//��һ�ֳɽ���
};

#endif	//RADAR_WATCHER_H