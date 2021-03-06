/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011, 2012 Stefano Giorgio
 Copyright (C) 2013, 2014 Nikolay

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

#ifndef MM_EX_CHECKINGPANEL_H_
#define MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------
#include "mmpanelbase.h"
#include "constants.h"
#include "reports/mmDateRange.h"
#include "Model_Checking.h"
#include "Model_Account.h"
#include <memory>
//----------------------------------------------------------------------------
class mmCheckingPanel;
class mmFilterTransactionsDialog;
class mmGUIFrame;
//----------------------------------------------------------------------------

class TransactionListCtrl : public mmListCtrl
{
public:

    TransactionListCtrl ( mmCheckingPanel *cp, wxWindow *parent, const wxWindowID id = wxID_ANY );

    ~TransactionListCtrl();

    void createColumns ( mmListCtrl &lst );
    enum EIcons //m_imageList
    {
        ICON_RECONCILED,
        ICON_VOID,
        ICON_FOLLOWUP,
        ICON_NONE,
        ICON_DUPLICATE,
        ICON_DESC,
        ICON_ASC,
    };

public:
    enum EColumn
    {
        COL_IMGSTATUS = 0,
        COL_ID,
        COL_DATE,
        COL_DEF_SORT = COL_DATE,
        COL_NUMBER,
        COL_PAYEE_STR,
        COL_STATUS,
        COL_CATEGORY,
        COL_WITHDRAWAL,
        COL_DEPOSIT,
        COL_BALANCE,
        COL_NOTES,
        COL_UDFC01,
        COL_UDFC02,
        COL_UDFC03,
        COL_UDFC04,
        COL_UDFC05,
        COL_MAX, // number of columns
    };
    EColumn toEColumn ( const long col )
    {
        EColumn res = COL_DEF_SORT;
        if ( col >= 0 && col < COL_MAX )
        {
            res = static_cast<EColumn> ( col );
        }

        return res;
    }

public:
    EColumn g_sortcol = COL_DEF_SORT; // index of column to sort
    EColumn m_prevSortCol = COL_DEF_SORT;
    bool g_asc = true; // asc\desc sorting

    bool getSortOrder() const noexcept
    {
        return m_asc;
    }
    EColumn getSortColumn() const noexcept
    {
        return m_sortCol;
    }

    void setSortOrder ( const bool asc ) noexcept
    {
        m_asc = asc;
    }
    void setSortColumn ( const EColumn col ) noexcept
    {
        m_sortCol = col;
    }

    void setColumnImage ( EColumn col, int image );

public:
    void OnNewTransaction ( wxCommandEvent &event );
    void OnNewTransferTransaction ( wxCommandEvent &event );
    void OnDeleteTransaction ( wxCommandEvent &event );
    void OnEditTransaction ( wxCommandEvent &event );
    void OnDuplicateTransaction ( wxCommandEvent &event );
    void OnSetUserColour ( wxCommandEvent &event );
    void OnMoveTransaction ( wxCommandEvent &event );
    void OnOpenAttachment ( wxCommandEvent &event );
    /// Displays the split categories for the selected transaction
    void OnViewSplitTransaction ( wxCommandEvent &event );
    void OnOrganizeAttachments ( wxCommandEvent &event );
    void OnCreateReoccurance ( wxCommandEvent &event );
    void refreshVisualList ( const int trans_id = -1, const bool filter = true );
    long m_selectedIndex=-1;
    long m_selectedForCopy = -1; //The transaction ID if selected for copy
    long m_selectedID = -1; //Selected transaction ID
    wxString m_today;

protected:
    /* Sort Columns */
    void OnColClick ( wxListEvent &event ) override;

private:
    enum
    {
        MENU_TREEPOPUP_MARKRECONCILED = wxID_HIGHEST + 150,
        MENU_TREEPOPUP_MARKUNRECONCILED,
        MENU_TREEPOPUP_MARKVOID,
        MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP,
        MENU_TREEPOPUP_MARKDUPLICATE,
        MENU_TREEPOPUP_MARKDELETE,
        MENU_TREEPOPUP_MARKRECONCILED_ALL,
        MENU_TREEPOPUP_MARKUNRECONCILED_ALL,
        MENU_TREEPOPUP_MARKVOID_ALL,
        MENU_TREEPOPUP_MARK_ADD_FLAG_FOLLOWUP_ALL,
        MENU_TREEPOPUP_MARKDUPLICATE_ALL,
        MENU_TREEPOPUP_DELETE_VIEWED,
        MENU_TREEPOPUP_DELETE_FLAGGED,
        MENU_TREEPOPUP_DELETE_UNRECONCILED,

