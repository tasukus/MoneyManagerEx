﻿// -*- C++ -*-
//=============================================================================
/**
 *      Copyright: (c) 2013 - 2025 Guan Lisheng (guanlisheng@gmail.com)
 *      Copyright: (c) 2017 - 2018 Stefano Giorgio (stef145g)
 *      Copyright: (c) 2022 Mark Whalley (mark@ipx.co.uk)
 *
 *      @file
 *
 *      @author [sqlite2cpp.py]
 *
 *      @brief
 *
 *      Revision History:
 *          AUTO GENERATED at 2025-05-08 09:16:56.228434.
 *          DO NOT EDIT!
 */
//=============================================================================
#pragma once

#include "DB_Table.h"

struct DB_Table_CURRENCYHISTORY_V1 : public DB_Table
{
    struct Data;
    typedef DB_Table_CURRENCYHISTORY_V1 Self;

    /** A container to hold list of Data records for the table*/
    struct Data_Set : public std::vector<Self::Data>
    {
        /**Return the data records as a json array string */
        wxString to_json() const
        {
            StringBuffer json_buffer;
            PrettyWriter<StringBuffer> json_writer(json_buffer);

            json_writer.StartArray();
            for (const auto & item: *this)
            {
                json_writer.StartObject();
                item.as_json(json_writer);
                json_writer.EndObject();
            }
            json_writer.EndArray();

            return json_buffer.GetString();
        }
    };

    /** A container to hold a list of Data record pointers for the table in memory*/
    typedef std::vector<Self::Data*> Cache;
    typedef std::map<int64, Self::Data*> Index_By_Id;
    Cache cache_;
    Index_By_Id index_by_id_;
    Data* fake_; // in case the entity not found

    /** Destructor: clears any data records stored in memory */
    ~DB_Table_CURRENCYHISTORY_V1() 
    {
        delete this->fake_;
        destroy_cache();
    }
     
    /** Removes all records stored in memory (cache) for the table*/ 
    void destroy_cache()
    {
        std::for_each(cache_.begin(), cache_.end(), std::mem_fn(&Data::destroy));
        cache_.clear();
        index_by_id_.clear(); // no memory release since it just stores pointer and the according objects are in cache
    }

    /** Creates the database table if the table does not exist*/
    bool ensure(wxSQLite3Database* db)
    {
        if (!exists(db))
        {
            try
            {
                db->ExecuteUpdate("CREATE TABLE CURRENCYHISTORY_V1(CURRHISTID INTEGER PRIMARY KEY, CURRENCYID INTEGER NOT NULL, CURRDATE TEXT NOT NULL, CURRVALUE NUMERIC NOT NULL, CURRUPDTYPE INTEGER, UNIQUE(CURRENCYID, CURRDATE))");
                this->ensure_data(db);
            }
            catch(const wxSQLite3Exception &e) 
            { 
                wxLogError("CURRENCYHISTORY_V1: Exception %s", e.GetMessage().utf8_str());
                return false;
            }
        }

        this->ensure_index(db);

        return true;
    }

    bool ensure_index(wxSQLite3Database* db)
    {
        try
        {
            db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS IDX_CURRENCYHISTORY_CURRENCYID_CURRDATE ON CURRENCYHISTORY_V1(CURRENCYID, CURRDATE)");
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CURRENCYHISTORY_V1: Exception %s", e.GetMessage().utf8_str());
            return false;
        }

        return true;
    }

    void ensure_data(wxSQLite3Database* db)
    {
        db->Begin();
        db->Commit();
    }
    
    struct CURRHISTID : public DB_Column<int64>
    { 
        static wxString name() { return "CURRHISTID"; } 
        explicit CURRHISTID(const int64 &v, OP op = EQUAL): DB_Column<int64>(v, op) {}
    };
    
    struct CURRENCYID : public DB_Column<int64>
    { 
        static wxString name() { return "CURRENCYID"; } 
        explicit CURRENCYID(const int64 &v, OP op = EQUAL): DB_Column<int64>(v, op) {}
    };
    
    struct CURRDATE : public DB_Column<wxString>
    { 
        static wxString name() { return "CURRDATE"; } 
        explicit CURRDATE(const wxString &v, OP op = EQUAL): DB_Column<wxString>(v, op) {}
    };
    
    struct CURRVALUE : public DB_Column<double>
    { 
        static wxString name() { return "CURRVALUE"; } 
        explicit CURRVALUE(const double &v, OP op = EQUAL): DB_Column<double>(v, op) {}
    };
    
