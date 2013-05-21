/************************************************************************/
/* �ļ����ƣ�BlockInfoItem.cpp
/* ����ʱ�䣺2013-01-08 09:28
/*
/* ��    �������ڹ���������
/*
/* ���¼������ʱ��Ϊ1���Ӹ���һ��
/* ���㷽ʽ��ȡǰʮֻ��Ʊ���м�Ȩƽ��
/************************************************************************/
#include "StdAfx.h"
#include "BlockInfoItem.h"
#include "DataEngine.h"
#include "BlockCodeManager.h"

CBlockInfoItem::CBlockInfoItem( const QString& _file,const QString& _parent )
	: bUpdateMin(false)
	, bUpdateDay(false)
	, fNowVolume(FLOAT_NAN)
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
	, fLastClose(FLOAT_NAN)
	, fOpenPrice(FLOAT_NAN)
	, fNewPrice(FLOAT_NAN)
	, fLowPrice(FLOAT_NAN)
	, fHighPrice(FLOAT_NAN)
{
	QFileInfo _info(_file);
	if(!_info.exists())
		return;

	parentName = _parent;
	blockFilePath = _file;
	blockName = _info.baseName();

	if(_info.isDir())
	{
		QDir dir(blockFilePath);
		QFileInfoList listEntity = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(const QFileInfo& _f,listEntity)
		{
			if(parentName.isEmpty())
				appendBlock(new CBlockInfoItem(_f.absoluteFilePath(),blockName));
			else
				appendBlock(new CBlockInfoItem(_f.absoluteFilePath(),parentName+"|"+blockName));
		}
	}
	else
	{
		QFile file(blockFilePath);
		if(file.open(QFile::ReadOnly))
		{
			QString qsType = file.readLine().trimmed();
			if(qsType == "RegExp")
			{
				QString qsExp = file.readLine().trimmed();
				if(!qsExp.isEmpty())
				{
					QRegExp _exp(qsExp);
					_exp.setPatternSyntax(QRegExp::Wildcard);
					QList<CStockInfoItem*> listStocks = CDataEngine::getDataEngine()->getStockInfoList();
					foreach(CStockInfoItem* p,listStocks)
					{
						if(_exp.exactMatch(p->getCode()))
						{
							addStock(p);
						}
					}
				}
			}
			else
			{
				file.seek(0);
				while(!file.atEnd())
				{
					QString code = file.readLine();
					code = code.trimmed();
					if(!code.isEmpty())
					{
						addStock(CDataEngine::getDataEngine()->getStockInfoItem(code));
					}
				}
			}
			file.close();
		}
	}


	/*���ð�����*/
	QString qsAbsPath = "";
	if(parentName.isEmpty())
		qsAbsPath = blockName;
	else
		qsAbsPath = parentName+"|"+blockName;
	blockCode = CBlockCodeManager::getBlockCode(qsAbsPath);

	connect(&timerUpdate,SIGNAL(timeout()),this,SLOT(updateData()));
	timerUpdate.start(1*1000);
}

CBlockInfoItem::~CBlockInfoItem(void)
{
	clearTmpData();
}

QString CBlockInfoItem::getAbsPath()
{
	if(parentName.isEmpty())
		return blockName;
	return parentName+"|"+blockName;
}

CBlockInfoItem* CBlockInfoItem::querySubBlock( const QStringList& _parent )
{
	QStringList listBlocks = _parent;
	if(listBlocks.size()<0)
		return 0;

	if(blocksInBlock.contains(listBlocks[0]))
	{
		CBlockInfoItem* pBlockItem = blocksInBlock[listBlocks[0]];
		if(listBlocks.size()==1)
			return pBlockItem;
		else
		{
			listBlocks.removeAt(0);
			return pBlockItem->querySubBlock(listBlocks);
		}
	}
	return 0;
}

bool CBlockInfoItem::isChildOf( CBlockInfoItem* parent)
{
	return getAbsPath().indexOf(parent->getAbsPath())>-1;
}

