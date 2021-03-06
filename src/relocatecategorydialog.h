/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Stefano Giorgio

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

#ifndef MM_EX_RELOCATECATEGORYDIALOG_H_
#define MM_EX_RELOCATECATEGORYDIALOG_H_

#include <wx/dialog.h>
class wxButton;
class wxStaticText;
class wxButton;

class relocateCategoryDialog : public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS ( relocateCategoryDialog );
    wxDECLARE_EVENT_TABLE();

public:
    relocateCategoryDialog();
    relocateCategoryDialog ( wxWindow *parent,const int sourceCatID = -1, const int sourceSubCatID = -1 );

    bool Create ( wxWindow *parent, wxWindowID id, const wxString &caption, const wxPoint &pos, const wxSize &size, long style );

    void CreateControls();

    // utility functions
    void OnSelectSource ( wxCommandEvent &event );
    void OnSelectDest ( wxCommandEvent &event );
    void OnOk ( wxCommandEvent &event );

    int updatedCategoriesCount() const;

private:
    void IsOkOk();
    int m_sourceCatID = -1;
    int m_sourceSubCatID = -1;
    wxButton *m_buttonSource = nullptr;
    wxStaticText *m_info = nullptr;

    int m_destCatID = -1;
    int m_destSubCatID = -1;
    wxButton *m_buttonDest=nullptr;
    int m_changedRecords = 0;
};

#endif

