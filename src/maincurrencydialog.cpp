/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2015 Gabriele-V
 Copyright (C) 2016 Nikolay Akimov

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

#include "maincurrencydialog.h"
#include "currencydialog.h"
#include "constants.h"
#include "defs.h"
#include "images_list.h"
#include "mmTextCtrl.h"
#include "mmSimpleDialogs.h"
#include "paths.h"
#include "util.h"
#include "option.h"
#include "validators.h"
#include "Model_CurrencyHistory.h"
#include "Model_Infotable.h"

#include <vector>
#include <wx/dataview.h>

wxIMPLEMENT_DYNAMIC_CLASS ( mmMainCurrencyDialog, wxDialog );

wxBEGIN_EVENT_TABLE ( mmMainCurrencyDialog, wxDialog )
    EVT_BUTTON ( wxID_ADD, mmMainCurrencyDialog::OnBtnAdd )
    EVT_BUTTON ( wxID_EDIT, mmMainCurrencyDialog::OnBtnEdit )
    EVT_BUTTON ( wxID_SELECTALL, mmMainCurrencyDialog::OnBtnSelect )
    EVT_BUTTON ( wxID_REMOVE, mmMainCurrencyDialog::OnBtnDelete )
    EVT_BUTTON ( wxID_CANCEL, mmMainCurrencyDialog::OnCancel )

    EVT_MENU_RANGE ( MENU_ITEM1, MENU_ITEM3, mmMainCurrencyDialog::OnMenuSelected )

    EVT_DATAVIEW_ITEM_ACTIVATED ( wxID_ANY, mmMainCurrencyDialog::OnListItemActivated )
    EVT_DATAVIEW_SELECTION_CHANGED ( wxID_ANY, mmMainCurrencyDialog::OnListItemSelected )
    EVT_DATAVIEW_ITEM_CONTEXT_MENU ( wxID_ANY, mmMainCurrencyDialog::OnItemRightClick )

    EVT_BUTTON ( HISTORY_ADD, mmMainCurrencyDialog::OnHistoryAdd )
    EVT_BUTTON ( HISTORY_DELETE, mmMainCurrencyDialog::OnHistoryDelete )
    EVT_BUTTON ( HISTORY_UPDATE, mmMainCurrencyDialog::OnHistoryUpdate )
    EVT_BUTTON ( HISTORY_DELUNUSED, mmMainCurrencyDialog::OnHistoryDeleteUnused )
    EVT_LIST_ITEM_SELECTED ( wxID_ANY, mmMainCurrencyDialog::OnHistorySelected )
    EVT_LIST_ITEM_DESELECTED ( wxID_ANY, mmMainCurrencyDialog::OnHistoryDeselected )
wxEND_EVENT_TABLE()

mmMainCurrencyDialog::mmMainCurrencyDialog ( wxWindow *parent, int currencyID, bool bEnableSelect)
    : bEnableSelect_ ( bEnableSelect )
{
    ColName_[CURR_BASE]   = _( "Base" );
    ColName_[CURR_SYMBOL] = _( "Symbol" );
    ColName_[CURR_NAME]   = _( "Name" );
    ColName_[BASE_RATE]   = _( "Last Rate" );
    ColName_[CURR_HIST] = _( "Hist." );
    m_currency_id = currencyID == -1 ? Option::instance().getBaseCurrencyID() : currencyID;
    constexpr long style = wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCLOSE_BOX;
    Create ( parent, wxID_ANY, _( "Currency Dialog" ), wxDefaultPosition, wxDefaultSize, style );
}

bool mmMainCurrencyDialog::Create ( wxWindow *parent
    , wxWindowID id
    , const wxString &caption
    , const wxPoint &pos
    , const wxSize &size
    , long style )
{
    SetExtraStyle ( GetExtraStyle() |wxWS_EX_BLOCK_EVENTS );
    const bool bret = wxDialog::Create ( parent, id, caption, pos, size, style );

    if ( bret == false )
    {
        return false;
    }

    CreateControls();
    SetIcon ( mmex::getProgramIcon() );
    fillControls();
    Centre();
    return true;
}

