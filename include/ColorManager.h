/************************************************************************/
/* �ļ����ƣ�ColorManager.h
/* ����ʱ�䣺2012-11-30 14:38
/*
/* ��    ������ɫ������
/*           ���ڸ��ֳ��õ���ɫ����
/************************************************************************/

#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H
#include <QtCore>
#include "rsd_global.h"

#define	COLOR_BLOCK_SIZE	21

inline uint rRGB(int r, int g, int b)// set RGB value
{ return (0xffu << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }

class RSDSHARED_EXPORT CColorManager
{
public:
	static void initAll();
	static void reloadBlockColors();		//���¼���ɫ����ɫ��
private:
	static void initCommonColor();
	static void initBlockColors();

public:
	/*��ȡ��ǰ��֧�ֵ�ɫ����ɫ�б�*/
	static QStringList getBlockColorList();
	/*ͨ��ֵfVal����ȡ����mode������Ӧ��ֵ*/
	static uint getBlockColor(const QString& mode,float fVal);
	/*ͨ������ֵ����������Ϊmode�Ķ�Ӧ��ɫֵ*/
	static uint getBlockColor(const QString& mode,int index);
	//��ȡ��ɫ��
	static bool getBlockColor(const QString& mode,QVector<uint>& vColors);

public:
	/*��ȡ���õ���ɫ��ͨ������ֵ*/
	static uint getCommonColor(int index);

private:
	static QVector<uint> DefaultColor;	//Ĭ��ɫ���
	static QMap<QString,QVector<uint>> BlockColors;	//ɫ����ɫ���ࡣ��ͨ���ٷֱ�������ȡ��ɫ����������
	static QVector<uint> CommonColor;		//���õ���ɫ��
};


#endif	//COLOR_MANAGER_H