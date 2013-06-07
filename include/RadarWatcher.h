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


class CRadarWatcher : public QObject
{
	Q_OBJECT
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

protected slots:
	virtual void onStockReportComing(CStockInfoItem* pItem) = 0;

protected:
	int m_iSec;									//���Ӽ��
	float m_fHold;								//��ֵ

private:
	int m_id;									//�״�id
	RadarType m_type;							//�״�����
	CBlockInfoItem* m_pWatcherBlock;			//���ӵİ��
};

class RSDSHARED_EXPORT CRadarManager : public QObject
{
	Q_OBJECT
public:
	static CRadarManager* getRadarManager();
	
public:
	/*
		�����״����
		pBlock ���ӵİ��
		_t �״�����
		iSec ���ӵ�ʱ����
		_hold ��ֵ
	*/
	CRadarWatcher* createRadarWatcher(CBlockInfoItem* pBlock,
		RadarType _t,int iSec,float _hold,int iId=-1);
	
	/*
		��ȡ���еļ����״�
	*/
	QList<CRadarWatcher*> getRadarWatchers();

	//׷�Ӳ����ļ�����Ϣ
	void appendRadar(RRadarData* pRadar);

	//���ؼ����״�
	void loadRadars();
	//��������״�
	void saveRadars();
	//���������״�
	void releaseRadars();

signals:
	void radarAlert(RRadarData* pRadar);

protected:
	CRadarManager(){};
	~CRadarManager(){};

private:
	QMap<int,CRadarWatcher*> m_listWatchers;	//���еļ�����
	QList<RRadarData*> m_listRadar;				//Ԥ���״�����

private:
	static CRadarManager* m_pSelf;
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