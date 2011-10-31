// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_common_Table_hpp
#define cf3_common_Table_hpp

////////////////////////////////////////////////////////////////////////////////

#include "common/Log.hpp"
#include "common/Component.hpp"
#include "common/OptionArray.hpp"
#include "common/OptionT.hpp"
#include "common/Signal.hpp"
#include "common/XML/SignalOptions.hpp"

#include "common/ArrayBufferT.hpp"
#include "common/LibCommon.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace common {

////////////////////////////////////////////////////////////////////////////////

/// @brief Component holding a 2 dimensional array of a templated type
///
/// The internal structure is that of a boost::multi_array,
/// so storage is contingent in memory for reducing cache missing
//
/// The table can be filled through a buffer. The buffer avoids
/// the typical reallocation in a std::vector. Flushing the buffer
/// will resize the table and copy itself into the table.
/// This happens automatically when the buffer is destroyed, or can
/// also be done manually. @see class ArrayBufferT
/// Before using the table one has to be sure that
/// the buffer is flushed.
///
/// @author Willem Deconinck
/// @author Bart Janssens
/// @author Tiago Quintino

template<typename ValueT>
class Table : public common::Component
{
public: // typedefs

  /// @brief the value type stored in each entry of the 2-dimensional table
  typedef ValueT value_type;

  /// @brief the type of the internal structure of the table
  typedef boost::multi_array<ValueT,2> ArrayT;

  /// @brief the type of a row in the internal structure of the table
  typedef typename boost::subarray_gen<ArrayT,1>::type Row;

  /// @brief the const type of a row in the internal structure of the table
  typedef const typename boost::const_subarray_gen<ArrayT,1>::type ConstRow;

  /// @brief the type of the buffer used to interact with the table
  typedef ArrayBufferT<ValueT> Buffer;

  /// @brief boost::shared_ptr shortcut of this component
  typedef boost::shared_ptr<Table> Ptr;

  /// @brief boost::shared_ptr shortcut of this component const version
  typedef boost::shared_ptr<Table const> ConstPtr;

public: // functions

  /// Contructor
  /// @param name of the component
  Table ( const std::string& name )  : Component ( name )
  {
    regist_signal ( "resize" )
        ->description( "Resize the table" )
        ->pretty_name("Resize" )
        ->connect   ( boost::bind ( &Table::signal_resize,    this, _1 ) )
        ->signature ( boost::bind ( &Table::signature_resize, this, _1 ) );
  }

  /// Get the component type name
  /// @returns the component type name
  static std::string type_name () { return "Table<"+common::class_name<ValueT>()+">"; }

  /// Initialize the array with a fixed column size and remove all existing rows, if any.
  /// The number of rows can be changed dynamically.
  /// @param[in] nb_cols number of columns in the table.
  void set_row_size(const Uint nb_cols)
  {
    m_array.resize(boost::extents[size()][nb_cols]);
  }

  /// Resize the array to the given number of rows
  /// @param[in] nb_rows The number of rows after resizing
  virtual void resize(const Uint nb_rows)
  {
    m_array.resize(boost::extents[nb_rows][row_size()]);
  }

  /// Modifiable access to the internal structure
  /// @return A reference to the array data
  ArrayT& array() { return m_array; }

  /// Non-modifiable access to the internal structure
  /// @return A const reference to the array data
  const ArrayT& array() const { return m_array; }

  /// Create a buffer with a given number of entries
  /// @param[in] buffersize the size that the buffer is allocated with
  ///                       the default value is 16384
  /// @return A Buffer object that can fill this Array
  Buffer create_buffer(const size_t buffersize=16384)
  {
    // make sure the array has its columnsize defined
    cf3_assert(row_size() > 0);
    return Buffer(m_array,buffersize);
  }

  typename Buffer::Ptr create_buffer_ptr(const size_t buffersize=16384)
  {
    // make sure the array has its columnsize defined
    cf3_assert(row_size() > 0);
    return typename Buffer::Ptr ( new Buffer (m_array,buffersize) );
  }


  /// Operator to have modifiable access to a table-row
  /// @return A mutable row of the underlying array
  Row operator[](const Uint idx) { return m_array[idx]; }

  /// Operator to have non-modifiable access to a table-row
  /// @return A const row of the underlying array
  ConstRow operator[](const Uint idx) const { return m_array[idx]; }

  /// Number of rows, excluding rows that may be in the buffer
  /// @return The number of local rows in the array
  Uint size() const { return m_array.size(); }

  /// Number of columns , or number of elements of one table-row
  /// @return The number of elements in each row, i.e. the number of columns of the array
  /// @note All row_sizes are the same, so an index is not required, but
  /// could be passed to be consistent with DynTable with variable row_sizes
  Uint row_size(Uint i=0) const { return m_array.shape()[1]; }

  /// copy a given row into the array, The row type must have the size() function declared
  /// @param[in] array_idx the index of the row that will be set
  /// @param[in] row       the row that will be copied into the array
  template<typename VectorT>
  void set_row(const Uint array_idx, const VectorT& row)
  {
    cf3_assert(row.size() == row_size());

    Row row_to_set = m_array[array_idx];

    for(Uint j=0; j<row.size(); ++j)
      row_to_set[j] = row[j];
  }

  void signal_resize ( common::SignalArgs& node )
  {
    common::XML::SignalOptions options( node );
    std::vector<Uint> size = options.array<Uint>("size");
    resize(size[0]);
    set_row_size(size[1]);
  }

  void signature_resize ( common::SignalArgs& node)
  {
    common::XML::SignalOptions options( node );

    std::vector<Uint> size;

    options.add_option< common::OptionArrayT<Uint> >("size", size )
        ->description("Vector of [nb_rows,nb_cols]");
  }

private: // data

  /// storage of the array
  ArrayT m_array;
};

/////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Table<bool>::ConstRow row);
std::ostream& operator<<(std::ostream& os, const Table<Uint>::ConstRow row);
std::ostream& operator<<(std::ostream& os, const Table<int>::ConstRow row);
std::ostream& operator<<(std::ostream& os, const Table<Real>::ConstRow row);
std::ostream& operator<<(std::ostream& os, const Table<std::string>::ConstRow row);

std::ostream& operator<<(std::ostream& os, const Table<bool>& table);
std::ostream& operator<<(std::ostream& os, const Table<Uint>& table);
std::ostream& operator<<(std::ostream& os, const Table<int>& table);
std::ostream& operator<<(std::ostream& os, const Table<Real>& table);
std::ostream& operator<<(std::ostream& os, const Table<std::string>& table);

////////////////////////////////////////////////////////////////////////////////

template<typename RowT>
std::vector<typename RowT::value_type> to_vector(RowT& table_row)
{
  std::vector<typename RowT::value_type> vec(table_row.size());
  for (Uint i=0; i<vec.size(); ++i)
    vec[i] = table_row[i];
  return vec;
}

////////////////////////////////////////////////////////////////////////////////

} // common
} // cf3

////////////////////////////////////////////////////////////////////////////////

#endif // cf3_common_Table_hpp
