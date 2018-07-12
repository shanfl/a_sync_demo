/********************************************************
** filename: engine.cpp
** Copyright (c) 2006, Hunan Agriculture University.
** Author: zdx (zdxster AT gmail DOT com)
** Date: 2006/05/08
** Description: the source file of game engine.
*********************************************************/

#include <tchar.h>
#include "engine.h"

// Singleton Design Patterns
dxGame* dxGame::instance = 0;

//���޶����ṹλͼ��400��
//��:0վ��1�ߣ�2�̣�3����4����5ʬ
//��:0վ��1�ߣ�2�ܣ�3����4����5ʬ
static Motion act[6]={0,5,40,10,120,10,200,10,280,10,360,5};

/////////////////////////////////////////////////////////////////////////////
// Class dxGame

dxGame::dxGame ()
			: _width (0)
			, _height (0)
{
}

dxGame::~dxGame ()
{
}

// ��ʼ��
bool
dxGame::Initialize ()
{
// 1. ����ͼ�δ�����
	_scrdc = CreateDC (_T("DISPLAY"), NULL, NULL, NULL);	// ������Ļ�豸����
	_mapdc = CreateCompatibleDC (_scrdc);	// ������ͼ�豸����
	_mapdc_t = CreateCompatibleDC (_scrdc);	// �����ݴ��豸����
	_hbmp = CreateCompatibleBitmap (_scrdc, SCR_WIDTH, SCR_HEIGHT);	// �����ݴ�λͼ 0
	_hbmp_t = CreateCompatibleBitmap (_scrdc, SCR_WIDTH, SCR_HEIGHT);	// �����ݴ�λͼ 1
	SelectObject (_mapdc_t, _hbmp_t);	// �ݴ�λͼ 1 ���ݴ��豸����
//	_oldmak = (HBITMAP)SelectObject (_mapdc, _hbmp)	// ��ͼ�豸���� DC ѡ��λͼ���� 0
	_memdc = CreateCompatibleDC (_scrdc);
	
// 2. ����ͼƬĿ¼
	GetCurrentDirectory (256, extpath);

// 3. ��ʼ�����ֱ���
	stmp=NULL;rtmp=NULL;jtmp=NULL;

	return true;
}

void
dxGame::Exit ()
{
	DeleteObject (_hbmp);
	DeleteObject (_hbmp_t);
	DeleteDC (_mapdc);
	DeleteDC (_mapdc_t);
	DeleteDC (_memdc);
	DeleteDC (_scrdc);
}

int
dxGame::GetItemByPID (short int pid)
{
	for(int i=0;i<obj.size();i++) {
		if (obj[i].genus == 0 && obj[i].PID == pid)
			return i;
	}
	
	return -1;
}

void
dxGame::BlebSort () // ð������
{
	Sprite tman;
	for(int i=0;i<obj.size()-1;i++)
		for(int j=i+1;j<obj.size();j++)
			if(obj[i].y>obj[j].y){
				tman=obj[i];
				obj[i]=obj[j];
				obj[j]=tman;
			}
}

bool 
dxGame::LoadBMP (wstring cc)
{
	DeleteObject(_hbit);
	_hbit = (HBITMAP)LoadImage (NULL,
															cc.c_str(),
															IMAGE_BITMAP,
															0,	
															0,	
															LR_LOADFROMFILE | LR_CREATEDIBSECTION
														 );
	if (_hbit == NULL) return false;	
	DIBSECTION ds;	
	BITMAPINFOHEADER &bm = ds.dsBmih;
	GetObject(_hbit,sizeof(ds),&ds);
	_width = bm.biWidth;
	_height = bm.biHeight;
	return true;
}

