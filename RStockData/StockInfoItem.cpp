/************************************************************************/
/* �ļ����ƣ�StockInfoItem.cpp
/* ����ʱ�䣺2013-11-08 09:28
/*
/* ��    �������ڴ洢��ֻ��Ʊ����
/************************************************************************/

#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"
#include "Hz2Py.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
	, fLast5Volume(0.0)
{
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
	qsCode = code;
	wMarket = market;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;
}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
	, fLast5Volume(0.0)
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;

	qsCode = QString::fromLocal8Bit(info.code);
	wMarket = info.wMarket;
	qsMarket = CDataEngine::getMarketStr(wMarket);
	qsOnly = qsCode+qsMarket;
	//fLast5Volume = 0.0;
	//QList<qRcvHistoryData*> list = getLastHistory(5);
	//foreach(qRcvHistoryData* pHis,list)
	//{
	//	fLast5Volume = fLast5Volume+pHis->fVolume;
	//}
}

CStockInfoItem::~CStockInfoItem(void)
{
	delete pLastReport;
	delete pCurrentReport;

	{
		QMap<time_t,qRcvPowerData*>::iterator iter = mapPowers.begin();			//��Ȩ����
		while(iter!=mapPowers.end())
		{
			delete iter.value();
			++iter;
		}
		mapPowers.clear();
	}
}

void CStockInfoItem::setReport( qRcvReportData* p )
{
	if(p->tmTime<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//���µ�Report������ӵ��ֱ�������
	qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);
	
	updateItemInfo();
	appendFenBis(QList<qRcvFenBiData*>()<<pFenBi);
}

void CStockInfoItem::setReport( RCV_REPORT_STRUCTExV3* p )
{
	if(p->m_time<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	CDataEngine::setCurrentTime(pCurrentReport->tmTime);

	//���µ�Report������ӵ��ֱ�������
	qRcvFenBiData* pFenBi = new qRcvFenBiData(pCurrentReport);

	updateItemInfo();
	appendFenBis(QList<qRcvFenBiData*>()<<pFenBi);
}

void CStockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
	QList<qRcvHistoryData*> listHistory;
	int iCountFromFile = -1;
	if(list.size()>130)
	{
		listHistory = CDataEngine::getDataEngine()->getHistoryList(this);
	}
	else
	{
		iCountFromFile = list.size();
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

	QMap<time_t,qRcvHistoryData*>::iterator iter = mapHistorys.begin();
	while(iter!=mapHistorys.end())
	{
		delete iter.value();
		++iter;
	}
	mapHistorys.clear();

	emit stockItemHistoryChanged(qsOnly);
}


QList<qRcvPowerData*> CStockInfoItem::getPowerList()
{
	return mapPowers.values();
}

void CStockInfoItem::appendPowers( const QList<qRcvPowerData*>& list )
{
	foreach(qRcvPowerData* p,list)
	{
		if(mapPowers.contains(p->tmTime))
		{
			qRcvPowerData* pBefore = mapPowers[p->tmTime];
			if(pBefore!=p)
				delete pBefore;
		}
		mapPowers[p->tmTime] = p;
	}
}


void CStockInfoItem::appendFenBis( const QList<qRcvFenBiData*>& list )
{
	CAbstractStockItem::appendFenBis(list);
	resetBuySellVOL();
	return;
//	CDataEngine::getDataEngine()->exportFenBiData(qsCode,mapFenBis.values());
}


void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));


	//��ӯ��
	if(pCurrentReport&&baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��

	//��ͨ��ֵ
	if(pCurrentReport&&baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

	emit stockItemReportChanged(qsOnly);
}


QString CStockInfoItem::getName() const
{

	return qsName;
}

float CStockInfoItem::getIncrease() const
{
	return fIncrease;
}

float CStockInfoItem::getVolumeRatio() const
{
	return fVolumeRatio;
}

float CStockInfoItem::getTurnRatio() const
{
	return fTurnRatio;
}

float CStockInfoItem::getLastClose() const
{
	if(pCurrentReport)
		return pCurrentReport->fLastClose;
	return FLOAT_NAN;
}

float CStockInfoItem::getOpenPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fOpen;

	return FLOAT_NAN;
}

float CStockInfoItem::getHighPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fHigh;

	return FLOAT_NAN;
}

float CStockInfoItem::getLowPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fLow;

	return FLOAT_NAN;
}

