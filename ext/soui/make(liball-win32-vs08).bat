rem LIB_ALL ����ģ�������LIB��
rem USING_MT ָʾ������Ŀʹ��MT��ʽ����CRT
rem CAN_DEBUG ָʾΪ���ɵ�Release�汾�������Է���

SET mt=1
SET unicode=1
SET wchar=1

if exist .\config\build.cfg del .\config\build.cfg
set configStr=[BuiltConfig]
echo %configStr% >> .\config\build.cfg
set configStr=UNICODE=%unicode%
echo %configStr% >> .\config\build.cfg
set configStr=WCHAR=%wchar%
echo %configStr% >> .\config\build.cfg
set configStr=MT=%mt%
echo %configStr% >> .\config\build.cfg

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" x86
tools\qmake -tp vc -r -spec .\tools\mkspecs\win32-msvc2008 "CONFIG += USING_MT CAN_DEBUG LIB_ALL"
rem soui.sln