/*******************************************************
Copyright (C) 2006-2012

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
#pragma once

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filtertransdialog.cpp"
#endif

#include <wx/dialog.h>
#include "Model_Checking.h"
#include "Model_Billsdeposits.h"
#include "reports/mmDateRange.h"
#include <wx/bmpbuttn.h>

class mmTextCtrl;
class mmCustomData;
class mmHTMLBuilder;
class wxCheckBox;
class wxChoice;
class wxDatePickerCtrl;
class wxComboBox;
class wxRadioBox;

class mmFilterTransactionsDialog: public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS ( mmFilterTransactionsDialog );
    wxDECLARE_EVENT_TABLE();

public:
    /// Constructors
    mmFilterTransactionsDialog();
    ~mmFilterTransactionsDialog();
    mmFilterTransactionsDialog ( wxWindow *parent, int account_id );

    virtual int ShowModal();

    bool checkAll ( const Model_Checking::Full_Data &tran, int accountID );
    bool checkAll ( const Model_Billsdeposits::Full_Data &tran );
    void getDescription ( mmHTMLBuilder &hb, bool html = true );
    bool SomethingSelected();
    void ResetFilterStatus();
    void setAccountToolTip ( const wxString &tip ) const;
    bool getStatusCheckBox();
    bool getAccountCheckBox();
    int getAccountID() const noexcept;
    bool getCategoryCheckBox();

    bool getSimilarStatus() const noexcept;
    int getCategId() const noexcept;
    int getSubCategId() const noexcept;

private:
    void getFilterStatus();
    void BuildPayeeList();

    bool getDateRangeCheckBox();
    bool getAmountRangeCheckBox();

    template<class MODEL, class FULL_DATA = typename MODEL::Full_Data>
    bool checkPayee ( const FULL_DATA &tran );
    template<class MODEL, class FULL_DATA = typename MODEL::Full_Data>
    bool checkCategory ( const FULL_DATA &tran );
    template<class MODEL, class FULL_DATA = typename MODEL::Full_Data>
    bool checkAmount ( const FULL_DATA &tran );

    wxString getNumber();
    wxString getNotes();

private:
    /// Returns true if Status string matches.
    bool compareStatus ( const wxString &itemStatus, const wxString &filterStatus ) const;

    bool getTypeCheckBox();
    bool allowType ( const wxString &typeState, bool sameAccount ) const;
    bool getPayeeCheckBox();
    bool getNumberCheckBox();
    bool getNotesCheckBox();
    void clearSettings();

    /// Creation
    bool Create ( wxWindow *parent
                  , wxWindowID id
                  , const wxString &caption
                  , const wxPoint &pos
                  , const wxSize &size
                  , long style );

    /// Creates the controls and sizers
    void CreateControls();
    void dataToControls();
    const wxString GetStoredSettings ( int id );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOXACCOUNT
    void OnCheckboxClick ( wxCommandEvent &event );

    void OnButtonokClick ( wxCommandEvent &event );
    void OnButtoncancelClick ( wxCommandEvent &event );
    void SaveSettings ( int menu_item );
    void OnButtonClearClick ( wxCommandEvent &event );
    void OnMoreFields ( wxCommandEvent &event );
    void OnPayeeUpdated ( wxCommandEvent &event );
    void OnTextEntered ( wxCommandEvent &event );
    void OnDateRangeChanged ( wxCommandEvent &event );
    void OnSaveSettings ( wxCommandEvent &event );
    void OnSettingsSelected ( wxCommandEvent &event );

    void OnCategs ( wxCommandEvent &event );
    wxString to_json ( bool i18n = false );
    void from_json ( const wxString &data );

    wxString settings_string_;
    wxString prev_value_;
    wxCheckBox *accountCheckBox_ = nullptr;
    wxChoice *accountDropDown_ = nullptr;
    wxCheckBox *m_dateRangeCheckBox = nullptr;
    wxDatePickerCtrl *m_fromDateCtrl = nullptr;
    wxDatePickerCtrl *m_toDateCtrl = nullptr;
    wxCheckBox *payeeCheckBox_ = nullptr;
    wxComboBox *cbPayee_ = nullptr;
    wxCheckBox *categoryCheckBox_ = nullptr;
    wxButton *btnCategory_ = nullptr;
    wxBitmapButton *m_btnSaveAs = nullptr;
    wxChoice *m_setting_name = nullptr;
    wxCheckBox *similarCategCheckBox_ = nullptr;
    wxCheckBox *statusCheckBox_ = nullptr;
    wxChoice *choiceStatus_ = nullptr;
    wxCheckBox *typeCheckBox_ = nullptr;
    wxCheckBox *cbTypeWithdrawal_ = nullptr;
    wxCheckBox *cbTypeDeposit_ = nullptr;
    wxCheckBox *cbTypeTransferTo_ = nullptr;
    wxCheckBox *cbTypeTransferFrom_ = nullptr;
    wxCheckBox *amountRangeCheckBox_ = nullptr;
    mmTextCtrl *amountMinEdit_ = nullptr;
    mmTextCtrl *amountMaxEdit_ = nullptr;
    wxCheckBox *notesCheckBox_ = nullptr;
    wxTextCtrl *notesEdit_ = nullptr;
    wxCheckBox *transNumberCheckBox_ = nullptr;
    wxTextCtrl *transNumberEdit_ = nullptr;
    wxChoice *m_date_ranges = nullptr;

    wxString m_begin_date;
    wxString m_end_date;
    int categID_=-1;
    int subcategID_=-1;
    bool bSimilarCategoryStatus_ = false;
    int payeeID_ = -1;
    wxString payeeStr_;
    int refAccountID_ = -1;
    wxString refAccountStr_;
    double m_min_amount = 0.0;
    double m_max_amount = 0.0;
    wxString m_filterStatus;

    mmCustomData *m_custom_fields = nullptr;
    std::vector<mmDateRange *> m_all_date_ranges;

    enum RepPanel
    {
        ID_CHOICE_DATE_RANGE = wxID_HIGHEST + 1,
    };

};