        MENU_TREEPOPUP_VIEW_SPLIT_CATEGORIES,
        MENU_TREEPOPUP_ORGANIZE_ATTACHMENTS,
        MENU_TREEPOPUP_CREATE_REOCCURANCE,
        MENU_SUBMENU_MARK_ALL,

        MENU_VIEW_,
        MENU_VIEW_DELETE_TRANS,
        MENU_VIEW_DELETE_FLAGGED,

        MENU_ON_COPY_TRANSACTION,
        MENU_ON_PASTE_TRANSACTION,
        MENU_ON_NEW_TRANSACTION,
        MENU_ON_DUPLICATE_TRANSACTION,

        MENU_ON_SET_UDC0, //Default colour
        MENU_ON_SET_UDC1, //User defined colour 1
        MENU_ON_SET_UDC2, //User defined colour 2
        MENU_ON_SET_UDC3, //User defined colour 3
        MENU_ON_SET_UDC4, //User defined colour 4
        MENU_ON_SET_UDC5, //User defined colour 5
        MENU_ON_SET_UDC6, //User defined colour 6
        MENU_ON_SET_UDC7, //User defined colour 7

        MENU_TREEPOPUP_NEW_WITHDRAWAL,
        MENU_TREEPOPUP_NEW_DEPOSIT,
        MENU_TREEPOPUP_NEW_TRANSFER,
        MENU_TREEPOPUP_EDIT2,
        MENU_TREEPOPUP_MOVE2,
        MENU_TREEPOPUP_DELETE2,
        ID_PANEL_CHECKING_STATIC_BITMAP_VIEW,
    };

    DECLARE_NO_COPY_CLASS ( TransactionListCtrl )
    wxDECLARE_EVENT_TABLE();

private:
    mmCheckingPanel *m_cp = nullptr;

    wxListItemAttr m_attr1;  // style1
    wxListItemAttr m_attr2;  // style2
    wxListItemAttr m_attr3;  // style, for future dates
    wxListItemAttr m_attr4;  // style, for future dates
    wxListItemAttr m_attr11; // user defined style 1
    wxListItemAttr m_attr12; // user defined style 2
    wxListItemAttr m_attr13; // user defined style 3
    wxListItemAttr m_attr14; // user defined style 4
    wxListItemAttr m_attr15; // user defined style 5
    wxListItemAttr m_attr16; // user defined style 6
    wxListItemAttr m_attr17; // user defined style 7

    /* required overrides for virtual style list control */
    wxString OnGetItemText ( long item, long column ) const override;
    int OnGetItemColumnImage ( long item, long column ) const override;
    wxListItemAttr *OnGetItemAttr ( long item ) const override;

private:
    void OnMouseRightClick ( wxMouseEvent &event );
    void OnListLeftClick ( wxMouseEvent &event );

    void OnListItemSelected ( wxListEvent &event );

    void OnListItemActivated ( wxListEvent &event );
    void OnListItemUnSelected ( wxListEvent &event );
    void updateInfoPanel();
    void OnMarkTransaction ( wxCommandEvent &event );
    void OnMarkAllTransactions ( wxCommandEvent &event );
    void OnListKeyDown ( wxListEvent &event );
    void OnChar ( wxKeyEvent &event );
    void OnCopy ( wxCommandEvent &WXUNUSED ( event ) );
    void OnPaste ( wxCommandEvent &WXUNUSED ( event ) );
    int OnPaste ( Model_Checking::Data *tran );

    bool TransactionLocked ( const wxString &transdate );

    /* The topmost visible item - this will be used to set
    where to display the list again after refresh */
    long m_topItemIndex= -1;
    EColumn m_sortCol = COL_DEF_SORT;
};

//----------------------------------------------------------------------------

class mmCheckingPanel : public mmPanelBase
{
public:

    mmCheckingPanel ( wxWindow *parent
                      , mmGUIFrame *frame
                      , int accountID
                      , int id = wxID_ANY
                    );

    ~mmCheckingPanel();

    /// Display the split categories for the selected transaction.
    void DisplaySplitCategories (const int transID );
    /// Refresh account screen with new details
    void DisplayAccountDetails ( const int accountID );

    void SetSelectedTransaction (const int transID );

    void RefreshList (const int transID = -1 );

