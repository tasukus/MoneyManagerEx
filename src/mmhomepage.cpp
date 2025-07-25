/*******************************************************
Copyright (C) 2014 - 2021 Nikolay Akimov
Copyright (C) 2021-2023 Mark Whalley (mark@ipx.co.uk)

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

#include <algorithm>
#include <cmath>
#include "mmhomepage.h"
#include "html_template.h"
#include "billsdepositspanel.h"
#include "option.h"
#include "optionsettingshome.h"
#include "constants.h"
#include "reports/reportbase.h"

#include "model/Model_Stock.h"
#include "model/Model_StockHistory.h"
#include "model/Model_Category.h"
#include "model/Model_Currency.h"
#include "model/Model_CurrencyHistory.h"
#include "model/Model_Payee.h"
#include "model/Model_Asset.h"
#include "model/Model_Setting.h"

static const wxString TOP_CATEGS = R"(
<table class = 'table'>
  <tr class='active'>
    <th>%s</th>
    <th nowrap class='text-right sorttable_nosort'>
      <a id='%s_label' onclick='toggleTable("%s"); ' href='#%s' oncontextmenu='return false;'>[-]</a>
    </th>
  </tr>
  <tr>
    <td style='padding: 0px; padding-left: 0px; padding-right: 0px; width: 100%%;' colspan='2'>
    <table class = 'sortable table' id='%s'>
    <thead>
      <tr><th>%s</th><th class='text-right'>%s</th></tr>
    </thead>
   <tbody>
%s
   </tbody>
</table>
</td></tr>
</table>
)";


htmlWidgetStocks::htmlWidgetStocks()
    : title_(_t("Stocks"))
{
    grand_total_ = 0.0;
}

htmlWidgetStocks::~htmlWidgetStocks()
{
}

const wxString htmlWidgetStocks::getHTMLText()
{
    double grand_gain_lost    = 0;
    double grand_market_value = 0;  // Track the grand total of market values
    double grand_cash_balance = 0;  // Track the grand total of cash balances
    const wxDate today = wxDate::Today();

    wxString output = "";
    const auto &accounts = Model_Account::instance().find(Model_Account::ACCOUNTTYPE(Model_Account::TYPE_NAME_INVESTMENT, EQUAL));
    if (!accounts.empty())
    {
        output = R"(<div class="shadow">)";
        output += "<table class ='sortable table'><col style='width: 50%'><col style='width: 12.5%'><col style='width: 12.5%'><col style='width: 12.5%'><col style='width: 12.5%'><thead><tr class='active'><th>\n";
        output += _t("Stocks") + "</th><th class = 'text-right'>" + _t("Gain/Loss") + "</th>\n";
        output += "<th class='text-right'>" + _t("Market Value") + "</th>\n";
        output += "<th class='text-right'>" + _t("Cash Balance") + "</th>\n";
        output += "<th class='text-right'>" + _t("Total") + "</th>\n";
        output += wxString::Format("<th nowrap class='text-right sorttable_nosort'><a id='%s_label' onclick='toggleTable(\"%s\");' href='#%s' oncontextmenu='return false;'>[-]</a></th>\n"
            , "INVEST", "INVEST", "INVEST");
        output += "</tr></thead><tbody id='INVEST'>\n";
        wxString body = "";
        for (const auto& account : accounts)
        {
            if (Model_Account::status_id(account) != Model_Account::STATUS_ID_OPEN) continue;

            double conv_rate = Model_CurrencyHistory::getDayRate(account.CURRENCYID, today);
            auto inv_bal = Model_Account::investment_balance(account);
            double cash_bal = Model_Account::balance(account);

            grand_gain_lost    += (inv_bal.first - inv_bal.second) * conv_rate;
            grand_market_value += inv_bal.first * conv_rate;
            grand_cash_balance += cash_bal * conv_rate;
            grand_total_       += (inv_bal.first + cash_bal) * conv_rate;

            body += "<tr>";
            body += wxString::Format("<td sorttable_customkey='*%s*'><a href='stock:%lld' oncontextmenu='return false;' target='_blank'>%s</a>%s</td>\n"
                , account.ACCOUNTNAME, account.ACCOUNTID, account.ACCOUNTNAME,
                account.WEBSITE.empty() ? "" : wxString::Format("&nbsp;&nbsp;&nbsp;&nbsp;(<a href='%s' oncontextmenu='return false;' target='_blank'>WWW</a>)", account.WEBSITE));
            body += wxString::Format("<td class='money' sorttable_customkey='%f'>%s</td>\n"
                , inv_bal.first - inv_bal.second
                , Model_Account::toCurrency(inv_bal.first - inv_bal.second, &account));
            body += wxString::Format("<td class='money' sorttable_customkey='%f'>%s</td>\n"
                , inv_bal.first
                , Model_Account::toCurrency(inv_bal.first, &account));
            body += wxString::Format("<td class='money' sorttable_customkey='%f'>%s</td>\n"
                , cash_bal
                , Model_Account::toCurrency(cash_bal, &account));
            body += wxString::Format("<td colspan='2' class='money' sorttable_customkey='%f'>%s</td>"
                , inv_bal.first + cash_bal
                , Model_Account::toCurrency(inv_bal.first + cash_bal, &account));
            body += "</tr>";
        }

        if (!body.empty())
        {
            output += body;
            output += "</tbody><tfoot><tr class = 'total'><td>" + _t("Total:") + "</td>";
            output += wxString::Format("<td class='money'>%s</td>"
                , Model_Currency::toCurrency(grand_gain_lost));
            output += wxString::Format("<td class='money'>%s</td>"
                , Model_Currency::toCurrency(grand_market_value));
            output += wxString::Format("<td class='money'>%s</td>"
                , Model_Currency::toCurrency(grand_cash_balance));
            output += wxString::Format("<td colspan='2' class='money'>%s</td></tr></tfoot></table>\n"
                , Model_Currency::toCurrency(grand_total_));
            output += "</div>";
        }
    }
    return output;
}

double htmlWidgetStocks::get_total()
{
    return grand_total_;
}

////////////////////////////////////////////////////////


htmlWidgetTop7Categories::htmlWidgetTop7Categories()
{
    date_range_ = new mmLast30Days();
    title_ = wxString::Format(_t("Top Withdrawals: %s"), date_range_->local_title());
}

htmlWidgetTop7Categories::~htmlWidgetTop7Categories()
{
    if (date_range_) delete date_range_;
}

const wxString htmlWidgetTop7Categories::getHTMLText()
{

    std::vector<std::pair<wxString, double> > topCategoryStats;
    getTopCategoryStats(topCategoryStats, date_range_);
    wxString output, data;

    if (!topCategoryStats.empty())
    {
        output = R"(<div class="shadow">)";
        for (const auto& i : topCategoryStats)
        {
            data += "<tr>";
            data += wxString::Format("<td>%s</td>", (i.first.IsEmpty() ? wxString::FromUTF8Unchecked("…") : i.first));
            data += wxString::Format("<td class='money' sorttable_customkey='%f'>%s</td>\n"
                , i.second
                , Model_Currency::toCurrency(i.second));
            data += "</tr>\n";
        }
        const wxString idStr = "TOP_CATEGORIES";
        output += wxString::Format(TOP_CATEGS, title_, idStr, idStr, idStr, idStr, _t("Category"), _t("Summary"), data);
        output += "</div>";
    }

    return output;
}

void htmlWidgetTop7Categories::getTopCategoryStats(
    std::vector<std::pair<wxString, double> > &categoryStats
    , const mmDateRange* date_range) const
{
    //Temporary map
    std::map<int64 /*category*/, double> stat;

    const auto split = Model_Splittransaction::instance().get_all();
    const auto &transactions = Model_Checking::instance().find(
        Model_Checking::TRANSDATE(date_range->start_date(), GREATER_OR_EQUAL)
        , Model_Checking::TRANSDATE(date_range->end_date().FormatISOCombined(), LESS_OR_EQUAL)
        , Model_Checking::STATUS(Model_Checking::STATUS_ID_VOID, NOT_EQUAL)
        , Model_Checking::TRANSCODE(Model_Checking::TYPE_ID_TRANSFER, NOT_EQUAL));

    for (const auto &trx : transactions)
    {
        // Do not include asset or stock transfers or deleted transactions in income expense calculations.
        if (Model_Checking::foreignTransactionAsTransfer(trx) || !trx.DELETEDTIME.IsEmpty())
            continue;

        bool withdrawal = Model_Checking::type_id(trx) == Model_Checking::TYPE_ID_WITHDRAWAL;
        double convRate = Model_CurrencyHistory::getDayRate(Model_Account::instance().get(trx.ACCOUNTID)->CURRENCYID, trx.TRANSDATE);

        if (const auto it = split.find(trx.TRANSID); it == split.end())
        {
            int64 category = trx.CATEGID;
            if (withdrawal)
                stat[category] -= trx.TRANSAMOUNT * convRate;
            else
                stat[category] += trx.TRANSAMOUNT * convRate;
        }
        else
        {
            for (const auto& entry : it->second)
            {
                int64 category = entry.CATEGID;
                double val = entry.SPLITTRANSAMOUNT
                    * convRate
                    * (withdrawal ? -1 : 1);
                stat[category] += val;
            }
        }
    }

    categoryStats.clear();
    for (const auto& i : stat)
    {
        if (i.second < 0)
        {
            std::pair <wxString, double> stat_pair;
            stat_pair.first = Model_Category::full_name(i.first);
            stat_pair.second = i.second;
            categoryStats.push_back(stat_pair);
        }
    }

    std::stable_sort(categoryStats.begin(), categoryStats.end()
        , [](const std::pair<wxString, double> x, const std::pair<wxString, double> y)
    { return x.second < y.second; }
    );

    int counter = 0;
    std::vector<std::pair<wxString, double> >::iterator iter;
    for (iter = categoryStats.begin(); iter != categoryStats.end(); )
    {
        counter++;
        if (counter > 7)
            iter = categoryStats.erase(iter);
        else
            ++iter;
    }
}

