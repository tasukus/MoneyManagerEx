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

#pragma once

#include "mmpanelbase.h"
#include "Model_Budget.h"

class wxListCtrl;
class wxListEvent;
class wxStaticText;
class mmBudgetingPanel;

/* Custom ListCtrl class that implements virtual LC style */
class budgetingListCtrl : public mmListCtrl
{
    DECLARE_NO_COPY_CLASS ( budgetingListCtrl )
    wxDECLARE_EVENT_TABLE();

public:
    explicit budgetingListCtrl ( mmBudgetingPanel *cp, wxWindow *parent, const wxWindowID id );

public:
    /* required overrides for virtual style list control */
    wxString OnGetItemText ( long item, long column ) const override;
    wxListItemAttr *OnGetItemAttr ( long item ) const override;
    int OnGetItemImage ( long item ) const override;

    void OnListItemSelected ( wxListEvent &event );
    void OnListItemActivated ( wxListEvent &event );

private:
    wxListItemAttr attr3_; // style3
    mmBudgetingPanel *cp_;
    long selectedIndex_;
};

class mmBudgetingPanel : public mmPanelBase
{
    wxDECLARE_EVENT_TABLE();

public:
    mmBudgetingPanel ( int budgetYearID
                       , wxWindow *parent
                       , wxWindowID winid = wxID_ANY
                       , const wxPoint &pos = wxDefaultPosition
                       , const wxSize &size = wxDefaultSize
                       , long style = wxTAB_TRAVERSAL | wxNO_BORDER
                       , const wxString &name = "mmBudgetingPanel" );
    ~mmBudgetingPanel();

    /* updates the checking panel data */
    void initVirtualListControl();
    int col_max() const noexcept
    {
        return COL_MAX;
    }

    /* Getter for Virtual List Control */
    wxString getItem ( long item, long column );

    void DisplayBudgetingDetails ( int budgetYearID );
    int GetBudgetYearID() const noexcept
    {
        return budgetYearID_;
    }
    wxString GetCurrentView() const
    {
        return currentView_;
    }
    int GetItemImage ( long item ) const;
    void OnListItemActivated ( int selectedIndex );
    int GetTransID ( long item )
    {
        return budget_[item].first;
    }

    void RefreshList();

    wxString BuildPage() const override
    {
        return listCtrlBudget_->BuildPage ( GetPanelTitle() );
    }

private:
    std::vector<std::pair<int, int> > budget_;
    std::map<int, std::pair<double, double> > budgetTotals_;
    std::map<int, std::map<int, Model_Budget::PERIOD_ENUM> > budgetPeriod_;
    std::map<int, std::map<int, double> > budgetAmt_;
    std::map<int, std::map<int, std::map<int, double> > > categoryStats_;
    bool monthlyBudget_;
    budgetingListCtrl *listCtrlBudget_=nullptr;
    wxString currentView_;
    int budgetYearID_;
    wxString m_budget_offset_date;

    wxImageList *m_imageList=nullptr;
    wxStaticText *budgetReportHeading_=nullptr;
    wxStaticText *income_estimated_=nullptr;
    wxStaticText *income_actual_=nullptr;
    wxStaticText *income_diff_=nullptr;
    wxStaticText *expenses_estimated_=nullptr;
    wxStaticText *expenses_actual_=nullptr;
    wxStaticText *expenses_diff_=nullptr;

    bool Create ( wxWindow *parent, wxWindowID winid
                  , const wxPoint &pos = wxDefaultPosition
                  , const wxSize &size = wxDefaultSize
                  , long style = wxTAB_TRAVERSAL | wxNO_BORDER
                  , const wxString &name = "mmBudgetingPanel" );

    void CreateControls();
    void sortTable() override;
    bool DisplayEntryAllowed ( int categoryID, int subcategoryID );
    void UpdateBudgetHeading();
    double getEstimate ( int category, int subcategory ) const;
    wxString GetPanelTitle() const;

    /* Event handlers for Buttons */
    void OnViewPopupSelected ( wxCommandEvent &event );
    void OnMouseLeftDown ( wxMouseEvent &event );

    enum EColumn
    {
        COL_ICON = 0,
        COL_CATEGORY,
        COL_SUBCATEGORY,
        COL_FREQUENCY,
        COL_AMOUNT,
        COL_ESTIMATED,
        COL_ACTUAL,
        COL_MAX, // number of columns
    };
};