void mmMainCurrencyDialog::fillControls()
{
    currencyListBox_->DeleteAllItems();
    itemButtonEdit_->Disable();
    itemButtonDelete_->Disable();
    const int baseCurrencyID = Option::instance( ).getBaseCurrencyID( );
    cbShowAll_->SetValue ( !Model_Infotable::instance().GetBoolInfo ( "SHOW_HIDDEN_CURRENCIES", true ) );
    cbHideHistoric_->SetValue ( Model_Infotable::instance().GetBoolInfo ( "HIDE_HISTORIC_CURRENCIES", true ) );
    const bool skip_unused = cbShowAll_->IsChecked( );
    const bool skip_historic = cbHideHistoric_->IsChecked( );
    const wxString today = wxDate::Today().FormatISODate();

    for ( const auto &currency : Model_Currency::instance().all ( Model_Currency::COL_CURRENCYNAME ) )
    {
        const int currencyID = currency.CURRENCYID;
        const bool is_base_currency = ( baseCurrencyID == currencyID );

        if ( skip_unused && !is_base_currency && !Model_Account::is_used ( currency ) )
        {
            continue;
        }

        const bool is_currency_historical = ( currency.HISTORIC == 1 );

        if ( skip_historic && is_currency_historical )
        {
            continue;
        }

        wxVector<wxVariant> data;
        data.push_back ( wxVariant ( is_base_currency ? L"\u2691" : L"" ) );
        data.push_back ( wxVariant ( currency.CURRENCY_SYMBOL ) );
        data.push_back ( wxVariant ( wxGetTranslation ( currency.CURRENCYNAME ) ) );
        data.push_back ( wxVariant ( wxString() <<Model_CurrencyHistory::getLastRate ( currencyID, today ) ) );
        data.push_back ( wxVariant ( is_currency_historical ? L"\u2713" : L"" ) );
        currencyListBox_->AppendItem ( data, static_cast<wxUIntPtr> ( currencyID ) );

        if ( m_currency_id == currencyID )
        {
            currencyListBox_->SelectRow ( currencyListBox_->GetItemCount() - 1 );
            itemButtonEdit_->Enable();

            if ( !skip_unused )
            {
                itemButtonDelete_->Enable ( !is_base_currency && !Model_Account::is_used ( currency ) );
            }
        }
    }

    //Ensure that the selected item is visible.
    const wxDataViewItem item ( currencyListBox_->GetCurrentItem() );
    currencyListBox_->EnsureVisible ( item );
}