////////////////////////////////////////////////////////


htmlWidgetBillsAndDeposits::htmlWidgetBillsAndDeposits(const wxString& title, mmDateRange* date_range)
    : date_range_(date_range)
    , title_(title)
{}

htmlWidgetBillsAndDeposits::~htmlWidgetBillsAndDeposits()
{
    if (date_range_) delete date_range_;
}

const wxString htmlWidgetBillsAndDeposits::getHTMLText()
{
    wxString output = "";
    wxDate today = wxDate::Today();

    //                    days, payee, description, amount, account, notes
    std::vector< std::tuple<int, wxString, wxString, double, const Model_Account::Data*, wxString> > bd_days;
    for (const auto& entry : Model_Billsdeposits::instance().all(Model_Billsdeposits::COL_TRANSDATE))
    {
        int daysPayment = Model_Billsdeposits::TRANSDATE(&entry)
            .Subtract(today).GetDays();
        if (daysPayment > 14)
            break; // Done searching for all to include

        int repeats = entry.REPEATS.GetValue() % BD_REPEATS_MULTIPLEX_BASE; // DeMultiplex the Auto Executable fields

        // ignore inactive entries
        if (repeats >= Model_Billsdeposits::REPEAT_IN_X_DAYS && repeats <= Model_Billsdeposits::REPEAT_EVERY_X_MONTHS && entry.NUMOCCURRENCES < 0)
            continue;

        int daysOverdue = Model_Billsdeposits::NEXTOCCURRENCEDATE(&entry)
            .Subtract(today).GetDays();
        wxString daysRemainingStr = (daysPayment > 0
            ? wxString::Format(wxPLURAL("%d day remaining", "%d days remaining", daysPayment), daysPayment)
            : wxString::Format(wxPLURAL("%d day delay!", "%d days delay!", -daysPayment), -daysPayment));
        if (daysOverdue < 0)
            daysRemainingStr = wxString::Format(wxPLURAL("%d day overdue!", "%d days overdue!", std::abs(daysOverdue)), std::abs(daysOverdue));

        wxString accountStr = "";
        const auto *account = Model_Account::instance().get(entry.ACCOUNTID);
        if (account) accountStr = account->ACCOUNTNAME;

        wxString payeeStr = "";
        if (Model_Billsdeposits::type_id(entry) == Model_Checking::TYPE_ID_TRANSFER)
        {
            const Model_Account::Data *to_account = Model_Account::instance().get(entry.TOACCOUNTID);
            if (to_account) payeeStr = to_account->ACCOUNTNAME;
            payeeStr += " &larr; " + accountStr;
        }
        else
        {
            const Model_Payee::Data* payee = Model_Payee::instance().get(entry.PAYEEID);
            payeeStr = accountStr;
            payeeStr += (Model_Billsdeposits::type_id(entry) == Model_Checking::TYPE_ID_WITHDRAWAL ? " &rarr; " : " &larr; ");
            if (payee) payeeStr += payee->PAYEENAME;
        }
        double amount = (Model_Billsdeposits::type_id(entry) == Model_Checking::TYPE_ID_WITHDRAWAL ? -entry.TRANSAMOUNT : entry.TRANSAMOUNT);
        wxString notes = HTMLEncode(entry.NOTES);
        bd_days.push_back(std::make_tuple(daysPayment, payeeStr, daysRemainingStr, amount, account, notes));
    }

    //std::sort(bd_days.begin(), bd_days.end());
    //std::reverse(bd_days.begin(), bd_days.end());
    ////////////////////////////////////

    if (!bd_days.empty())
    {
        static const wxString idStr = "BILLS_AND_DEPOSITS";

        output = R"(<div class="shadow">)";
        output += "<table class='table'>\n<thead>\n<tr class='active'><th>";
        output += wxString::Format("<a href=\"billsdeposits:\" oncontextmenu=\"return false;\" target=\"_blank\">%s</a></th>\n<th></th>\n", title_);
        output += wxString::Format("<th nowrap class='text-right sorttable_nosort'>%i <a id='%s_label' onclick=\"toggleTable('%s'); \" href='#%s' oncontextmenu='return false;'>[-]</a></th></tr>\n"
            , static_cast<int>(bd_days.size()), idStr, idStr, idStr);
        output += "</thead></table>\n";

        output += wxString::Format("<table class='table' id='%s'>\n", idStr);
        output += wxString::Format("<thead><tr><th>%s</th>\n<th class='text-right'>%s</th>\n<th class='text-right'>%s</th></tr></thead>\n"
            , _t("Account/Payee"), _t("Amount"), _t("Payment"));

        for (const auto& item : bd_days)
        {
            output += wxString::Format("<tr %s>\n", std::get<0>(item) < 0 ? "class='danger'" : "");
            output += "<td>" + std::get<1>(item);
            wxString notes = std::get<5>(item);
            if (notes.Length() > 150)
                notes = notes.Left(150) + wxString::FromUTF8Unchecked("…");
            if (!notes.IsEmpty())
                output += wxString::Format("<br><i>%s</i>", notes);

            output += "</td>";
            output += wxString::Format("<td class='money'>%s</td>\n"
                , Model_Account::toCurrency(std::get<3>(item), std::get<4>(item)));
            output += "<td  class='money'>" + std::get<2>(item) + "</td></tr>\n";
        }
        output += "</table>\n";
        output += "</div>";
    }
    return output;
}

