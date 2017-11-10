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
* @file       MusicPlayer.h
* @version    v1.0      
* @author     BensonLaur   
* @date       2017/01/08
* 
* Describe    MusicPlayer�࣬���Խ��д������豸�����ֵĲ��š��������õȳ������
*/

#pragma once
#include "stdafx.h"


class MusicPlayer
{
public:
	//���캯��
	MusicPlayer();

	//��������·�� �� Ŀ�괰�ھ��
	void init(LPCTSTR musicPathName, HWND hostWnd);

	//�򿪲���������
	void openStart();

	//�������ر�����
	void closeStop();

	//��ĳ��λ�ÿ�ʼ��������
	void play(int milliSecondPosition);

	//seek��״̬Ϊstop������λ�÷����˸ı��ʱ�ɻ�ø�����λ�ò�����
	void playAfterSeek();

	//��ͣ����
	void pause();
	
	//����ͣ��״̬�ָ�����
	void resume();

	//ֹͣ����
	void stop();

	//ǰ�������һ��ʱ��
	void shift(int milliSecond);

	//�õ������ĳ���
	int getLength();

	//���ص�ǰ��λ�ã����룩
	int getPosition();

	//��õ�ǰģʽ״̬   ����״̬�鿴 https://msdn.microsoft.com/en-us/library/dd798405(v=vs.85).aspx#MCI_STATUS_MODE
	DWORD_PTR  getModeStatus();

	//�������ļ�
	MCIERROR openDevice();

	//�ر������ļ�
	MCIERROR closeDevice();

	//����������С(0-1000)
	int setVolumn(int volumn);

	//��õ�ǰ����ƽ��ֵ��Ĭ�ϳ�ʼ��������Ϊ���ֵ1000��
	int getVolumn();

private:
	bool isParamReady(){return  _tcslen(m_szMusicPathName)==0?false:true;}

	//����ָ����λ��
	void seek(int position);

private:
	TCHAR m_szMusicPathName[_MAX_PATH];		/* �������ֵ��ļ�·���� */
	HWND m_hdlHostWnd;						/* ��Ϣ���͵� �Ĵ��ڵľ��*/
	bool m_bIsParamReady;					/* ��¼�������ļ�·������Ŀ�괰�ھ�����Ƿ�׼���� */
	
	int m_nVolumn;							/* ��ǰ���� 0 ~ 1000 */

	//����mciSendCommand�����ĵĲ���
	MCI_OPEN_PARMS m_mciOpen;		//�򿪲���
	MCI_PLAY_PARMS m_mciPlay;		//���Ų���
	MCI_GENERIC_PARMS m_mciStop;	//ֹͣ����
	MCI_GENERIC_PARMS m_mciClose;	//�رղ���
	MCI_GENERIC_PARMS m_mciPause;	//��ͣ����
	MCI_GENERIC_PARMS m_mciResume;	//��ͣ�ָ�����
	MCI_STATUS_PARMS m_mciStatus;	//��ѯ״̬���� ������������ https://msdn.microsoft.com/en-us/library/dd798405(v=vs.85).aspx
	MCI_SEEK_PARMS m_mciSeek;		//��λ����
	MCI_SET_PARMS  m_mciSet;		//���ò���
	MCI_DGV_SETAUDIO_PARMS m_mciSetVolumn;//��������
	MCI_STATUS_PARMS m_mciGetVolumn;//��ѯ��������
};