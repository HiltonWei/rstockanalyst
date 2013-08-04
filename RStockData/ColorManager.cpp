#include "StdAfx.h"
#include "ColorManager.h"

QVector<uint> CColorManager::CommonColor;

CColorItem* CColorManager::m_pDefaultColor = NULL;
QMap<QString,CColorItem*> CColorManager::m_mapColors;


CColorItem::CColorItem( const QVector<uint>& vClrs )
	: m_vColors(vClrs)
{

}

CColorItem::~CColorItem()
{

}


void CColorManager::initAll()
{
	initCommonColor();
	initBlockColors();
}

void CColorManager::reloadBlockColors()
{
	//�����ǰ����ɫ��
	QMap<QString,CColorItem*>::iterator iter = m_mapColors.begin();
	while(iter!=m_mapColors.end())
	{
		delete iter.value();
	}
	m_mapColors.clear();


	QString qsDir = QString("%1/config/blockcolors").arg(qApp->applicationDirPath());
	QDir dir(qsDir);
	QFileInfoList list = dir.entryInfoList(QStringList()<<"*.clr",QDir::Files);

	foreach(const QFileInfo& info,list)
	{
		QVector<uint> colors;
		QFile file(info.absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;

		QString qsContent = file.readAll();
		QStringList listColors = qsContent.split("\n");

		foreach(const QString& clr,listColors)
		{
			QStringList RGBs = clr.trimmed().split(",");
			if(RGBs.size()<3)
				continue;

			int iR = RGBs[0].toInt();
			int iG = RGBs[1].toInt();
			int iB = RGBs[2].toInt();

			colors.push_back(rRGB(iR,iG,iB));
		}

		m_mapColors[info.completeBaseName()] = new CColorItem(colors);

		file.close();
	}
}

void CColorManager::initCommonColor()
{
	//http://blog.csdn.net/daichanglin/article/details/1563299
	CommonColor.push_back(rRGB(220,20,60));		//�ɺ�
	CommonColor.push_back(rRGB(255,240,245));		//����ĵ���ɫ
	CommonColor.push_back(rRGB(128,0,128));		//��ɫ
	CommonColor.push_back(rRGB(75,0,130));		//����
	CommonColor.push_back(rRGB(0,0,205));			//���е���ɫ
	CommonColor.push_back(rRGB(0,0,128));			//������
	CommonColor.push_back(rRGB(176,196,222));		//������
	CommonColor.push_back(rRGB(135,206,235));		//����ɫ
	CommonColor.push_back(rRGB(0,191,255));		//����ɫ
	CommonColor.push_back(rRGB(60,179,113));		//�������ɫ
	CommonColor.push_back(rRGB(0,100,0));			//����ɫ
	CommonColor.push_back(rRGB(255,255,0));		//����ɫ
	CommonColor.push_back(rRGB(128,128,0));		//���ɫ
	CommonColor.push_back(rRGB(255,215,0));		//��ɫ
	CommonColor.push_back(rRGB(222,184,135));		//��ʵ����
	CommonColor.push_back(rRGB(192,192,192));		//����ɫ
}

void CColorManager::initBlockColors()
{
	//��ʼ����������ɫ��
	QVector<uint> vColors;
	for (int i=0;i<21;++i)
	{
		int iColor = (255.0/(20))*i;
		vColors.push_back(rRGB(iColor,iColor,iColor));
	}
	m_pDefaultColor = new CColorItem(vColors);

	reloadBlockColors();		//���ļ�������ɫ��
}

QStringList CColorManager::getBlockColorList()
{
	return m_mapColors.keys();
}

uint CColorManager::getBlockColor( const QString& mode,float fVal )
{
	int iColor = fVal+10.5;
	if(iColor>(20))
		iColor = (20);
	if(iColor<0)
		iColor = 0;

	if(m_mapColors.contains(mode))
	{
		return m_mapColors[mode][iColor];
	}
	else
	{
		return m_pDefaultColor[iColor];
	}
}

uint CColorManager::getBlockColor( const QString& mode,int index )
{
	if(index>(20))
		index = (20);
	if(index<0)
		index = 0;

	if(m_mapColors.contains(mode))
	{
		return m_mapColors[mode][index];
	}
	else
	{
		return m_pDefaultColor[index];
	}
}

bool CColorManager::getBlockColor( const QString& mode,QVector<uint>& vColors )
{
	if(m_mapColors.contains(mode))
	{
		vColors = m_mapColors[mode];
		return true;
	}
	else
	{
		vColors = m_pDefaultColor;
		return false;
	}
}

uint CColorManager::getCommonColor( int index )
{
	return CommonColor[(index%(CommonColor.size()))];
}