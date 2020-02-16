/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)

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

#ifndef MODEL_SPLITTRANSACTIONS_H
#define MODEL_SPLITTRANSACTIONS_H

#include "Model.h"
#include "Model_Currency.h"
#include "Table_Splittransactions.h"

struct Split
{
    Split ( const int cagte = 0, const int subCage = 0, const double splittransamount = 0.0 )
    {
        CATEGID = cagte;
        SUBCATEGID = subCage;
        SPLITTRANSAMOUNT = splittransamount;
    }
    int CATEGID = 0;
    int SUBCATEGID = 0;
    double SPLITTRANSAMOUNT = 0.0;
};

class Model_Splittransaction : public Model<DB_Table_SPLITTRANSACTIONS>
{
public:
    Model_Splittransaction();
    ~Model_Splittransaction();

public:
    /**
    Initialize the global Model_Splittransaction table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for Model_Splittransaction table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Splittransaction &instance ( wxSQLite3Database *db );

    /**
    * Return the static instance address for Model_Splittransaction table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Splittransaction &instance();

public:
    static double get_total ( const Data_Set &rows );
    static double get_total ( const std::vector<Split> &local_splits );
    static const wxString get_tooltip ( const std::vector<Split> &local_splits, const Model_Currency::Data *currency );
    std::map<int, Model_Splittransaction::Data_Set> get_all();
    size_t update ( const Data_Set &rows, int transactionID );
};

#endif
