/************************************************************************/
/* �ļ����ƣ�FuncBase.h
/* ����ʱ�䣺2013-02-26 03:54
/*
/* ��    ����������չ�⣬ʵ���˻����ĺ�������
/************************************************************************/

extern "C"{_declspec(dllexport) int ExportAllFuncs(QMap<QString,lua_CFunction>& mapFuncs);}

static int my_lua_add(lua_State* _L);