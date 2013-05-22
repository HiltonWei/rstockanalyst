#include "StdAfx.h"
#include <time.h>
#include <QApplication>
#include <QtXml>
#include "DataEngine.h"
#include "STKDRV.h"
#include "Hz2Py.h"


//�����ʱ���ݵĺ�����ʱ��
int getTimeMapBySec(QMap<time_t,int>& mapTimes,const time_t& tmBegin, const time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
		//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}

int getDayMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	time_t tmNow = (_tmEnd+3600*24);			//������һ������
	while(tmNow>=_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= 3600*24;
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addDays(8-dtNow.dayOfWeek());	//�����һ������
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= (3600*24*7);
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addMonths(1);					//�����һ������
	dtNow = QDate(dtNow.year(),dtNow.month(),1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-1);
		dtNow = QDate(dtNow.year(),dtNow.month(),1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();
	dtNow = dtNow.addMonths(3);					//�����һ������
	dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-3);
		dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addYears(1);					//�����һ������
	dtNow = QDate(dtNow.year(),1,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addYears(-1);
		dtNow = QDate(dtNow.year(),1,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

/*ͨ���������ݻ�ȡָ�������ڵ�����*/
RStockData* getColorBlockItemByMins(const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData;
	//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	qRcvFenBiData* pFirst = list.first();
	pData->tmTime = pFirst->tmTime;
	pData->fOpen = pFirst->fPrice;
	pData->fClose = pLast->fPrice;
	pData->fHigh = pFirst->fPrice;
	pData->fLow = pFirst->fPrice;

	if(pLastFenbi)
	{
		pData->fAmount = pLast->fAmount-pLastFenbi->fAmount;
		pData->fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		pData->fAmount = pLast->fAmount;
		pData->fVolume = pLast->fVolume;
	}

	foreach(qRcvFenBiData* p,list)
	{
		if(pData->fLow>p->fPrice)
			pData->fLow = p->fPrice;
		if(pData->fHigh<p->fPrice)
			pData->fHigh = p->fPrice;
	}
	return pData;
}

/*ͨ���������ݻ�ȡһ�������ڵ�����*/
RStockData* getColorBlockItemByDays(const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return NULL;
	RStockData* pData = new RStockData();

	qRcvHistoryData* pFirst = list.first();
	qRcvHistoryData* pLast = list.last();
	pData->tmTime = pFirst->time;
	pData->fOpen = pFirst->fOpen;
	pData->fClose = pLast->fClose;


	pData->fLow = pFirst->fLow;
	pData->fHigh = pFirst->fHigh;
	pData->fAmount = 0;
	pData->fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(pData->fLow>p->fLow)
			pData->fLow = p->fLow;
		if(pData->fHigh<p->fHigh)
			pData->fHigh = p->fHigh;
		pData->fAmount+=p->fAmount;
		pData->fVolume+=p->fVolume;
	}
	return pData;
}

CDataEngine* CDataEngine::m_pDataEngine = 0;

time_t CDataEngine::m_tmCurrentDay = 0;
time_t CDataEngine::m_tmCurrent = 0;

CDataEngine* CDataEngine::getDataEngine()
{
	if(m_pDataEngine == NULL)
		m_pDataEngine = new CDataEngine;
	return m_pDataEngine;
}

void CDataEngine::releaseDataEngine()
{
	if(m_pDataEngine)
		delete m_pDataEngine;
}

void CDataEngine::importData()
{
	QString qsDir = qApp->applicationDirPath();
	{
		//����F10 ����
		/*����б������ݣ��ȵ��뱾������*/
		QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
		if(QFile::exists(qsBaseInfo))
		{
			qDebug()<<"Import F10 data...";
			int iCount = importBaseInfo(qsBaseInfo);
			if(iCount<1)
			{
				QFile::remove(qsBaseInfo);
				qDebug()<<"Import F10 data from "<<qsBaseInfo<<" error!";
			}
			else
				qDebug()<<iCount<<" F10 data had been imported.";
		}

		//�����������F10���ݣ������ٴε���������F10 ���ݡ�
		QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
		QString qsF10File = QFileInfo(settings.value("driver").toString()).absolutePath() + "/��������.fin";
		if(QFile::exists(qsF10File))
		{
			qDebug()<<"Import F10 Data from "<<qsF10File;
			CDataEngine::importBaseInfoFromFinFile(qsF10File);	
		}
	}
	{
		//�������һ���Reports����
		QString qsReportFile = QString("%1/data/reports").arg(qsDir);
		qDebug()<<"Import reports data from "<<qsReportFile;
		int iCount = importReportsInfo(qsReportFile);
		if(iCount<1)
		{
			QFile::remove(qsReportFile);
			qDebug()<<"Import reports data from "<<qsReportFile<<" error!";
		}
		else
			qDebug()<<iCount<<" reports data had been imported.";
	}
	{
		//���뵱��ķֱ�����
		QString qsFenBiFile = QString("%1/data/FenBi/%2").arg(qsDir).arg(QDate::currentDate().toString("yyyyMMdd"));
		while(!QFile::exists(qsFenBiFile))
		{
			static int iAddDays = -1;
			qsFenBiFile = QString("%1/data/FenBi/%2").arg(qsDir).arg(QDate::currentDate().addDays(iAddDays).toString("yyyyMMdd"));
			if(iAddDays<-10)			//����10�Ժ��򰴲����ڷֱ����ݴ���
				break;
			--iAddDays;
		}

		qDebug()<<"Import FenBi data from "<<qsFenBiFile;
		int iCount = importFenBisData(qsFenBiFile);
		if(iCount<1)
		{
			qDebug()<<"Import FenBi data from "<<qsFenBiFile<<" error!";
		}
		else
			qDebug()<<iCount<<" FenBi data had been imported.";
	}

	{
		importBlocksData("");
	}
}

void CDataEngine::exportData()
{
	QString qsDir = qApp->applicationDirPath();
	QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
	{
		QDir().mkpath(qsDir+"/data");
		qDebug()<<"Export F10 data...";
		int iCount = exportBaseInfo(qsBaseInfo);
		qDebug()<<iCount<<" F10 data had been exported.";
	}

	{
		//�������һ���Reports����
		QString qsReportDir = qsDir + "/data";
		if(!QDir().exists(qsReportDir))
			QDir().mkpath(qsReportDir);
		QString qsReportFile = QString("%1/reports").arg(qsReportDir);
		qDebug()<<"Export reports data to "<<qsReportFile;
		int iCount = exportReportsInfo(qsReportFile);
		qDebug()<<iCount<<" reports data had been exported.";
	}

	{
		//��������ķֱ�����
		QString qsFenBiDir = QString("%1/data/FenBi")
			.arg(qsDir);
		QDir().mkpath(qsFenBiDir);
		qDebug()<<"Export FenBis data to "<<qsFenBiDir;
		int iCount = exportFenBisData(QString("%1/%2").arg(qsFenBiDir).arg(QDateTime::fromTime_t(m_tmCurrent).toString("yyyyMMdd")));
		qDebug()<<iCount<<" FenBis data had been exported.";
	}
}

int CDataEngine::importBaseInfoFromFinFile( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return 0;

	int iFlag;
	file.read((char*)&iFlag,4);
	int iTotal;
	file.read((char*)&iTotal,4);

	int iCout = 0;

	while(true)
	{
		WORD wMarket;
		if(file.read((char*)&wMarket,2)!=2) break;
		if(!file.seek(file.pos()+2)) break;

		char chCode[STKLABEL_LEN];
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN) break;


		float fVal[38];
		if(file.read((char*)fVal,sizeof(float)*38)!=(sizeof(float)*38)) break;

		qRcvBaseInfoData d(fVal);
		d.wMarket = wMarket;
		memcpy(d.code,chCode,STKLABEL_LEN);

		QString qsCode = QString::fromLocal8Bit(chCode);

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(d);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(d);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCout;
	}

	return iCout;
}

int CDataEngine::importBaseInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream out(&file);

	int iCount = 0;
	while(true)
	{
		qRcvBaseInfoData baseInfo;
		int iSize = out.readRawData((char*)&baseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;


		float fLast5Volume = 0.0;
		if(out.readRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
			break;

		QString qsCode = QString::fromLocal8Bit(baseInfo.code);
		if(qsCode.isEmpty())
			return -1;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importReportsInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream in(&file);

	int iCount = 0;
	while(true)
	{
		qRcvReportData* pReport = new qRcvReportData;
		quint32 _t;
		in>>_t>>pReport->wMarket>>pReport->qsCode>>pReport->qsName;
		pReport->tmTime = _t;

		int iSize = in.readRawData((char*)&pReport->fLastClose,sizeof(float)*27);
		if(iSize!=(sizeof(float)*27))
			break;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(pReport->qsCode);
		if(pItem==NULL)
		{
			pItem = new CStockInfoItem(pReport->qsCode,pReport->wMarket);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}
		pItem->setReport(pReport);

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importFenBisData( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	int iCount = 0;
	while(true)
	{
		char chCode[STKLABEL_LEN];
		int iSize;
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN)
			break;
		if(file.read((char*)&iSize,sizeof(iSize))!=sizeof(iSize))
			break;

		char* pFenBiData = new char[iSize];
		if(file.read(pFenBiData,iSize)!=iSize)
		{
			delete pFenBiData;
			break;
		}

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(QString::fromLocal8Bit(chCode));
		if(!pItem)
		{
			delete pFenBiData;
			continue;
		}

		int iIndex = 0;
		QList<qRcvFenBiData*> listFenBis;
	//	QString qsMin;
		while(iIndex<iSize)
		{
			qRcvFenBiData* pData = new qRcvFenBiData();
			memcpy(pData,&pFenBiData[iIndex],sizeof(qRcvFenBiData));
			listFenBis.push_back(pData);
		//	qsMin+= QString("%1\r\n").arg(QDateTime::fromTime_t(pData->tmTime).toString("hh:mm:ss"));

			iIndex += sizeof(qRcvFenBiData);
		}
		pItem->appendFenBis(listFenBis);
		delete pFenBiData;

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importBlocksData( const QString& /*qsPath*/ )
{
	QDir dir(CDataEngine::getDataEngine()->getStockBlockDir());
	QFileInfoList listEntity = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot,QDir::DirsLast);
	foreach(const QFileInfo& _f,listEntity)
	{
		CBlockInfoItem* pBlock = new CBlockInfoItem(_f.absoluteFilePath());
		pBlock->initChildren();
	}
	return CDataEngine::getDataEngine()->getStockBlocks().size();
}


int CDataEngine::exportBaseInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		qRcvBaseInfoData* pBaseInfo = pItem->getBaseInfo();

		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;
		float fLast5Volume = pItem->getLast5Volume();
		if(out.writeRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
			break;
		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::exportReportsInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		//���浱�����е�Reports
		qRcvReportData* pReport = pItem->getCurrentReport();
		if(pReport&&(!pReport->qsCode.isEmpty()))
		{
			out<<pReport->tmTime<<pReport->wMarket<<pReport->qsCode<<pReport->qsName;
			//ֱ�ӿ���ʣ�������float����
			if(out.writeRawData((char*)&pReport->fLastClose,sizeof(float)*27)!=(sizeof(float)*27))
				break;
			//		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
			//		if(iSize!=sizeof(qRcvBaseInfoData))
			//			break;
			++iCount;
		}
	}

	file.close();
	return iCount;
}

int CDataEngine::exportFenBisData( const QString& qsFile )
{
	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	foreach(CStockInfoItem* pItem,listItem)
	{
		//���浱�����еķ�������
		QList<qRcvFenBiData*> listFenBis= pItem->getFenBiList();
		int iSize = listFenBis.size()*sizeof(qRcvFenBiData);
		char* pFenBiData = new char[iSize];
		for(int i = 0; i<listFenBis.size(); ++i)
		{
			qRcvFenBiData* pData = listFenBis[i];
			memcpy(pFenBiData+i*sizeof(qRcvFenBiData),pData,sizeof(qRcvFenBiData));
		}
		//foreach(qRcvMinuteData* pData,listMins)
		//{
		//	minData.append(pData->tmTime);
		//	minData.append(pData->fPrice);
		//	minData.append(pData->fVolume);
		//	minData.append(pData->fAmount);
		//	minData<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//	out<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//}
		char chCode[STKLABEL_LEN];
		memset(chCode,0,STKLABEL_LEN);
		QByteArray arrCode = pItem->getCode().toLocal8Bit();
		memcpy(chCode,arrCode.data(),arrCode.size());
		file.write(chCode,STKLABEL_LEN);
		file.write((char*)&iSize,sizeof(int));

		file.write(pFenBiData,iSize);
		file.flush();

		delete pFenBiData;

		++iCount;
	}

	file.close();
	return iCount;
}

//���̺����ݵ�����
int CDataEngine::exportCloseData()
{
	QString qsDir = QString("%1/data/5min").arg(qApp->applicationDirPath());
	QDir().mkpath(qsDir);
	QMap<time_t,int> mapTimes = getTodayTimeMap(Min5);

	QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
	foreach(CStockInfoItem* pItem,listStocks)
	{
		//����5min����
		CDataEngine::getDataEngine()->export5MinData(pItem->getCode(),mapTimes);

		//�������report׷��Ϊ��ʷ����
		pItem->appendHistorys(QList<qRcvHistoryData*>()<<new qRcvHistoryData(pItem->getCurrentReport()));
	}

	return -1;
}


bool CDataEngine::isStockOpenDay( time_t tmDay )
{
	QDate tmDate = QDateTime::fromTime_t(tmDay).date();
	int iWeek = tmDate.dayOfWeek();
	if((iWeek == 6)||(iWeek == 7))
		return false;

	return true;
}

bool CDataEngine::isStockOpenTime( time_t tmMin )
{
	QTime tmTime = QDateTime::fromTime_t(tmMin).time();
	uint tmT = tmTime.hour()*60+tmTime.second();

	if(tmT<(9*60+25))
		return false;
	
	if(tmT>11*60+35 && tmT<12*60+55)
		return false;

	if(tmT>15*60+5)
		return false;

	return true;
}

time_t CDataEngine::getOpenSeconds()
{
	return 3600*4;
}

time_t CDataEngine::getOpenSeconds( time_t tmTime )
{
	time_t tmEnd2 = m_tmCurrentDay+3600*15;
	if(tmTime>tmEnd2)
		return 3600*4;

	time_t tmBegin2 = m_tmCurrentDay+3600*13;
	if(tmTime>tmBegin2)
		return 3600*2+(tmTime-tmBegin2);

	time_t tmEnd1 = m_tmCurrentDay+1800*23;
	if(tmTime>tmEnd1)
		return 3600*2;

	time_t tmBegin1 = m_tmCurrentDay + 1800*19;
	if(tmTime>tmBegin1)
		return tmTime-tmBegin1;


	return 0;
}

time_t CDataEngine::getCurrentTime()
{
	if(m_tmCurrent<1)
		return QDateTime::currentDateTime().toTime_t();
	return m_tmCurrent;
}
void CDataEngine::setCurrentTime(const time_t& t)
{
	if(m_tmCurrent<t)
		m_tmCurrent = t;
}

QMap<time_t,int> CDataEngine::getTodayTimeMap( RStockCircle _c )
{
//	int iCount = 1024;				//����1024��ʱ��
	QMap<time_t,int> mapTimes;
	if(AutoCircle == _c)
	{
		/*
		2���ӵ�10s���ڣ�6*2=12��
		10���ӵ�1m���ڣ�10��
		����Ϊ5m����
		*/
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9��25����
		time_t tmCurrent = (tmCur+Sec10)/Sec10*Sec10;//���϶Է���ȡ��
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);

		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + Sec10;		//3������(���һ������)
		}

		/*�����Ϻ����¶����һ������*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmTmp = tmCurrent;
			if((tmTmp-tmNoon2)>=120)
			{
				//10s����
				getTimeMapBySec(mapTimes,tmTmp-120,tmTmp,Sec10);
				tmTmp = tmTmp - 120;
				if((tmTmp-tmNoon2)>=600)
				{
					//1min����
					getTimeMapBySec(mapTimes,tmTmp-600,tmTmp,Min1);
					tmTmp = tmTmp - 600;
					//5min����
					getTimeMapBySec(mapTimes,tmNoon2-Min5,tmTmp,Min5);
					getTimeMapBySec(mapTimes,tmLast-Min5,tmNoon1+Min5,Min5);
				}
				else
				{
					//1min����
					getTimeMapBySec(mapTimes,tmNoon2-Min1,tmTmp,Min1);
					//5min����
					getTimeMapBySec(mapTimes,tmLast-Min5,tmNoon1+Min5,Min5);
				}
			}
			else
			{
				//10s����
				getTimeMapBySec(mapTimes,tmNoon2-Sec10,tmTmp,Sec10);
				//1min����
				getTimeMapBySec(mapTimes,tmNoon1-540,tmNoon1+Min1,Min1);
				//5min����
				getTimeMapBySec(mapTimes,tmLast-Min5,tmNoon1-540,Min5);

			}
		}
		else
		{
			time_t tmTmp = tmCurrent;
			if(tmCurrent>tmNoon1)
				tmTmp = tmNoon1+Sec10;
			//10s����
			if((tmTmp-tmLast)>120)
				getTimeMapBySec(mapTimes,tmTmp-120,tmTmp,Sec10);
			else
				getTimeMapBySec(mapTimes,tmLast,tmTmp,Sec10);
			tmTmp = tmTmp-120;
			//1min����
			if((tmTmp-tmLast)>600)
				getTimeMapBySec(mapTimes,tmTmp-600,tmTmp,Min1);
			else
				getTimeMapBySec(mapTimes,tmLast,tmTmp,Min1);
			tmTmp = tmTmp-600;
			//5min����
			getTimeMapBySec(mapTimes,tmLast,tmTmp,Min5);
		}
	}
	else if(_c<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9��25����
		time_t tmCurrent = (tmCur+_c*2)/_c*_c;//���϶Է���ȡ��
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);


		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + _c;		//3������(���һ������)
		}
		/*�����Ϻ����¶����һ������*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-_c;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapBySec(mapTimes,tmBegin,tmCurrent,_c);
		}

		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-_c;
			time_t tmEnd = tmNoon1+_c;
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,_c);
		}
		else if(tmCurrent>tmLast)
		{
			time_t tmBegin = tmLast-_c;
			time_t tmEnd = tmCurrent;
			getTimeMapBySec(mapTimes,tmBegin,tmEnd,_c);
		}
	}
	else
	{
		time_t tmBegin = QDateTime(QDate(2000,1,1)).toTime_t();
		time_t tmEnd = (CDataEngine::getCurrentTime()/(3600*24))*3600*24 - 8*3600;

		if(_c == Day)
		{
			getDayMapByHistory(mapTimes,tmBegin,tmEnd);
		}
		else if(_c == DayN)
		{
			//Ŀǰδʹ��
		}
		else if(_c == Week)
		{
			getWeekMapByHistory(mapTimes,tmBegin,tmEnd);
		}
		else if(_c == Month)
		{
			getMonthMapByHistory(mapTimes,tmBegin,tmEnd);
		}
		else if(_c == Month3)
		{
			getMonth3MapByHistory(mapTimes,tmBegin,tmEnd);
		}
		else if(_c == Year)
		{
			getYearMapByHistory(mapTimes,tmBegin,tmEnd);
		}
	}

	return mapTimes;
}

QMap<time_t,RStockData*>* CDataEngine::getColorBlockItems( const QMap<time_t,int>& mapTimes, const QList<qRcvFenBiData*>& minutes )
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
	{
		return pMap;
	}

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvFenBiData*> listFenBi;
		foreach(qRcvFenBiData* pFenBi,minutes)
		{
			if(pFenBi->tmTime>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByMins(listFenBi,NULL));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvFenBiData*> listPer;
	qRcvFenBiData* pLastFenBi = NULL;
	foreach(qRcvFenBiData* pFenBi,minutes)
	{
		if(pFenBi->tmTime<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
		{
			listPer.push_back(pFenBi);
		}
		else
		{
			pMap->insert(tmBegin,getColorBlockItemByMins(listPer,pLastFenBi));
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//��1000���ʱ��

				if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//��ȡ���һ�����ݿ飬�����м���
	RStockData* pLastData = getColorBlockItemByMins(listPer,pLastFenBi);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"FenBi Map is not enouph";
	}

	return pMap;
}

QMap<time_t,RStockData*>* CDataEngine::getColorBlockItems( const QMap<time_t,int>& mapTimes, const QList<qRcvHistoryData*>& minutes )
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
		return pMap;

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvHistoryData*> listFenBi;
		foreach(qRcvHistoryData* pFenBi,minutes)
		{
			if(pFenBi->time>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByDays(listFenBi));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvHistoryData*> listPer;
	qRcvHistoryData* pLastFenBi = NULL;
	foreach(qRcvHistoryData* pFenBi,minutes)
	{
		if(pFenBi->time<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
			listPer.push_back(pFenBi);
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByDays(listPer));
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//��1000���ʱ��

				if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//��ȡ���һ�����ݿ飬�����м���
	RStockData* pLastData = getColorBlockItemByDays(listPer);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"Day Map is not enouph";
	}
	return pMap;
}


CDataEngine::CDataEngine(void)
{
	m_qsHistroyDir = qApp->applicationDirPath()+"/data/history/";
	QDir().mkpath(m_qsHistroyDir);

	m_qsBlocksDir = qApp->applicationDirPath()+"/config/blocks/";
	QDir().mkpath(m_qsBlocksDir);

	m_qsNewsDir = qApp->applicationDirPath()+"/data/news/";
	QDir().mkpath(m_qsNewsDir);

	m_qsF10Dir = qApp->applicationDirPath()+"/data/F10/";
	QDir().mkpath(m_qsF10Dir);

	m_qs5Min = qApp->applicationDirPath()+"/data/5min/";
	QDir().mkpath(m_qs5Min);

	m_qsFenBiDir = qApp->applicationDirPath()+"/data/FenBi/";
	QDir().mkpath(m_qsFenBiDir);

	//���뺺��->ƴ�����ձ�
	CHz2Py::initHz2PyTable(qApp->applicationDirPath()+"/config/PY.hz");
}

CDataEngine::~CDataEngine(void)
{
#ifndef _DEBUG
	QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
	while(iter!=m_mapStockInfos.end())
	{
		delete iter.value();
		++iter;
	}
#endif // _DEBUG
	m_mapStockInfos.clear();
}

QList<CBlockInfoItem*> CDataEngine::getTopLevelBlocks()
{
	return m_listTopLevelBlocks;
}

QList<CBlockInfoItem*> CDataEngine::getStockBlocks()
{
	return m_mapBlockInfos.values();
}

CBlockInfoItem* CDataEngine::getStockBlock( const QString& qsCode )
{
	if(m_mapBlockInfos.contains(qsCode))
	{
		return m_mapBlockInfos[qsCode];
	}
	return NULL;
}

bool CDataEngine::isHadBlock( const QString& block )
{
	return m_mapBlockInfos.contains(block);
}

void CDataEngine::setBlockInfoItem( CBlockInfoItem* _p )
{
	if(_p->getAbsPath()==_p->getName())
		m_listTopLevelBlocks.push_back(_p);
	m_mapBlockInfos[_p->getCode()] = _p;
}


QList<CStockInfoItem*> CDataEngine::getStocksByMarket( WORD wMarket )
{
	QList<CStockInfoItem*> listStocks;
	QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
	while(iter!=m_mapStockInfos.end())
	{
		if((*iter)->getMarket() == wMarket)
			listStocks.push_back(iter.value());
		++iter;
	}

	return listStocks;
}

QList<CStockInfoItem*> CDataEngine::getStockInfoList()
{
	return m_mapStockInfos.values();
}

CStockInfoItem* CDataEngine::getStockInfoItem( const QString& qsCode )
{
	if(m_mapStockInfos.contains(qsCode))
	{
		return m_mapStockInfos[qsCode];
	}
	return NULL;
}

void CDataEngine::setStockInfoItem( CStockInfoItem* p )
{
	m_mapStockInfos[p->getCode()] = p;
}


CAbstractStockItem* CDataEngine::getStockItem(const QString& qsCode)
{
	if(m_mapStockInfos.contains(qsCode))
	{
		return m_mapStockInfos[qsCode];
	}

	if(m_mapBlockInfos.contains(qsCode))
	{
		return m_mapBlockInfos[qsCode];
	}

	return NULL;
}


void CDataEngine::appendNews( const QString& title, const QString& content )
{
	QString qsNewsPath = m_qsNewsDir + title;

	QDir().mkpath(QFileInfo(qsNewsPath).absolutePath());
	QFile file(qsNewsPath);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}

void CDataEngine::appendF10( const QString& title, const QString& content )
{
	QString qsF10Path = m_qsF10Dir + title;

	QDir().mkpath(QFileInfo(qsF10Path).absolutePath());
	QFile file(qsF10Path);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}

bool CDataEngine::showF10Data( const QString& code )
{
	/*��ʾF10����*/
	QString qsF10Path = QString("%1%2.txt").arg(m_qsF10Dir).arg(code);
	if(QFile::exists(qsF10Path))
	{
		return QProcess::startDetached("notepad.exe",QStringList()<<qsF10Path);
	}
	return false;
}

bool CDataEngine::exportHistoryData( const QString& qsCode, const QList<qRcvHistoryData*>& list )
{
	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(qsCode);

	QFile file(qsDayData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvHistoryData* pData, list)
	{
		int iSize = out.writeRawData((char*)pData,sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

bool CDataEngine::exportHistoryData( const QString& qsCode, const QList<qRcvHistoryData*>& list, int iOffset )
{
	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(qsCode);

	QFile file(qsDayData);
	if(!file.open(QFile::ReadWrite))
		return false;

	int iPos = 0;
	if(iOffset>=0)
	{
		iPos = file.size()-sizeof(qRcvHistoryData)*iOffset;
	}
	if(iPos<0)
	{
		iPos = 0;
	}

	if(!(file.resize(iPos))||(!file.seek(iPos)))
		return false;

	foreach(qRcvHistoryData* pData, list)
	{
		int iSize = file.write((char*)pData,sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( const QString& code )
{
	QList<qRcvHistoryData*> list;

	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(code);
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;

	QDataStream inStream(&file);
	while(!inStream.atEnd())
	{
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = inStream.readRawData(reinterpret_cast<char*>(pData),sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			delete pData;
			break;
		}
		list.push_back(pData);
	}

	file.close();
	return list;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( const QString& code, int count )
{
	QList<qRcvHistoryData*> list;

	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(code);
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;
	int iDataSize = sizeof(qRcvHistoryData);

	int iPos = file.size()-iDataSize*count;
	if(iPos<0)
		iPos = 0;
	file.seek(iPos);

	while(true)
	{
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = file.read(reinterpret_cast<char*>(pData),iDataSize);
		if(iSize!=iDataSize)
		{
			delete pData;
			break;
		}
		list.push_back(pData);
	}

	file.close();
	return list;
}

bool CDataEngine::export5MinData( const QString& qsCode, const QMap<time_t,int>& mapTimes )
{
	//����5�������ݣ����ڷǽ��յ�����ֻ��5minΪ��С��λ�洢
	//60*sizeof(RStockData)
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
	if(!pItem)
		return false;

	int iSizeOfStruct = sizeof(RStockData);
	int iSizeOfTime = sizeof(time_t);
	int iCountOfDay = 60;			//�涨ÿ�����Ϊ60��5�������ݿ�
	int iSizeOfDay = iSizeOfStruct*iCountOfDay;
	int iTotalCount = 10;
	int iOffsetBegin = 0;
	qRcvReportData* pReport = pItem->getCurrentReport();
	if(pReport->tmTime<1)
		return false;

	time_t tmDate = QDateTime(QDateTime::fromTime_t(pReport->tmTime).date()).toTime_t();


	QString qsFileName = QString("%1%2").arg(m_qs5Min).arg(pItem->getCode());
	QFile file(qsFileName);
	if(!file.open(QFile::ReadWrite))
	{
		qDebug()<<"Open file\""<<qsFileName<<"\" error!";
		return false;
	}

	QDataStream out(&file);
	int iHeaderSize = iTotalCount*iSizeOfTime;

	//��ȡ�������ݵĴ洢λ��
	if(file.size()<iHeaderSize)
	{
		file.seek(0);
		out.writeRawData((char*)&tmDate,iSizeOfTime);
		for (int i=1;i<iTotalCount;++i)
		{
			out<<(time_t)0;
		}
		iOffsetBegin = iHeaderSize;
	}
	else
	{
		file.seek(0);
		QMap<time_t,int> mapDates;
		for (int i=0;i<iTotalCount;++i)
		{
			time_t tmpDate = 0;
			out.readRawData((char*)(&tmpDate),iSizeOfTime);
			if(tmpDate==0)
				break;
			mapDates[tmpDate] = i;
		}

		if(mapDates.contains(tmDate))
		{
			iOffsetBegin = iHeaderSize+mapDates[tmDate]*iSizeOfDay;
		}
		else
		{
			int iIndexOfLast = mapDates.value(mapDates.keys().last());
			if(iIndexOfLast<0)
			{
				qDebug()<<"Can not export 5min data";
				file.close();
				return false;
			}
			else
			{
				iIndexOfLast = (iIndexOfLast+1)%iTotalCount;
				file.seek(iIndexOfLast*iSizeOfTime);
				out.writeRawData((char*)&tmDate,iSizeOfTime);
				iOffsetBegin = iHeaderSize+iIndexOfLast*iSizeOfDay;
			}
		}
	}


	file.seek(iOffsetBegin);
	QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();
	QMap<time_t,RStockData*>* pMap = CDataEngine::getColorBlockItems(mapTimes,FenBis);
	if(!pMap)
	{
		qDebug()<<"Can not get 5min data";
		file.close();
		return false;
	}


	QMap<time_t,RStockData*>::iterator iter = pMap->begin();
	int iCount = 0;
	while(iter!=pMap->end())
	{
		RStockData* pData = iter.value();
		if(pData)
		{
			out.writeRawData((char*)pData,iSizeOfStruct);
			delete pData;
			++iCount;
		}
		++iter;
	}
	RStockData dataNull;
	while(iCount<iCountOfDay)
	{
		out.writeRawData((char*)&dataNull,iSizeOfStruct);
		++iCount;
	}

	delete pMap;
	file.close();
	return true;
}

QMap<time_t,RStockData*>* CDataEngine::get5MinData( const QString& qsCode )
{
	QMap<time_t,RStockData*>* pMapData = new QMap<time_t,RStockData*>;
	QString qsFileName = QString("%1%2").arg(m_qs5Min).arg(qsCode);
	if(!QFile::exists(qsFileName))
		return pMapData;

	QFile file(qsFileName);
	if(!file.open(QFile::ReadOnly))
	{
		qDebug()<<"Open file\""<<qsFileName<<"\" error!";
		return pMapData;
	}

	int iSizeOfStruct = sizeof(RStockData);
	int iSizeOfTime = sizeof(time_t);
	int iCountOfDay = 60;			//�涨ÿ�����Ϊ60��5�������ݿ�
	int iSizeOfDay = iSizeOfStruct*iCountOfDay;
	int iTotalCount = 10;

	QDataStream out(&file);
	int iHeaderSize = iTotalCount*iSizeOfTime;
	//��ȡ�������ݵĴ洢λ��
	if(file.size()<iHeaderSize)
		return pMapData;
	int iPos = 0;
	time_t tmTmp = 0;
	while(iPos<iTotalCount)
	{
		file.seek(iPos*iSizeOfTime);
		if(out.readRawData((char*)(&tmTmp),iSizeOfTime)!=iSizeOfTime)
			break;
		if(tmTmp!=0 && iPos<iTotalCount)
		{
			file.seek(iHeaderSize+iSizeOfDay*iPos);
			int iIndex = 0;
			while(iIndex<60)
			{
				RStockData* pData = new RStockData();
				if(out.readRawData((char*)pData,iSizeOfStruct) == iSizeOfStruct)
				{
					if(pData->tmTime == 0)
					{
						delete pData;
						break;
					}
					if(!pMapData->contains(pData->tmTime))
					{
						pMapData->insert(pData->tmTime,pData);
					}
					else
						delete pData;
				}
				else
				{
					delete pData;
					break;
				}
			}
		}
		++iPos;
	}

	file.close();
	return pMapData;
}


bool CDataEngine::exportFenBiData( const QString& qsCode, const long& lDate, const QList<qRcvFenBiData*>& list )
{
	QString qsPath = QString("%1%2/").arg(m_qsFenBiDir).arg(lDate);
	QDir().mkpath(qsPath);
	QString qsFenBiData = QString("%1%2").arg(qsPath).arg(qsCode);

	QFile file(qsFenBiData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvFenBiData* pData, list)
	{
		//int iSize = file.write((char*)pData,sizeof(qRcvFenBiData));
		//if(iSize!=sizeof(qRcvFenBiData))
		//{
		//	file.close();
		//	return false;
		//}
		QString qsLine = QString("time:%1	Price:%2	Buy1:%3	Sell1:%4\r\n")
			.arg(pData->tmTime).arg(pData->fPrice)
			.arg(pData->fBuyPrice[0]).arg(pData->fSellPrice[1]);
		file.write(qsLine.toLocal8Bit());
	}

	file.close();
	return true;
}

