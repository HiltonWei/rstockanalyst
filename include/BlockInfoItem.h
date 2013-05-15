/************************************************************************/
/* �ļ����ƣ�BlockInfoItem.h
/* ����ʱ�䣺2013-01-08 09:28
/*
/* ��    �������ڹ���������
/************************************************************************/
#ifndef	BLOCK_INFO_ITEM_H
#define BLOCK_INFO_ITEM_H
#include <QtCore>
#include "StockInfoItem.h"
#include "rsd_global.h"

class RSDSHARED_EXPORT CBlockInfoItem : public CStockInfoItem
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _file,const QString& _parent);						//���캯����ֻ�������ƣ�Ȼ��ͨ������ȥ��ȡ�ļ��е��б�
	~CBlockInfoItem(void);

public:
	QString getBlockName() const{ return blockName; }			//��ȡ�������
	QString getAbsPath();										//��ȡ���ľ���·��
	QList<CStockInfoItem*> getStockList();						//��ȡ��Ʊ�б�
	QList<CBlockInfoItem*> getBlockList();						//��ȡ����б�

	bool hasBlocks();											//���ظð���Ƿ����Ӱ��

	bool appendStocks(QList<CStockInfoItem*> list);		//��������ӹ�Ʊ
	bool removeStocks(QList<CStockInfoItem*> list);		//�Ӱ����ɾ����Ʊ

	CBlockInfoItem* querySubBlock(const QStringList& _parent);	//�����Ӱ��

private:
	bool appendStocks(QList<QString> list);				//��������ӹ�Ʊ
	bool removeStocks(QList<QString> list);				//�Ӱ����ɾ����Ʊ
	bool appendBlock(CBlockInfoItem* pBlock);			//��Ӱ��


protected slots:
	void stockFenbiChanged(const QString& _code);	//ĳֻ��Ʊ�ķֱ����ݷ����ı�
	void stockHistoryChanged(const QString& _code);	//ĳֻ��Ʊ����ʷ���ݷ����ı�

private:
	void addStock(CStockInfoItem* p);				//��ӹ�Ʊ
	void removeStock(CStockInfoItem* p);			//ɾ����Ʊ
	void clearTmpData();							//������ʱ�ڴ档

private:
	QString parentName;							//���ĸ����·��
	QString blockName;							//�������
	QString blockFilePath;						//������ݵĴ洢·��
	QList<CStockInfoItem*> stocksInBlock;		//�ð���µĹ�Ʊ��Ϣ
	QMap<QString,CBlockInfoItem*> blocksInBlock;		//�ð���µ��Ӱ��
};

#endif	//BLOCK_INFO_ITEM_H