void 
dxGame::TransBlt (HDC hdc0,	
								  int nX0,int nY0,
								  int nW0,int nH0,
								  HDC hdc1,
								  int nX1,int nY1,
								  int nW1,int nH1,
								  UINT tCol
								 )
{
	HBITMAP hBMP = CreateCompatibleBitmap (hdc0, nW0, nH0);
	HBITMAP mBMP = CreateBitmap (nW0, nH0, 1, 1, NULL);	
	HDC hDC = CreateCompatibleDC (hdc0);
	HDC mDC = CreateCompatibleDC (hdc0);
	HBITMAP oldBMP = (HBITMAP)SelectObject (hDC, hBMP);
	HBITMAP oldmBMP = (HBITMAP)SelectObject (mDC, mBMP);
	if (nW0 == nW1 && nH0 == nH1)
		BitBlt (hDC, 0, 0, nW0, nH0, hdc1, nX1, nY1, SRCCOPY);
	else
		StretchBlt (hDC, 0, 0, nW0, nH0, hdc1, nX1, nY1, nW1, nH1, SRCCOPY);

	SetBkColor (hDC, tCol);
	BitBlt (mDC, 0, 0, nW0, nH0, hDC, 0, 0, SRCCOPY);
	SetBkColor (hDC, RGB(0,0,0));
	SetTextColor (hDC, RGB(255,255,255));
	BitBlt (hDC, 0, 0, nW0, nH0, mDC, 0, 0, SRCAND);
	SetBkColor (hdc0, RGB(255,255,255));
	SetTextColor (hdc0, RGB(0,0,0));

	BitBlt (hdc0, nX0, nY0, nW0, nH0, mDC, 0, 0, SRCAND);
	BitBlt (hdc0, nX0, nY0, nW0, nH0, hDC, 0, 0, SRCPAINT);

	SelectObject (hDC, oldBMP);				
	DeleteDC (hDC);
	SelectObject (mDC, oldmBMP);
	DeleteDC (mDC);
	DeleteObject (hBMP);
	DeleteObject (mBMP);
}

// ���ý�ɫ��ֵ
void 
dxGame::SetSprite (int i)
{
	int Position = 400;
	
	obj[i].ocs = obj[i].kind * Position+act[obj[i].action].start
			  			 + obj[i].azimuth * act[obj[i].action].step;
	obj[i].oct = act[obj[i].action].step + obj[i].ocs;
	obj[i].oc = obj[i].ocs;

	obj[i].tussle = 0;
}

// ��ɫ�ƶ�
void 
dxGame::SpriteMove(int i)
{
	int stx,sty,dx,dy;
	if(obj[i].genus==2) return;
	switch(obj[i].action) 
	{
		case 2: {stx=9;sty=6;break;}	//�߷�
	    case 1: {stx=4;sty=2;break;}	//�ܷ�
		default:{stx=2;sty=1;break;}
	}
	dx=obj[i].endx-obj[i].x;	//��ǰ��Ŀ��λ�ò�
	dy=obj[i].endy-obj[i].y;
	if (dx==0&&dy==0) return;	//����Ŀ��㣬���ء�
	int adx=abs(dx);
	int ady=abs(dy);
	if(adx<stx) stx=adx;	//λ�ò�㲽��������ȡλ�ò��ֵ
	if(ady<sty) sty=ady;
	if(dx!=0) obj[i].x+=adx/dx*stx;	//��λ�ƶ��������dx>0&&dy>0����ȡΪ������
	if(dy!=0) obj[i].y+=ady/dy*sty;	
}

// ��λת��
void 
dxGame::RudderShift(int i)
{
	//��ǰ��Ŀ��λ�ò�
	int dx=obj[i].endx-obj[i].x;
	int dy=obj[i].endy-obj[i].y;
	if(obj[i].tussle==0){
		if(dx==0&&dy==0){	//Ϊ0��������վ����λ����
			obj[i].action=0;
			goto label;
		}	
		obj[i].action=1;	//�趨����(1��)
	}
	if(dx<0&&dy>0)		{obj[i].azimuth=1;goto label;}	//����
	if(dx<0&&dy<0)		{obj[i].azimuth=3;goto label;}	//����
	if(dx>0&&dy<0)		{obj[i].azimuth=5;goto label;}	//����
	if(dx>0&&dy>0)		{obj[i].azimuth=7;goto label;}	//����
	if      (dy>0)		{obj[i].azimuth=0;goto label;}	//��
	if(dx<0)			{obj[i].azimuth=2;goto label;}	//��
	if      (dy<0)		{obj[i].azimuth=4;goto label;}	//��
	if(dx>0)			{obj[i].azimuth=6;goto label;}	//��
label:	SetSprite(i);
}

