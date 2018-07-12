/********************************************************
** filename: test.h
** Copyright (c) 2006, Hunan Agriculture University.
**
** Author: zdx (zdxster AT gmail DOT com)
**
** Date: 2006/05/08
**
** Description: the header file of game demo.
*********************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include "engine.h"
#include "ace/Auto_Event.h"
#include "connector.h"

#include <vfw.h>
#pragma comment (lib,"vfw32.lib")

// Define a new application type
class BasicApplication : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();

public:
	//
	// 连接以及重新连接时使用的对象，它的默认状态未信号状态，直到
	//	收到连接或重新连接回复后被通知。
	//
	ACE_Auto_Event _connEvent;
	bool _success;
};

// Define a new frame type
class BasicFrame : public wxFrame
{
public:
	BasicFrame( const wxChar *title, wxSize& size);
	~BasicFrame();

  // event handlers
	void OnClose(wxCloseEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnLButtonDown(wxMouseEvent& event);
	void OnRButtonDown(wxMouseEvent& event);
	void OnTimer(wxTimerEvent& event);

	// custom functions
	void Init();

private:
	HWND m_hWnd;
	HDC m_hDC;
	HWND m_hMCI;
	wxTimer m_timer;

  DECLARE_EVENT_TABLE()
};

// class LogonDlg 登录对话框
class LogonDlg : public wxDialog
{
public:
	LogonDlg (wxWindow *parent);
	~LogonDlg();

protected:
	void onHelp (wxCommandEvent& /* event */);
	void onClickAffirm (wxCommandEvent& /*event*/);

public:
	wxTextCtrl* m_server;
	wxTextCtrl* m_user;
	wxTextCtrl* m_word;
	Connector connector;

	enum
	{
		IDC_BTN_AFFIRM,
		IDC_TXT_USERNAME
	};

	DECLARE_EVENT_TABLE()
};

class ChatDlg : public wxDialog
{
public:
	ChatDlg(wxWindow *parent);
	~ChatDlg();

public:
	wxTextCtrl* m_chat;

protected:
	void OnEnter (wxCommandEvent& /* event */);

private:
	enum
	{
		IDC_TXT_CHAT
	};

	DECLARE_EVENT_TABLE()
};

#endif
