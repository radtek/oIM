######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 18 14:01:52 2016
######################################################################

TEMPLATE = app
TARGET = qqlogin
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += .

INCLUDEPATH += . \
			   ./extend.ctrls \
			   ./extend.events \
			   ./extend.skins \
			   ../../utilities/include \
			   ../../soui/include \
			   ../../components \
			   
dir = ../..
include($$dir/common.pri)

CONFIG(debug,debug|release){
	LIBS += utilitiesd.lib souid.lib imm32.lib
}
else{
	LIBS += utilities.lib soui.lib imm32.lib
}

PRECOMPILED_HEADER = stdafx.h

# Input
HEADERS += GroupChatFrame.h \
           MainDlg.h \
           MenuWrapper.h \
           resource.h \
           stdafx.h \
           utils.h \
           extend.ctrls/ExtendCtrls.h \
           extend.ctrls/SButtonEx.h \
           extend.ctrls/SImageView.h \
           extend.ctrls/SSplitBar.h \
           extend.ctrls/imre/ClipboardConverter.h \
           extend.ctrls/imre/dataobject.h \
           extend.ctrls/imre/HTMLParser.h \
           extend.ctrls/imre/HTTPDownloader.h \
           extend.ctrls/imre/ImgProvider.h \
           extend.ctrls/imre/IRichEditObjHost.h \
           extend.ctrls/imre/RichEditObj.h \
           extend.ctrls/imre/RichEditObjEvents.h \
           extend.ctrls/imre/RichEditObjFactory.h \
           extend.ctrls/imre/RichEditOleBase.h \
           extend.ctrls/imre/RichEditOleCallback.h \
           extend.ctrls/imre/RichEditOleCtrls.h \
           extend.ctrls/imre/RichEditUnitConverter.h \
           extend.ctrls/imre/SImRichEdit.h \
           extend.skins/ExtendSkins.h \
           extend.skins/SAntialiasSkin.h \
           extend.skins/SVscrollbar.h \	
           extend.events/ExtendEvents.h \
           ../../controls.extend/SChromeTabCtrl.h \
           ../../controls.extend/SImageMaskWnd.h \
           ../../controls.extend/STurn3DView.h \
           ../../controls.extend/image3d/3dlib.h \
           ../../controls.extend/image3d/3dmatrix.h \
           ../../controls.extend/image3d/3dTransform.h \
           ../../controls.extend/FileHelper.h \
           ../../controls.extend/slistboxex.h

SOURCES += GroupChatFrame.cpp \
           MainDlg.cpp \
           MenuWrapper.cpp \
           QQLogin.cpp \
           utils.cpp \
           extend.ctrls/ExtendCtrls.cpp \
           extend.ctrls/SButtonEx.cpp \
           extend.ctrls/SImageView.cpp \
           extend.ctrls/SplitBar.cpp \
           extend.skins/ExtendSkins.cpp \
           extend.skins/SAntialiasSkin.cpp \
           extend.ctrls/imre/ClipboardConverter.cpp \
           extend.ctrls/imre/dataobject.cpp \
           extend.ctrls/imre/HTMLParser.cpp \
           extend.ctrls/imre/HTTPDownloader.cpp \
           extend.ctrls/imre/ImgProvider.cpp \
           extend.ctrls/imre/RichEditObj.cpp \
           extend.ctrls/imre/RichEditObjFactory.cpp \
           extend.ctrls/imre/RichEditOleBase.cpp \
           extend.ctrls/imre/RichEditOleCallback.cpp \
           extend.ctrls/imre/RichEditOleCtrls.cpp \
           extend.ctrls/imre/RichEditUnitConverter.cpp \
           extend.ctrls/imre/SImRichedit.cpp \
           ../../controls.extend/SChromeTabCtrl.cpp \
           ../../controls.extend/SImageMaskWnd.cpp \
           ../../controls.extend/STurn3DView.cpp \
           ../../controls.extend/image3d/3dlib.cpp \
           ../../controls.extend/image3d/3dmatrix.cpp \
           ../../controls.extend/image3d/3dtransform.cpp \
           ../../controls.extend/slistboxex.cpp

RC_FILE += qqlogin.rc