int
dxGame::GetAzimuth (int dx, int dy)
{
	int azimuth = 7;
		
	if(dx<0&&dy>0)		{azimuth=1;goto t_label;}	//����
	if(dx<0&&dy<0)		{azimuth=3;goto t_label;}	//����
	if(dx>0&&dy<0)		{azimuth=5;goto t_label;}	//����
	if(dx>0&&dy>0)		{azimuth=7;goto t_label;}	//����
	if      (dy>0)		{azimuth=0;goto t_label;}	//��
	if(dx<0)			{azimuth=2;goto t_label;}	//��
	if      (dy<0)		{azimuth=4;goto t_label;}	//��
	if(dx>0)			{azimuth=6;goto t_label;}	//��

t_label:
	return azimuth;
}

// ������ʾ
bool
dxGame::SpriteShow(int i)
{
	wstring cc;
	switch(obj[i].genus){
	case 0:
		cc = _T("man");
		break;
	case 1:
		cc = _T("beast");
		break;
	case 2:
		cc = _T("scape");
		break;
	default:
		return false;
		break;
	}
	if(!GetGraph(cc, obj[i].oc)) return false;
	if(obj[i].w==0||obj[i].h==0){
		obj[i].w=_width;
		obj[i].h=_height;
	}
	//����ɫ��ƫ��λ��
	int x=obj[i].x-obj[i].w/2;
	int y=obj[i].y-obj[i].h;	
	if(obj[i].genus!=2)	//��(2)�Ǿ���û��ƫ��λ��
    {
		int x0=0,y0=0;
		if(obj[i].genus==0)
		{
			x0=rox[obj[i].oc];
			y0=roy[obj[i].oc];
		}else if(obj[i].genus==1) {
			x0=sox[obj[i].oc];
			y0=soy[obj[i].oc];
		}
		if(obj[i].azimuth>4) x0=_width-x0;	//�������������Ϸ�λ����
		x=obj[i].x-x0;	//�������ʾλ��
		y=obj[i].y-y0;
	}
	TransBlt(_mapdc_t,x,y,_width,_height,_memdc,0,0,_width,_height,RGB(255,255,255));
	if(obj[i].genus==2) return true;
	SpriteMove(i);	
	//��һ������
	obj[i].oc++;
	//���������������з�λת��
	if(obj[i].oc>=obj[i].oct)RudderShift(i);
	return true;
}

void 
dxGame::SpriteFight (int i)
{
	if(obj[i].genus==1&&obj[i].kind==5) return;
	for(int j=0;j<obj.size();j++){
		if(j==i) continue;	
		int x=obj[j].x;int y=obj[j].y-obj[j].h/2;
		int tx=obj[i].endx-x;int ty=obj[i].endy-y;
		x=obj[i].x-x;y=obj[i].y-y;
		if(tx-x<1&&ty-y<1&&abs(tx)<obj[j].w/2&&abs(ty)<obj[j].h/2&&abs(x)<obj[j].w/2&&abs(y)<obj[j].h/2){
			if(obj[j].genus==1&&obj[j].kind==5)
			{
				x=obj[i].x-obj[j].x;
				y=obj[i].y-obj[j].y;
				if(x==0&&y<0){
					obj[i].azimuth=0;
				}
				if(x>0&&y< 0){
					obj[i].azimuth=1;
				}
				if(x>0&&y==0){
					obj[i].azimuth=2;
				}
				if(x>0&&y> 0){
					obj[i].azimuth=3;
				}
				if(x==0&&y>0){
					obj[i].azimuth=4;
				}
				if(x<0&&y> 0){
					obj[i].azimuth=5;
				}
				if(x<0&&y==0){
					obj[i].azimuth=6;
				}
				if(x<0&&y< 0){
					obj[i].azimuth=7;
				}
				obj[i].action=3;
				obj[i].tussle=1;
				if(obj[i].genus==0)obj[i].action=2;
				break;
			}
		}
	}
}

