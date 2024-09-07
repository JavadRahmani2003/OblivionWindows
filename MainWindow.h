#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/timer.h>
#include <wx/process.h>
#include <wx/utils.h>
#include <wx/msw/registry.h>
#include <wx/taskbar.h>
#include <wx/socket.h>

#include "wintun/include/wintun.h"

//Main Variables -> memmbered in mainframe void
wxString AppName = "OblivionWindows";

wxRegKey ChkThme(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize\\");
wxRegKey SysChkThme(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize\\");

wxStaticText* myText = nullptr;

wxPanel* MainPanel = nullptr;

wxButton* CreateNewConnection = nullptr;//add connection
wxButton* RemoveOldConnection = nullptr;//remove connection

wxCheckBox* ScanUse = nullptr;
wxCheckBox* ShowDriverLog = nullptr;

wxRadioButton* dnsOnly = nullptr;
wxRadioButton* VPNOnly = nullptr;

wxString StatusConnection = "Disconnected";

wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);

bool ProxyIsOn = false;

wxString Args = "";

wxSocketClient* m_socket = nullptr;

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MainFrame : public wxFrame
{
public:
    MainFrame();
private:
    void CreateConnection(wxCommandEvent& event);
    void RemoveConnection(wxCommandEvent& event);
    void QuitFromTaskbar(wxTaskBarIconEvent& event);
    void TaskbarLeftClick(wxTaskBarIconEvent& event);
    void CloseToHide(wxCloseEvent& event);
    void ChangeColorApp(wxSysColourChangedEvent& event);
    void ConnectToServer(wxSocketEvent& event);
};

wxIMPLEMENT_APP(MyApp);
