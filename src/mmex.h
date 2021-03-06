/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

//----------------------------------------------------------------------------
#ifndef MM_EX_MMEX_H_
#define MM_EX_MMEX_H_
//----------------------------------------------------------------------------
#include <wx/app.h>
#include <wx/debugrpt.h>
#include <wx/snglinst.h>

//----------------------------------------------------------------------------
class mmGUIFrame;
class wxSQLite3Database;
//----------------------------------------------------------------------------

class mmGUIApp : public wxApp
{
public:
    mmGUIApp();

    wxLanguage getGUILanguage() const;
    bool setGUILanguage ( wxLanguage lang );
    mmGUIFrame *m_frame = nullptr;
    const wxString getOptParam() const
    {
        return m_optParam;
    }
    wxSQLite3Database *getSettingDB() const noexcept
    {
        return m_setting_db;
    }
    void setSettingDB ( wxSQLite3Database *db ) noexcept
    {
        m_setting_db = db;
    }

private:
    void OnInitCmdLine ( wxCmdLineParser &parser ) override;
    bool OnCmdLineParsed ( wxCmdLineParser &parser ) override;
    void reportFatalException ( wxDebugReport::Context );
    bool OnInit() override;
    int OnExit() override;
    void OnFatalException() override; // called when a crash occurs in this application
    void HandleEvent ( wxEvtHandler *handler, wxEventFunction func, wxEvent &event ) const override;
    wxSingleInstanceChecker *m_checker = nullptr;

    wxSQLite3Database *m_setting_db = nullptr;
    wxString m_optParam = wxEmptyString;
    wxLanguage m_lang=wxLANGUAGE_UNKNOWN; // GUI translation language displayed
    wxLocale m_locale;
public:
    int FilterEvent ( wxEvent &event ) override;
};

//----------------------------------------------------------------------------
wxDECLARE_APP ( mmGUIApp );
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#endif // MM_EX_MMEX_H_
//----------------------------------------------------------------------------