void mmMainCurrencyDialog::CreateControls()
{
    wxBoxSizer *mainBoxSizer = new wxBoxSizer ( wxHORIZONTAL );
    this->SetSizer ( mainBoxSizer );
    wxBoxSizer *itemBoxSizer2 = new wxBoxSizer ( wxVERTICAL );
    mainBoxSizer->Add ( itemBoxSizer2, g_flagsExpand );
    wxBoxSizer *itemBoxSizer22 = new wxBoxSizer ( wxHORIZONTAL );
    itemBoxSizer2->Add ( itemBoxSizer22, wxSizerFlags ( g_flagsExpand ).Proportion ( 0 ) );
    wxBitmapButton *update_button = new wxBitmapButton ( this
        , wxID_STATIC, mmBitmap ( png::CURRATES ) );
    itemBoxSizer22->Add ( update_button, g_flagsH );
    update_button->Connect ( wxID_STATIC, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler ( mmMainCurrencyDialog::OnOnlineUpdateCurRate ), nullptr, this );
    update_button->SetToolTip ( _( "Update latest rate only for all used currencies" ) );
    itemBoxSizer22->Add ( new wxStaticText ( this, wxID_STATIC
            , _( "Online Update" ) ), g_flagsH );
    itemBoxSizer22->AddSpacer ( 10 );
    cbShowAll_ = new wxCheckBox ( this, wxID_SELECTALL, _( "Hide Unused" ), wxDefaultPosition
        , wxDefaultSize, wxCHK_2STATE );
    cbShowAll_->SetToolTip ( _( "Hide currencies unused in this database" ) );
    cbShowAll_->Connect ( wxID_SELECTALL, wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler ( mmMainCurrencyDialog::OnShowHiddenChbClick ), nullptr, this );
    itemBoxSizer22->Add ( cbShowAll_, g_flagsH );
    itemBoxSizer22->AddSpacer ( 10 );
    cbHideHistoric_ = new wxCheckBox ( this, wxID_SELECTALL, _( "Hide Historic" ), wxDefaultPosition
        , wxDefaultSize, wxCHK_2STATE );
    cbHideHistoric_->SetToolTip ( _( "Hide historical currencies" ) );
    cbHideHistoric_->Connect ( wxID_SELECTALL, wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler ( mmMainCurrencyDialog::OnHideHistoricClick ), nullptr, this );
    itemBoxSizer22->Add ( cbHideHistoric_, g_flagsH );
    wxBoxSizer *itemBoxSizer3 = new wxBoxSizer ( wxHORIZONTAL );
    itemBoxSizer2->Add ( itemBoxSizer3, g_flagsExpand );
    //TODO:provide proper style and fix validator (does not working)
    currencyListBox_ = new wxDataViewListCtrl ( this
        , wxID_ANY, wxDefaultPosition, wxSize ( -1, 200 ) /*, wxDV_HORIZ_RULES, mmDoubleValidator(4)*/ );
    currencyListBox_->AppendTextColumn ( ColName_[CURR_BASE], wxDATAVIEW_CELL_INERT, 30 );
    currencyListBox_->AppendTextColumn ( ColName_[CURR_SYMBOL], wxDATAVIEW_CELL_INERT, 60
        , wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE );
    currencyListBox_->AppendTextColumn ( ColName_[CURR_NAME], wxDATAVIEW_CELL_INERT, 140
        , wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE );
    currencyListBox_->AppendTextColumn ( ColName_[BASE_RATE], wxDATAVIEW_CELL_INERT, 80 );
    currencyListBox_->AppendTextColumn ( ColName_[CURR_HIST], wxDATAVIEW_CELL_INERT, 30 );
    itemBoxSizer3->Add ( currencyListBox_, g_flagsExpand );
    wxPanel *buttonsPanel = new wxPanel ( this, wxID_ANY );
    itemBoxSizer2->Add ( buttonsPanel, wxSizerFlags ( g_flagsV ).Center() );
    wxBoxSizer *buttonsSizer = new wxBoxSizer ( wxVERTICAL );
    buttonsPanel->SetSizer ( buttonsSizer );
    wxStdDialogButtonSizer *itemBoxSizer66 = new wxStdDialogButtonSizer;
    buttonsSizer->Add ( itemBoxSizer66 );
    wxButton *itemButton7 = new wxButton ( buttonsPanel, wxID_ADD, _( "&Add " ) );
    itemBoxSizer66->Add ( itemButton7, g_flagsH );
    itemButtonEdit_ = new wxButton ( buttonsPanel, wxID_EDIT, _( "&Edit " ) );
    itemBoxSizer66->Add ( itemButtonEdit_, g_flagsH );
    itemButtonEdit_->Disable();
    itemButtonDelete_ = new wxButton ( buttonsPanel, wxID_REMOVE, _( "&Remove " ) );
    itemBoxSizer66->Add ( itemButtonDelete_, g_flagsH );
    itemButtonDelete_->Disable();
    wxStdDialogButtonSizer *itemBoxSizer9 = new wxStdDialogButtonSizer;
    buttonsSizer->Add ( itemBoxSizer9, wxSizerFlags ( g_flagsExpand ).Border ( wxALL, 0 ) );
    wxButton *itemButtonSelect = new wxButton ( buttonsPanel, wxID_SELECTALL, _( "&Select" ) );
    itemBoxSizer9->Add ( itemButtonSelect, wxSizerFlags ( g_flagsExpand ).Proportion ( 4 ) );

    if ( Model_Account::instance().all().size() > 0 )
    {
        itemButtonSelect->SetToolTip ( _( "Select the currently selected currency as the selected currency for the account" ) );
    }
    else
    {
        itemButtonSelect->SetToolTip ( _( "Select the currently selected currency as base currency" ) );
    }

    if ( bEnableSelect_ == false )
    {
        itemButtonSelect->Disable();
    }

    //Some interfaces has no any close buttons, it may confuse user. Cancel button added
    wxButton *itemCancelButton = new wxButton ( buttonsPanel, wxID_CANCEL, wxGetTranslation ( g_CloseLabel ) );
    itemBoxSizer9->Add ( itemCancelButton, g_flagsH );
    itemCancelButton->SetFocus();
    //History Panel
    wxBoxSizer *rightBoxSizer = new wxBoxSizer ( wxVERTICAL );
    mainBoxSizer->Add ( rightBoxSizer, g_flagsExpand );
    historyStaticBox_ = new wxStaticBox ( this, wxID_ANY, _( "Currency History Options" ) );
    wxStaticBoxSizer *historyStaticBox_Sizer = new wxStaticBoxSizer ( historyStaticBox_, wxVERTICAL );
    rightBoxSizer->Add ( historyStaticBox_Sizer, g_flagsExpand );
    valueListBox_ = new wxListCtrl ( this, wxID_ANY, wxDefaultPosition, wxSize ( 250, 150 )
        , wxLC_REPORT );
    historyStaticBox_Sizer->Add ( valueListBox_, g_flagsExpand );
    wxListItem col0, col1;
    // Add first column
    col0.SetId ( 0 );
    col0.SetText ( _( "Date" ) );
    col0.SetWidth ( 90 );
    valueListBox_->InsertColumn ( 0, col0 );
    // Add second column
    col1.SetId ( 1 );
    col1.SetText ( _( "Value" ) );
    col1.SetAlign ( wxLIST_FORMAT_RIGHT );
    col1.SetWidth ( 100 );
    valueListBox_->InsertColumn ( 1, col1 );
    //History Buttons
    wxPanel *values_panel = new wxPanel ( this, wxID_ANY );
    historyStaticBox_Sizer->Add ( values_panel, wxSizerFlags ( g_flagsV ).Centre() );
    wxStdDialogButtonSizer  *values_sizer = new wxStdDialogButtonSizer;
    values_panel->SetSizer ( values_sizer );
    values_sizer->Add ( new wxStaticText ( values_panel, wxID_STATIC, _( "Date" ) ), g_flagsH );
    valueDatePicker_ = new wxDatePickerCtrl ( values_panel, wxID_ANY, wxDefaultDateTime,
        wxDefaultPosition, wxSize ( 120, -1 ), wxDP_DROPDOWN | wxDP_SHOWCENTURY );
    values_sizer->Add ( valueDatePicker_, g_flagsH );
    valueDatePicker_->SetToolTip ( _( "Specify the date of currency value" ) );
    values_sizer->Add ( new wxStaticText ( values_panel, wxID_STATIC, _( "Value" ) ), g_flagsH );
    valueTextBox_ = new mmTextCtrl ( values_panel, wxID_ANY, wxGetEmptyString()
        , wxDefaultPosition, wxSize ( 120, -1 ), wxALIGN_RIGHT | wxTE_PROCESS_ENTER
        , mmCalcValidator() );
    valueTextBox_->SetToolTip ( _( "Enter the currency value" ) );
    values_sizer->Add ( valueTextBox_, g_flagsH );
    wxPanel *buttons_panel = new wxPanel ( this, wxID_ANY );
    historyStaticBox_Sizer->Add ( buttons_panel, wxSizerFlags ( g_flagsV ).Centre() );
    wxStdDialogButtonSizer  *buttons_sizer = new wxStdDialogButtonSizer;
    buttons_panel->SetSizer ( buttons_sizer );
    m_button_download_history = new wxBitmapButton ( buttons_panel, HISTORY_UPDATE, mmBitmap ( png::CURRATES ) );
    m_button_download_history->SetToolTip ( _( "Download Currency Values history" ) );
    historyButtonAdd_ = new wxButton ( buttons_panel, HISTORY_ADD, _( "&Add / Update " )
        , wxDefaultPosition, wxSize ( -1, m_button_download_history->GetSize().GetY() ) );
    historyButtonAdd_->SetToolTip ( _( "Add Currency Values to history" ) );
    historyButtonDelete_ = new wxButton ( buttons_panel, HISTORY_DELETE, _( "&Delete " )
        , wxDefaultPosition, wxSize ( -1, m_button_download_history->GetSize().GetY() ) );
    historyButtonDelete_->SetToolTip ( _( "Delete selected Currency Values" ) );
    wxBitmapButton *buttonDelUnusedHistory = new wxBitmapButton ( buttons_panel
        , HISTORY_DELUNUSED, mmBitmap ( png::VOID_STAT ) );
    buttonDelUnusedHistory->SetToolTip ( _( "Delete Currency Values history for unused currencies" ) );
    buttons_sizer->Add ( m_button_download_history, g_flagsH );
    buttons_sizer->Add ( historyButtonAdd_, g_flagsH );
    buttons_sizer->Add ( historyButtonDelete_, g_flagsH );
    buttons_sizer->Add ( buttonDelUnusedHistory, g_flagsH );
    this->SetMinSize ( wxSize ( 800,550 ) );
    this->Fit();
}

