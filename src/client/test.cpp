/********************************************************
** filename: test.cpp
** Copyright (c) 2006, Hunan Agriculture University.
**
** Author: zdx (zdxster AT gmail DOT com)
**
** Date: 2006/05/08
**
** Description: the source file of game demo.
*********************************************************/

#include "ace/ace.h"
#include "ace/INET_Addr.h"
#include "MyTask.h"
#include "var_def.h"
#include "../common/Test_def.h"
#include <wx/statline.h>
#include <lm.h>
#include <memory>
#include "test.h"

#define PIC_PATH			_T("./pic")
#define VOC_PATH			_T("./voc")
#define DEF_SPACE			8

BOOL connected = FALSE ;
Client *client = 0;
ULONG identifier = 0;

/////////////////////////////////////////////////////////////////////////////////
// BasicApplication: 
// The implementation of this class

IMPLEMENT_APP(BasicApplication)
MyTask* task_t = 0;

bool BasicApplication::OnInit()
{
	ACE::init();

	task_t = new MyTask (ACE_Proactor::instance());

	if (task_t->start (1) != 0)
		return false;

	_success = false;
  wxSize size;
  size.Set(SCR_WIDTH, SCR_HEIGHT);
  BasicFrame *frame = new BasicFrame(_T("DEMO"), size);

	auto_ptr<LogonDlg> dlgLogon (new LogonDlg (frame));
	if (dlgLogon->ShowModal () == wxID_CANCEL) {
  	return false;
	}
	
	ACE_Time_Value tv;
	tv.msec (TIMEOUT);	
	if (_connEvent.wait (&tv, 0) == -1) 
		return false;

	if (! _success) {
		wxMessageBox (_T("登录失败：账号/密码错误或该账号已登录！"));
		return false;
	}

	frame->Init();
  frame->Show(TRUE);
  SetTopWindow(frame);

	return true;	
}

int BasicApplication::OnExit()
{
	task_t->stop();	

	ACE::fini();
	delete task_t;
	task_t = 0;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
// BasicFrame: 
// The implementation of this class

//	ids for BasicFrame items
	enum
	{
		TIMER_ID
	};

BEGIN_EVENT_TABLE(BasicFrame, wxFrame)
		EVT_CLOSE(BasicFrame::OnClose)
		EVT_PAINT(BasicFrame::OnPaint)
		EVT_LEFT_DOWN(BasicFrame::OnLButtonDown)
		EVT_RIGHT_DOWN(BasicFrame::OnRButtonDown)
		EVT_TIMER(TIMER_ID, BasicFrame::OnTimer)
END_EVENT_TABLE()

// ctor
BasicFrame::BasicFrame (const wxChar *title, wxSize& size)
          : wxFrame ((wxFrame *) NULL,
 		     -1,
 		     title, 
 		     wxDefaultPosition, 
 		     size, 
 		     wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX
                 	 )
          , m_timer(this, TIMER_ID)
{
	SetIcon(wxICON(demo));
	Centre();

}

// dtor
BasicFrame::~BasicFrame()
{
	_GM->Exit ();
	ReleaseDC(m_hWnd, m_hDC);
}

void
BasicFrame::Init()
{
	// TODO:
	_GM->Initialize ();
	m_hWnd = (HWND)GetHandle();
	m_hDC = GetDC(m_hWnd);
	TCHAR str[256];
	swprintf(str, _T("%s/bgmusic.mid"), VOC_PATH);	
	if (m_hMCI != NULL) {
		MCIWndClose (m_hMCI);
		MCIWndDestroy (m_hMCI);
		m_hMCI = NULL;
	}
	m_hMCI = MCIWndCreate (NULL, NULL, WS_POPUP | //WS_VISIBLE |
		               MCIWNDF_NOPLAYBAR|MCIWNDF_NOMENU,
		               str);
	MCIWndPlay (m_hMCI);
	swprintf(str, _T("%s/bgimage.bmp"), PIC_PATH);	
	_GM->LoadBMP (str);
	SelectObject(_GM->_mapdc, _GM->_hbit);
	_GM->LoadData();

	int tx=SCR_WIDTH/2-150;
	int ty=SCR_HEIGHT/2-80;

	m_timer.Start(100);    // 0.1 second interval
}

void
BasicFrame::OnClose(wxCloseEvent& event)
{
	if (::wxMessageBox (_T("确认是否离开？"), 
 			    _T("退出"), 
			    wxYES_NO | wxICON_QUESTION ) == wxYES)
	{
		event.Skip();
	}
}

void 
BasicFrame::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
	// TODO:
}