    struct CURRUPDTYPE : public DB_Column<int64>
    { 
        static wxString name() { return "CURRUPDTYPE"; } 
        explicit CURRUPDTYPE(const int64 &v, OP op = EQUAL): DB_Column<int64>(v, op) {}
    };
    
    typedef CURRHISTID PRIMARY;
    enum COLUMN
    {
        COL_CURRHISTID = 0
        , COL_CURRENCYID = 1
        , COL_CURRDATE = 2
        , COL_CURRVALUE = 3
        , COL_CURRUPDTYPE = 4
    };

    /** Returns the column name as a string*/
    static wxString column_to_name(const COLUMN col)
    {
        switch(col)
        {
            case COL_CURRHISTID: return "CURRHISTID";
            case COL_CURRENCYID: return "CURRENCYID";
            case COL_CURRDATE: return "CURRDATE";
            case COL_CURRVALUE: return "CURRVALUE";
            case COL_CURRUPDTYPE: return "CURRUPDTYPE";
            default: break;
        }
        
        return "UNKNOWN";
    }

    /** Returns the column number from the given column name*/
    static COLUMN name_to_column(const wxString& name)
    {
        if ("CURRHISTID" == name) return COL_CURRHISTID;
        else if ("CURRENCYID" == name) return COL_CURRENCYID;
        else if ("CURRDATE" == name) return COL_CURRDATE;
        else if ("CURRVALUE" == name) return COL_CURRVALUE;
        else if ("CURRUPDTYPE" == name) return COL_CURRUPDTYPE;

        return COLUMN(-1);
    }
    
    /** Data is a single record in the database table*/
    struct Data
    {
        friend struct DB_Table_CURRENCYHISTORY_V1;
        /** This is a instance pointer to itself in memory. */
        Self* table_;
    
        int64 CURRHISTID;//  primary key
        int64 CURRENCYID;
        wxString CURRDATE;
        double CURRVALUE;
        int64 CURRUPDTYPE;

        int64 id() const
        {
            return CURRHISTID;
        }

        void id(const int64 id)
        {
            CURRHISTID = id;
        }

        auto operator < (const Data& other) const
        {
            return this->id() < other.id();
        }

        auto operator < (const Data* other) const
        {
            return this->id() < other->id();
        }

        bool equals(const Data* r) const
        {
            if(CURRHISTID != r->CURRHISTID) return false;
            if(CURRENCYID != r->CURRENCYID) return false;
            if(!CURRDATE.IsSameAs(r->CURRDATE)) return false;
            if(CURRVALUE != r->CURRVALUE) return false;
            if(CURRUPDTYPE != r->CURRUPDTYPE) return false;
            return true;
        }
        
        explicit Data(Self* table = nullptr ) 
        {
            table_ = table;
        
            CURRHISTID = -1;
            CURRENCYID = -1;
            CURRVALUE = 0.0;
            CURRUPDTYPE = -1;
        }

        explicit Data(wxSQLite3ResultSet& q, Self* table = nullptr )
        {
            table_ = table;
        
            CURRHISTID = q.GetInt64(0); // CURRHISTID
            CURRENCYID = q.GetInt64(1); // CURRENCYID
            CURRDATE = q.GetString(2); // CURRDATE
            CURRVALUE = q.GetDouble(3); // CURRVALUE
            CURRUPDTYPE = q.GetInt64(4); // CURRUPDTYPE
        }

        Data(const Data& other) = default;

        Data& operator=(const Data& other)
        {
            if (this == &other) return *this;

            CURRHISTID = other.CURRHISTID;
            CURRENCYID = other.CURRENCYID;
            CURRDATE = other.CURRDATE;
            CURRVALUE = other.CURRVALUE;
            CURRUPDTYPE = other.CURRUPDTYPE;
            return *this;
        }

        template<typename C>
        bool match(const C &) const
        {
            return false;
        }

        bool match(const Self::CURRHISTID &in) const
        {
            return this->CURRHISTID == in.v_;
        }

        bool match(const Self::CURRENCYID &in) const
        {
            return this->CURRENCYID == in.v_;
        }

        bool match(const Self::CURRDATE &in) const
        {
            return this->CURRDATE.CmpNoCase(in.v_) == 0;
        }

        bool match(const Self::CURRVALUE &in) const
        {
            return this->CURRVALUE == in.v_;
        }

        bool match(const Self::CURRUPDTYPE &in) const
        {
            return this->CURRUPDTYPE == in.v_;
        }

        // Return the data record as a json string
        wxString to_json() const
        {
            StringBuffer json_buffer;
            PrettyWriter<StringBuffer> json_writer(json_buffer);

			json_writer.StartObject();			
			this->as_json(json_writer);
            json_writer.EndObject();

            return json_buffer.GetString();
        }

