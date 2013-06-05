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
	CRadarWatcher(void);
	~CRadarWatcher(void);

signals:
	void radarAlert(RRadarData* pRadar);

private:
	QList<RRadarData*> m_listRadar;							//Ԥ���״�����
};

#endif	//RADAR_WATCHER_H