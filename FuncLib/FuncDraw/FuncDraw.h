/************************************************************************/
/* �ļ����ƣ�FuncBase.h
/* ����ʱ�䣺2013-02-26 03:54
/*
/* ��    ����������չ�⣬ʵ���˻����ĺ�������
/************************************************************************/
#include <QtCore>
#include "StockInfoItem.h"

extern "C"{_declspec(dllexport) QMap<const char*,lua_CFunction>* ExportAllFuncs();}

/*
����: ����K��ͼ
�������: ��ͼ����  ��������: 4

ͨ������Ĳ�������K��ͼ
�÷�:
DrawK(OPEN,CLOSE,HIGH,LOW)
����:
DrawK(OPEN,CLOSE,HIGH,LOW)
�����̼ۣ����̼ۣ���߼ۣ���ͼۻ��Ƴ�K��ͼ
*/
int my_lua_drawk(lua_State* _L);

/*
����: �����߶�ͼ
�������: ��ͼ����  ��������: 1

ͨ������Ĳ��������߶�ͼ
�÷�:
DrawLine(OPEN)
����:
DrawK(OPEN)
�����̼ۻ���Ϊ����ͼ
*/
int my_lua_drawLine(lua_State* _L);

/*
����: ������״ͼ
�������: ��ͼ����  ��������: 1

ͨ������Ĳ���������״ͼ
�÷�:
DrawHist(VOLUME)
����:
DrawHist(VOLUME)
����������״ͼ
*/
int my_lua_drawHistogram(lua_State* _L);