void 
BasicFrame::OnTimer(wxTimerEvent& event)
{
    // TODO: do whatever you want to do every second here
	if(MCIWndGetPosition(m_hMCI) >=MCIWndGetLength(m_hMCI))
		MCIWndPlay(m_hMCI);	//背景音乐

	//为防止闪屏，所有对象先显示在暂存区DCBK1，一屏完成后再翻转到主屏上。
	BitBlt(_GM->_mapdc_t, 0, 0, SCR_WIDTH, SCR_HEIGHT, _GM->_mapdc, 0, 0, SRCCOPY);	//用地图 _mapdc 刷新 _mapdc_t

	for(int i=0; i<_GM->obj.size(); i++){
		if(_GM->obj[i].action==4)continue;
		if(!_GM->SpriteShow(i)){
			m_timer.Stop ();	
			::MessageBox(NULL, _T("资源读取失败!"), _T("错误"), MB_OK);
			return;
		}

		TCHAR cc[256];
		_itow( _GM->obj[i].PID, cc, 10);
//		1+_T("0")
		SetTextColor(_GM->_mapdc_t, RGB(255,0,0));
		SetBkMode(_GM->_mapdc_t, TRANSPARENT);
		TextOut(_GM->_mapdc_t, _GM->obj[i].x - 5, _GM->obj[i].y + 5, cc, lstrlen(cc));
//		SpriteFight(i);
	}
	
	SetTextColor(_GM->_mapdc_t, RGB(0,0,255));
	for (int j=0; j<_GM->chat.size(); j++)
		TextOut(_GM->_mapdc_t, 0, SCR_HEIGHT*0.5 + j*16, _GM->chat[j].c_str(), _GM->chat[j].size());
	BitBlt(m_hDC, 0, 0, SCR_WIDTH, SCR_HEIGHT, _GM->_mapdc_t, 0, 0, SRCCOPY);	//将 _mapdc_t 的内容显示到当前窗口
}

void 
BasicFrame::OnLButtonDown(wxMouseEvent& event)
{
	int x = event.GetX(), y = event.GetY();
	int n = _GM->GetItemByPID (_GM->selfPID);
	if (n == -1) {
		wxMessageBox(_T("item not found."));
		return;
	}

	PERSONAE direct;
	direct.hd.identifier = identifier + 4;
	direct.hd.length = sizeof(PERSONAE) - sizeof(MsgHead);
	direct.hd.req_type = REQ_DIRECT;
	direct.hd.retry_times = 0;
	direct.hd.type = MSG_REQUEST;

	direct.PID = _GM->selfPID;
	direct.kind = _GM->obj[n].kind;
	direct.tx = x;
	direct.ty = y;
	direct.azimuth = _GM->GetAzimuth (x-_GM->obj[n].x, y-_GM->obj[n].y);

	ACE_Message_Block *mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof (PERSONAE)));
	mb->copy ((char*)&direct, sizeof(PERSONAE));
	mb->wr_ptr(sizeof(PERSONAE));

	if(client != 0)
	{
		client->start_write(mb);
	}
}

void 
BasicFrame::OnRButtonDown(wxMouseEvent& event)
{
	auto_ptr<ChatDlg> dlgChat (new ChatDlg (this));
	dlgChat->ShowModal ();
	if (dlgChat->m_chat->GetValue().IsEmpty())
		return;

	if (_tcscmp(dlgChat->m_chat->GetValue(), _T("/cls")) == 0) {
		_GM->chat.clear();
		return;
	}

	RTChat chat;
	chat.hd.identifier = identifier + 5;
	chat.hd.length = sizeof(ReqChat) - sizeof(MsgHead);
	chat.hd.req_type = REQ_CHAT;
	chat.hd.retry_times = 0;
	chat.hd.type = MSG_REQUEST;

	TCHAR cc[256];
	_itow(_GM->selfPID, cc, 10);
	_tcscat(cc, _T(": "));
	_tcscat(cc, dlgChat->m_chat->GetValue());
	_tcscpy(chat.message, cc);

	ACE_Message_Block *mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof (RTChat)));
	mb->copy ((char*)&chat, sizeof(RTChat));
	mb->wr_ptr(sizeof(RTChat));

	if(client != 0)
	{
		client->start_write(mb);
	}
}

/////////////////////////////////////////////////////////////////////////////
// LogonDlg:
// The implementation of this class

