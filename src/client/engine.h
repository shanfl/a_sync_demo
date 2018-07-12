/********************************************************
** filename: engine.h
** Copyright (c) 2006, Hunan Agriculture University.
** Author: zdx (zdxster AT gmail DOT com)
** Date: 2006/05/08
** Description: the header file of game engine.
*********************************************************/

#ifndef __ENGINE_H__
#define __ENGINE_H__

#pragma once

#include <windows.h>
#include <string>
#include <vector>

using namespace std;

#define SCR_WIDTH 		600
#define SCR_HEIGHT		480
#define RBUF					801			// �������
#define SBUF					2801		// �������
#define JBUF					100			// �������
#define getPCH(type)	(type*)malloc(sizeof(type)) 
#define MAN_AMOUNT		1000

// ���ٷ��ʵ���ʵ��	
#define	_GM	(dxGame::getInstance ())

typedef struct
{
	short int PID;
	short int genus;	//���[0��, 1��, 2��]
	short int x,y;	//��ǰλ��
	short int endx,endy;	//Ŀ��λ��
	short int oc;	//ƫ�Ƽ���
	short int ocs,oct;	//ƫ�Ƽ�����ֵ����ֵ
	short int action;	//����(��[0վ,1��,2��,3��,4��];
						//��[0վ,1��,2��,3��,4��,5ʬ])
	short int kind;	//��ɫ(��[0��1Ů];
						//��[0��1��2��3¹4��5��6ȸ])
	short int w,h;	//��ɫλͼ��С
	short int azimuth;	//��λ
	short int tussle;	//��
}Sprite;

typedef struct{
	int start;	//�������
	unsigned short step;	//��������
}Motion;

/////////////////////////////////////////////
// the Game Class library 
class dxGame
{
public:
  dxGame();
  virtual ~dxGame();

public:
	HDC _scrdc;						// ��Ļ�豸����
	HDC _memdc;						// �����豸����
	HDC _mapdc;						// ��ͼ�豸����
	HDC _mapdc_t;					// �ݴ��豸����
	HBITMAP _hbit;   			// ��ͼ������λͼ�ڴ�
	HBITMAP _hbmp;				// �ݴ�λͼ�ڴ�
	HBITMAP _hbmp_t;     	// �ݴ�λͼ�ڴ�
	HBITMAP _oldmak;
	int _width, _height;	// ����ͼƬ�Ŀ���
	TCHAR extpath[256];
	FILE	*sfile, *rfile, *jfile;
	BYTE	*stmp,*rtmp,*jtmp;
	int		sbuf[SBUF];
	int		rbuf[RBUF];
	int		jbuf[JBUF];
	short int rox[RBUF];	//�˵�ƫ����
	short int roy[RBUF];
	short int sox[SBUF];	//�޵�ƫ����
	short int soy[SBUF];
	vector <Sprite> obj;
	short int selfPID;
	vector<wstring> chat;
	
public:
	// Singleton Design Patterns
	static dxGame* getInstance ()
	{
		if (instance == 0) {
			instance = new dxGame;
		}
		
		return instance;
	}
	
	bool Initialize ();
	void Exit ();
	void BlebSort ();
	void OptSort ();
	bool LoadBMP (wstring cc);
	void TransBlt (HDC hdc0,	
								 int nX0,int nY0,
								 int nW0,int nH0,
								 HDC hdc1,
								 int nX1,int nY1,
								 int nW1,int nH1,
								 UINT tCol
								);
	void SetSprite (int i);
	void SpriteMove (int i);
	void RudderShift (int i);
	bool SpriteShow (int i);
	void SpriteFight (int i);
	void LoadData();
	void MemCopy(unsigned char *mu,unsigned char *yu,int z);
	bool GetGraph(wstring str,int p);
	int GetItemByPID (short int pid);
	int GetAzimuth (int dx, int dy);
private:
  static dxGame* instance;
};

#endif
