// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/compare.hpp>

#include "Common/URI.hpp"
#include "Common/BasicExceptions.hpp"
#include "Common/StringConversion.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

////////////////////////////////////////////////////////////////////////////////

  template <>
  Common_API std::string to_str<bool> (const bool & v)
  {
    return v ? "true" : "false";
  }

  template <>
  Common_API std::string to_str<int> (const int & v)
  {
    return boost::lexical_cast<std::string>(v);
  }

  template <>
  Common_API std::string to_str<unsigned long> (const unsigned long & v)
  {
    return boost::lexical_cast<std::string>(v);
  }

  template <>
  Common_API std::string to_str<Uint> (const Uint & v)
  {
    return boost::lexical_cast<std::string>(v);
  }

  template <>
  Common_API std::string to_str<Real> (const Real & v)
  {
    std::ostringstream oss;
    oss << v;
    return oss.str();
  }

  template <>
  Common_API std::string to_str<URI> (const URI & v)
  {
    return v.string();
  }

  template <>
  Common_API std::string to_str<std::string> (const std::string & v)
  {
    return v;
  }

  template <>
  Common_API std::string to_str<std::vector<bool> > (const std::vector<bool> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<int> > (const std::vector<int> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<unsigned long> > (const std::vector<unsigned long> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<Uint> > (const std::vector<Uint> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<Real> > (const std::vector<Real> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<URI> > (const std::vector<URI> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += to_str(v[i]) + " ";
      s += to_str(v.back());
    }
    return s;
  }

  template <>
  Common_API std::string to_str<std::vector<std::string> > (const std::vector<std::string> & v)
  {
    std::string s = "";
    if (v.size())
    {
      for (Uint i=0; i<v.size()-1; ++i)
        s += v[i] + " ";
      s += v.back();
    }
    return s;
  }

  ////////////////////////////////////////////////////////////////////////////////

  /// Converts from std::string
  /// Don't use this to convert to a char, use c_str for that.
  /// Typical use is to convert to numbers.
  /// @param str string to convert from
  /// @return converter type
  template <>
  Common_API bool from_str<bool> (const std::string& str)
  {
    bool match = false;
    boost::algorithm::is_equal test_equal;

    if ( test_equal(str,"true") ||
         test_equal(str,"on")   ||
         test_equal(str,"1")     )
    {
      return true;
    }

    if ( test_equal(str,"false") ||
         test_equal(str,"off")   ||
         test_equal(str,"0")      )
    {
      return false;
    }

    if (!match)
      throw ParsingFailed (FromHere(), "Incorrect option conversion to bool of string [" + str + "]" );
    return true;
  }

  template <>
  Common_API int from_str<int> (const std::string& str)
  {
    return boost::lexical_cast<int> (str );
  }

  template <>
  Common_API unsigned long from_str<unsigned long> (const std::string& str)
  {
    return boost::lexical_cast<unsigned long> ( str );
  }

  template <>
  Common_API Uint from_str<Uint> (const std::string& str)
  {
    return boost::lexical_cast<Uint> ( str );
  }

  template <>
  Common_API Real from_str<Real> (const std::string& str)
  {
    return boost::lexical_cast<Real> ( str );
  }

  template <>
  Common_API URI from_str<URI> (const std::string& str)
  {
    return URI( str );
  }

  template <>
  Common_API std::string from_str<std::string> (const std::string& str)
  {
    return str;
  }

////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF
