/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  BensonLaur

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       BSMessageBox.h
* @version    v1.0      
* @author     BensonLaur   
* @date       2017/01/08
* 
* Describe    BSMessageBox �࣬ʵ��SOUI��SMessageBoxImpl�ӿڣ�ʵ���Զ���ͼ�����ʾ
*/

#pragma once
#include "stdafx.h"


class BSMessageBox: public SMessageBoxImpl
{
public:
	//��������ͼ��ĺ���
	BOOL OnSetIcon( UINT uType )
	{
		SIconWnd *pIcon=(SIconWnd *)FindChildByName(NAME_MSGBOX_ICON);
        if(!pIcon) return FALSE;
		pIcon->SetVisible(FALSE,TRUE);

		SWindow *iconInfo = FindChildByID(R.id.img_icon_information);
		SWindow *iconQues = FindChildByID(R.id.img_icon_question);
		SWindow *iconExcl = FindChildByID(R.id.img_icon_exclamation);

        switch(uType&0xF0)
        {
        case MB_ICONINFORMATION:
			
			if(iconInfo && !iconInfo->IsVisible())
				iconInfo->SetVisible(TRUE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);

            break;

        case MB_ICONQUESTION:

			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && !iconQues->IsVisible())
				iconQues->SetVisible(TRUE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);
            break;

			
        case MB_ICONEXCLAMATION:
			
			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && !iconExcl->IsVisible())
				iconExcl->SetVisible(TRUE,TRUE);

            break;

        case MB_ICONHAND:
        default:
            
			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);

            break;
        }
        return TRUE;
	}
};