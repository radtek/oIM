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
* @file       LyricPlayer.h
* @version    v1.0      
* @author     BensonLaur   
* @date       2017/01/08
* 
* Describe    LyricPlayer�࣬������ ��ʹ�������ҳ�洦�� ��ʲ�������Ľӿ�
*/

#pragma once
#include "stdafx.h"
#include "MusicPlayer.h"

#include <vector>
using namespace std;

class TimeLineInfo;

/*
*	@brief ��ʲ��������洢�ʹ��� ��ʹ���Ԥ������ ��ʹ�õ�������
*/
class LyricPlayer
{
public:
	LyricPlayer();

	//���ø���·��
	//��������·��ʱ�����벥��������Ҫ�� ��Ϣ�������ڵľ��
	void setMusicPath(LPCTSTR pathName,HWND hostWnd);
	
	void setLyricPath(LPCTSTR pathName);

	//���� LyricPlayer�� �������Ϊ��
	void reloadPlayer();

	//���Ÿ����������� Ԥ����ʼ
	void playingStart(SHostWnd *wnd);

	//Ԥ������
	void playingEnd(SHostWnd *wnd);

	//����startPointF ��ʼֵ
	void setStartPoint();

	//�õ���startPointF �ĺ����ֵ
	// note: �������ڹ�����ʣ����úͲ������� ����ʱ��Ĳ�ֵ���������ǰ�� ����ʱ��ƫ�ƣ��ɴ��������Ƿ�����µ�һ�и��
	//		 ���ڼ�������ͣ ��ǰ�������˵ȹ��ܺ󣬸�ʱ���ֵ���޷������ж� ����Ƿ�Ӧ�ù���������ʱ���øú���
	int getMsDiffFromStartPoint();

	//���������ߺ��˶��ᵼ�£���ǰ�з����仯������Ҫ�ȸ�����ȡֵ
	void updateCurLine();

private:
	//��������
	void playMusic();

	//ֹͣ����
	void stopMusic();

public:
	TCHAR m_szMusicPathName[_MAX_PATH];			/*��� ѡ�� ��2���ӽ���ѡ���·����*/
	TCHAR m_szLyricPathName[_MAX_PATH];

	vector<TimeLineInfo> m_vLineInfo;		/* �����ʱ����Ϣ�ĸ�ʵ�ÿһ�еľ�����Ϣ ����һ�д���λ��Ϊ 0 ���� 1*/

	int				m_nCurLine;				/* ��ǰʱ���� �������� (��һ��Ϊ��1 ���� 0)*/
	int				m_nTotalLine;			/* ��ʱ����Ϣ�� �����������������У���������ʱ�䵫��û��ʵ��У� */
	
	MusicPlayer		m_musicPlayer;			/* �����ʹ���Ԥ�����������ֵĲ��� */
private:
	ULARGE_INTEGER  startPointF;			/* ��Ӧ�� FILETIME ��Ϊ�˵õ�ʱ��ʹ��FILETIME(��λ100ns)*/ 
											/* ʹ�� ���� setStartPoint ����startPointF ��ʼֵ*/
											/*ʹ�ú��� getMsDiffFromStartPoint �õ���startPointF �ĺ����ֵ */
};


/*
*	@brief ���洦��һ�и���ļ��� ����lrc�ļ�����ʱ����ĸ���ļ����ĸ�����
*/
class TimeLineInfo
{
public:
	TimeLineInfo(SStringT timeLine)
	{
		//��ʼ����Ļ�����Ա����Ϣ
		m_strTimeLine = timeLine;
		int pos = m_strTimeLine.Find(_T(']'));
		SStringT timeStr = m_strTimeLine.Left(pos+1);

		m_strLine = m_strTimeLine.Right(m_strTimeLine.GetLength()-pos-1);
		m_nmSesonds = TimeStringToMSecond(timeStr,timeStr.GetLength());

		if(m_strLine.GetLength()==0)
			m_bIsEmptyLine = true;
		else
			m_bIsEmptyLine = false;
	}
private:
	//��ʱ���ǩ�ַ����õ���Ӧ�ĺ���ʱ��
	int TimeStringToMSecond(LPCTSTR timeStr, int length)
	{
		//TODO���쳣�׳�����

		TCHAR szMinute[5];	//����
		TCHAR szSecond[5];	//��
		TCHAR szMSecond[5];	//����

		int i,j;
		//�õ�: ��. ��λ��
		int pos1 = -1,pos2 = -1,pos3 = length-1;
		for(i=0; i<length; i++)
		{
			if(_T(':') == timeStr[i])
				pos1 = i;
			if(_T('.') == timeStr[i])
				pos2 = i;
		}

		//�õ�����ʱ��ε��ַ���
		for(j=0,i=1; i < pos1; i++,j++)
			szMinute[j] = timeStr[i];
		szMinute[j] = _T('\0');

		for(j=0, i = pos1+1; i<pos2; i++, j++)
			szSecond[j] = timeStr[i];
		szSecond[j] = _T('\0');

		for(j=0, i = pos2+1; i<pos3; i++,j++ )
			szMSecond[j] = timeStr[i];
		szMSecond[j] = _T('\0');

		int millisecond = DecStrToDecimal(szMinute) * 60000 + DecStrToDecimal(szSecond)*1000 + DecStrToDecimal(szMSecond);
		return millisecond;
	}

	//�����޷���ʮ���ƴ���Ӧ�����֣�ʮ���ƴ������� 023��12��04 ����ʽ����ֵΪ0��999��
	int DecStrToDecimal(LPCTSTR timeStr)
	{
		int bit = _tcslen(timeStr);
		int result = 0;
		for(int i=0; i< bit; i++)
		{
			result *= 10;
			result += timeStr[i]-_T('0');
		}
		return result;
	}

public:
	SStringT m_strTimeLine;		/* ֱ�Ӵ洢 ���ļ���ȡ��һ���� */
	SStringT m_strLine;			/* �洢ȥ��ʱ����֮������� */
	int	m_nmSesonds;			/* �洢ʱ���Ƕ�Ӧ�ĺ���ʱ�� */
	bool m_bIsEmptyLine;		/* �Ƿ�Ϊ���У�ֻ��ʱ���ǣ� */
};