//��������Դ
void 
dxGame::LoadData()
{
	FILE *f;
	int len,i,j;
	wstring cc;

//	dar�ļ��Ĵ���
	cc = _T("./scape.dar");
	f=_wfopen(cc.c_str(), _T("r"));
	if(f==NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}
	fscanf(f,"%d",&len);
	for(i=0;i<len;i++)
		fscanf(f,"%d,%d,%d",&jbuf[i],&j,&j);
	fclose(f);

	cc = _T("./beast.dar");
	f=_wfopen(cc.c_str(), _T("r"));
	if(f==NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}
	fscanf(f,"%d",&len);
	for(i=0;i<len;i++)
		fscanf(f,"%d,%d,%d",&sbuf[i],&sox[i],&soy[i]);
	fclose(f);

	cc=_T("./man.dar");
	f=_wfopen(cc.c_str(),_T("r"));
	if(f==NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}
	fscanf(f,"%d",&len);
	for(i=0;i<len;i++)
		fscanf(f,"%d,%d,%d",&rbuf[i],&rox[i],&roy[i]);
	fclose(f);

//	gam�ļ��Ĵ���
	cc=_T("./beast.gam");
	if((sfile = _wfopen(cc.c_str(), _T("rb"))) == NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}
	cc=_T("./man.gam");
	if((rfile = _wfopen(cc.c_str(), _T("rb"))) == NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}
	cc=_T("./scape.gam");
	if((jfile = _wfopen(cc.c_str(), _T("rb"))) == NULL){
		::MessageBox(NULL, _T("�ļ���ȡʧ��!"), _T("����"), MB_OK);
		return;
	}

	fseek(sfile, 0, SEEK_END);
	UINT tlen = ftell(sfile);
	stmp=(BYTE *)new BYTE[tlen];
	rewind(sfile);
	fread (stmp, tlen, 1, sfile);
	fclose (sfile);

	fseek(rfile, 0, SEEK_END);
	tlen = ftell(rfile);
	rtmp=(BYTE *)new BYTE[tlen];
	rewind(rfile);
	fread (rtmp, tlen, 1, rfile);
	fclose(rfile);

	fseek(jfile, 0, SEEK_END);
	tlen = ftell(jfile);
	jtmp=(BYTE *)new BYTE[tlen];
	rewind(jfile);
	fread (jtmp, tlen, 1, jfile);
	fclose(jfile);
}

void 
dxGame::MemCopy(unsigned char *mu,unsigned char *yu,int z)
{
	memcpy(mu,yu,z);
	return;
	int z0=z/32;
	int z1=z-z0*32;
	   _asm
	   {//pand mm0,mm4	  ;//packed and  
		mov	esi,yu  
		mov	edi,mu 
		mov	ecx,z0
 Mcont:
		movq	mm0,qword ptr [esi]
		movq	qword ptr [edi],mm0
		movq	mm0,qword ptr [esi][8]
		movq	qword ptr [edi][8],mm0
		movq	mm0,qword ptr [esi][16]
		movq	qword ptr [edi][16],mm0
		movq	mm0,qword ptr [esi][24]
		movq	qword ptr [edi][24],mm0
		add		esi,8*4
		add		edi,8*4
		loop	Mcont
		mov		ecx,z1;
		repe	movsb;
		emms			;//empty MMX state
	   }
}

bool 
dxGame::GetGraph(wstring cc,int p)
{
	int len;
	if(p<0) return false;
	if(stmp==NULL||rtmp==NULL||jtmp==NULL)return false;
	BYTE *tmp;
	if(cc == _T("man")){
		if(p>RBUF-1) return false;
		len=rbuf[p+1]-rbuf[p];
		tmp=(BYTE *)new BYTE[len];
		MemCopy(tmp,rtmp+rbuf[p],len);
	}
	if(cc == _T("beast")){
		if(p>SBUF-1) return false;
		len=sbuf[p+1]-sbuf[p];
		tmp=(BYTE *)new BYTE[len];
		MemCopy(tmp,stmp+sbuf[p],len);
	}
	if(cc == _T("scape")){
		if(p>JBUF-6) return false;
		len=jbuf[p+1]-jbuf[p];
		tmp=(BYTE *)new BYTE[len];
		MemCopy(tmp,jtmp+jbuf[p],len);
	}
	LPBITMAPINFOHEADER bm=(BITMAPINFOHEADER *)tmp;
	bm->biSize = sizeof(BITMAPINFOHEADER);
	_width=bm->biWidth;
	_height=bm->biHeight;
	SelectObject(_memdc,_hbmp);
	StretchDIBits(_memdc,0,0,_width,_height,0,0,_width,_height,
				  tmp+256*sizeof(RGBQUAD)+bm->biSize,
				  (BITMAPINFO* )bm,DIB_RGB_COLORS,SRCCOPY);

	delete[] tmp;
	return true;
}
