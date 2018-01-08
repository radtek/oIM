TEMPLATE = subdirs
TARGET = demos
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += .

include(../cpy-cfg.pri)

SUBDIRS += UiEditor
SUBDIRS += demo
SUBDIRS += qqlogin
SUBDIRS += 360
SUBDIRS += 360Preview
SUBDIRS += souispy
SUBDIRS += PcManager
SUBDIRS += SoTool
SUBDIRS += mclistview_demo
SUBDIRS += souitest
SUBDIRS += VUI
SUBDIRS += BesLyric
SUBDIRS += QQMain
SUBDIRS += FrogPlay
SUBDIRS += MultiLangs
SUBDIRS += TXMgr
SUBDIRS += SLogViewer
SUBDIRS += souieditor
SUBDIRS += DropDemo
SUBDIRS += ThreeClearGame
SUBDIRS += baiduwp

CONFIG(c++11){
	SUBDIRS += timdemo	
	CONFIG(x64){
             	timdemo64.depends += soui64
	}
	else{
		timdemo.depends += soui
	}
}

CONFIG(x64){
	36064.depends += soui64 skia64
	360Preview64.depends += soui64 skia64
	demo64.depends += soui64 mhook64 smiley64 skia64
	qqlogin64.depends += soui64 skia64
	souispy64.depends += soui64 skia64
	PcManager64.depends += soui64 skia64
	SoTool64.depends += soui64 skia64
	mclistview_demo64.depends += soui64 skia64
	UiEditor64.depends += soui64 skia64 scintilla64
	BesLyric64.depends += soui64 skia64
	QQMain64.depends += soui64 skia64
	VUI64.depends += soui64 skia64
	MultiLangs64.depends += soui64
	TXMgr64.depends += soui64
	SLogViewer64.depends += soui64 scintilla64
	souieditor64.depends += soui64 scintilla64
	DropDemo64.depends += soui64
	souitest64.depends += soui64
	ThreeClearGame.depends += soui64
}
else{
	360.depends += soui skia
	360Preview.depends += soui skia
	demo.depends += soui mhook smiley skia
	qqlogin.depends += soui skia
	souispy.depends += soui skia
	PcManager.depends += soui skia
	SoTool.depends += soui skia
	mclistview_demo.depends += soui skia
	UiEditor.depends += soui skia scintilla
	BesLyric.depends += soui skia
	QQMain.depends += soui skia
	VUI.depends += soui skia
	MultiLangs.depends += soui
	TXMgr.depends += soui
             SLogViewer.depends += soui scintilla
	souieditor.depends += soui scintilla
	DropDemo.depends += soui
	souitest.depends += soui
	ThreeClearGame.depends += soui
}