QList<CAbstractStockItem*> CBlockInfoItem::getAbsStockList()
{
	QList<CAbstractStockItem*> list;
	foreach(CBlockInfoItem* _p,blocksInBlock)
		list.push_back(_p);
	foreach(CStockInfoItem* _p,stocksInBlock)
		list.push_back(_p);

	return list;
}

QList<CStockInfoItem*> CBlockInfoItem::getStockList()
{
	return stocksInBlock;
}

QList<CBlockInfoItem*> CBlockInfoItem::getBlockList()
{
	return blocksInBlock.values();
}

bool CBlockInfoItem::hasBlocks()
{
	return blocksInBlock.size()>0 ? true : false;
}

bool CBlockInfoItem::appendStocks( QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getCode());

	if(appendStocks(listCodes))
	{
		foreach(CStockInfoItem* pItem,list)
		{
			if(!stocksInBlock.contains(pItem))
				stocksInBlock.push_back(pItem);
		}
		return true;
	}
	else
		return false;
}

bool CBlockInfoItem::appendStocks( QList<QString> list )
{
	if(blockFilePath.isEmpty())
		return false;

	QFile file(blockFilePath);
	if(!file.open(QFile::Append|QFile::WriteOnly))
		return false;
	foreach(const QString& e,list)
	{
		file.write(QString(e+"\r\n").toAscii());
	}
	file.close();

	return true;
}

bool CBlockInfoItem::removeStocks( QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getCode());

	if(removeStocks(listCodes))
	{
		foreach(CStockInfoItem* pItem,list)
		{
			stocksInBlock.removeOne(pItem);
		}
		return true;
	}
	else
		return false;
}

bool CBlockInfoItem::removeStocks( QList<QString> list )
{
	if(blockFilePath.isEmpty())
		return false;

	QMap<QString,QString> mapStocks;
	{
		//��ȡ�ļ��еĹ�Ʊ����
		QFile file(blockFilePath);
		if(!file.open(QFile::ReadOnly))
			return false;

		while(!file.atEnd())
		{
			QString code = file.readLine();
			code = code.trimmed();
			if(!code.isEmpty())
			{
				mapStocks[code] = code;
			}
		}
		file.close();
	}
	{
		//ɾ����Ӧ�Ĺ�Ʊ����
		foreach(const QString& e,list)
		{
			mapStocks.remove(e);
		}
	}
	{
		//����д��ȥ
		QFile file(blockFilePath);
		if(!file.open(QFile::Truncate|QFile::WriteOnly))
			return false;

		QMap<QString,QString>::iterator iter = mapStocks.begin();
		while(iter!=mapStocks.end())
		{
			file.write(QString(iter.value()+"\r\n").toAscii());
			++iter;
		}
		file.close();
	}

	return true;
}

bool CBlockInfoItem::appendBlock( CBlockInfoItem* pBlock )
{
	blocksInBlock[pBlock->getBlockName()] = pBlock;
	return true;
}

void CBlockInfoItem::stockFenbiChanged( const QString& _code )
{
	bUpdateMin = true;
}

void CBlockInfoItem::stockHistoryChanged( const QString& _code )
{
	bUpdateDay = true;
}

