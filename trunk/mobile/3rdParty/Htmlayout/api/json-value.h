/*
 * Terra Informatica Sciter Engine
 * http://terrainformatica.com/sciter
 * 
 * JSON value class. 
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2006, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief value, aka variant, aka discriminated union
 *
 * classes here:
 *
 * json::value - the value
 * json::string - wchar string with refcounted data 
 * json::named_value - key/value pair
 *  
 **/


#ifndef __json_value_h__
#define __json_value_h__

#pragma once

#pragma pack(push,4)

#ifdef __cplusplus

#include "json-aux.h"
#include "json-slice.h"

/**json namespace.*/
namespace json
{
  using aux::slice;
  using aux::wchars;

  struct named_value;
  class  string;
   
  /// \class value
  /// \brief The value type, can hold values of bool, int, double, string(wchar_t) and array(value)
  /// value is also known as JSON value, see: http://json.org/

  struct value 
  {
    friend struct array_data;
    friend struct bytes_data;
    friend struct string_data;
    friend class  value_istream;
    friend class  value_ostream;

  public:

    /// Type of the value.
    enum VALUETYPE 
    { 
      V_UNDEFINED = 0,  ///< empty
      V_BOOL = 1,       ///< bool
      V_INT = 2,        ///< int
      V_REAL = 3,       ///< double
      V_STRING = 4,     ///< string of wchar_t
      V_ARRAY = 5,      ///< array of value elements
      V_MAP = 6,        ///< map of name/value pairs - simple map
      V_BYTES = 7       ///< vector of bytes, a.k.a. blob
    };

    value()                            :v_type(V_UNDEFINED) { data.l_val = 0; }
    value(const value& src) :v_type(V_UNDEFINED) { set(src); }
    
    /// constructor
    value(bool v)             :v_type(V_UNDEFINED) { set(v); }
    /// constructor
    value(int v)              :v_type(V_UNDEFINED) { set(v); }
    value(unsigned int v)     :v_type(V_UNDEFINED) { set((int)v); }
    /// constructor
    value(double v)           :v_type(V_UNDEFINED) { set(v); }
    /// constructor
    value(const byte* utf8, size_t length) :v_type(V_UNDEFINED) { set(utf8, length); }
    value(const byte* utf8) :v_type(V_UNDEFINED) { if(utf8) set(utf8, strlen((const char*)utf8)); }
    /// constructor
    value(const wchar_t* psz) :v_type(V_UNDEFINED) { set(psz); }
    value(const wchar_t* ps, size_t length) :v_type(V_UNDEFINED) { set(ps, length); }
    /// constructor
    value(const value* v, size_t n) :v_type(V_UNDEFINED) { set_array(v,n); }
    /// constructor
    value(const string& s);
        
    /// simple value::array() constructors
    static value array( value v0 )  { value t; t.set_array(0,1); t.nth(0,v0); }
    static value array( value v0, value v1 ) 
                                    { value t; t.set_array(0,2); t.nth(0,v0); t.nth(1,v1); }
    static value array( value v0, value v1, value v2 ) 
                                    { value t; t.set_array(0,3); t.nth(0,v0); t.nth(1,v1); t.nth(2,v2);  }
    static value array( value v0, value v1, value v2, value v3 ) 
                                    { value t; t.set_array(0,4); t.nth(0,v0); t.nth(1,v1); t.nth(2,v2); t.nth(3,v3);  }
    static value array( value v0, value v1, value v2, value v3, value v4 ) 
                                    { value t; t.set_array(0,5); t.nth(0,v0); t.nth(1,v1); t.nth(2,v2); t.nth(3,v3); t.nth(4,v4); }

    static value bytes( slice<byte> data, bool copy ) { value t; t.set_bytes(data,copy); return t; }

    /// current type
    VALUETYPE  type() const  { return v_type; }

    /// clear it and free resources
    inline void  clear();

    /// set new value
    inline void  set(const value& src); 