////////////////////////////////////////////////////////

//* Income vs Expenses *//
const wxString htmlWidgetIncomeVsExpenses::getHTMLText()
{
    OptionSettingsHome home_options;
    wxSharedPtr<mmDateRange> date_range(home_options.get_inc_vs_exp_date_range());

    double tIncome = 0.0, tExpenses = 0.0;
    std::map<int64, std::pair<double, double> > incomeExpensesStats;

    //Calculations
    const auto &transactions = Model_Checking::instance().find(
        Model_Checking::TRANSDATE(date_range.get()->start_date(), GREATER_OR_EQUAL)
        , Model_Checking::TRANSDATE(date_range.get()->end_date().FormatISOCombined(), LESS_OR_EQUAL)
        , Model_Checking::STATUS(Model_Checking::STATUS_ID_VOID, NOT_EQUAL)
        , Model_Checking::TRANSCODE(Model_Checking::TYPE_ID_TRANSFER, NOT_EQUAL)
    );

    for (const auto& pBankTransaction : transactions)
    {

        // Do not include asset or stock transfers or deleted transactions in income expense calculations.
        if (Model_Checking::foreignTransactionAsTransfer(pBankTransaction) || !pBankTransaction.DELETEDTIME.IsEmpty())
            continue;

        double convRate = Model_CurrencyHistory::getDayRate(Model_Account::instance().get(pBankTransaction.ACCOUNTID)->CURRENCYID, pBankTransaction.TRANSDATE);

        int64 idx = pBankTransaction.ACCOUNTID;
        if (Model_Checking::type_id(pBankTransaction) == Model_Checking::TYPE_ID_DEPOSIT)
            incomeExpensesStats[idx].first += pBankTransaction.TRANSAMOUNT * convRate;
        else
            incomeExpensesStats[idx].second += pBankTransaction.TRANSAMOUNT * convRate;
    }

    for (const auto& account : Model_Account::instance().all())
    {
        int64 idx = account.ACCOUNTID;
        tIncome += incomeExpensesStats[idx].first;
        tExpenses += incomeExpensesStats[idx].second;
    }


    StringBuffer json_buffer;
    PrettyWriter<StringBuffer> json_writer(json_buffer);
    json_writer.StartObject();
    json_writer.Key("0");
    json_writer.String(wxString::Format(_t("Income vs. Expenses: %s"), date_range.get()->local_title()).utf8_str());
    json_writer.Key("1");
    json_writer.String(_t("Type").utf8_str());
    json_writer.Key("2");
    json_writer.String(_t("Amount").utf8_str());
    json_writer.Key("3");
    json_writer.String(_t("Income").utf8_str());
    json_writer.Key("4");
    json_writer.String(Model_Currency::toCurrency(tIncome).utf8_str());
    json_writer.Key("5");
    json_writer.String(_t("Expenses").utf8_str());
    json_writer.Key("6");
    json_writer.String(Model_Currency::toCurrency(tExpenses).utf8_str());
    json_writer.Key("7");
    json_writer.String(_t("Difference:").utf8_str());
    json_writer.Key("8");
    json_writer.String(Model_Currency::toCurrency(tIncome - tExpenses).utf8_str());
    json_writer.Key("9");
    json_writer.String(_t("Income/Expenses").utf8_str());
    json_writer.Key("10");
    json_writer.String(wxString::FromCDouble(tIncome, 2).utf8_str());
    json_writer.Key("11");
    json_writer.String(wxString::FromCDouble(tExpenses, 2).utf8_str());
    json_writer.EndObject();

    wxLogDebug("======= mmHomePagePanel::getIncomeVsExpensesJSON =======");
    wxLogDebug("RapidJson\n%s", wxString::FromUTF8(json_buffer.GetString()));

    return wxString::FromUTF8(json_buffer.GetString());
}