void mmMainCurrencyDialog::OnBtnAdd ( wxCommandEvent &WXUNUSED ( event ) )
{
    mmCurrencyDialog dlg ( this, nullptr );

    if ( dlg.ShowModal() == wxID_OK )
    {
        m_currency_id = dlg.getCurrencyID();
    }

    fillControls();
}

void mmMainCurrencyDialog::OnBtnEdit ( wxCommandEvent &WXUNUSED ( event ) )
{
    const Model_Currency::Data *const currency = Model_Currency::instance().get ( m_currency_id );

    if ( currency )
    {
        mmCurrencyDialog ( this, currency ).ShowModal();
    }

    fillControls();
}

void mmMainCurrencyDialog::OnBtnSelect ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( m_currency_id > -1 )
    {
        EndModal ( wxID_OK );
    }
}

void mmMainCurrencyDialog::OnBtnDelete ( wxCommandEvent &WXUNUSED ( event ) )
{
    const int selected_index = currencyListBox_->GetSelectedRow();

    if ( selected_index < 0 )
    {
        return;
    }

    Model_Currency::Data *currency = Model_Currency::instance().get ( m_currency_id );

    if ( !currency )
    {
        return;
    }

    if ( wxMessageBox ( _( "Do you really want to delete the selected Currency?" )
            , _( "Currency Dialog" )
            , wxYES_NO | wxNO_DEFAULT | wxICON_ERROR ) == wxYES )
    {
        Model_Currency::instance().remove ( m_currency_id );
        m_currency_id = -1;
        fillControls();
    }
}

bool mmMainCurrencyDialog::Execute ( wxWindow *parent, int &currencyID )
{
    mmMainCurrencyDialog dlg ( parent, currencyID );
    const bool ok = dlg.ShowModal() == wxID_OK;
    currencyID = dlg.m_currency_id;
    dlg.Destroy();
    return ( ok );
}

