/************************************************************************/
/* �ļ����ƣ�AbstractStockItem.h
/* ����ʱ�䣺2013-05-20 14:14
/*
/* ��    ������Ʊ���ݵĴ������
/************************************************************************/
#ifndef ABSTRACT_STOCK_ITEM_H
#define ABSTRACT_STOCK_ITEM_H
#include <QtCore>
#include "rsd_global.h"

class RSDSHARED_EXPORT CAbstractStockItem : public QObject
{
	Q_OBJECT
public:
	CAbstractStockItem(void);
	~CAbstractStockItem(void);

public:
	/*�������ֶΣ�ֻ��*/
	virtual QString getCode() const = 0;		//��Ʊ����
	virtual WORD getMarket() const = 0;			//��Ʊ�г�
	virtual QString getName() const = 0;		//��Ʊ����
	virtual float getIncrease() const = 0;		//�Ƿ�
	virtual float getVolumeRatio() const = 0;	//����
	virtual float getTurnRatio() const = 0;		//������
	virtual float getLastClose() const = 0;		//��һ�����̼�
	virtual float getOpenPrice() const = 0;		//���տ��̼�
	virtual float getHighPrice() const = 0;		//������߼�
	virtual float getLowPrice() const = 0;		//������ͼ�
	virtual float getNewPrice() const = 0;		//���¼�
	virtual float getTotalVolume() const = 0;	//��ȡ����
	virtual float getTotalAmount() const = 0;	//��ȡ����
	virtual float getNowVolume() const = 0;		//��ȡ����
	virtual float getIncSpeed() const = 0;		//����
	virtual float getPriceFluctuate() const = 0;//�ǵ����۸񲨶�
	virtual float getAmplitude() const = 0;		//���
	virtual float getAvePrice() const = 0;		//����
	virtual float getPERatio() const = 0;		//��ӯ��
	virtual float getLTSZ() const = 0;			//��ͨ��ֵ
	virtual float getZGB() const = 0;			//�ܹɱ�
	virtual float getSellVOL() const = 0;		//������
	virtual float getBuyVOL() const = 0;		//������
	virtual float getBIDVOL() const = 0;		//ί����
	virtual float getASKVOL() const = 0;		//ί����
	virtual float getCommRatio() const = 0;		//ί��
	virtual float getCommSent() const = 0;		//ί��

	virtual float getLast5Volume() = 0;			//��ȡ��ȥ5�յĳɽ��������ڼ������ȣ�

	virtual float getMgjzc() = 0;				//ÿ�ɾ��ʲ�
	virtual float getMgsy() = 0;				//��ȡÿ������


	/*�жϹؼ���_key�Ƿ�ƥ��*/
	virtual bool isMatch(const QString& _key) = 0;

	bool isInstanceOfStock();
	bool isInstanceOfBlock();
signals:
	void stockItemReportChanged(const QString&);	//�������ݸ���
	void stockItemHistoryChanged(const QString&);	//��ʷ���ݸ���
	void stockItemFenBiChanged(const QString&);		//�ֱ����ݸ���
};

#endif //ABSTRACT_STOCK_ITEM_H