float CStockInfoItem::getNewPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fNewPrice;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalVolume() const
{
	if(pCurrentReport)
		return pCurrentReport->fVolume;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalAmount() const
{
	if(pCurrentReport)
		return pCurrentReport->fAmount;

	return FLOAT_NAN;
}

float CStockInfoItem::getNowVolume() const
{
	return fNowVolume;
}

float CStockInfoItem::getIncSpeed() const
{
	//���磬ĳ����Ʊ5����֮ǰ�Ĺɼ���10Ԫ�������ڵļ۸���10.1Ԫ���������Ʊ��5��������Ϊ��
	return fIncreaseSpeed;
}

float CStockInfoItem::getPriceFluctuate() const
{
	//�����ǵ�����%��=����ǰ��-ǰ���̼ۣ�/ ǰ���̼� * 100
	return fPriceFluctuate;
}

float CStockInfoItem::getAmplitude() const
{
	/*
		һ����Ʊ���=������߼�/������ͼۡ�100%-100%
		������Ʊ���=��������߼ۣ�������ͼ�)/�������̼ۡ�100%
		���ݷɺ������õڶ��ּ��㷽����
	*/
	return fAmplitude;
}

float CStockInfoItem::getAvePrice() const
{
	return fAvePrice;
}

float CStockInfoItem::getPERatio() const
{
	//��ӯ�ʣ���̬��ӯ�ʣ�=���¼ۣ�ÿ������
	return fPERatio;
}

float CStockInfoItem::getLTSZ() const
{
	//��ͨ��ֵ ����ֵ������Ŀǰ��Ʊ�м۳�����ͨ�ɣ���ֵͨ����Ŀǰ��Ʊ�м۳�����[1]ͨ�ɱ���
	return fLTSZ;
}

float CStockInfoItem::getZGB() const
{
	//�ܹɱ� ����ֵ������Ŀǰ��Ʊ�м۳����ܹɱ�����ֵͨ����Ŀǰ��Ʊ�м۳�����[1]ͨ�ɱ���
	return fZGB;
}

float CStockInfoItem::getSellVOL() const
{
	//������
	return fSellVOL;
}

float CStockInfoItem::getBuyVOL() const
{
	//������
	return fBuyVOL;
}

float CStockInfoItem::getBIDVOL() const
{
	//ί����
	return fBuyVolume;
}

float CStockInfoItem::getASKVOL() const
{
	//ί����
	return fSellVolume;
}

float CStockInfoItem::getCommRatio() const
{
	//ί��
	//(ί��������ί������)/(ί������+ί������)*100
	return fCommRatio;
}

float CStockInfoItem::getCommSent() const
{
	//ί��
	return fCommSent;
}


float CStockInfoItem::getLast5Volume()
{
	//��ȥ5�յĳɽ�����
	return fLast5Volume;
}

float CStockInfoItem::getMgjzc()
{
	//��ȡÿ�ɾ��ʲ�
	return baseInfo.fMgjz;
}

float CStockInfoItem::getMgsy()
{
	//��ȡÿ������
	return baseInfo.fMgsy;
}


void CStockInfoItem::updateItemInfo()
{
	//���ù�Ʊ����
	if(qsName.isEmpty())
	{
		qsName = pCurrentReport->qsName;
		//���´ʿ���еļ�ƴ
		shortName = CHz2Py::getHzFirstLetter(qsName);
	}
	if(baseInfo.code[0]==0)
	{
		strcpy_s(baseInfo.code,qsCode.toLocal8Bit().data());
		baseInfo.wMarket = wMarket;
	}

	//�Ƿ�
	if(pCurrentReport->fNewPrice>0.0 && pCurrentReport->fLastClose>0.0)
		fIncrease = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose)*100.0/pCurrentReport->fLastClose;
	/*�����ʣ��ֲ
		������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
		�����й����ɽ���/��ͨ�ܹ���*100%��
	*/
	if(baseInfo.fZgb>0)
		fTurnRatio = pCurrentReport->fVolume/baseInfo.fZgb*100;

	//��ӯ��
	if(baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��
	//��ͨ��ֵ
	if(baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;
	if(baseInfo.fZgb>0)
		fZGB = baseInfo.fZgb*pCurrentReport->fNewPrice;

	//�ǵ����۸񲨶�
	fPriceFluctuate = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose);

	//���
	fAmplitude = (pCurrentReport->fHigh-pCurrentReport->fLow)/pCurrentReport->fLastClose;

	//����
	fAvePrice = (pCurrentReport->fAmount/pCurrentReport->fVolume)/100;

	{
		//ί��������
		fBuyVolume = 0.0;
		fBuyVolume += pCurrentReport->fBuyVolume[0];
		fBuyVolume += pCurrentReport->fBuyVolume[1];
		fBuyVolume += pCurrentReport->fBuyVolume[2];
		fBuyVolume += pCurrentReport->fBuyVolume4;
		fBuyVolume += pCurrentReport->fBuyVolume5;

		//ί��������
		fSellVolume = 0.0;
		fSellVolume += pCurrentReport->fSellVolume[0];
		fSellVolume += pCurrentReport->fSellVolume[1];
		fSellVolume += pCurrentReport->fSellVolume[2];
		fSellVolume += pCurrentReport->fSellVolume4;
		fSellVolume += pCurrentReport->fSellVolume5;

		//ί��
		if(pCurrentReport&&(fBuyVolume>0||fSellVolume>0))
		{
			fCommRatio = ((fBuyVolume-fSellVolume)/(fBuyVolume+fSellVolume))*100;
			fCommSent = fBuyVolume-fSellVolume;
		}
		else
		{
			fCommRatio = FLOAT_NAN;
			fCommSent = FLOAT_NAN;
		}
	}

	{
		//�����̼���
		//if(pCurrentReport->fNewPrice>0&&pLastReport->fNewPrice>0)
		//{
		//	if(QDateTime::fromTime_t(pCurrentReport->tmTime).date()>QDateTime::fromTime_t(pLastReport->tmTime).date())
		//	{
		//		fSellVOL = fBuyVOL = 0;
		//	}
		//}
		fNowVolume = (pCurrentReport->fVolume)-(fBuyVOL+fSellVOL);
		//if(fNowVolume>0)
		//{
		//	if(pCurrentReport->fNewPrice>pCurrentReport->fBuyPrice[0])
		//		fSellVOL += fNowVolume;
		//	else
		//		fBuyVOL += fNowVolume;
		//}
	}

	if(pLastReport->fNewPrice > 0.0)
	{
		//����Report�ĶԱȼ���
		fIncreaseSpeed = (pCurrentReport->fNewPrice-pLastReport->fNewPrice)/pLastReport->fNewPrice;
	}

	{
		/*���ȼ��㣺
			���ȣ��ֳɽ�����/����ȥ5��ƽ��ÿ���ӳɽ���*�����ۼƿ���ʱ�䣨�֣��� 
			�����ȴ���1ʱ��˵������ÿ���ӵ�ƽ���ɽ���Ҫ���ڹ�ȥ5�յ�ƽ����ֵ�����ױȹ�ȥ5�ջ𱬣�
			��������С��1ʱ��˵�����ڵĳɽ��Ȳ��Ϲ�ȥ5�յ�ƽ��ˮƽ��
		*/
		if(fLast5Volume>0.0)
		{
			time_t tmSeconds = CDataEngine::getOpenSeconds(pCurrentReport->tmTime);
			fVolumeRatio = (pCurrentReport->fVolume)/((fLast5Volume/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
		}
	}

	emit stockItemReportChanged(qsOnly);
	emit stockItemReportComing(this);
}

void CStockInfoItem::resetBuySellVOL()
{
	//���¼���������
	if(mapFenBis.size()>0)
	{
		//�Էֱ�����������������
		fBuyVOL = 0;		//����������
		fSellVOL = 0;

		bool bSell = true;			//����տ�ʼ������

		qRcvFenBiData* pLastFenBi = 0;
		QList<qRcvFenBiData*> list = mapFenBis.values();
		foreach(qRcvFenBiData* p,list)
		{
			float fVOL = p->fVolume;
			if(pLastFenBi)
				fVOL = (p->fVolume)-(pLastFenBi->fVolume);

			if(p->fBuyPrice[0]>0.0)
			{
				if(p->fPrice>p->fBuyPrice[0])
				{
					fSellVOL += fVOL;
					bSell = true;
				}
				else
				{
					fBuyVOL += fVOL;
					bSell = false;
				}
			}
			else
			{
				if(pLastFenBi)
				{
					if(p->fPrice<pLastFenBi->fPrice)
					{
						fSellVOL += fVOL;
						bSell = true;
					}
					else if(p->fPrice>pLastFenBi->fPrice)
					{
						fBuyVOL += fVOL;
						bSell = false;
					}
					else
					{
						if(bSell)
							fSellVOL+=fVOL;
						else
							fBuyVOL +=fVOL;
					}
				}
				else
				{
					if(bSell)
						fSellVOL+=fVOL;
					else
						fBuyVOL +=fVOL;
				}

			}

			pLastFenBi = p;
		}
	}
}

bool CStockInfoItem::isMatch( const QString& _key )
{
	//�жϴ����Ƿ�ƥ��
	if(qsCode.indexOf(_key,0,Qt::CaseInsensitive)>-1)
		return true;

	//�ж����Ƽ�ƴ�Ƿ�ƥ��
	for (int i = 0; i < _key.size(); ++i)
	{
		if(i>=shortName.size())
			return false;
		QList<QChar> _l = shortName[i];
		bool bMatch = false;
		foreach(const QChar& _c,_l)
		{
			if(_c == _key[i])
			{
				bMatch = true;
				break;
			}
		}
		if(!bMatch)
			return false;
	}

	return true;
}