bool mmMainCurrencyDialog::Execute ( int &currencyID )
{
    bool result = false;
    mmMainCurrencyDialog dlg ( nullptr, currencyID );
    dlg.m_static_dialog = true;
    dlg.SetTitle ( _( "Base Currency Selection" ) );
    dlg.valueListBox_->Enable ( false );
    dlg.valueTextBox_->Enable ( false );
    dlg.valueDatePicker_->Enable ( false );
    dlg.historyButtonAdd_->Enable ( false );
    dlg.historyButtonDelete_->Enable ( false );

    if ( dlg.ShowModal() == wxID_OK )
    {
        currencyID = dlg.m_currency_id;
        result = true;
    }

    dlg.Destroy();
    return result;
}

void mmMainCurrencyDialog::OnListItemSelected ( wxDataViewEvent &event )
{
    const int selected_index = currencyListBox_->GetSelectedRow();
    wxString currName = "";

    if ( selected_index >= 0 )
    {
        const wxDataViewItem item = event.GetItem();
        m_currency_id = static_cast<int> ( currencyListBox_->GetItemData ( item ) );
        Model_Currency::Data *currency = Model_Currency::instance().get ( m_currency_id );

        if ( currency )
        {
            currName = currency->CURRENCYNAME;
            itemButtonEdit_->Enable();
            wxString base_currency_symbol;
            Model_Currency::GetBaseCurrencySymbol ( base_currency_symbol );
            m_button_download_history->Enable ( currency->CURRENCY_SYMBOL != base_currency_symbol );
        }
    }

    if ( !bEnableSelect_ )  // prevent user deleting currencies when editing accounts.
    {
        const int baseCurrencyID = Option::instance().getBaseCurrencyID();
        Model_Currency::Data *currency = Model_Currency::instance().get ( m_currency_id );

        if ( currency )
        {
            itemButtonDelete_->Enable ( !Model_Account::is_used ( currency ) && m_currency_id != baseCurrencyID );
            currName = currency->CURRENCYNAME;
        }
    }

    historyStaticBox_->SetLabel ( wxString::Format ( _( "Currency History Options: %s " ), wxGetTranslation ( currName ) ) );
    valueDatePicker_->SetValue ( wxDateTime::Today() );
    valueTextBox_->SetValue ( wxEmptyString );
    ShowCurrencyHistory();
}

void mmMainCurrencyDialog::OnListItemActivated ( wxDataViewEvent &WXUNUSED ( event ) )
{
    wxCommandEvent evt ( wxEVT_COMMAND_BUTTON_CLICKED, wxID_ANY );

    if ( bEnableSelect_ )
    {
        OnBtnSelect ( evt );
    }
    else
    {
        OnBtnEdit ( evt );
    }
}

bool mmMainCurrencyDialog::OnlineUpdateCurRate ( int curr_id, bool hide )
{
    wxString msg = wxEmptyString;
    const bool ok = GetOnlineCurrencyRates ( msg, curr_id, cbShowAll_->IsChecked() );

    if ( ok )
    {
        if ( !hide )
        {
            wxMessageDialog msgDlg ( this, msg, _( "Online update currency rate" ) );
            msgDlg.ShowModal();
        }

        fillControls();
        ShowCurrencyHistory();
    }
    else
    {
        wxMessageDialog msgDlg ( this, msg, _( "Error" ), wxOK | wxICON_ERROR );
        msgDlg.ShowModal();
    }

    return ok;
}

void mmMainCurrencyDialog::OnOnlineUpdateCurRate ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( !m_static_dialog )  //Abort when trying to set base currency
    {
        OnlineUpdateCurRate ( -1, false );
    }
}

void mmMainCurrencyDialog::OnMenuSelected ( wxCommandEvent &event )
{
    switch ( event.GetId() )
    {
        case MENU_ITEM1:
        {
            if ( SetBaseCurrency ( m_currency_id ) )
            {
                itemButtonDelete_->Disable();
                fillControls();
                ShowCurrencyHistory();
            }
        }
        break;

        case MENU_ITEM2:
        {
            OnlineUpdateCurRate ( m_currency_id, false );
            break;
        }

        case MENU_ITEM3:
        {
            wxCommandEvent e;
            this->OnBtnEdit ( e );
        }
        break;
    }
}

