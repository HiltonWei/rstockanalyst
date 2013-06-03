#include "StdAfx.h"
#include "AbstractStockItem.h"
#include "StockInfoItem.h"
#include "BlockInfoItem.h"


CAbstractStockItem::CAbstractStockItem(void)
	: pCurrentReport(NULL)
{

}


CAbstractStockItem::~CAbstractStockItem(void)
{
	QMultiMap<time_t,qRcvFenBiData*>::iterator iter = mapFenBis.begin();
	while(iter!=mapFenBis.end())
	{
		delete iter.value();
		++iter;
	}
	mapFenBis.clear();
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
	return QList<qRcvHistoryData*>();
}

QList<RStockData*> CAbstractStockItem::get5MinList()
{
	return QList<RStockData*>();
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
						mapFenBis.remove(p->tmTime,p1);
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

	emit stockItemFenBiChanged(qsCode);
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