BEGIN_EVENT_TABLE (LogonDlg, wxDialog)	
	EVT_BUTTON (wxID_HELP, LogonDlg::onHelp)
	EVT_BUTTON (wxID_OK, LogonDlg::onClickAffirm)
END_EVENT_TABLE()

// ctor
LogonDlg:: LogonDlg(wxWindow *parent)
			: wxDialog (parent, wxID_ANY, wxEmptyString, 
				    wxDefaultPosition, wxDefaultSize)
			, m_server (0)	
			, m_user (0)
			, m_word (0)
{
	/*	对话框原型如下：
	------------------------------------------------
	欢迎您来到多人游戏Demo...
	------------------------------------------------
	
			服务器：|                          		|	 
			帐号：	|                          		|	 
			密码：  |                          		|	 
	 ----------------------------------------------
	[帮助]															[登录] [退出] 
	*/

	SetTitle (_T("用户登录"));

	wxBoxSizer *dlgSizer = new wxBoxSizer (wxVERTICAL);

	dlgSizer->Add (new wxStaticText (this, wxID_ANY, 
		       _T("欢迎您来到多人游戏Demo")),
		       0, wxALL | wxALIGN_CENTRE, DEF_SPACE);

	wxFlexGridSizer *flexSizer = new wxFlexGridSizer(2, 2, 
																									 DEF_SPACE, DEF_SPACE);
	dlgSizer->Add (flexSizer, 1, wxLEFT | wxRIGHT | wxEXPAND, DEF_SPACE);
	flexSizer->AddGrowableCol (1,1);

#define DEF_TEXTCTRL_LENGTH	SCR_WIDTH*0.5
	flexSizer->SetMinSize (DEF_TEXTCTRL_LENGTH, 
			       flexSizer->GetMinSize().GetHeight());

	// 服务器
	flexSizer->Add (new wxStaticText(this, wxID_ANY, 
					_T("服务器：")), 
	     		0, wxALIGN_CENTRE_VERTICAL);
	m_server = new wxTextCtrl(this, wxID_ANY, _T("127.0.0.1"));
	flexSizer->Add(m_server, 0, wxALIGN_CENTRE_VERTICAL | wxEXPAND);

	// 账号
	flexSizer->Add (new wxStaticText(this, wxID_ANY, 
					_T("账号：")), 
			0, wxALIGN_CENTRE_VERTICAL);
	m_user = new wxTextCtrl(this, IDC_TXT_USERNAME, _T("test"));
	flexSizer->Add(m_user, 0, wxALIGN_CENTRE_VERTICAL | wxEXPAND);

	// 密码
	flexSizer->Add (new wxStaticText(this, wxID_ANY, 
					_T("密码：")), 
			0, wxALIGN_CENTRE_VERTICAL);
	m_word = new wxTextCtrl(this, wxID_ANY, _T("123456"), 
													wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	flexSizer->Add(m_word, 0, wxALIGN_CENTRE_VERTICAL | wxEXPAND);

	dlgSizer->Add (new wxStaticLine (this), 
				   0, 
				   wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 
				   DEF_SPACE);

	wxBoxSizer *btnSizer = new wxBoxSizer (wxHORIZONTAL);
	dlgSizer->Add (btnSizer, 0, wxALL | wxEXPAND, 0);

	// 帮助
	wxBoxSizer *helpSizer = new wxBoxSizer (wxHORIZONTAL);
	btnSizer->Add (helpSizer, 1, wxALL | wxALIGN_LEFT, DEF_SPACE);
	wxButton *help = new wxButton (this, wxID_HELP, 
																 _T("帮助"));
	helpSizer->Add (help, 0, wxEXPAND, DEF_SPACE);

	// 登录，退出
	wxBoxSizer *otherSizer = new wxBoxSizer (wxHORIZONTAL);
	btnSizer->Add (otherSizer, 0, wxALL | wxALIGN_RIGHT, DEF_SPACE);	
	wxButton *affirmBtn = new wxButton (this, wxID_OK, 
															_T("登录"));
	wxButton *cancelBtn = new wxButton (this, wxID_CANCEL, 
																			_T("退出"));
	
	otherSizer->Add (affirmBtn, 0, wxLEFT | wxEXPAND, DEF_SPACE);
	otherSizer->Add (cancelBtn, 0, wxLEFT | wxEXPAND, DEF_SPACE);

	affirmBtn->SetDefault ();

	SetSizer (dlgSizer);		
	dlgSizer->Fit (this);
	dlgSizer->SetSizeHints (this);
	Centre ();
}

// dtor
LogonDlg::~LogonDlg() 
{
}

// protected
void 
LogonDlg::onHelp (wxCommandEvent& event)
{
	::wxMessageBox (_T("暂未提供帮助功能！"));
}

// protected
void 
LogonDlg::onClickAffirm (wxCommandEvent& event)
{
	try {
		wxBusyCursor busy;

	  if (m_server->GetValue().IsEmpty()) {
	  	::wxMessageBox (_T("服务器名不能为空!"));
	  }
	  else if (m_user->GetValue().IsEmpty()) {
	  	::wxMessageBox (_T("账号不能为空!"));
		}
		else {
			// TODO: 连接并发送登录信息到服务器
			if(!connected)
			{
				ACE_INET_Addr addr;
				addr.set(PROACTOR_PORT, m_server->GetValue ());//ACE_LOCALHOST);
				connector.start(addr);

				ACE_Time_Value tv;
				tv.msec (TIMEOUT);	
				BasicApplication* app = (BasicApplication*)wxApp::GetInstance();
				if (app->_connEvent.wait (&tv, 0) == -1) {
					wxMessageBox(_T("无法连接到服务器!"));
					return;
				}
			}
			if(! connected) {
				wxMessageBox(_T("无法连接到服务器!"));
				return;
			}
		
			ReqLogin login;
		
			login.hd.identifier = identifier;
			login.hd.length = sizeof(ReqLogin) - sizeof(MsgHead);
			login.hd.req_type = REQ_LOGIN;
			login.hd.retry_times = 0;
			login.hd.type = MSG_REQUEST;
		
			_tcscpy(login.name, m_user->GetValue ());
			_tcscpy(login.key, m_word->GetValue ());
		
			ACE_Message_Block *mb = 0;
			ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof (ReqLogin)));
			mb->copy ((char*)&login, sizeof(ReqLogin));
			mb->wr_ptr(sizeof(ReqLogin));
		
			if(client != 0)
			{
				client->start_write(mb);
			}

{
			ReqLogin login;
		
			login.hd.identifier = identifier;
			login.hd.length = sizeof(ReqLogin) - sizeof(MsgHead);
			login.hd.req_type = REQ_LOGIN;
			login.hd.retry_times = 0;
			login.hd.type = MSG_REQUEST;
		
			_tcscpy(login.name, m_user->GetValue ());
			_tcscpy(login.key, m_word->GetValue ());
		
			ACE_Message_Block *mb = 0;
			ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof (ReqLogin)));
			mb->copy ((char*)&login, sizeof(ReqLogin));
			mb->wr_ptr(sizeof(ReqLogin));
		
			if(client != 0)
			{
				client->start_write(mb);
			}

}
			wxDialog::OnOK (event);
		}
	}
	catch (...) {
		throw;
	}	
}