void mmMainCurrencyDialog::OnItemRightClick ( wxDataViewEvent &event )
{
    // disable this function when setting base currency
    if ( m_static_dialog )
    {
        return;
    }

    wxCommandEvent ev ( wxEVT_COMMAND_MENU_SELECTED, wxID_ANY );
    ev.SetEventObject ( this );
    wxMenu *mainMenu = new wxMenu;
    mainMenu->Append ( new wxMenuItem ( mainMenu, MENU_ITEM1, _( "Set as Base Currency" ) ) );
    mainMenu->Append ( new wxMenuItem ( mainMenu, MENU_ITEM2, _( "Online Update Currency Rate" ) ) );
    mainMenu->Append ( new wxMenuItem ( mainMenu, MENU_ITEM3, _( "&Edit " ) ) );
    const int baseCurrencyID = Option::instance().getBaseCurrencyID();

    if ( baseCurrencyID == m_currency_id )
    {
        mainMenu->Enable ( MENU_ITEM1, false );
    }

    PopupMenu ( mainMenu );
    delete mainMenu;
    event.Skip();
}

void mmMainCurrencyDialog::OnShowHiddenChbClick ( wxCommandEvent &WXUNUSED ( event ) )
{
    Model_Infotable::instance().Set ( "SHOW_HIDDEN_CURRENCIES", !cbShowAll_->IsChecked() );
    fillControls();
}

void mmMainCurrencyDialog::OnHideHistoricClick ( wxCommandEvent &WXUNUSED ( event ) )
{
    Model_Infotable::instance().Set ( "HIDE_HISTORIC_CURRENCIES", cbHideHistoric_->IsChecked() );
    fillControls();
}

void mmMainCurrencyDialog::OnCancel ( wxCommandEvent &WXUNUSED ( event ) )
{
    EndModal ( wxID_CANCEL );
}

void mmMainCurrencyDialog::ShowCurrencyHistory()
{
    if ( m_static_dialog )
    {
        return;    //Abort when trying to set base currency
    }

    valueListBox_->DeleteAllItems();
    const int baseCurrencyID = Option::instance().getBaseCurrencyID();

    if ( m_currency_id <= 0 || m_currency_id == baseCurrencyID )
    {
        historyButtonAdd_->Disable();
        historyButtonDelete_->Disable();
        return;
    }
    else
    {
        historyButtonAdd_->Enable();
        historyButtonDelete_->Enable();
    }

    Model_CurrencyHistory::Data_Set histData = Model_CurrencyHistory::instance().find ( Model_CurrencyHistory::CURRENCYID ( m_currency_id ) );
    std::stable_sort ( histData.begin(), histData.end(), SorterByCURRDATE() );
    std::reverse ( histData.begin(), histData.end() );

    if ( !histData.empty() )
    {
        int idx = 0;

        for ( const auto &d : histData )
        {
            wxListItem item;
            item.SetId ( idx );
            item.SetData ( d.CURRHISTID );
            valueListBox_->InsertItem ( item );
            valueListBox_->SetItem ( idx, 0, mmGetDateForDisplay ( d.CURRDATE ) );
            valueListBox_->SetItem ( idx, 1, wxString::Format ( "%f", d.CURRVALUE ) );
            idx++;
        }

        valueListBox_->RefreshItems ( 0, --idx );
    }
}

void mmMainCurrencyDialog::OnHistoryAdd ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( m_currency_id <= 0 )
    {
        return;
    }

    wxString listStr;
    double dPrice = 0.0;
    Model_Currency::Data *currency = Model_Currency::instance().get ( m_currency_id );
    wxString currentPriceStr = valueTextBox_->GetValue().Trim();

    if ( !Model_Currency::fromString ( currentPriceStr, dPrice, currency ) || ( dPrice < 0.0 ) )
    {
        return;
    }

    Model_CurrencyHistory::instance().addUpdate ( m_currency_id, valueDatePicker_->GetValue(), dPrice, Model_CurrencyHistory::MANUAL );
    fillControls();
    ShowCurrencyHistory();
}

void mmMainCurrencyDialog::OnHistoryDelete ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( valueListBox_->GetSelectedItemCount() <= 0 )
    {
        return;
    }

    long item = -1;
    Model_CurrencyHistory::instance().Savepoint();

    for ( ;; )
    {
        item = valueListBox_->GetNextItem ( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

        if ( item == -1 )
        {
            break;
        }

        Model_CurrencyHistory::instance().remove ( static_cast<int> ( valueListBox_->GetItemData ( item ) ) );
    }

    Model_CurrencyHistory::instance().ReleaseSavepoint();
    fillControls();
    ShowCurrencyHistory();
}