    /// assign utf8 encoded string
    void  set(const byte* utf8, size_t length);
    /// assign string
    void  set(const wchar_t* src);
    void  set(const wchar_t* src, size_t length);
    void  set(const string& src);
    /// assign int
    void  set(int i)    { clear(); data.i_val = i; v_type = V_INT; }
    /// assign bool
    void  set(bool b)   { clear(); data.i_val = (int)b; v_type = V_BOOL; }
    /// assign float
    void  set(double r) { clear(); data.r_val = r; v_type = V_REAL; }
    /// assign vector of values
    void  set_array(const value* v, size_t n);
    /// assign value to key, convert this value to map if needed.
    void  set(const value& key, const value& val)
    {
      v2k(key, val);
    }
    void  set(const wchar_t* key, const value& val)
    {
      value vk(key); v2k(vk, val);
    }
    void set_bytes( slice<byte> data, bool copy ); 

    void  add(const wchar_t* key, const value& val)
    {
      value vk(key); v2k(vk, val);
    }
    value item(const wchar_t* key ) const
    {
      if( !is_map() ) return value();
      return k2v(value(key));
    }
    
    ~value()  { clear(); }

    /// indicator
    bool is_undefined() const { return (v_type == V_UNDEFINED); }
    /// indicator
    bool is_int() const { return (v_type == V_INT); }
    /// indicator
    bool is_real() const { return (v_type == V_REAL); }
    /// indicator
    bool is_bool() const { return (v_type == V_BOOL); }
    /// indicator
    bool is_string() const { return (v_type == V_STRING); }
    /// indicator
    bool is_array() const { return (v_type == V_ARRAY); }
    /// indicator
    bool is_map() const { return (v_type == V_MAP); }
    /// indicator
    bool is_bytes() const { return (v_type == V_BYTES); }

    value& operator = (const wchar_t* ps) { set(ps); return *this; }
    value& operator = (int v) { set(v); return *this; }
    value& operator = (double v) { set(v); return *this; }
    value& operator = (bool v) { set(v); return *this; }
    value& operator = (const value& v) { set(v); return *this; }
    value& operator = (const string& s) { set(s); return *this; }
    
    //value& operator = (const std::vector<value>& a) { if(a.size() != 0) set(&a[0], a.size()); return *this; }
    //value& operator = (const std::wstring& s) { set(s.c_str()); return *this; }

    bool operator == ( const value& rs ) const;

    bool operator != ( const value& rs ) const { return !(operator==(rs)); }

    /// fetcher
    int                 get(int def_val) const 
                        { 
                          if(v_type == V_REAL) return int(data.r_val); 
                          if(v_type == V_INT) return data.i_val; 
                          return def_val;
                        }
    uint                get(unsigned int def_val) const 
                        { 
                          if(v_type == V_REAL) return (unsigned int)(data.r_val); 
                          if(v_type == V_INT) return (unsigned int)data.i_val; 
                          return def_val;
                        }
    /// fetcher
    double              get(double def_val) const  
                        { 
                          if(v_type == V_REAL) return data.r_val; 
                          if(v_type == V_INT) return double(data.i_val); 
                          return def_val;
                        }
    /// fetcher
    bool                get(bool def_val) const { return (v_type == V_BOOL)? (data.i_val != 0): def_val; }
    /// fetcher
    const wchar_t*      get(const wchar_t* def_val) const;
/// fetcher
    bool                get(slice<byte>& data) const;
    

    /// get array size and n-th element (if any)        
    int                 length() const;
    /// get n-th element of the array
    value               nth(int n) const;
    /// set n-th element of the array, if the value is not an array then convert to array first.
    void                nth(int n, const value& t);
    /// sappend element to the array, if the value is not an array then convert to array first.
    void                append(const value& t);

    /// get value by key in the map
    value               k2v(const value& k) const;
    /// set value to key in the map
    void                v2k(const value& k, const value& v);
    /// set value to key in the map


    const named_value*  get_first() const; 


    // Joel's rewording of the gets above
    /// converter
    operator bool()           { assert(v_type == V_BOOL); return get( false ); }
    /// converter
    operator int()            { assert(v_type == V_INT); return get( 0 ); }
    /// converter
    operator double()         { assert(v_type == V_REAL); return get( 0.0 ); }
    /// converter
    operator const wchar_t*() { assert(v_type == V_STRING); return get((const wchar_t*)0); }
    /// indexer
    value operator []( int index ) const { assert(v_type == V_ARRAY); if(v_type == V_ARRAY) return  nth( index ); return value(); }
    /// map read access
    const value operator []( const value& key ) const { assert(v_type == V_MAP); if(v_type == V_MAP) return  k2v( key ); return value(); }
    /// map read access
    const value operator []( const wchar_t* key ) const { assert(v_type == V_MAP); if(v_type == V_MAP) { value vkey(key); return k2v( vkey ); } return value(); }
   