htmlWidgetIncomeVsExpenses::~htmlWidgetIncomeVsExpenses()
{
}

const wxString htmlWidgetStatistics::getHTMLText()
{
    StringBuffer json_buffer;
    PrettyWriter<StringBuffer> json_writer(json_buffer);
    json_writer.StartObject();

    json_writer.Key("NAME");
    json_writer.String(_t("Transaction Statistics").utf8_str());

    wxSharedPtr<mmDateRange> date_range;
    if (Option::instance().getIgnoreFutureTransactions())
        date_range = new mmCurrentMonthToDate;
    else
        date_range = new mmCurrentMonth;

    Model_Checking::Data_Set all_trans;
    if (Option::instance().getIgnoreFutureTransactions()) {
        all_trans = Model_Checking::instance().find(
            Model_Checking::TRANSDATE(wxDateTime(23,59,59,999), LESS_OR_EQUAL));
    }
    else {
        all_trans = Model_Checking::instance().all();
    }
    int countFollowUp = 0;
    int total_transactions = 0;

    std::map<int64, std::pair<double, double> > accountStats;
    for (const auto& trx : all_trans)
    {
        if (!trx.DELETEDTIME.IsEmpty())
            continue;

        total_transactions++;

        // Do not include asset or stock transfers in income expense calculations.
        if (Model_Checking::foreignTransactionAsTransfer(trx))
            continue;

        if (Model_Checking::status_id(trx) == Model_Checking::STATUS_ID_FOLLOWUP)
            countFollowUp++;

        accountStats[trx.ACCOUNTID].first += Model_Checking::account_recflow(trx, trx.ACCOUNTID);
        accountStats[trx.ACCOUNTID].second += Model_Checking::account_flow(trx, trx.ACCOUNTID);

        if (Model_Checking::type_id(trx) == Model_Checking::TYPE_ID_TRANSFER)
        {
            accountStats[trx.TOACCOUNTID].first += Model_Checking::account_recflow(trx, trx.TOACCOUNTID);
            accountStats[trx.TOACCOUNTID].second += Model_Checking::account_flow(trx, trx.TOACCOUNTID);
        }
    }


    if (countFollowUp > 0)
    {
        json_writer.Key(_t("Follow Up On Transactions: ").utf8_str());
        json_writer.Double(countFollowUp);
    }

    json_writer.Key(_t("Total Transactions: ").utf8_str());
    json_writer.Int(total_transactions);
    json_writer.EndObject();

    wxLogDebug("======= mmHomePagePanel::getStatWidget =======");
    wxLogDebug("RapidJson\n%s", wxString::FromUTF8(json_buffer.GetString()));

    return wxString::FromUTF8(json_buffer.GetString());
}