    wxString BuildPage() const override;

private:
    enum
    {
        ID_PANEL_CHECKING_STATIC_BALHEADER1 = wxID_HIGHEST + 50,
        ID_PANEL_CHECKING_STATIC_BALHEADER2,
        ID_PANEL_CHECKING_STATIC_BALHEADER3,
        ID_PANEL_CHECKING_STATIC_BALHEADER4,
        ID_PANEL_CHECKING_STATIC_BALHEADER5,
        ID_PANEL_CHECKING_STATIC_DETAILS,
        ID_TRX_FILTER,
    };
    enum menu
    {
        MENU_VIEW_ALLTRANSACTIONS = 0,
        MENU_VIEW_TODAY,
        MENU_VIEW_CURRENTMONTH,
        MENU_VIEW_LAST30,
        MENU_VIEW_LAST90,
        MENU_VIEW_LASTMONTH,
        MENU_VIEW_LAST3MONTHS,
        MENU_VIEW_LAST12MONTHS,
        MENU_VIEW_CURRENTYEAR,
        MENU_VIEW_CURRENTFINANCIALYEAR,
        MENU_VIEW_LASTYEAR,
        MENU_VIEW_LASTFINANCIALYEAR,
        MENU_VIEW_STATEMENTDATE,
        MENU_VIEW_FILTER_DIALOG,
    };
    static std::unordered_map<enum menu, wxString> menu_labels;

    wxDECLARE_EVENT_TABLE();
    friend class TransactionListCtrl; // needs access to m_core, initdb_, ...

    wxButton *m_btnNew=nullptr;
    wxButton *m_btnEdit=nullptr;
    wxButton *m_btnDuplicate=nullptr;
    wxButton *m_btnDelete=nullptr;
    wxButton *m_btnAttachment=nullptr;
    wxStaticText *m_header_text=nullptr;
    wxStaticText *m_info_panel=nullptr;
    wxStaticText *m_info_panel_mini=nullptr;
    wxButton *m_bitmapTransFilter=nullptr;
    std::unique_ptr<mmFilterTransactionsDialog> m_trans_filter_dlg;

    enum menu m_currentView;
    int m_AccountID;
    bool m_transFilterActive;
    wxString m_begin_date;
    wxString m_end_date;
    double m_filteredBalance = 0.0;
    double m_account_balance;
    double m_reconciled_balance;

    TransactionListCtrl *m_listCtrlAccount = nullptr;
    Model_Account::Data *m_account = nullptr;
    Model_Currency::Data *m_currency = nullptr;
    wxScopedPtr<wxImageList> m_imageList;
    Model_Checking::Full_Data_Set m_trans;

    void initViewTransactionsHeader();
    void setDateRange();
    void setAccountSummary();
    void sortTable() override;
    void filterTable();
    void updateTable();
    void markSelectedTransaction ( int trans_id );
    void CreateControls();

    bool Create (
        wxWindow *parent,
        wxWindowID winid,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString &name = "mmCheckingPanel"
    );
    void enableEditDeleteButtons ( bool en );

    void OnNewTransaction ( wxCommandEvent &event );
    void OnDeleteTransaction ( wxCommandEvent &event );
    void OnEditTransaction ( wxCommandEvent &event );
    void OnDuplicateTransaction ( wxCommandEvent &event );
    void OnMoveTransaction ( wxCommandEvent &event );
    void OnOpenAttachment ( wxCommandEvent &event );
    void OnMouseLeftDown ( wxCommandEvent &event );
    void OnViewPopupSelected ( wxCommandEvent &event );
    void OnSearchTxtEntered ( wxCommandEvent &event );

    void DeleteViewedTransactions();
    void DeleteFlaggedTransactions ( const wxString &status );

    /* updates the checking panel data */
    void showTips();
    void updateExtraTransactionData ( int selIndex );
    const wxString GetPanelTitle ( const Model_Account::Data &account ) const;

    /* Getter for Virtual List Control */
    const wxString getItem ( const long item, const TransactionListCtrl::EColumn column );

private:
    static void mmPlayTransactionSound();
private:
    mmGUIFrame *m_frame;
};
//----------------------------------------------------------------------------
#endif // MM_EX_CHECKINGPANEL_H_
//----------------------------------------------------------------------------

inline static bool SorterByUDFC01 ( const Model_Checking::Full_Data &i, const Model_Checking::Full_Data &j )
{
    return ( i.UDFC01 < j.UDFC01 );
}
inline static bool SorterByUDFC02 ( const Model_Checking::Full_Data &i, const Model_Checking::Full_Data &j )
{
    return ( i.UDFC02 < j.UDFC02 );
}
inline static bool SorterByUDFC03 ( const Model_Checking::Full_Data &i, const Model_Checking::Full_Data &j )
{
    return ( i.UDFC03 < j.UDFC03 );
}
inline static bool SorterByUDFC04 ( const Model_Checking::Full_Data &i, const Model_Checking::Full_Data &j )
{
    return ( i.UDFC04 < j.UDFC04 );
}
inline static bool SorterByUDFC05 ( const Model_Checking::Full_Data &i, const Model_Checking::Full_Data &j )
{
    return ( i.UDFC05 < j.UDFC05 );
}
