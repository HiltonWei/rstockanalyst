/************************************************************************/
/* �ļ����ƣ�FuncBase.h
/* ����ʱ�䣺2013-02-26 03:54
/*
/* ��    ����������չ�⣬ʵ���˻����ĺ�������
/************************************************************************/
#include <QtCore>
#include "StockInfoItem.h"

extern "C"{_declspec(dllexport) int ExportAllFuncs(QMap<QString,lua_CFunction>& mapFuncs);}

static int my_lua_high(lua_State* _L);
static int my_lua_low(lua_State* _L);
static int my_lua_open(lua_State* _L);
static int my_lua_close(lua_State* _L);
