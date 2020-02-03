/*******************************************************
 Copyright (C) 2017 Nikolay Akimov

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

#ifndef MM_EX_TRANSACTIONSUPDATE_H_
#define MM_EX_TRANSACTIONSUPDATE_H_

#include "defs.h"
#include "mmcombobox.h"
class mmTextCtrl;

class transactionsUpdateDialog : public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS(transactionsUpdateDialog);
    wxDECLARE_EVENT_TABLE();

public:
    transactionsUpdateDialog();
    ~transactionsUpdateDialog();
    transactionsUpdateDialog(wxWindow* parent, int account_id, std::vector<int>& transaction_id);

private:
    bool Create(wxWindow* parent
                , wxWindowID id
                , const wxString& caption
                , const wxPoint& pos
                , const wxSize& size
                , long style);

    void CreateControls();
    void OnOk(wxCommandEvent& event);
    void OnCheckboxClick(wxCommandEvent& event);
    void onFocusChange(wxChildFocusEvent& event);
    void OnCategChange(wxCommandEvent& event);

private:
    wxCheckBox* m_payee_checkbox = nullptr;
    mmComboBox* m_payee = nullptr;
    wxCheckBox* m_date_checkbox = nullptr;
    wxDatePickerCtrl* m_dpc = nullptr;
    wxCheckBox* m_status_checkbox = nullptr;
    wxChoice* m_status_choice = nullptr;
    wxCheckBox* m_categ_checkbox = nullptr;
    wxButton* m_categ_btn = nullptr;
    wxCheckBox* m_type_checkbox = nullptr;
    wxChoice* m_type_choice = nullptr;
    wxCheckBox* m_amount_checkbox = nullptr;
    mmTextCtrl* m_amount_ctrl = nullptr;
    wxCheckBox* m_notes_checkbox = nullptr;
    wxCheckBox* m_append_checkbox = nullptr;
    wxTextCtrl* m_notes_ctrl = nullptr;
    std::vector<int> m_transaction_id;
    Model_Currency::Data* m_currency=nullptr;
    int m_categ_id = -1;
    int m_subcateg_id = -1;
};

#endif
