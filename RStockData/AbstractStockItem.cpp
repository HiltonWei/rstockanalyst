#include "StdAfx.h"
#include "AbstractStockItem.h"
#include "StockInfoItem.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"


CAbstractStockItem::CAbstractStockItem(void)
	: pCurrentReport(NULL)
	, fLast5Volume(0.0)
{
	pCurrent5Min = new RBlockData;
}


CAbstractStockItem::~CAbstractStockItem(void)
{
	{
		//��շֱ�����
		QMap<time_t,qRcvFenBiData*>::iterator iter = mapFenBis.begin();
		while(iter!=mapFenBis.end())
		{
			delete iter.value();
			++iter;
		}
		mapFenBis.clear();
	}

	{
		//�����ʷ5��������
		QMap<time_t,RStockData*>::iterator iter = mapMinDatas.begin();		//5������ʷ����
		while(iter!=mapMinDatas.end())
		{
			delete iter.value();
			++iter;
		}
		mapMinDatas.clear();
	}

	{
		//�����������
		foreach(qRcvHistoryData* pData,listHistories)
		{
			delete pData;
		}
		listHistories.clear();
	}
}

void CAbstractStockItem::initStockItem()
{
	//��ȡ��ȥ5�յĳɽ����������ڼ������ȵ���Ϣ
	//QList<qRcvHistoryData*> list = CDataEngine::getDataEngine()->getHistoryList(this,5);
	//foreach(qRcvHistoryData* pHis,list)
	//{
	//	fLast5Volume = fLast5Volume+pHis->fVolume;
	//	delete pHis;
	//}
	//list.clear();

	CDataEngine::getDataEngine()->importMinData(this,mapMinDatas);
}

qRcvReportData* CAbstractStockItem::getCurrentReport() const
{
	return pCurrentReport;
}

QList<qRcvFenBiData*> CAbstractStockItem::getFenBiList()
{
	//��ȡ�ֱ����ݣ�δ�깤
	return mapFenBis.values();
}

QList<qRcvHistoryData*> CAbstractStockItem::getHistoryList()
{
	if(listHistories.size()<1)
	{
		listHistories = CDataEngine::getDataEngine()->getHistoryList(this);
	}
	return listHistories;
}

QList<qRcvHistoryData*> CAbstractStockItem::getLastHistory( int count )
{
	if(listHistories.size()<1)
	{
		listHistories = CDataEngine::getDataEngine()->getHistoryList(this);
	}
	if(listHistories.size()>count)
	{
		return listHistories.mid(listHistories.size()-count);
	}
	return listHistories;
}

void CAbstractStockItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
	QList<qRcvHistoryData*> listHistory;
	int iCountFromFile = -1;
	bool bRemoveList = true;		//�Ƿ����ɾ��listHistory�е�����

	if(list.size()>130)
	{
		//��ȡ������ʷ����
		bRemoveList = false;
		listHistory = getHistoryList();
	}
	else
	{
		//����ȡ5������
		iCountFromFile = list.size();
		if(iCountFromFile<5)
			iCountFromFile = 5;
		listHistory = CDataEngine::getDataEngine()->getHistoryList(this,iCountFromFile);
	}

	QMap<time_t,qRcvHistoryData*> mapHistorys;		//��������
	foreach(qRcvHistoryData* p,listHistory)
	{
		if(mapHistorys.contains(p->time))
		{
			qRcvHistoryData* pBefore = mapHistorys[p->time];
			if(pBefore!=p)
				delete pBefore;
		}
		mapHistorys[p->time] = p;
	}
	foreach(qRcvHistoryData* p,list)
	{
		if(mapHistorys.contains(p->time))
		{
			qRcvHistoryData* pBefore = mapHistorys[p->time];
			if(pBefore!=p)
				delete pBefore;
		}
		mapHistorys[p->time] = p;
	}


	listHistory.clear();
	listHistory = mapHistorys.values();
	{
		//���5�յ�ȫ���ɽ������������ȵļ��㣩
		fLast5Volume = 0.0;
		for (int i=1;i<=5;++i)
		{
			int iIndex = listHistory.size()-i;
			if(iIndex<0)
				break;
			fLast5Volume = fLast5Volume + listHistory[iIndex]->fVolume;
		}
		updateItemInfo();
	}

	CDataEngine::getDataEngine()->exportHistoryData(this,listHistory,iCountFromFile);

	if(bRemoveList)
	{
		QMap<time_t,qRcvHistoryData*>::iterator iter = mapHistorys.begin();
		while(iter!=mapHistorys.end())
		{
			delete iter.value();
			++iter;
		}
		mapHistorys.clear();
	}
	else
	{
		listHistories = listHistory;
	}

	emit stockItemHistoryChanged(qsOnly);
}