    /// converter
    string to_string() const;

    void emit(utf8::ostream& os) const;
   
    /// converter
    static value from_string(const wchar_t* str)
    {
      if(str == 0 || str[0] == 0)
        return value(); // undefined

      //guessing conversion
      const wchar_t* endstr = str + wcslen(str);
      wchar_t* lastptr;
      int i = wcstol(str,&lastptr,10); if(lastptr == endstr) return value(i);
      double d = wcstod(str,&lastptr); if(lastptr == endstr) return value(d);
      if(wcsicmp(str,L"true")==0) return value(true);
      if(wcsicmp(str,L"false")==0) return value(false);
      return value(str);
    }

    //static void* mem_alloc(size_t sz) { return ::HeapAlloc( ::GetProcessHeap(), HEAP_ZERO_MEMORY, sz ); }
    //static void  mem_free(void* mp) { ::HeapFree( ::GetProcessHeap(), 0, mp ); }

    typedef void (__cdecl *wiper_t)( void * );

  protected:
   
    VALUETYPE      v_type;
    
    union data_slot 
    {
      int            i_val;
      double         r_val;
      uint64         l_val;
      string_data*   s_val;
      array_data*    a_val; // array data
      named_value*   m_val; // simple map of name/value pairs.
      bytes_data*    b_val; // vector of bytes
    } data;
    
  };

  struct named_value
  {
    //friend class value_ostream;
    //friend class value_istream;
    value           key;
    value           val;
    named_value*    next;
    value::wiper_t  wipe;
    long            n_refs;

    named_value(): next(0), n_refs(1) {}
    ~named_value() { assert(n_refs == 0);  }

  public:


    void add_ref() { ++n_refs; }
    void release() 
    { 
      if (--n_refs <= 0) 
      {
        if(next) next->release();
        wipe(this); 
      }
    }

    static named_value* find( named_value* root, const value& k ) 
      { named_value* t = root; while(t) if( t->key == k ) return t; else t = t->next; return 0; }

    static bool get( const named_value* root, const value& k, value& v )
      { named_value* t = find( const_cast<named_value*>(root),k); if( t ) { v = t->val; return true; } return false; }
    
    static void set( named_value* &root, const value& k, const value& v )
    {   
        named_value* t = find(root,k); 
        if( !t ) 
        {
          t = (named_value*)zalloc(sizeof(named_value));
          t->wipe = &free;
          t->n_refs = 1;
          t->next = root;
          t->key = k;
          root = t; 
        }
        t->val = v;
    }

    static size_t length(const named_value* seq) 
    {
      size_t sz = 0;
      while( seq ) { ++sz; seq = seq->next; }
      return sz;
    }
  };

  struct array_data
  {
    volatile long   ref_count;
    size_t          allocated; // length of allocated buffer 
    size_t          length;    // number of elements used in the buffer 
    value::wiper_t  wipe;
    // value elements[] - go here
    value* elements() { return reinterpret_cast<value*>(this + 1); }

    static array_data* allocate( const value* values, size_t n, size_t extra )
    {
      array_data* ad = (array_data*) zalloc( sizeof(array_data) + (n + extra) * sizeof(value) );
      ad->wipe = &free;
      ad->ref_count = 1;
      ad->length = n;
      ad->allocated = n + extra;
      value* v = ad->elements();
      for( size_t i = 0; i < n; ++i ) { v[i] = values[i]; }
      return ad;
    }
    static array_data* allocate( size_t n, size_t extra )
    {
      array_data* ad = (array_data*) zalloc( sizeof(array_data) + (n + extra) * sizeof(value) );
      ad->wipe = &free;
      ad->ref_count = 1;
      ad->length = n;
      ad->allocated = n + extra;
      return ad;
    }

