#include "MainWindow.h"

//The Main Frame Show
bool MyApp::OnInit()
{
    MainFrame* frame = new MainFrame();

    wxIcon icon("AppIcon.ico",wxBITMAP_TYPE_ICO);
    frame->SetIcon(icon);
    frame->Show(true);
    frame->Center();

    SetTopWindow(frame);
    return true;
}

enum
{
    StatusText,
    NewConnection,
    RemoveConnections,
    DriverLog,
    ScanArg,
    GoolArg,
    DnsMode,
    VpnMode
};

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, AppName, wxDefaultPosition, wxSize(500, 600), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    MainPanel = new wxPanel(this);

    m_socket = new wxSocketClient();
    m_socket->SetEventHandler(*this,wxID_ANY);
    m_socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_LOST_FLAG);
    m_socket->Notify(true);

    myText = new wxStaticText(MainPanel, StatusText, "Status: " + StatusConnection, wxDefaultPosition, wxDefaultSize);
    myText->SetFont(wxFont(36, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    myText->Centre();
    Sizer->Add(myText, 0, wxALIGN_CENTRE | wxALIGN_CENTER, 0);
    MainPanel->SetSizer(Sizer);

    ShowDriverLog = new wxCheckBox(MainPanel,DriverLog,"Show Driver Logs?",wxPoint(160,100),wxSize(200,20));
    ShowDriverLog->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    ShowDriverLog->SetToolTip("Please Set this On if you want make sure the vpn is fully working");

    CreateNewConnection = new wxButton(MainPanel, NewConnection, "Connect", wxPoint(180, 120), wxSize(140, 60));
    CreateNewConnection->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    RemoveOldConnection = new wxButton(MainPanel, RemoveConnections, "Disconnect", wxPoint(180, 185), wxSize(140,60));
    RemoveOldConnection->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    ScanUse = new wxCheckBox(MainPanel, ScanArg, "Use Random Servers - \" This option is for \nDNS Mode and VPN mode \" ", wxPoint(140, 245), wxDefaultSize);
    
    dnsOnly = new wxRadioButton(MainPanel, DnsMode, "DNS Mode (Bypass websites we blocked them)", wxPoint(140,280), wxDefaultSize);
    VPNOnly = new wxRadioButton(MainPanel, VpnMode, "VPN Mode (Bypass websites they blocked \nour region)", wxPoint(140,308), wxDefaultSize);

    //Default Settings
    ShowDriverLog->SetValue(wxCHK_CHECKED);
    ScanUse->SetValue(wxCHK_CHECKED);
    VPNOnly->SetValue(wxCHK_CHECKED);

    wxTaskBarIcon* taskbarBtn = new wxTaskBarIcon();
    wxIcon icon("AppIcon.ico", wxBITMAP_TYPE_ICO);
    taskbarBtn->SetIcon(icon);
    taskbarBtn->Bind(wxEVT_TASKBAR_LEFT_DCLICK, &MainFrame::TaskbarLeftClick, this);
    taskbarBtn->Bind(wxEVT_TASKBAR_RIGHT_DOWN, &MainFrame::QuitFromTaskbar, this);

    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::CloseToHide));
    Connect(wxEVT_SYS_COLOUR_CHANGED, wxSysColourChangedEventHandler(MainFrame::ChangeColorApp));

    Bind(wxEVT_SOCKET, &MainFrame::ConnectToServer, this);

    CreateNewConnection->Bind(wxEVT_BUTTON, &MainFrame::CreateConnection, this);
    RemoveOldConnection->Bind(wxEVT_BUTTON, &MainFrame::RemoveConnection,this);
}

// The Events and Functions
void MainFrame::CreateConnection(wxCommandEvent& event)
{
    if (ScanUse->IsChecked())
    {
        Args += " --scan ";
    }
    //******************************************
    if (VPNOnly->GetValue() == true)
    {
        Args += " --gool "; // Goes to Gool or vpn mode
    }
    else
    {
        Args += " "; // Goes to dns mode
    }
    //******************************************
    if (ShowDriverLog->IsChecked())
    {
        wxExecute("Driver.exe -v" + Args);
    }
    else
    {
        wxExecute("Driver.exe", wxEXEC_HIDE_CONSOLE);
    }
    //******************************************
    if (ProxyIsOn == false)
    {
        wxExecute("DriverLauncher.bat", wxEXEC_HIDE_CONSOLE);
        ProxyIsOn = true;
    }
    else
    {
        wxMessageBox("Proxy is ALREADY On!", "PROXY : Error 0x1", wxOK | wxICON_ERROR);
    }
    //******************************************
    StatusConnection = "Connecting...";
    myText->SetLabel("Status: " + StatusConnection);

    myText->Refresh();
}

void MainFrame::RemoveConnection(wxCommandEvent& event)
{
    Args = "";
    this->Layout();
    if (ProxyIsOn == true)
    {
        wxExecute("DriverLauncher.bat", wxEXEC_HIDE_CONSOLE);
        ProxyIsOn = false;
    }
    else
    {
        wxMessageBox("Proxy is ALREADY Off!", "PROXY : Error 0x0", wxOK | wxICON_ERROR);
    }

    wxExecute("taskkill /f /im Driver.exe /t", wxEXEC_HIDE_CONSOLE);

    StatusConnection = "Disconnected";
    myText->SetLabel("Status: " + StatusConnection);
    myText->Refresh();
}

void MainFrame::TaskbarLeftClick(wxTaskBarIconEvent& event)
{
    Show(true);
}

void MainFrame::QuitFromTaskbar(wxTaskBarIconEvent& event)
{
    wxExecute("taskkill /f /im "+AppName+".exe /t",wxEXEC_HIDE_CONSOLE);
}

void SetStatusTextToConnected()
{
    StatusConnection = "Connected";
    myText->SetLabel("Status: " + StatusConnection);
    myText->Refresh();
}

void MainFrame::CloseToHide(wxCloseEvent& event)
{
    Hide();
    event.Veto();
}

void MainFrame::ChangeColorApp(wxSysColourChangedEvent& event)
{
    wxSystemAppearance ChangeAppTheme = wxSystemSettings::GetAppearance();
    if (ChangeAppTheme.IsUsingDarkBackground())
    {
        MainPanel->SetBackgroundColour(wxColor(30, 30, 30));
        MainPanel->SetForegroundColour(wxColor(250, 250, 250));
    }
    else
    {
        MainPanel->SetBackgroundColour(wxColor(250, 250, 250));
        MainPanel->SetForegroundColour(wxColor(30, 30, 30));
    }
}

void MainFrame::ConnectToServer(wxSocketEvent& event)
{
    switch (event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
        myText->SetLabel("Connected");
        break;
    case wxSOCKET_LOST:
        myText->SetLabel("Connection lost!");
        break;
    }
}