void CBlockInfoItem::updateData()
{
	/*
	���¼������ʱ��Ϊ1���Ӹ���һ��
	���㷽ʽ��ȡǰʮֻ��Ʊ���м�Ȩƽ��
	*/
	if(bUpdateMin)
	{
//		double dTotalGB = 0.0;		//�ܹɣ���
		double dLTG = 0.0;			//��ͨ�ɣ���
		double dLastClose = 0.0;	//�������̼�
		double dOpen = 0.0;			//���տ��̼�
		double dNew = 0.0;			//���¼�
		double dLow = 0.0;			//��ͼ�
		double dHigh = 0.0;			//��߼�
		int iCount = stocksInBlock.size();
		if(iCount>10)
			iCount = 10;
		for(int i=0;i<iCount;++i)
		{
			CStockInfoItem* pStock = stocksInBlock[i];
			float fLTAG = pStock->getBaseInfo()->fLtAg;		//��ͨ��
//			dTotalGB += pStock->getBaseInfo()->fZgb;
			dLTG += fLTAG;
			dLastClose += pStock->getLastClose()*fLTAG;
			dOpen += pStock->getOpenPrice()*fLTAG;
			dNew += pStock->getNewPrice()*fLTAG;
			dLow += pStock->getLowPrice()*fLTAG;
			dHigh += pStock->getHighPrice()*fLTAG;
		}

		fLastClose = dLastClose/dLTG;
		fOpenPrice = dOpen/dLTG;
		fNewPrice = dNew/dLTG;
		fLowPrice = dLow/dLTG;
		fHighPrice = dHigh/dLTG;


		//�Ƿ�
		if(fNewPrice>0.0 && fLastClose>0.0)
			fIncrease = (fNewPrice-fLastClose)*100.0/fLastClose;
		bUpdateMin = false;
	}
	if(bUpdateDay)
	{

		bUpdateDay = false;
	}
}

void CBlockInfoItem::addStock( CStockInfoItem* _p )
{
	if(!_p)
		return;

	if(!stocksInBlock.contains(_p))
	{
		stocksInBlock.push_back(_p);
		connect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
		connect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
	}
}

void CBlockInfoItem::removeStock( CStockInfoItem* _p )
{
	if(!_p)
		return;
	stocksInBlock.removeOne(_p);
	disconnect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
	disconnect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
}

void CBlockInfoItem::clearTmpData()
{
	foreach(CStockInfoItem* _p,stocksInBlock)
	{
		disconnect(_p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(stockHistoryChanged(const QString&)));
		disconnect(_p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(stockFenbiChanged(const QString&)));
	}
	stocksInBlock.clear();
}

QString CBlockInfoItem::getCode() const
{
	return blockCode;
}

WORD CBlockInfoItem::getMarket() const
{
	return BB_MARKET_EX;
}

QString CBlockInfoItem::getName() const
{
	return blockName;
}

float CBlockInfoItem::getIncrease() const
{
	//����
	return fIncrease;
}

float CBlockInfoItem::getVolumeRatio() const
{
	//
	return 0.0;
}

float CBlockInfoItem::getTurnRatio() const
{

	return 0.0;
}

float CBlockInfoItem::getLastClose() const
{

	return fLastClose;
}

float CBlockInfoItem::getOpenPrice() const
{

	return fOpenPrice;
}

float CBlockInfoItem::getHighPrice() const
{

	return fHighPrice;
}

float CBlockInfoItem::getLowPrice() const
{

	return fLowPrice;
}

float CBlockInfoItem::getNewPrice() const
{

	return fNewPrice;
}

float CBlockInfoItem::getTotalVolume() const
{

	return 0.0;
}

float CBlockInfoItem::getTotalAmount() const
{

	return 0.0;
}

float CBlockInfoItem::getNowVolume() const
{

	return 0.0;
}

float CBlockInfoItem::getIncSpeed() const
{

	return 0.0;
}

float CBlockInfoItem::getPriceFluctuate() const
{

	return 0.0;
}

float CBlockInfoItem::getAmplitude() const
{

	return 0.0;
}

float CBlockInfoItem::getAvePrice() const
{

	return 0.0;
}

float CBlockInfoItem::getPERatio() const
{

	return 0.0;
}

float CBlockInfoItem::getLTSZ() const
{

	return 0.0;
}

float CBlockInfoItem::getZGB() const
{

	return 0.0;
}

float CBlockInfoItem::getSellVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getBuyVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getBIDVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getASKVOL() const
{

	return 0.0;
}

float CBlockInfoItem::getCommRatio() const
{

	return 0.0;
}

float CBlockInfoItem::getCommSent() const
{

	return 0.0;
}

float CBlockInfoItem::getLast5Volume()
{

	return 0.0;
}

float CBlockInfoItem::getMgjzc()
{

	return 0.0;
}

float CBlockInfoItem::getMgsy()
{

	return 0.0;
}

bool CBlockInfoItem::isMatch( const QString& _key )
{
	return true;
}
