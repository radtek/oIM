/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// MusicPlayer.cpp :  ʵ��  MusicPlayer�� �Ľӿ�	
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MusicPlayer.h"

MusicPlayer::MusicPlayer()
{
	this->m_hdlHostWnd = NULL;
	this->m_szMusicPathName[0]=_T('\0');
	m_bIsParamReady = false;

	this->m_nVolumn = 1000;
}

//��������·�� �� Ŀ�괰�ھ��
void MusicPlayer::init(LPCTSTR musicPathName, HWND hostWnd)
{
	_tcscpy(this->m_szMusicPathName,musicPathName);
	this->m_hdlHostWnd = hostWnd;
	m_bIsParamReady = true;
}

//�򿪲���������
void MusicPlayer::openStart()
{
	if( isParamReady() )
	{
		MCIERROR mcierror = openDevice();
		if(mcierror)
		{
			 TCHAR buf[128]={0};
			 mciGetErrorString(mcierror, buf,128);
			 SMessageBox(this->m_hdlHostWnd,buf,_T("��ʾ"),0);
		}
		else
		{
			//��ͷ��ʼ����
			play(0);
		}
	}
	else
		SMessageBox(this->m_hdlHostWnd,_T("���Ų�����û���úã�"),_T("��ʾ"),0);

}

//�������ر�����
void MusicPlayer::closeStop()
{
	stop();
	closeDevice();
}

//��������
void MusicPlayer::play(int milliSecondPosition)
{	
	//���������ٲ���
	this->setVolumn(this->m_nVolumn);

	m_mciPlay.dwCallback=(DWORD)this->m_hdlHostWnd;
	m_mciPlay.dwFrom = (DWORD)milliSecondPosition; //������ʼλ��msΪ��λ
	mciSendCommand(m_mciOpen.wDeviceID, MCI_PLAY, MCI_NOTIFY|MCI_FROM, (DWORD)(LPVOID)&m_mciPlay); 
}

//seek��״̬Ϊstop������λ�÷����˸ı��ʱ�ɻ�ø�����λ�ò�����
void MusicPlayer::playAfterSeek()
{
	play(getPosition());
}

//��ͣ����
void MusicPlayer::pause()
{
	m_mciPause.dwCallback = (DWORD)this->m_hdlHostWnd;
	mciSendCommand(m_mciOpen.wDeviceID, MCI_PAUSE, MCI_WAIT|MCI_FROM, (DWORD)(LPVOID)&m_mciPause); 
}

//����ͣ��״̬�ָ�����
void MusicPlayer::resume()
{
	m_mciResume.dwCallback = (DWORD)this->m_hdlHostWnd;
	mciSendCommand(m_mciOpen.wDeviceID, MCI_RESUME, MCI_WAIT|MCI_FROM, (DWORD)(LPVOID)&m_mciResume); 
}

//ֹͣ����
void MusicPlayer::stop()
{
	m_mciStop.dwCallback = (DWORD_PTR)this->m_hdlHostWnd;	//������Ϣ�Ĵ��ڵľ��
	mciSendCommand(m_mciOpen.wDeviceID, MCI_STOP, MCI_WAIT, (DWORD)(LPMCI_GENERIC_PARMS)&m_mciStop); 
}

//ǰ�������һ��ʱ��
void MusicPlayer::shift(int milliSecond)
{
	int pos = getPosition();
	int len = getLength();
	int nextPos = pos + milliSecond;
	nextPos = min(max(0,nextPos),len);
	seek(nextPos);
}

//�õ������ĳ���
int MusicPlayer::getLength()
{
	m_mciStatus.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciStatus.dwItem = MCI_STATUS_LENGTH;
	m_mciStatus.dwReturn = 0;
	mciSendCommand(m_mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&m_mciStatus); 
	return m_mciStatus.dwReturn;
}

//���ص�ǰ��λ��
int MusicPlayer::getPosition()
{
	m_mciStatus.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciStatus.dwItem = MCI_STATUS_POSITION;
	m_mciStatus.dwReturn = 0;
	// ָ�� MCI_STATUS_MODE ʱ����Ҫͬʱָ��MCI_STATUS_ITEM��������ȷ��m_mciStatus ��dwItem��������Ч
	mciSendCommand(m_mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_MODE | MCI_STATUS_ITEM, (DWORD)&m_mciStatus); 
	return m_mciStatus.dwReturn;
}

//��õ�ǰģʽ״̬
DWORD_PTR  MusicPlayer::getModeStatus()
{
	m_mciStatus.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciStatus.dwItem = MCI_STATUS_MODE;
	m_mciStatus.dwReturn = 0;
	// ָ�� MCI_STATUS_MODE ʱ����Ҫͬʱָ��MCI_STATUS_ITEM��������ȷ��m_mciStatus ��dwItem��������Ч
	mciSendCommand(m_mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&m_mciStatus); 
	return m_mciStatus.dwReturn;
}

//�������ļ�
MCIERROR MusicPlayer::openDevice()
{
	m_mciOpen.lpstrDeviceType = _T("mpegvideo"); //Ҫ�������ļ�����
	m_mciOpen.lpstrElementName = m_szMusicPathName; //Ҫ�������ļ�·��

	return mciSendCommand( 0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT ,(DWORD)&m_mciOpen); //���ļ�����
}

//�ر������ļ�
MCIERROR MusicPlayer::closeDevice()
{
	m_mciClose.dwCallback = (DWORD_PTR)this->m_hdlHostWnd;	//������Ϣ�Ĵ��ڵľ��
	return mciSendCommand( m_mciOpen.wDeviceID, MCI_CLOSE, MCI_WAIT,(DWORD)(LPMCI_STATUS_PARMS)&m_mciClose);
}

//����������С(0-1000)
int MusicPlayer::setVolumn(int volumn)
{	
	this->m_nVolumn = min(max(0,volumn),1000);

	m_mciSetVolumn.dwCallback=(DWORD_PTR)this->m_hdlHostWnd;	//������Ϣ�Ĵ��ڵľ��
	m_mciSetVolumn.dwItem=MCI_DGV_SETAUDIO_VOLUME;
	m_mciSetVolumn.dwValue= this->m_nVolumn;

	mciSendCommand(m_mciOpen.wDeviceID,MCI_SETAUDIO,MCI_DGV_SETAUDIO_ITEM  |MCI_DGV_SETAUDIO_VALUE,(DWORD)&m_mciSetVolumn);

	return this->m_nVolumn;
}

//��õ�ǰ����ƽ��ֵ������ƽ������
int MusicPlayer::getVolumn()
{
	m_mciGetVolumn.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciGetVolumn.dwItem = MCI_DGV_STATUS_VOLUME;
	m_mciGetVolumn.dwReturn = 0;

	mciSendCommand(m_mciOpen.wDeviceID, MCI_STATUS, MCI_DGV_STATUS_NOMINAL | MCI_STATUS_ITEM, (DWORD)&m_mciGetVolumn); 
	return m_mciGetVolumn.dwReturn;
}

//����ָ����λ��
void MusicPlayer::seek(int position)
{
	m_mciSet.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciSet.dwTimeFormat = MCI_FORMAT_MILLISECONDS;  //����ʱ���ʽΪ ����
	mciSendCommand(m_mciOpen.wDeviceID,MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)&m_mciSeek);//ָ���豸ʶ���ʱ���ʽ

	m_mciSeek.dwCallback =  (DWORD_PTR)this->m_hdlHostWnd;
	m_mciSeek.dwTo = position;
	mciSendCommand(m_mciOpen.wDeviceID,MCI_SEEK, MCI_TO, (DWORD)&m_mciSeek);
}

