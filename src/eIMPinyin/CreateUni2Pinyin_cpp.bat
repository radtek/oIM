@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
ECHO //Unicode(UTF16) to pinyin table>>Uni2Pinyin.h
ECHO.>>Uni2Pinyin.h
ECHO const char* Uni2Pinyin(int i32Char)>>Uni2Pinyin.h
ECHO {>>Uni2Pinyin.h
ECHO 	switch ( i32Char )>>Uni2Pinyin.h
ECHO 	{										                  //Char^|U10  ^|U16   ^|GBK   ^|GB2312^|BIG5  >>Uni2Pinyin.h
FOR /F "skip=1 tokens=1-6,8" %%i IN (test.txt) DO (

	SET "strTmp11=%%i"
	SET "strTmp12=%%j"
	SET "strTmp13=%%k"
	SET "strTmp14=%%l"
	SET "strTmp15=%%m"
	SET "strTmp16=%%n"
	SET "strTmp17=%%o"
		
	:: ECHO !strTmp11! !strTmp12! !strTmp13! !strTmp14! !strTmp15! !strTmp16! !strTmp17!
	SET "strTmp17=!strTmp17:(=!"
	SET "strTmp17=!strTmp17:,= !"
	SET "strTmp17=!strTmp17:)=!"
	SET "strTmp17=!strTmp17:0=!"
	SET "strTmp17=!strTmp17:1=!"
	SET "strTmp17=!strTmp17:2=!"
	SET "strTmp17=!strTmp17:3=!"
	SET "strTmp17=!strTmp17:4=!"
	SET "strTmp17=!strTmp17:5=!"
	
	:: 排序strTmp17
	FOR %%P IN (!strTmp17!) DO (
		ECHO %%P
	)>>BBB.txt
	SORT BBB.txt > AAA.txt
	SET "strTmp17="
	FOR /F %%Q IN (AAA.txt) DO (
		SET "strTmp17=!strTmp17! %%Q"
	)
	DEL AAA.TXT
	DEL BBB.TXT
	
	:: 去重strTmp17
	SET "str=#"
	SET "strTmp2=xxx"
	FOR %%j IN (!strTmp17!) DO ( 
		SET "strTmp3=%%j"
		:: ECHO %%j "!strTmp2!" "!strTmp3!"
		if "!strTmp2!" neq "!strTmp3!" (
			SET "str=!str!\0%%j" & SET "strTmp2=!strTmp3!"
		)
	) 
	SET "str=!str!\0"
	SET "str=!str:#\0=!"
	SET "str="!str!"
	SET "under=                                     "
	SET "str=!str!";!under!"
	SET "str=!str:~0,37!"

	ECHO 	case 0x!strTmp13!:  return !str!//  !strTmp11!^|!strTmp12!^|0x!strTmp13!^|0x!strTmp14!^|0x!strTmp15!^|0x!strTmp16!
)>>Uni2Pinyin.h	
ECHO		default: return "";>>Uni2Pinyin.h
ECHO		}>>Uni2Pinyin.h
ECHO }>>Uni2Pinyin.h
PAUSE
	