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

#include "budgetyeardialog.h"
#include "budgetyearentrydialog.h"
#include "util.h"
#include "paths.h"
#include "constants.h"
#include "model/Model_Budgetyear.h"

wxIMPLEMENT_DYNAMIC_CLASS(mmBudgetYearDialog, wxDialog);

wxBEGIN_EVENT_TABLE( mmBudgetYearDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmBudgetYearDialog::OnOk)
    EVT_BUTTON(wxID_ADD, mmBudgetYearDialog::OnAdd)
    EVT_BUTTON(ID_ADD_MONTH, mmBudgetYearDialog::OnAddMonth)
    EVT_BUTTON(wxID_DELETE, mmBudgetYearDialog::OnDelete)
wxEND_EVENT_TABLE()

mmBudgetYearDialog::mmBudgetYearDialog( )
: m_listBox()
{
    budgetYearID_ = -1;
}

mmBudgetYearDialog::mmBudgetYearDialog(wxWindow* parent)
{
    budgetYearID_ = -1;

    long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX;
    Create(parent, wxID_ANY, _t("Budget Editor"), wxDefaultPosition, wxSize(500, 300), style);
}

bool mmBudgetYearDialog::Create(wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos,
    const wxSize& size, long style)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();
    fillControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    this->SetInitialSize();
    SetIcon(mmex::getProgramIcon());
    Centre();
    return true;
}

void mmBudgetYearDialog::fillControls()
{
    int index = 0;
    for (const auto& e: Model_Budgetyear::instance().all(Model_Budgetyear::COL_BUDGETYEARNAME))
    {
        const wxString& payeeString = e.BUDGETYEARNAME;
        int64 budgetYearID = e.BUDGETYEARID;
        m_listBox->Insert(payeeString, index++, new mmListBoxItem(budgetYearID, payeeString));
    }
}

void mmBudgetYearDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW | wxALL, 5);

    m_listBox = new wxListBox(this, wxID_ANY
        , wxDefaultPosition, wxSize(100, 200));
    itemBoxSizer3->Add(m_listBox, 1, wxGROW | wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 1, wxGROW|wxALL, 5);

    wxButton* itemButton7 = new wxButton(this, wxID_ADD
        , _t("Add &Year"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL);
    mmToolTip(itemButton7, _t("Add a new budget year"));

    wxButton* itemBudgetMonth = new wxButton(this, ID_ADD_MONTH
        , _t("Add &Month"));
    itemBoxSizer5->Add(itemBudgetMonth, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    mmToolTip(itemBudgetMonth, _t("Add a new budget month"));

    wxButton* itemButtonDelete = new wxButton(this, wxID_DELETE
        , _t("&Delete "));
    itemBoxSizer5->Add(itemButtonDelete, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    mmToolTip(itemButtonDelete, _t("Delete existing budget"));

    wxStaticLine* line = new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    itemBoxSizer2->Add(line, 0, wxGROW | wxALL, 5);

    wxPanel* itemPanel25 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer2->Add(itemPanel25, 0, wxALIGN_RIGHT, 5);

    wxBoxSizer* itemButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemButtonSizer);

    wxButton* itemButtonOK = new wxButton(itemPanel25, wxID_OK, _t("&OK "));
    itemButtonSizer->Add(itemButtonOK, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxBOTTOM, 5);

    wxButton* itemButtonCancel = new wxButton(itemPanel25, wxID_CANCEL, wxGetTranslation(g_CancelLabel));
    itemButtonSizer->Add(itemButtonCancel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxBOTTOM, 5);
    itemButtonCancel->SetFocus();
}

void mmBudgetYearDialog::OnAdd(wxCommandEvent& /*event*/)
{
    mmBudgetYearEntryDialog dlg(this); 
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_listBox->Clear();
        fillControls();
    }
}
 
void mmBudgetYearDialog::OnAddMonth(wxCommandEvent& /*event*/)
{
    mmBudgetYearEntryDialog dlg(this, true); 
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_listBox->Clear();
        fillControls();
    }
}

void mmBudgetYearDialog::OnDelete(wxCommandEvent& /*event*/)
{
    wxString budgetYearString = m_listBox->GetStringSelection();
    int64 budgetYearID = Model_Budgetyear::instance().Get(budgetYearString);
    Model_Budgetyear::instance().remove(budgetYearID);
    m_listBox->Clear();
    fillControls();
}
 
void mmBudgetYearDialog::OnOk(wxCommandEvent& /*event*/)
{
    EndModal(wxID_OK);
}