htmlWidgetStatistics::~htmlWidgetStatistics()
{
}

const wxString htmlWidgetGrandTotals::getHTMLText(double tBalance, double tReconciled, double tAssets, double tStocks)
{

    const wxString tReconciledStr  = wxString::Format("%s: <span class='money'>%s</span>"
                                        , _t("Reconciled")
                                        , Model_Currency::toCurrency(tReconciled));
    const wxString tAssetStr  = wxString::Format("%s: <span class='money'>%s</span>"
                                        , _t("Assets")
                                        , Model_Currency::toCurrency(tAssets));
    const wxString tStockStr  = wxString::Format("%s: <span class='money'>%s</span>"
                                        , _t("Stock")
                                        , Model_Currency::toCurrency(tStocks));
    const wxString tBalanceStr  = wxString::Format("%s: <span class='money'>%s</span>"
                                        , _t("Balance")
                                        , Model_Currency::toCurrency(tBalance));

    StringBuffer json_buffer;
    PrettyWriter<StringBuffer> json_writer(json_buffer);
    json_writer.StartObject();
    json_writer.Key("NAME");
    json_writer.String(_t("Total Net Worth").utf8_str());
    json_writer.Key("RECONVALUE");
    json_writer.String(tReconciledStr.utf8_str());
    json_writer.Key("ASSETVALUE");
    json_writer.String(tAssetStr.utf8_str());
    json_writer.Key("STOCKVALUE");
    json_writer.String(tStockStr.utf8_str());
    json_writer.Key("BALVALUE");
    json_writer.String(tBalanceStr.utf8_str());


    json_writer.EndObject();

    wxLogDebug("======= mmHomePagePanel::getGrandTotalsJSON =======");
    wxLogDebug("RapidJson\n%s", wxString::FromUTF8(json_buffer.GetString()));

    return wxString::FromUTF8(json_buffer.GetString());
}

