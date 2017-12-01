#pragma once

#include "Pinyin\I_Pinyin.h"
#include <vector>

using namespace std;

class C_Pinyin: public I_Pinyin
{
	DECALRE_PLUGIN_(C_Pinyin)

public:
	C_Pinyin(void);
	~C_Pinyin(void);

private:
	int		m_i32Flag;	// 1 for fast combination

protected:
	inline	int Vect2Str(vector<SOUI::SStringA>& vectPinyin, SOUI::SStringA& szPinyin);

public:
	virtual int GetUniPinyins( const WCHAR* pszText, SOUI::SStringA& szPinyin, SOUI::SStringA& szSearch );
	virtual int GetUtf8Pinyins( const char* pszText, SOUI::SStringA& szPinyin, SOUI::SStringA& szSearch );
	virtual const char* GetPinyin( int i32Code, int i32Encoding, int* pi32Bytes  );

	virtual BOOL SetFlag(int i32Flag = 0);
	virtual int  GetFlag() const;

};

