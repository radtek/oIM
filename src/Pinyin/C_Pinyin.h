#pragma once

#include "eIMPinyin\IeIMPinyin.h"
#include <vector>

using namespace std;

class C_eIMPinyin: public I_EIMPinyin
{
	DECALRE_PLUGIN_(C_eIMPinyin)

public:
	C_eIMPinyin(void);
	~C_eIMPinyin(void);

private:
	int		m_i32Flag;	// 1 for fast combination

protected:
	inline	int Vect2Str(vector<eIMStringA>& vectPinyin, eIMStringA& szPinyin);

public:
	virtual int GetUniPinyins( const WCHAR* pszText, eIMStringA& szPinyin, eIMStringA& szSearch );
	virtual int GetUtf8Pinyins( const char* pszText, eIMStringA& szPinyin, eIMStringA& szSearch );
	virtual const char* GetPinyin( int i32Code, int i32Encoding, int* pi32Bytes  );

	virtual BOOL SetFlag(int i32Flag = 0);
	virtual int  GetFlag() const;

};

