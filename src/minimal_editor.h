#pragma once
#include <wx/stc/stc.h>

class MinimalEditor : public wxStyledTextCtrl
{
    wxDECLARE_DYNAMIC_CLASS ( MinimalEditor );
    wxDECLARE_EVENT_TABLE();

public:
    MinimalEditor() : wxStyledTextCtrl()
    {
        return;
    }
    explicit MinimalEditor ( wxWindow *parent, wxWindowID id = wxID_ANY );
    bool SetFont ( const wxFont &font ) override;
    void SetLexerSql();
    void SetLexerLua();
    void SetLexerHtml();
protected:
    void OnMarginClick ( wxStyledTextEvent & );
    void OnText ( wxStyledTextEvent & );
    wxFont m_font;
};