htmlWidgetGrandTotals::~htmlWidgetGrandTotals()
{
}

const wxString htmlWidgetAssets::getHTMLText()
{
    Model_Account::Data_Set asset_accounts = Model_Account::instance().find(Model_Account::ACCOUNTTYPE(Model_Account::TYPE_NAME_ASSET));
    if (asset_accounts.empty())
        return wxEmptyString;

    std::stable_sort(asset_accounts.begin(), asset_accounts.end(), SorterByACCOUNTNAME());

    static const int MAX_ASSETS = 10;
    wxString output;
    output << R"(<div class="shadow">)"
           << R"(<table class='sortable table'><col style='width: 50%'><col style='width: 12.5%'><col style='width: 12.5%'><col style='width: 12.5%'><col style='width: 12.5%'>)"
           << "<thead><tr class='active'>\n"
           << "<th>" << _t("Assets") << "</th>"
           << "<th class='text-right'>" << _t("Initial Value") << "</th>\n"
           << "<th class='text-right'>" << _t("Current Value") << "</th>\n"
           << "<th class='text-right'>" << _t("Cash Balance") << "</th>\n"
           << "<th class='text-right'>" << _t("Total") << "</th>\n"
           << wxString::Format("<th nowrap class='text-right sorttable_nosort'><a id='%s_label' onclick='toggleTable(\"%s\");' href='#%s' oncontextmenu='return false;'>[-]</a></th>\n",
                               "ASSETS", "ASSETS", "ASSETS")
           << "</tr></thead><tbody id='ASSETS'>\n";

    int rows = 0;
    double initialDisplayed = 0.0, initialTotal = 0.0;
    double currentDisplayed = 0.0, currentTotal = 0.0;
    double cashDisplayed = 0.0, cashTotal = 0.0;

    auto renderRow = [](const wxString& name, double initial, double current, double cash) -> wxString {
        wxString row;
        row << "<tr>";
        row << wxString::Format("<td sorttable_customkey='*%s*'>%s</td>\n", name, name);
        row << wxString::Format("<td class='money' sorttable_customkey='%.2f'>%s</td>\n", initial, Model_Currency::toCurrency(initial));
        row << wxString::Format("<td class='money' sorttable_customkey='%.2f'>%s</td>\n", current, Model_Currency::toCurrency(current));
        row << wxString::Format("<td class='money' sorttable_customkey='%.2f'>%s</td>\n", cash, Model_Currency::toCurrency(cash));
        row << wxString::Format("<td colspan='2' class='money' sorttable_customkey='%.2f'>%s</td>\n", current + cash, Model_Currency::toCurrency(current + cash));
        row << "</tr>\n";
        return row;
    };

    for (const auto& asset : asset_accounts)
    {
        if (Model_Account::status_id(asset) != Model_Account::STATUS_ID_OPEN) continue;

        double cash = Model_Account::balance(asset);
        auto inv = Model_Account::investment_balance(asset);
        double current = inv.first;
        double initial = inv.second;

        initialTotal += initial;
        currentTotal += current;
        cashTotal += cash;

        if (rows++ < MAX_ASSETS)
        {
            initialDisplayed += initial;
            currentDisplayed += current;
            cashDisplayed += cash;
            output << renderRow(asset.ACCOUNTNAME, initial, current, cash);
        }
    }

    if (rows > MAX_ASSETS)
    {
        wxString otherAssets = _t("Other Assets");
        output << renderRow(wxString::Format("%s (%d)", otherAssets, rows - MAX_ASSETS),
                            initialTotal - initialDisplayed,
                            currentTotal - currentDisplayed,
                            cashTotal - cashDisplayed);
    }

    output << "<tfoot><tr class='total'><td>" << _t("Total:") << "</td>\n"
           << wxString::Format("<td class='money'>%s</td>\n", Model_Currency::toCurrency(initialTotal))
           << wxString::Format("<td class='money'>%s</td>\n", Model_Currency::toCurrency(currentTotal))
           << wxString::Format("<td class='money'>%s</td>\n", Model_Currency::toCurrency(cashTotal))
           << wxString::Format("<td colspan='2' class='money'>%s</td></tr></tfoot></table>\n",
                               Model_Currency::toCurrency(currentTotal + cashTotal))
           << "</div>";

    return output;
}