        // Add the field data as json key:value pairs
        void as_json(PrettyWriter<StringBuffer>& json_writer) const
        {
            json_writer.Key("CURRHISTID");
            json_writer.Int64(this->CURRHISTID.GetValue());
            json_writer.Key("CURRENCYID");
            json_writer.Int64(this->CURRENCYID.GetValue());
            json_writer.Key("CURRDATE");
            json_writer.String(this->CURRDATE.utf8_str());
            json_writer.Key("CURRVALUE");
            json_writer.Double(this->CURRVALUE);
            json_writer.Key("CURRUPDTYPE");
            json_writer.Int64(this->CURRUPDTYPE.GetValue());
        }

        row_t to_row_t() const
        {
            row_t row;
            row(L"CURRHISTID") = CURRHISTID.GetValue();
            row(L"CURRENCYID") = CURRENCYID.GetValue();
            row(L"CURRDATE") = CURRDATE;
            row(L"CURRVALUE") = CURRVALUE;
            row(L"CURRUPDTYPE") = CURRUPDTYPE.GetValue();
            return row;
        }

        void to_template(html_template& t) const
        {
            t(L"CURRHISTID") = CURRHISTID.GetValue();
            t(L"CURRENCYID") = CURRENCYID.GetValue();
            t(L"CURRDATE") = CURRDATE;
            t(L"CURRVALUE") = CURRVALUE;
            t(L"CURRUPDTYPE") = CURRUPDTYPE.GetValue();
        }

        /** Save the record instance in memory to the database. */
        bool save(wxSQLite3Database* db, bool force_insert = false)
        {
            if (db && db->IsReadOnly()) return false;
            if (!table_ || !db) 
            {
                wxLogError("can not save CURRENCYHISTORY_V1");
                return false;
            }

            return table_->save(this, db, force_insert);
        }

        /** Remove the record instance from memory and the database. */
        bool remove(wxSQLite3Database* db)
        {
            if (!table_ || !db) 
            {
                wxLogError("can not remove CURRENCYHISTORY_V1");
                return false;
            }
            
            return table_->remove(this, db);
        }

        void destroy()
        {
            delete this;
        }
    };

    enum
    {
        NUM_COLUMNS = 5
    };

    size_t num_columns() const { return NUM_COLUMNS; }

    /** Name of the table*/    
    wxString name() const { return "CURRENCYHISTORY_V1"; }

    DB_Table_CURRENCYHISTORY_V1() : fake_(new Data())
    {
        query_ = "SELECT CURRHISTID, CURRENCYID, CURRDATE, CURRVALUE, CURRUPDTYPE FROM CURRENCYHISTORY_V1 ";
    }

    /** Create a new Data record and add to memory table (cache)*/
    Self::Data* create()
    {
        Self::Data* entity = new Self::Data(this);
        cache_.push_back(entity);
        return entity;
    }
    
    /** Create a copy of the Data record and add to memory table (cache)*/
    Self::Data* clone(const Data* e)
    {
        Self::Data* entity = create();
        *entity = *e;
        entity->id(-1);
        return entity;
    }

    /**
    * Saves the Data record to the database table.
    * Either create a new record or update the existing record.
    * Remove old record from the memory table (cache)
    */
    bool save(Self::Data* entity, wxSQLite3Database* db, bool force_insert = false)
    {
        wxString sql = wxEmptyString;
        if (entity->id() <= 0 || force_insert) //  new & insert
        {
            sql = "INSERT INTO CURRENCYHISTORY_V1(CURRENCYID, CURRDATE, CURRVALUE, CURRUPDTYPE, CURRHISTID) VALUES(?, ?, ?, ?, ?)";
        }
        else
        {
            sql = "UPDATE CURRENCYHISTORY_V1 SET CURRENCYID = ?, CURRDATE = ?, CURRVALUE = ?, CURRUPDTYPE = ? WHERE CURRHISTID = ?";
        }

        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(sql);

            stmt.Bind(1, entity->CURRENCYID);
            stmt.Bind(2, entity->CURRDATE);
            stmt.Bind(3, entity->CURRVALUE);
            stmt.Bind(4, entity->CURRUPDTYPE);
            stmt.Bind(5, entity->id() > 0 ? entity->CURRHISTID : newId());

            stmt.ExecuteUpdate();
            stmt.Finalize();

            if (entity->id() > 0) // existent
            {
                for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
                {
                    Self::Data* e = *it;
                    if (e->id() == entity->id()) 
                        *e = *entity;  // in-place update
                }
            }
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CURRENCYHISTORY_V1: Exception %s, %s", e.GetMessage().utf8_str(), entity->to_json());
            return false;
        }

