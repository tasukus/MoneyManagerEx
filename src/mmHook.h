/*******************************************************
 Copyright (C) 2014 Guan Lisheng (guanlisheng@gmail.com)

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
#include "option.h"

class CommitCallbackHook : public wxSQLite3Hook
{
public:
    bool CommitCallback() override
    {
        Option::instance().setDatabaseUpdated ( true );
        return false;
    }
};

class UpdateCallbackHook : public wxSQLite3Hook
{
public:
    void UpdateCallback ( wxUpdateType type, const wxString &database, const wxString &table, wxLongLong rowid ) override
    {
        switch ( type )
        {
            case SQLITE_DELETE:
                wxLogDebug ( "SQLITE_DELETE" );
                break;
            case SQLITE_INSERT:
                wxLogDebug ( "SQLITE_INSERT" );
                break;
            case SQLITE_UPDATE:
                wxLogDebug ( "SQLITE_UPDATE" );
                break;
            default:
                wxLogDebug ( "UNKNOWN type" );
                break;
        }
        wxLogDebug ( "database: %s, table: %s, rowid: %lld", database, table, rowid );

        // TODO sync search index from full text search
    }
};