htmlWidgetAssets::~htmlWidgetAssets()
{
}

//

htmlWidgetAccounts::htmlWidgetAccounts()
{
    get_account_stats();
}

void htmlWidgetAccounts::get_account_stats()
{

    wxSharedPtr<mmDateRange> date_range;
    if (Option::instance().getIgnoreFutureTransactions())
        date_range = new mmCurrentMonthToDate;
    else
        date_range = new mmCurrentMonth;

    Model_Checking::Data_Set all_trans;
    if (Option::instance().getIgnoreFutureTransactions())
    {
        all_trans = Model_Checking::instance().find(Model_Checking::TRANSDATE(
            Option::instance().UseTransDateTime() ? wxDateTime::Now() : wxDateTime(23, 59, 59, 999), LESS_OR_EQUAL));
    }
    else
    {
        all_trans = Model_Checking::instance().all();
    }

    for (const auto& trx : all_trans)
    {
        accountStats_[trx.ACCOUNTID].first += Model_Checking::account_recflow(trx, trx.ACCOUNTID);
        accountStats_[trx.ACCOUNTID].second += Model_Checking::account_flow(trx, trx.ACCOUNTID);

        if (Model_Checking::type_id(trx) == Model_Checking::TYPE_ID_TRANSFER)
        {
            accountStats_[trx.TOACCOUNTID].first += Model_Checking::account_recflow(trx, trx.TOACCOUNTID);
            accountStats_[trx.TOACCOUNTID].second += Model_Checking::account_flow(trx, trx.TOACCOUNTID);
        }
    }

}

const wxString htmlWidgetAccounts::displayAccounts(double& tBalance, double& tReconciled, int type = Model_Account::TYPE_ID_CHECKING)
{
    static const std::vector < std::pair <wxString, wxString> > typeStr
    {
        { "CASH_ACCOUNTS_INFO",   _t("Cash Accounts") },
        { "ACCOUNTS_INFO",        _t("Bank Accounts") },
        { "CARD_ACCOUNTS_INFO",   _t("Credit Card Accounts") },
        { "LOAN_ACCOUNTS_INFO",   _t("Loan Accounts") },
        { "TERM_ACCOUNTS_INFO",   _t("Term Accounts") },
        { "INVEST_ACCOUNTS_INFO", _t("Investment Accounts") },
        { "ASSET_ACCOUNTS_INFO",  _t("Asset Accounts") },
        { "SHARE_ACCOUNTS_INFO",  _t("Share Accounts") },
    };

    const wxString idStr = typeStr[type].first;
    wxString output = "<table class = 'sortable table'>\n";
    output += R"(<col style="width:50%"><col style="width:25%"><col style="width:25%">)";
    output += "<thead><tr><th nowrap>\n";
    output += typeStr[type].second;

    output += "</th><th class = 'text-right'>" + _t("Reconciled") + "</th>\n";
    output += "<th class = 'text-right'>" + _t("Balance") + "</th>\n";
    output += wxString::Format("<th nowrap class='text-right sorttable_nosort'><a id='%s_label' onclick=\"toggleTable('%s'); \" href='#%s' oncontextmenu='return false;'>[-]</a></th>\n"
        , idStr, idStr, idStr);
    output += "</tr></thead>\n";
    output += wxString::Format("<tbody id = '%s'>\n", idStr);

    wxString body = "";
    const wxDate today = wxDate::Today();
    double tabBalance = 0.0, tabReconciled = 0.0;
    wxString vAccts = Model_Setting::instance().getViewAccounts();
    auto accounts = Model_Account::instance().find(
        Model_Account::ACCOUNTTYPE(Model_Account::type_name(type)),
        Model_Account::STATUS(Model_Account::STATUS_ID_CLOSED, NOT_EQUAL)
    );
    std::stable_sort(accounts.begin(), accounts.end(), SorterByACCOUNTNAME());
    for (const auto& account : accounts)
    {
        Model_Currency::Data* currency = Model_Account::currency(account);

        double currency_rate = Model_CurrencyHistory::getDayRate(account.CURRENCYID, today);
        double bal = account.INITIALBAL + accountStats_[account.ACCOUNTID].second; //Model_Account::balance(account);
        double reconciledBal = account.INITIALBAL + accountStats_[account.ACCOUNTID].first;
        tabBalance += bal * currency_rate;
        tabReconciled += reconciledBal * currency_rate;

        // show the actual amount in that account
        if (((vAccts == VIEW_ACCOUNTS_OPEN_STR && Model_Account::status_id(account) == Model_Account::STATUS_ID_OPEN) ||
            (vAccts == VIEW_ACCOUNTS_FAVORITES_STR && Model_Account::FAVORITEACCT(account)) ||
            (vAccts == VIEW_ACCOUNTS_ALL_STR)))
        {
            body += "<tr>";
            body += wxString::Format(R"(<td sorttable_customkey="*%s*" nowrap><a href="acct:%lld" oncontextmenu="return false;" target="_blank">%s</a>%s</td>)"
                , account.ACCOUNTNAME, account.ACCOUNTID, account.ACCOUNTNAME,
                account.WEBSITE.empty() ? "" : wxString::Format(R"(&nbsp;&nbsp;&nbsp;&nbsp;(<a href="%s" oncontextmenu="return false;" target="_blank">WWW</a>))", account.WEBSITE));
            body += wxString::Format("\n<td class='money' sorttable_customkey='%f' nowrap>%s</td>\n", reconciledBal, Model_Currency::toCurrency(reconciledBal, currency));
            body += wxString::Format("<td class='money' sorttable_customkey='%f' colspan='2' nowrap>%s</td>\n", bal, Model_Currency::toCurrency(bal, currency));
            body += "</tr>\n";
        }
    }
    output += body;
    output += "</tbody><tfoot><tr class ='total'><td>" + _t("Total:") + "</td>\n";
    output += "<td class='money'>" + Model_Currency::toCurrency(tabReconciled) + "</td>\n";
    output += "<td class='money' colspan='2'>" + Model_Currency::toCurrency(tabBalance) + "</td></tr></tfoot></table>\n";
    if (body.empty()) output.clear();

    tBalance += tabBalance;
    tReconciled += tabReconciled;

    return output;
}