void CAbstractStockItem::appendMinData( tagRStockData* pData )
{
	if(mapMinDatas.contains(pData->tmTime))
	{
		tagRStockData* pBefore = mapMinDatas[pData->tmTime];
		mapMinDatas.remove(pBefore->tmTime);
		delete pBefore;
	}
	mapMinDatas.insert(pData->tmTime,pData);
	return;
}

void CAbstractStockItem::appendFenBis( const QList<qRcvFenBiData*>& list )
{
	//׷�ӷֱ����ݣ�δ�깤
	if(mapFenBis.size()>0&&list.size()>0)
	{
		QDate myDate = QDateTime::fromTime_t(mapFenBis.begin().value()->tmTime).date();
		QDate theDate = QDateTime::fromTime_t(list.first()->tmTime).date();
		if(theDate>myDate)
		{
			//������������ݵ�ʱ����ڵ�ǰ��ʱ�䣬����յ�ǰ������
			foreach(qRcvFenBiData* p,mapFenBis.values())
				delete p;

			mapFenBis.clear();
		}
	}

	foreach(qRcvFenBiData* p,list)
	{
		bool bInsert = true;
		if(mapFenBis.contains(p->tmTime))
		{
			QList<qRcvFenBiData*> listV = mapFenBis.values(p->tmTime);
			foreach(qRcvFenBiData* p1,listV)
			{
				if(p1->fVolume == p->fVolume)
				{
					if(p1->fBuyPrice[0]>0.0)
					{
						//���֮ǰ�����ݲ������ҵ����ݣ���ɾ��֮ǰ��
						delete p1;
						mapFenBis.remove(p->tmTime);
						break;
					}
					else
					{
						//ɾ�����ڵ�����
						bInsert = false;
						delete p;
						break;
					}
				}
			}
		}
		if(bInsert)
			mapFenBis.insert(p->tmTime,p);
	}


	//���׷�ӵ����ݴ���5�������¶�5�������ݽ��м���
	if(list.size()>5 && (!CDataEngine::isLoading()))
	{
		recalcMinData();
	}

	emit stockItemFenBiChanged(qsOnly);
}


QString CAbstractStockItem::getOnly() const
{
	return qsOnly;
}

QString CAbstractStockItem::getCode() const
{
	return qsCode;
}

WORD CAbstractStockItem::getMarket() const
{
	return wMarket;
}

QString CAbstractStockItem::getMarketName() const
{
	return qsMarket;
}


bool CAbstractStockItem::isInstanceOfStock()
{
	CStockInfoItem* p = dynamic_cast<CStockInfoItem*>(this);
	return p ? true : false;
}

bool CAbstractStockItem::isInstanceOfBlock()
{
	CBlockInfoItem* p = dynamic_cast<CBlockInfoItem*>(this);
	return p ? true : false;
}

int CAbstractStockItem::getNewHighPriceCount()
{
	int iCount = 0;

	QMap<time_t,qRcvFenBiData*>::iterator iter=mapFenBis.begin();
	float fHighPrice = getOpenPrice();

	float fPrice = fHighPrice;
	int iTime = 0;
	while (iter!=mapFenBis.end())
	{
		int iT = (iter.key())/60;
		if(iT>iTime)
		{
			if(fPrice>fHighPrice)
			{
				iCount++;
				fHighPrice = fPrice;
			}
			iTime = iT;
		}
		fPrice = (*iter)->fPrice;

		++iter;
	}

	return iCount;
}

int CAbstractStockItem::getNewHighVolumeCount()
{
	int iCount = 0;

	QMap<time_t,qRcvFenBiData*>::iterator iter=mapFenBis.begin();
	if(iter==mapFenBis.end())
		return iCount;

	qRcvFenBiData* pLast = 0;
	qRcvFenBiData* pCur = 0;

	float fHighVolume = 0;
	int iTime = iter.key()/60;

	while (iter!=mapFenBis.end())
	{
		int iT = (iter.key())/60;
		if(iT>iTime)
		{
			if(pLast && pCur)
			{
				if((pCur->fVolume-pLast->fVolume)>fHighVolume)
				{
					iCount++;
					fHighVolume = (pCur->fVolume-pLast->fVolume);
				}
			}
			else if(pCur)
			{
				fHighVolume = pCur->fVolume;
			}
			else
			{
				fHighVolume = (*iter)->fVolume;
			}
			pLast = pCur;
			iTime = iT;
		}

		pCur = *iter;
		++iter;
	}

	return iCount;
}

QList<tagRStockData*> CAbstractStockItem::get5MinList()
{
	QList<tagRStockData*> list = map5MinDatas.values();
	if(pCurrent5Min->tmTime>0 && (!map5MinDatas.contains(pCurrent5Min->tmTime)))
	{
		list.push_back(pCurrent5Min);
	}
	return list;
}

RStockData* CAbstractStockItem::get5MinData( const time_t& tmTime )
{
	if(mapMinDatas.contains(tmTime))
		return mapMinDatas[tmTime];

	return 0;
}