void mmMainCurrencyDialog::OnHistoryUpdate ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( m_static_dialog )
    {
        return;    //Abort when trying to set base currency
    }

    Model_Currency::Data *CurrentCurrency = Model_Currency::instance().get ( m_currency_id );

    if ( !CurrentCurrency )
    {
        return mmErrorDialogs::MessageError ( this, _( "No currency selected!" ), _( "Currency history error" ) );
    }

    wxString base_currency_symbol;
    wxASSERT_MSG ( Model_Currency::GetBaseCurrencySymbol ( base_currency_symbol ), "Could not find base currency symbol" );
    wxString msg;
    std::map<wxDateTime, double> historical_rates;
    const bool UpdStatus = GetOnlineHistory ( historical_rates, CurrentCurrency->CURRENCY_SYMBOL, msg );

    if ( !UpdStatus )
    {
        return mmErrorDialogs::MessageError ( this
                , wxString::Format ( _( "Unable to download %s currency rates" )
                    , CurrentCurrency->CURRENCY_SYMBOL )
                , _( "Currency history error" ) );
    }

    const int msgResult = wxMessageBox ( _( "Do you want to add also dates without any transaction?" )
            , _( "Currency Dialog" ), wxYES_NO | wxNO_DEFAULT | wxICON_ERROR );

    if ( msgResult != wxYES && msgResult != wxNO )
    {
        return;
    }

    bool CheckDate = msgResult == wxNO;

    const bool Found = !historical_rates.empty();

    if ( Found )
    {
        Model_CurrencyHistory::instance().Savepoint();

        if ( CheckDate )
        {
            const std::map<wxDateTime, int> DatesList = Model_Currency::DateUsed ( m_currency_id );

            for ( const auto entry : DatesList )
            {
                wxLogDebug ( "%s %s",  entry.first.FormatISODate(), entry.first.FormatISOTime() );

                if ( historical_rates.find ( entry.first ) != historical_rates.end() )
                {
                    const wxDate date = entry.first;
                    wxLogDebug ( "%s", date.FormatISODate() );
                    Model_CurrencyHistory::instance().addUpdate ( m_currency_id, date, historical_rates[date], Model_CurrencyHistory::ONLINE );
                }
            }
        }
        else
        {
            for ( auto &entry : historical_rates )
            {
                Model_CurrencyHistory::instance().addUpdate ( m_currency_id, entry.first, entry.second, Model_CurrencyHistory::ONLINE );
            }
        }

        Model_CurrencyHistory::instance().ReleaseSavepoint();
        fillControls();
        ShowCurrencyHistory();
    }
    else
        mmErrorDialogs::MessageError ( this
            , wxString::Format ( _( "Unable to download history for symbol %s. History rates not available!" )
                , CurrentCurrency->CURRENCY_SYMBOL )
            , _( "Currency history error" ) );
}

void mmMainCurrencyDialog::OnHistoryDeleteUnused ( wxCommandEvent &WXUNUSED ( event ) )
{
    if ( m_static_dialog )
    {
        return;    //Abort when trying to set base currency
    }

    Model_CurrencyHistory::instance().Savepoint();
    auto currencies = Model_Currency::instance().all();

    for ( auto &currency : currencies )
    {
        if ( !Model_Account::is_used ( currency ) )
        {
            for ( const auto &r : Model_CurrencyHistory::instance().find ( Model_CurrencyHistory::CURRENCYID ( currency.CURRENCYID ) ) )
            {
                Model_CurrencyHistory::instance().remove ( r.id() );
            }
        }
        else
        {
            std::map<wxDateTime, int> DatesList = Model_Currency::DateUsed ( currency.CURRENCYID );
            wxDateTime CurrDate;

            for ( const auto &r : Model_CurrencyHistory::instance().find ( Model_CurrencyHistory::CURRENCYID ( currency.CURRENCYID ) ) )
            {
                CurrDate.ParseDate ( r.CURRDATE );

                if ( DatesList.find ( CurrDate ) == DatesList.end() )
                {
                    Model_CurrencyHistory::instance().remove ( r.id() );
                }
            }
        }
    }

    Model_CurrencyHistory::instance().ReleaseSavepoint();
    //fillControls();
    ShowCurrencyHistory();
}

void mmMainCurrencyDialog::OnHistorySelected ( wxListEvent &event )
{
    const long selectedIndex = event.GetIndex();
    const long histId = valueListBox_->GetItemData ( selectedIndex );
    Model_CurrencyHistory::Data *histData = Model_CurrencyHistory::instance().get ( histId );

    if ( histData->CURRHISTID > 0 )
    {
        valueDatePicker_->SetValue ( Model_CurrencyHistory::CURRDATE ( *histData ) );
        valueTextBox_->SetValue ( wxString::Format ( "%f", histData->CURRVALUE ) );
    }
}

void mmMainCurrencyDialog::OnHistoryDeselected ( wxListEvent &WXUNUSED ( event ) )
{
    valueDatePicker_->SetValue ( wxDateTime::Today() );
    valueTextBox_->SetValue ( wxEmptyString );
}