htmlWidgetAccounts::~htmlWidgetAccounts()
{
}

// Currency exchange rates
const wxString htmlWidgetCurrency::getHtmlText()
{

    const char* currencyRatesTemplate = R"(
<div class = "shadow">
<table class="table">
<thead>
<tr class='active'><th><TMPL_VAR FRAME_NAME></th>
<th nowrap class='text-right sorttable_nosort'>
<a id='CURRENCY_RATES_label' onclick='toggleTable("CURRENCY_RATES");' href='#CURRENCY_RATES' oncontextmenu='return false;'>[-]</a>
</th></tr>
<tbody id='CURRENCY_RATES'>
<tr>
<td style='padding: 0px; padding-left: 0px; padding-right: 0px; width: 100%;' colspan='2'>

<table class="table">
<thead>
<tr><th nowrap class="text-right sorttable_nosort"></th><TMPL_VAR HEADER></tr>
</thead>
<tbody>
<TMPL_LOOP NAME=CONTENTS>
<tr><td class ='success'><TMPL_VAR CURRENCY_SYMBOL></td><TMPL_VAR CONVERSION_RATE></tr>
</TMPL_LOOP>
</tbody>
</table>

</td>
</tr>
</tbody>
</table>
</div>
)";


    const wxString today = wxDate::Today().FormatISODate();
    std::map<wxString, double> usedRates;
    const auto currencies = Model_Currency::instance().all();

    for (const auto &currency : currencies)
    {
        if (Model_Account::is_used(currency)) {

            double convertionRate = Model_CurrencyHistory::getDayRate(currency.CURRENCYID
                , today);
            usedRates[currency.CURRENCY_SYMBOL] = convertionRate;

            if (usedRates.size() >= 10) {
                break;
            }
        }
    }

    if (usedRates.size() == 1) {
        return "";
    }
    wxString header;
    loop_t contents;
    for (const auto& i : usedRates)
    {
        row_t r;
        r(L"CURRENCY_SYMBOL") = i.first;
        wxString row;
        for (const auto& j : usedRates)
        {
            double value = j.second / i.second;
            row += wxString::Format("<td %s>%s</td>"
                , j.first == i.first ? "class ='active'" : "class='money'"
                , j.first == i.first ? "" : Model_Currency::toString(value, nullptr, 4)
            );
        }
        header += wxString::Format("<th class='text-center'>%s</th>", i.first);
        r(L"CONVERSION_RATE") = row;

        contents += r;
    }
    mm_html_template report(currencyRatesTemplate);
    report(L"CONTENTS") = contents;
    report(L"FRAME_NAME") = _t("Currency Exchange Rates");
    report(L"HEADER") = header;

    wxString out = wxEmptyString;
    try
    {
        out = report.Process();
    }
    catch (const syntax_ex& e)
    {
        return e.what();
    }
    catch (...)
    {
        return _t("Caught exception");
    }

    return out;

}

htmlWidgetCurrency::~htmlWidgetCurrency()
{
}
