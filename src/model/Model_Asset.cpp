/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)
 Copyright (C) 2022 Mark Whalley (mark@ipx.co.uk)

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

#include "Model_Asset.h"
#include "Model_Translink.h"
#include "Model_CurrencyHistory.h"

ChoicesName Model_Asset::TYPE_CHOICES = ChoicesName({
    { TYPE_ID_PROPERTY,  _n("Property") },
    { TYPE_ID_AUTO,      _n("Automobile") },
    { TYPE_ID_HOUSE,     _n("Household Object") },
    { TYPE_ID_ART,       _n("Art") },
    { TYPE_ID_JEWELLERY, _n("Jewellery") },
    { TYPE_ID_CASH,      _n("Cash") },
    { TYPE_ID_OTHER,     _n("Other") }
});

ChoicesName Model_Asset::STATUS_CHOICES = ChoicesName({
    { STATUS_ID_CLOSED, _n("Closed") },
    { STATUS_ID_OPEN,   _n("Open") }
});

ChoicesName Model_Asset::CHANGE_CHOICES = ChoicesName({
    { CHANGE_ID_NONE,       _n("None") },
    { CHANGE_ID_APPRECIATE, _n("Appreciates") },
    { CHANGE_ID_DEPRECIATE, _n("Depreciates") }
});

ChoicesName Model_Asset::CHANGEMODE_CHOICES = ChoicesName({
    { CHANGEMODE_ID_PERCENTAGE, _n("Percentage") },
    { CHANGEMODE_ID_LINEAR,     _n("Linear") }
});

Model_Asset::Model_Asset()
: Model<DB_Table_ASSETS_V1>()
{
}

Model_Asset::~Model_Asset()
{
}

/**
* Initialize the global Model_Asset table.
* Reset the Model_Asset table or create the table if it does not exist.
*/
Model_Asset& Model_Asset::instance(wxSQLite3Database* db)
{
    Model_Asset& ins = Singleton<Model_Asset>::instance();
    ins.db_ = db;
    ins.destroy_cache();
    ins.ensure(db);

    return ins;
}

/** Return the static instance of Model_Asset table */
Model_Asset& Model_Asset::instance()
{
    return Singleton<Model_Asset>::instance();
}

wxString Model_Asset::get_asset_name(int64 asset_id)
{
    Data* asset = instance().get(asset_id);
    if (asset)
        return asset->ASSETNAME;
    else
        return _t("Asset Error");
}

double Model_Asset::balance()
{
    double balance = 0.0;
    for (const auto& r: this->all())
    {
        balance += value(r).second;
    }
    return balance;
}

DB_Table_ASSETS_V1::ASSETTYPE Model_Asset::ASSETTYPE(TYPE_ID type, OP op)
{
    return DB_Table_ASSETS_V1::ASSETTYPE(type_name(type), op);
}

DB_Table_ASSETS_V1::STARTDATE Model_Asset::STARTDATE(const wxDate& date, OP op)
{
    return DB_Table_ASSETS_V1::STARTDATE(date.FormatISODate(), op);
}

wxDate Model_Asset::STARTDATE(const Data* r)
{
    return Model::to_date(r->STARTDATE);
}

wxDate Model_Asset::STARTDATE(const Data& r)
{
    return Model::to_date(r.STARTDATE);
}

Model_Currency::Data* Model_Asset::currency(const Data* /* r */)
{
    return Model_Currency::instance().GetBaseCurrency();
}

std::pair<double, double> Model_Asset::value(const Data* r)
{
    return instance().valueAtDate(r, wxDate::Today());
}

std::pair<double, double> Model_Asset::value(const Data& r)
{
    return instance().valueAtDate(&r, wxDate::Today());
}

std::pair<double, double> Model_Asset::valueAtDate(const Data* r, const wxDate date)
{
    std::pair<double /*initial*/, double /*market*/> balance;
    if (date >= STARTDATE(r))
    {
        Model_Translink::Data_Set translink_records = Model_Translink::instance().find(
            Model_Translink::LINKRECORDID(r->ASSETID),
            Model_Translink::LINKTYPE(Model_Attachment::REFTYPE_NAME_ASSET)
        );
        if (!translink_records.empty())
        {
            Model_Checking::Data_Set trans;
            for (const auto& link : translink_records)
            {
                const Model_Checking::Data* tran = Model_Checking::instance().get(link.CHECKINGACCOUNTID);
                if(tran && tran->DELETEDTIME.IsEmpty()) trans.push_back(*tran);
            }

            std::stable_sort(trans.begin(), trans.end(), SorterByTRANSDATE());

            wxDate last = date;
            for (const auto& tran: trans)
            {
                const wxDate tranDate = Model_Checking::TRANSDATE(tran);
                if (tranDate > date) break;

                if (last == date) last = tranDate;
                if (last < tranDate)
                {
                    wxTimeSpan diff_time = tranDate - last;
                    double diff_time_in_days = static_cast<double>(diff_time.GetDays());

                    int changeType = change_id(r);
                    if (changeType == CHANGE_ID_APPRECIATE)
                    {
                        balance.second *= exp((r->VALUECHANGERATE / 36500.0) * diff_time_in_days);
                    }
                    else if (changeType == CHANGE_ID_DEPRECIATE)
                    {
                        balance.second *= exp((-r->VALUECHANGERATE / 36500.0) * diff_time_in_days);
                    }

                    last = tranDate;
                }

                double amount = -1 * Model_Checking::account_flow(tran, tran.ACCOUNTID) *
                    Model_CurrencyHistory::getDayRate(Model_Account::instance().get(tran.ACCOUNTID)->CURRENCYID, tranDate);

                if (amount < 0) // sale, use unrealized g/l, and try best to keep the principal
                {
                    double unrealized_gl = balance.second -  balance.first;
                    balance.first += std::min(unrealized_gl + amount, 0.0);
                }
                else
                    balance.first += amount;

                balance.second += amount;

                // Self Transfer as Revaluation
                if (tran.ACCOUNTID == tran.TOACCOUNTID && Model_Checking::type_id(tran.TRANSCODE) == Model_Checking::TYPE_ID_TRANSFER)
                {
                    // TODO honor TRANSAMOUNT => TOTRANSAMOUNT
                    balance.second = tran.TOTRANSAMOUNT;
                }
            }

            wxTimeSpan diff_time = date - last;
            double diff_time_in_days = static_cast<double>(diff_time.GetDays());
            int changeType = change_id(r);
            if (changeType == CHANGE_ID_APPRECIATE)
            {
                balance.second *= exp((r->VALUECHANGERATE / 36500.0) * diff_time_in_days);
            }
            else if (changeType == CHANGE_ID_DEPRECIATE)
            {
                balance.second *= exp((-r->VALUECHANGERATE / 36500.0) * diff_time_in_days);
            }
        }
        else 
        {
            balance = {r->VALUE, r->VALUE};
            wxTimeSpan diff_time = date - STARTDATE(r);
            double diff_time_in_days = static_cast<double>(diff_time.GetDays());

            switch (change_id(r))
            {
            case CHANGE_ID_NONE:
                break;
            case CHANGE_ID_APPRECIATE:
                balance.second *= pow(1.0 + (r->VALUECHANGERATE / 36500.0), diff_time_in_days);
                break;
            case CHANGE_ID_DEPRECIATE:
                balance.second *= pow(1.0 - (r->VALUECHANGERATE / 36500.0), diff_time_in_days);
                break;
            default:
                break;
            }
        }
    }
    return balance;
}
