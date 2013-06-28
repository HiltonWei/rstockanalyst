#include "StdAfx.h"
#include "AbstractStockItem.h"
#include "StockInfoItem.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"


CAbstractStockItem::CAbstractStockItem(void)
	: pCurrentReport(NULL)
{

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
		QMap<time_t,RStockData*>::iterator iter = map5MinDatas.begin();		//5������ʷ����
		while(iter!=map5MinDatas.end())
		{
			delete iter.value();
			++iter;
		}
		map5MinDatas.clear();
	}
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
	return CDataEngine::getDataEngine()->getHistoryList(this);
}

QList<qRcvHistoryData*> CAbstractStockItem::getLastHistory( int count )
{
	return CDataEngine::getDataEngine()->getHistoryList(this,count);
}

void CAbstractStockItem::appendHistorys( const QList<qRcvHistoryData*>& /*list*/ )
{

}


void CAbstractStockItem::append5MinData( tagRStockData* pData )
{
	if(map5MinDatas.contains(pData->tmTime))
	{
		tagRStockData* pBefore = map5MinDatas[pData->tmTime];
		map5MinDatas.remove(pBefore->tmTime);
		delete pBefore;
	}
	map5MinDatas.insert(pData->tmTime,pData);
	return;
}

QList<RStockData*> CAbstractStockItem::get5MinList()
{
	return map5MinDatas.values();
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