bool mmMainCurrencyDialog::SetBaseCurrency ( const int baseCurrencyID )
{
    const int baseCurrencyOLD = Option::instance().getBaseCurrencyID();

    if ( baseCurrencyID == baseCurrencyOLD )
    {
        return true;
    }

    if ( Model_CurrencyHistory::instance().all().size() > 0 )
    {
        if ( wxMessageBox ( _( "Setting a new base currency will delete all historic rates,\n"
                    "it's possible to re-download them after setting the new base currency." ) + "\n\n"
                + _( "Do you want to continue?" ), _( "Currency Dialog" ), wxYES_NO | wxYES_DEFAULT | wxICON_ERROR )
            != wxYES )
        {
            return false;
        }
    }

    Option::instance().setBaseCurrencyID ( baseCurrencyID );
    Model_CurrencyHistory::instance().ResetCurrencyHistory();
    ShowCurrencyHistory();
    return true;
}

bool mmMainCurrencyDialog::GetOnlineHistory ( std::map<wxDateTime, double> &historical_rates
    , const wxString &symbol, wxString &msg )
{
    wxString base_currency_symbol;

    if ( !Model_Currency::GetBaseCurrencySymbol ( base_currency_symbol ) )
    {
        msg = _( "Could not find base currency symbol!" );
        return false;
    }

    const wxString URL = wxString::Format ( mmex::weblink::YahooQuotesHistory
            , wxString::Format ( "%s%s=X", symbol, base_currency_symbol )
            , "5y", "1d" ); //TODO: ask range and interval
    wxString json_data;
    auto err_code = http_get_data ( URL, json_data );

    if ( err_code != CURLE_OK )
    {
        msg = json_data;
        return false;
    }

    Document json_doc;

    if ( json_doc.Parse ( json_data.c_str() ).HasParseError() )
    {
        return false;
    }

    if ( !json_doc.HasMember ( "chart" ) || !json_doc["chart"].IsObject() )
    {
        return false;
    }

    Value chart = json_doc["chart"].GetObject();
    wxASSERT ( chart.HasMember ( "error" ) );

    if ( !chart.HasMember ( "error" ) || !chart["error"].IsNull() )
    {
        return false;
    }

    if ( !chart.HasMember ( "result" ) || !chart["result"].IsArray() )
    {
        return false;
    }

    Value result = chart["result"].GetArray();

    if ( !result.IsArray() || !result.Begin()->IsObject() )
    {
        return false;
    }

    Value data = result.Begin()->GetObject();

    if ( !data.HasMember ( "timestamp" ) || !data["timestamp"].IsArray() )
    {
        return false;
    }

    Value timestamp = data["timestamp"].GetArray();

    if ( !data.HasMember ( "indicators" ) || !data.IsObject() )
    {
        return false;
    }

    Value indicators = data["indicators"].GetObject();

    if ( !indicators.HasMember ( "adjclose" ) || !indicators["adjclose"].IsArray() )
    {
        return false;
    }

    Value quote_array = indicators["adjclose"].GetArray();
    Value quotes = quote_array.Begin()->GetObject();

    if ( !quotes.HasMember ( "adjclose" ) || !quotes["adjclose"].IsArray() )
    {
        return false;
    }

    Value quotes_closed = quotes["adjclose"].GetArray();

    if ( timestamp.Size() != quotes_closed.Size() )
    {
        return false;
    }

    std::map<wxDate, double> history;
    const wxDateTime today = wxDateTime::Today();
    wxDateTime first_date = today;
    double first_price = 0;
    bool only_1 = true;

    for ( rapidjson::SizeType i = 0; i < timestamp.Size(); i++ )
    {
        wxASSERT ( timestamp[i].IsInt() );
        const auto time = wxDateTime ( static_cast<time_t> ( timestamp[i].GetInt() ) ).GetDateOnly();

        if ( quotes_closed[i].IsFloat() )
        {
            double rate = quotes_closed[i].GetFloat();
            history[time] = rate;

            if ( first_date > time )
            {
                first_date = time;
                first_price = rate;
            }

            if ( rate != 1 )
            {
                only_1=false;
            }
        }
        else
        {
            wxLogDebug ( "%s %s", time.FormatISODate(), wxDateTime::GetWeekDayName ( time.GetWeekDay() ) );
        }
    }

    // Skip rates = 1 (Yahoo returns 1 with invalid Symbols)
    if ( only_1 )
    {
        return false;
    }

    double closed_price = first_price;

    for ( wxDateTime i = first_date; i < today; i.Add ( wxDateSpan::Days ( 1 ) ) )
    {
        wxLogDebug ( "Date: %s %s",i.FormatISODate(), i.FormatISOTime() );
        double rate = closed_price;

        if ( history.find ( i ) != history.end() )
        {
            rate = history[i];
            closed_price = rate;
        }

        historical_rates[i] = rate;
    }

    wxLogDebug ( "Date: %s %s Today: %s %s"
        , first_date.FormatISODate(), first_date.FormatISOTime()
        , today.FormatISODate(), today.FormatISOTime() );
    return true;
}