        if (entity->id() <= 0)
        {
            entity->id(db->GetLastRowId());
            index_by_id_.insert(std::make_pair(entity->id(), entity));
        }
        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(const int64 id, wxSQLite3Database* db)
    {
        if (id <= 0) return false;
        try
        {
            wxString sql = "DELETE FROM CURRENCYHISTORY_V1 WHERE CURRHISTID = ?";
            wxSQLite3Statement stmt = db->PrepareStatement(sql);
            stmt.Bind(1, id);
            stmt.ExecuteUpdate();
            stmt.Finalize();

            Cache c;
            for(Cache::iterator it = cache_.begin(); it != cache_.end(); ++ it)
            {
                Self::Data* entity = *it;
                if (entity->id() == id) 
                {
                    index_by_id_.erase(entity->id());
                    delete entity;
                }
                else 
                {
                    c.push_back(entity);
                }
            }
            cache_.clear();
            cache_.swap(c);
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("CURRENCYHISTORY_V1: Exception %s", e.GetMessage().utf8_str());
            return false;
        }

        return true;
    }

    /** Remove the Data record from the database and the memory table (cache) */
    bool remove(Self::Data* entity, wxSQLite3Database* db)
    {
        if (remove(entity->id(), db))
        {
            entity->id(-1);
            return true;
        }

        return false;
    }

    template<typename... Args>
    Self::Data* get_one(const Args& ... args)
    {
        for (auto& [_, item] : index_by_id_)
        {
            if (item->id() > 0 && match(item, args...)) 
            {
                ++ hit_;
                return item;
            }
        }

        ++ miss_;

        return 0;
    }
    
    /**
    * Search the memory table (Cache) for the data record.
    * If not found in memory, search the database and update the cache.
    */
    Self::Data* get(const int64 id, wxSQLite3Database* db)
    {
        if (id <= 0) 
        {
            ++ skip_;
            return nullptr;
        }

        if (auto it = index_by_id_.find(id); it != index_by_id_.end())
        {
            ++ hit_;
            return it->second;
        }
        
        ++ miss_;
        Self::Data* entity = nullptr;
        wxString where = wxString::Format(" WHERE %s = ?", PRIMARY::name().utf8_str());
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

            wxSQLite3ResultSet q = stmt.ExecuteQuery();
            if(q.NextRow())
            {
                entity = new Self::Data(q, this);
                cache_.push_back(entity);
                index_by_id_.insert(std::make_pair(id, entity));
            }
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().utf8_str(), e.GetMessage().utf8_str());
        }
        
        if (!entity) 
        {
            entity = this->fake_;
            // wxLogError("%s: %d not found", this->name().utf8_str(), id);
        }
 
        return entity;
    }
    /**
    * Search the database for the data record, bypassing the cache.
    */
    Self::Data* get_record(const int64 id, wxSQLite3Database* db)
    {
        if (id <= 0) 
        {
            ++ skip_;
            return nullptr;
        }

        Self::Data* entity = nullptr;
        wxString where = wxString::Format(" WHERE %s = ?", PRIMARY::name().utf8_str());
        try
        {
            wxSQLite3Statement stmt = db->PrepareStatement(this->query() + where);
            stmt.Bind(1, id);

            wxSQLite3ResultSet q = stmt.ExecuteQuery();
            if(q.NextRow())
            {
                entity = new Self::Data(q, this);
            }
            stmt.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().utf8_str(), e.GetMessage().utf8_str());
        }
        
        if (!entity) 
        {
            entity = this->fake_;
            // wxLogError("%s: %d not found", this->name().utf8_str(), id);
        }
 
        return entity;
    }

    /**
    * Return a list of Data records (Data_Set) derived directly from the database.
    * The Data_Set is sorted based on the column number.
    */
    const Data_Set all(wxSQLite3Database* db, const COLUMN col = COLUMN(0), const bool asc = true)
    {
        Data_Set result;
        try
        {
            wxSQLite3ResultSet q = db->ExecuteQuery(col == COLUMN(0) ? this->query() : this->query() + " ORDER BY " + column_to_name(col) + " COLLATE NOCASE " + (asc ? " ASC " : " DESC "));

            while(q.NextRow())
            {
                Self::Data entity(q, this);
                result.push_back(std::move(entity));
            }

            q.Finalize();
        }
        catch(const wxSQLite3Exception &e) 
        { 
            wxLogError("%s: Exception %s", this->name().utf8_str(), e.GetMessage().utf8_str());
        }

        return result;
    }
};

