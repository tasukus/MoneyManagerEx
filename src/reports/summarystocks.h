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

#ifndef _MM_EX_REPORTSUMMARYSTOCKS_H_
#define _MM_EX_REPORTSUMMARYSTOCKS_H_

#include "reportbase.h"
#include <vector>

class mmHTMLBuilder;

class mmReportSummaryStocks : public mmPrintableBase
{
public:
    mmReportSummaryStocks();
    void RefreshData() override;
    wxString getHTMLText() override;

private:
    void display_header ( mmHTMLBuilder &hb );
    // structure for sorting of data
    struct data_holder
    {
        wxString name;
        wxString symbol;
        wxString date;
        double qty;
        double purchase;
        double current;
        double commission;
        double gainloss;
        double value;
    };
    struct account_holder
    {
        int id;
        wxString name;
        std::vector<data_holder> data;
        double gainloss;
        double total;
    };
    std::vector<account_holder> m_stocks;
    double m_gain_loss_sum_total;
    double m_stock_balance;
};

class mmReportChartStocks : public mmPrintableBase
{
public:
    mmReportChartStocks();
    ~mmReportChartStocks();
    int report_parameters() const noexcept override;
    wxString getHTMLText() override;
};

#endif // _MM_EX_REPORTSUMMARYSTOCKS_H_