    static array_data* reallocate( array_data* a, size_t n )
    {
      if( n <= a->allocated )
      {
        a->length = n;
        return a;
      }
      else
      {
        array_data* t = allocate( n, n / 2 );
        value *dst = t->elements();
        value *src = a->elements();
        size_t tocopy = a->length < n?a->length:n;
        while( tocopy-- ) *dst++ = *src++;
        a->release();
        return t;
      }
    }
    
    void release()
    {
      if( --ref_count == 0 )
      {
        value* els = elements();
        for( size_t i = 0; i < length; ++i ) els[i].~value();
        wipe(this);
      }
    }
    void add_ref()
    {
      ++ref_count;
    }

    static bool equal( const array_data* v1, const array_data* v2 )
    {
      if( v1->length != v2->length ) return false;
      value* a1 = const_cast<array_data*>(v1)->elements();
      value* a2 = const_cast<array_data*>(v2)->elements();
      for( size_t i = 0; i < v1->length; ++i )
        if( a1[i] != a2[i] ) return false;
      return true;
    }
  };

  struct bytes_data
  {
    volatile long   ref_count;
    const byte*     data;
    size_t          length;    // number of elements used in the buffer 
    value::wiper_t  wipe;
    // value elements[] - go here
    slice<byte> elements() const { return slice<byte>(data,uint(length)); }

    static bytes_data* allocate_copy( slice<byte> data )
    {
      bytes_data* bd = (bytes_data*) zalloc( sizeof(bytes_data) + data.length );
      bd->wipe = &free;
      bd->ref_count = 1;
      bd->length = data.length;
      bd->data = (byte*)(bd+1);
      memcpy(const_cast<byte*>(bd->data), data.start, data.length);
      return bd;
    }

    static bytes_data* allocate_ref( slice<byte> data )
    {
      bytes_data* bd = (bytes_data*) zalloc(sizeof(bytes_data));
      bd->wipe = &free;
      bd->ref_count = 1;
      bd->length = data.length;
      bd->data = data.start;
      return bd;
    }
    
    void release()
    {
      if( --ref_count == 0 )
      {
        wipe(this);
      }
    }
    void add_ref()
    {
      ++ref_count;
    }

    static bool equal( const bytes_data* v1, const bytes_data* v2 )
    {
      return v1->elements() == v2->elements();
    }
  };




  struct string_data // ref_counted wchar buffer
  {
    long            n_refs;
    size_t          length;
    value::wiper_t  wiper;

    void add_ref() { ++n_refs; }
    void release() { if (--n_refs <= 0) wiper(this); }
    
    static string_data* allocate(const wchar_t* lpsz) 
    { 
      size_t n = wcslen(lpsz);
      string_data* pd = allocate(n); 
      wcscpy(pd->data(),lpsz); return pd; 
    }
    static string_data* allocate(const wchar_t* lps, size_t n) { string_data* pd = allocate(n); wcsncpy(pd->data(),lps,n); return pd; }
    static string_data* allocate(size_t n) { 
      string_data* pd = (string_data*) zalloc( sizeof(string_data) + sizeof(wchar_t) * (n + 1) ); 
              pd->wiper = &free;
              pd->length = n;
              pd->n_refs = 0; // fix. see http://terrainformatica.com/forums/topic.php?id=270&page
              return pd;
            }
    static string_data* allocate_from_utf8( const byte* utf8, size_t length)
    {
      pod::wchar_buffer buf;
      utf8::towcs(utf8,length,buf);
      return allocate(buf.data(), buf.length());
    }
    static string_data* empty()
    {
      static string_data* _empty = 0;
      if(!_empty) { _empty = allocate(size_t(0)); _empty->add_ref(); }
      return _empty;
    }
    wchar_t* data() const { return (wchar_t *)(this + 1); }
  };

  inline void  value::clear() 
    { 
      switch(v_type)
      {
        case V_STRING: data.s_val->release(); break;
        case V_ARRAY:  data.a_val->release(); break;
        case V_MAP:    data.m_val->release(); break;
        case V_BYTES:  data.b_val->release(); break;
      }
      data.l_val = 0; v_type = V_UNDEFINED; 
    }