/////////////////////////////////////////////////////////////////////////////
// ChatDlg:
// The implementation of this class

BEGIN_EVENT_TABLE (ChatDlg, wxDialog)	
	EVT_TEXT_ENTER (IDC_TXT_CHAT, OnEnter)
END_EVENT_TABLE()

ChatDlg::ChatDlg(wxWindow *parent)
	    : wxDialog(parent, wxID_ANY, wxEmptyString,
	               wxDefaultPosition, wxDefaultSize,
	               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
		, m_chat (0)	
{
	wxBoxSizer *dlgSizer = new wxBoxSizer (wxHORIZONTAL);

// TODO:
#define DEF_TEXTCTRL_WIDTH	SCR_WIDTH*0.7
	dlgSizer->SetMinSize(DEF_TEXTCTRL_WIDTH, dlgSizer->GetMinSize().GetHeight());

	// 交谈
	dlgSizer->Add(new wxStaticText (this, wxID_ANY, 
					_T("交谈：")), 
					0, wxALIGN_CENTRE_VERTICAL);
	m_chat = new wxTextCtrl (this, IDC_TXT_CHAT, 
				 wxEmptyString, 
				 wxDefaultPosition, 
				 wxDefaultSize, 
				 wxTE_PROCESS_ENTER);
	dlgSizer->Add (m_chat, 1, wxALIGN_CENTRE_VERTICAL | wxEXPAND);

	SetSizer (dlgSizer);		
	dlgSizer->Fit (this);
	Centre ();
}

ChatDlg::~ChatDlg() 
{
}

void
ChatDlg::OnEnter(wxCommandEvent& event)
{
	Close(true);
}
