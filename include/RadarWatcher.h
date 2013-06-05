/************************************************************************/
/* �ļ����ƣ�RadarWatcher.h
/* ����ʱ�䣺2013-06-05 19:51
/*
/* ��    �����״�����࣬�����������ݵ��쳣
/************************************************************************/
#ifndef	RADAR_WATCHER_H
#define	RADAR_WATCHER_H
#include <QtCore>
class CAbstractStockItem;
enum RadarType
{
	BigVolumn = 1,		//��ɽ���
	BigAmount = 2,		//��ɽ���
	MaxPrice = 4,		//���¸�
};

typedef struct tagRRadarData
{
	RadarType tpType;				//�״�����
	time_t tmTime;					//����ʱ��
	CAbstractStockItem* pStock;		//��Ʊָ��
	QString qsDesc;					//������Ϣ
} RRadarData;

class CRadarWatcher : public QObject
{
	Q_OBJECT
public:
	/*
		�����״����
		_t �״�����
		iSec ���ӵ�ʱ����
	*/
	static CRadarWatcher* createRadarWatcher(RadarType _t,int iSec);

public:
	CRadarWatcher(int _id,RadarType _t,int iSec);
	~CRadarWatcher(void);

public:
	int getId(){ return m_id; }

signals:
	void radarAlert(RRadarData* pRadar);

private:
	QList<RRadarData*> m_listRadar;				//Ԥ���״�����
	int m_id;									//�״�id
	RadarType m_type;							//�״�����
	int m_iSec;									//���Ӽ��

private:
	static QMap<int,CRadarWatcher*> m_listWatchers;		//���еļ�����
};

class CVolumnWatcher : public CRadarWatcher
{
public:
	CVolumnWatcher(int _id,int iSec);
	~CVolumnWatcher(void);
};

class CAmountWatcher : public CRadarWatcher
{
public:
	CAmountWatcher(int _id,int iSec);
	~CAmountWatcher(void);
};

class CMaxPriceWatcher : public CRadarWatcher
{
public:
	CMaxPriceWatcher(int _id,int iSec);
	~CMaxPriceWatcher(void);
};

#endif	//RADAR_WATCHER_H