  inline const wchar_t* value::get(const wchar_t* def_val) const { return (v_type == V_STRING)? data.s_val->data(): def_val; }

  inline void value::set(const byte* utf8, size_t length) 
  { 
    clear(); 
    v_type = V_STRING; 
    data.s_val = ( utf8 && utf8[0] && length )? 
        string_data::allocate_from_utf8(utf8,length):
        string_data::empty(); 
    data.s_val->add_ref();
  }
  inline void value::set(const wchar_t* src) { 
    clear(); v_type = V_STRING; 
    data.s_val = ( src && src[0] )?
      string_data::allocate(src):
      string_data::empty(); 
    data.s_val->add_ref();
  }
  inline void value::set(const wchar_t* src, size_t length) 
  { 
    clear(); v_type = V_STRING; 
    data.s_val = ( src && src[0] )?
      string_data::allocate(src, length):
      data.s_val = string_data::empty(); 
    data.s_val->add_ref();
  }

  inline void value::set(const value& src) 
    { 
      if( this == &src ) return;
      clear(); 
      v_type = src.v_type; 
      data.l_val = src.data.l_val;
      switch(v_type)
      {
        case V_STRING: data.s_val->add_ref(); break;
        case V_ARRAY:  data.a_val->add_ref(); break;
        case V_MAP:    data.m_val->add_ref(); break;
        case V_BYTES:  data.b_val->add_ref(); break;
      }
    }

  inline value value::k2v(const value& k) const
    {
      value v; named_value::get( data.m_val, k, v);
      return v;
    }

  inline const named_value* value::get_first() const
  {
    assert(v_type == V_MAP);
    return data.m_val;
  }

  inline void value::v2k(const value& k, const value& v)
    {
      if(!is_map()) { clear(); v_type = V_MAP; }
      named_value::set( data.m_val, k, v);
    }

  inline void value::set_array(const value* v, size_t n) 
  { 
    clear(); 
    v_type = V_ARRAY; 
    data.a_val = v? array_data::allocate(v,n,0): array_data::allocate(n,0); 
  }

  inline int value::length() const 
  { 
    if(v_type == V_ARRAY) return int(data.a_val->length);
    if(v_type == V_STRING) return int(data.s_val->length);
    if(v_type == V_MAP) return int(named_value::length(data.m_val));
    if(v_type == V_BYTES) return int(data.b_val->length);
    return 0;
  }

  inline value value::nth(int n) const 
  { 
    if( v_type != V_ARRAY ) return value();
    return ( n >= 0 && n < length() )? data.a_val->elements()[n]: value(); 
  }
  inline void  value::nth(int n, const value& t) 
  { 
    if ( n < 0 )
    {
      assert(false);
      return;
    }
    if( v_type != V_ARRAY )
      set_array(0,size_t(n) + 1);
    else if(n >= length())
      data.a_val = array_data::reallocate(data.a_val,size_t(n) + 1); 
    data.a_val->elements()[n] = t; 
  }

  inline void  value::append(const value& t) 
  { 
    int n = ( v_type == V_ARRAY )? length(): 0;
    nth(n,t);
  }

  inline void value::emit(utf8::ostream& os) const
  {
    switch(v_type)
    {
      case V_UNDEFINED: os << "null"; break;
      case V_BOOL:      os << (get(false)? "true": "false"); break;
      case V_INT:       { char buf[128]; sprintf(buf, "%d", get(0)); os << buf; } break;
      case V_REAL:      { char buf[128]; sprintf(buf, "%f", get(0.0)); os << buf; } break;
      case V_STRING:    {
                          os << "\"";
                          for(const wchar_t* pc = get(L""); *pc; ++pc) 
                            if( *pc == '"' ) os << "\\\""; else os << (char)*pc;
                          os << "\"";
                        } break;
      case V_ARRAY:     
        {
          int total = length(); 
          os << '['; 
          nth(0).emit(os);            
          for( int n = 1; n < total; ++n ) { os << ","; nth(n).emit(os); }
          os << ']'; 
        }  break;
      case V_MAP:     
        {
          const json::named_value* pnv = get_first();
          os << '{'; 
          while(pnv)
          {
            pnv->key.emit(os);
            os << ':';
            pnv->val.emit(os);
            if(pnv->next) os << ",\n";
            else break;
            pnv = pnv->next;
          }
          os << '}'; 
        } break;
    }
  }

