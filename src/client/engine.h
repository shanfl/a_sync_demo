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
#define RBUF					801			// 人最大数
#define SBUF					2801		// 兽最大数
#define JBUF					100			// 景最大数
#define getPCH(type)	(type*)malloc(sizeof(type)) 
#define MAN_AMOUNT		1000

// 快速访问单件实例	
#define	_GM	(dxGame::getInstance ())

typedef struct
{
	short int PID;
	short int genus;	//类别[0人, 1兽, 2景]
	short int x,y;	//当前位置
	short int endx,endy;	//目标位置
	short int oc;	//偏移计数
	short int ocs,oct;	//偏移计数初值和终值
	short int action;	//动作(人[0站,1走,2刺,3劈,4倒];
						//兽[0站,1走,2跑,3斗,4倒,5尸])
	short int kind;	//角色(人[0男1女];
						//兽[0豹1狼2猪3鹿4马5羊6雀])
	short int w,h;	//角色位图大小
	short int azimuth;	//方位
	short int tussle;	//打斗
}Sprite;

typedef struct{
	int start;	//动作起点
	unsigned short step;	//动作步数
}Motion;

/////////////////////////////////////////////
// the Game Class library 
class dxGame
{
public:
  dxGame();
  virtual ~dxGame();

public:
	HDC _scrdc;						// 屏幕设备场景
	HDC _memdc;						// 对象设备场景
	HDC _mapdc;						// 地图设备场景
	HDC _mapdc_t;					// 暂存设备场景
	HBITMAP _hbit;   			// 地图（对象）位图内存
	HBITMAP _hbmp;				// 暂存位图内存
	HBITMAP _hbmp_t;     	// 暂存位图内存
	HBITMAP _oldmak;
	int _width, _height;	// 对象图片的宽、高
	TCHAR extpath[256];
	FILE	*sfile, *rfile, *jfile;
	BYTE	*stmp,*rtmp,*jtmp;
	int		sbuf[SBUF];
	int		rbuf[RBUF];
	int		jbuf[JBUF];
	short int rox[RBUF];	//人的偏移量
	short int roy[RBUF];
	short int sox[SBUF];	//兽的偏移量
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
