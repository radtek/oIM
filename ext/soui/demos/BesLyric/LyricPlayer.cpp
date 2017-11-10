/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// LyricPlyer.cpp :  ʵ��  LyricPlayer�� �Ľӿ�	
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LyricPlayer.h"
#include "FileHelper.h"
#include <mmsystem.h> 
#pragma comment (lib, "winmm.lib")

//���캯��
LyricPlayer::LyricPlayer():
		 m_nCurLine(0),	
		 m_nTotalLine(0)
{
	m_szMusicPathName[0]= _T('\0');		
	m_szLyricPathName[0]= _T('\0');
}

//���ø���·��
void LyricPlayer::setMusicPath(LPCTSTR pathName, HWND hostWnd)
{
	_tcscpy(m_szMusicPathName,pathName);
	m_musicPlayer.init(m_szMusicPathName,hostWnd);
}
	
void LyricPlayer::setLyricPath(LPCTSTR pathName)
{
	_tcscpy(m_szLyricPathName,pathName);
}

//���� LyricPlayer�� �������Ϊ��
void LyricPlayer::reloadPlayer()
{
	m_vLineInfo.clear();
}

//���Ÿ����������� Ԥ����ʼ
void LyricPlayer::playingStart(SHostWnd *wnd)
{
	//���»�����������¼������
	m_nTotalLine = this->m_vLineInfo.size();
	m_nCurLine = 1;	//��ֵ��Ϊ1����ʱ��ѭ������Ҫ

	//�������Ϊ1�����Timer������ҳ�����ʾ
	wnd->SetTimer(102,1);
	//���ò��յ�ʱ�����
	setStartPoint();

	playMusic();
}

//Ԥ������
void LyricPlayer::playingEnd(SHostWnd *wnd)
{
	//�ر�Timer
	wnd->KillTimer(102);
	stopMusic();
}

//��������
void LyricPlayer::playMusic()
{
	m_musicPlayer.openStart();
}

//ֹͣ����
void LyricPlayer::stopMusic()
{
	m_musicPlayer.closeStop();
}

//����startPointF ��ʼֵ
void LyricPlayer::setStartPoint()
{
	SYSTEMTIME		currentPoint;				/* ��¼��ǰ��ʱ��� */
	GetLocalTime(&currentPoint);
	
	//ת���õ��õ� startPointF
	SystemTimeToFileTime(&currentPoint,(FILETIME*)&startPointF);
}

//�õ���startPointF �ĺ����ֵ
int LyricPlayer::getMsDiffFromStartPoint()
{
	SYSTEMTIME		currentPoint;				/* ��¼��ǰ��ʱ��� */
	ULARGE_INTEGER  currentPointF;				/* ��Ӧ�� FILETIME ��Ϊ�˵õ�ʱ��ʹ��FILETIME*/ 
	GetLocalTime(&currentPoint);
	
	SystemTimeToFileTime(&currentPoint,(FILETIME*)&currentPointF); 
	
	unsigned __int64 dft=currentPointF.QuadPart-startPointF.QuadPart; 
	int ms = (int)(dft/10000);//�õ����ĺ�����
	
	return ms;
}

//���������ߺ��˶��ᵼ�£���ǰ�з����仯������Ҫ�ȸ�����ȡֵ
void LyricPlayer::updateCurLine()
{
	//���ݵ�ǰ�������ŵ�λ�ã��жϵ�ǰ����һ��
	int pos = this->m_musicPlayer.getPosition();
	vector<TimeLineInfo>::iterator i= m_vLineInfo.begin();
	int j;
	for(j=1; i != m_vLineInfo.end(); i++,j++)
		if( i->m_nmSesonds > pos)//���ʱ�� ���ڵ�ǰ����λ��ʱ
		{
			//ȡǰһ��λ�ã���Ϊ m_nCurLine ��ֵ
			this->m_nCurLine = j-1; 
			break;
		}
}