  class string 
  {
    friend struct value;
    string_data* psd;

    void init(string_data* ps) { psd = ps; if(psd) psd->add_ref(); }
    void clear() { if(psd) { psd->release(); psd = 0; } }
  public:
    string():psd(0) {}
    string(const wchar_t* s):psd(0) { if(s) init(string_data::allocate(s)); }
    string(const wchar_t* s, size_t sz):psd(0) { if(s && sz) init(string_data::allocate(s,sz)); }
    string(string_data* ps):psd(0) { init(ps); }
    string(const string& s):psd(0) { init(s.psd); }
    
    string(pod::wchar_buffer& wcb):psd(0) { if(wcb.length()) init(string_data::allocate(wcb.data(),wcb.length())); }
    
    string& operator = (const string& v) { clear(); init(v.psd); return *this; }
    string& operator = (const wchar_t* s) { clear(); if(s) init(string_data::allocate(s)); return *this; }
    string& operator = (pod::wchar_buffer& wcb) { clear(); if(wcb.length()) init(string_data::allocate(wcb.data(),wcb.length())); }
       
    ~string() { clear(); }

    const wchar_t*  chars() const       { return psd? psd->data():L""; }
    int             length() const      { return psd? int(psd->length):0; } 
    operator  const wchar_t*() const    { return chars(); }
    operator  const wchars() const      { return wchars(chars(),length()); }

    bool operator == (const string& rs) const { return (wchars)(*this) == (wchars)(rs); }
  };

  inline value::value(const string& s) :v_type(V_UNDEFINED) { set(s); }
  inline void value::set(const string& s) { clear(); v_type = V_STRING; data.s_val = s.psd; data.s_val->add_ref(); }  

  inline string value::to_string() const
  {
    switch(v_type)
    {
      case V_UNDEFINED: return L"{undefined}";
      case V_BOOL:      return get(false)? L"true": L"false";
      case V_INT:       { wchar_t buf[128]; swprintf(buf, L"%d", get(0)); return buf; }
      case V_REAL:      { wchar_t buf[128]; swprintf(buf, L"%g", get(0.0)); return buf; }
      case V_STRING:    return string( data.s_val );
      case V_ARRAY:
            {
                aux::wostream buf;
                int total = length(); 
                if( total )
                {
                  buf << nth(0).to_string();
                  for( int n = 1; n < total; ++n ) { buf << ","; buf << nth(n).to_string(); }
                }
                return string( buf.data(), buf.length() );
            }
    }
    return string();
  }

  inline bool value::operator == ( const value& rs ) const 
  { 
    if(v_type != rs.v_type ) 
    {
      if( is_string() ||  rs.is_string())
        return to_string() == rs.to_string();
      return false;
    }
    if(data.l_val == rs.data.l_val ) return true;
    if( v_type == V_STRING )
      return wcscmp(data.s_val->data(), rs.data.s_val->data()) == 0;
    if( v_type == V_ARRAY )
      return array_data::equal(data.a_val, rs.data.a_val);
    //if( v_type == V_MAP )
    //  return array_data::equal(data.a_val, rs.data.a_val);
    return false; 
  }

  inline void value::set_bytes( slice<byte> bv, bool copy )
  {
    clear(); 
    v_type = V_BYTES; 
    data.b_val = copy? bytes_data::allocate_copy(bv): bytes_data::allocate_ref(bv); 
  }
  inline bool value::get( slice<byte>& bv ) const
  {
    if( v_type != V_BYTES )    
      return false;
    bv = data.b_val->elements();
    return true;
  }

} // ns json

typedef struct json::value JSON_VALUE;

#else // plain c

  typedef struct _JSON_VALUE
  {
    VALUETYPE      v_type;
    union data_slot 
    {
      int            i_val;
      double         r_val;
      __int64        l_val;
      void*          s_val;
      void*          a_val; // array data.
      void*          m_val; // simple map of name/value pairs.
      void*          b_val; // bytes_data.
    } data;
  } JSON_VALUE;

#endif

#pragma pack(pop)

#endif
