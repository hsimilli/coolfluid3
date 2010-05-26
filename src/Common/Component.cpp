#include <boost/cast.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include "Common/Component.hpp"
#include "Common/StringOps.hpp"
#include "Common/BasicExceptions.hpp"
#include "Common/Log.hpp"
#include "Common/CRoot.hpp"
#include "Common/ComponentIterator.hpp"

namespace CF {
namespace Common {

/////////////////////////////////////////////////////////////////////////////////////

Component::Component ( const CName& name ) :
    m_name (),
    m_path (),
    m_is_link (false),
    m_tags(":") // empty tags
{
  build_component(this);
  if (!CPath::is_valid_element( name ))
    throw InvalidPath(FromHere(), "Component name ["+name+"] is invalid");
  m_name = name;
}

/////////////////////////////////////////////////////////////////////////////////////

Component::~Component()
{
  /// @todo implement
}

/////////////////////////////////////////////////////////////////////////////////////


Component::Ptr Component::get ()
{
  return shared_from_this();
}

/////////////////////////////////////////////////////////////////////////////////////


void Component::rename ( const CName& name )
{
  if ( name == m_name.string() ) // skip if name does not change
    return;

  CPath new_full_path = m_path / name;

  if( !m_root.expired() )
  {
    // get the root and inform about the change in path
    boost::shared_ptr<CRoot> root =
        boost::dynamic_pointer_cast<CRoot>( m_root.lock() );

    root->define_component_path( new_full_path , shared_from_this() );
  }

  // rename object make be after modificatio of path in root
  // else root would not find the previous object
  m_name = name;
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::add_tag(const std::string& tag)
{
  m_tags += tag + ":";
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> Component::get_tags()
{
  std::vector<std::string> vec;

  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
  boost::char_separator<char> sep(":");
  Tokenizer tokens(m_tags, sep);

  for (Tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    vec.push_back(*tok_iter);

  return vec;
}

/////////////////////////////////////////////////////////////////////////////////////

bool Component::has_tag(const std::string& tag)
{
  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
  boost::char_separator<char> sep(":");
  Tokenizer tokens(m_tags, sep);

  for (Tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    if (*tok_iter == tag)
      return true;

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////

bool Component::has_component_with_tag(const std::string& tag)
{
  for(CompStorage_t::iterator it=m_components.begin(); it!=m_components.end(); ++it)
    if (it->second->has_tag(tag))
      return true;

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::add_component ( Component::Ptr subcomp )
{
  // check that no other component with such name exists
  if (m_components.find(subcomp->name()) != m_components.end() )
    throw ValueExists(FromHere(), "Component with name '"
                      + subcomp->name() + "' already exists in component '"
                      + name() + "' with path ["
                      + m_path.string() + "]");

  m_components[subcomp->name()] = subcomp;

  subcomp->change_parent( shared_from_this() );
}

/////////////////////////////////////////////////////////////////////////////////////


Component::Ptr Component::remove_component ( const CName& name )
{
  // find the component exists
  Component::CompStorage_t::iterator itr = m_components.find(name);

  if ( itr != m_components.end() )         // if exists
  {
    Component::Ptr comp = itr->second;     // get the component
    m_components.erase(itr);                // remove it from the storage
    comp->change_parent(Component::Ptr());  // set parent to invalid
    return comp;                             // return it to client to do somethinng typically delete it
  }
  else                                        // if does not exist
  {
    throw ValueNotFound(FromHere(), "Component with name '"
                        + name + "' does not exist in component '"
                        + this->name() + "' with path ["
                        + m_path.string() + "]");
  }

}

/////////////////////////////////////////////////////////////////////////////////////


Component::Ptr Component::get_component ( const CName& name )
{
  // find the component exists
  Component::CompStorage_t::iterator itr = m_components.find(name);

  if ( itr != m_components.end() )     // if exists
    return itr->second;                  // return it
  else                                   // if does not exist
    throw ValueNotFound(FromHere(), "Component with name '"
                        + name + "' does not exist in component '"
                        + this->name() + "' with path ["
                        + m_path.string() + "]");
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<Component::Ptr> Component::get_components_by_tag(const std::string& tag)
{
  std::vector<Component::Ptr> vec;
  for(CompStorage_t::const_iterator it=m_components.begin(); it!=m_components.end(); ++it)
  {
    if (it->second->has_tag(tag))
      vec.push_back(it->second);
  }
  return vec;
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::complete_path ( CPath& path )
{
  using namespace boost::algorithm;

//  CFinfo << "PATH [" << path.string() << "]\n" << CFendl;

  if (m_parent.expired()) 
    throw  InvalidPath(FromHere(), "Component \'" + name() + "\' has no parent");
  
  if (m_root.expired()) 
    throw  InvalidPath(FromHere(), "Component \'" + name() + "\' has no root");
  
  boost::shared_ptr<Component> parent = m_parent.lock();
  boost::shared_ptr<Component> root   = m_root.lock();

  std::string sp = path.string();

  if ( path.is_relative() ) // transform it to absolute
  {    
    if ( starts_with(sp,"/") ) // remove leading "/" if any
      boost::algorithm::replace_first(sp, "/", "" );

    // substitute leading "../" for full_path() of parent
    if (starts_with(sp,".."))
    {
      std::string pfp = parent->full_path().string();
      boost::algorithm::replace_first(sp, "..", pfp);
    }
    else // substitute leading "./" for full_path() of this component
      if (starts_with(sp,"."))
      {
        boost::algorithm::replace_first(sp, ".", full_path().string());
      }
  }

  cf_assert ( CPath(sp).is_absolute() );

  // break path in tokens and loop on them, while concatenaitng to a new path
  boost::char_separator<char> sep("/");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  tokenizer tok (sp,sep);

  path = "/" ;
  std::string last;
  for(tokenizer::iterator el=tok.begin(); el!=tok.end(); ++el)
  {
    if ( equals (*el, ".") ) continue;     // substitute any "/./" for nothing

    if ( equals (*el, "..") )              // substitute any "../" for base path
      path = path.base_path();
    else
      path /= *el;
  }

//  CFinfo << "FINAL PATH: [" << path.string() << "]\n" << CFendl;

  cf_assert ( path.is_complete() );
}

/////////////////////////////////////////////////////////////////////////////////////


void Component::change_parent ( Component::Ptr new_parent )
{
  if( !m_root.expired() )   // get the root and remove the current path
  {
    boost::shared_ptr<CRoot> root =
        boost::dynamic_pointer_cast<CRoot>( m_root.lock() );
    root->remove_component_path(full_path());
  }

  if( new_parent ) // valid ?
	{    
		m_path = new_parent->full_path(); // modify the path
    m_root = new_parent->m_root;      // modify the root
    
    if( !m_root.expired() )   // get the root and set the new path
    {
      boost::shared_ptr<CRoot> root =
          boost::dynamic_pointer_cast<CRoot>( m_root.lock() );
      root->define_component_path( full_path() , shared_from_this() );
    }
  }
  else // new parent is invalid
  {
    m_path = "";
    m_root.reset();
  }

  // modifiy the parent
  // may be invalid
  m_parent = new_parent;

  // modify the children
  BOOST_FOREACH( CompStorage_t::value_type c, m_components )
  {
    c.second->change_parent( shared_from_this() );
  }
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::move_component ( Component::Ptr new_parent )
{
  m_parent.lock()->remove_component( this->name() );
  new_parent->add_component( shared_from_this() );
}

/////////////////////////////////////////////////////////////////////////////////////

Component::Ptr Component::look_component ( const CPath& path )
{
  cf_assert ( !m_root.expired() );

  CPath lpath = path;

  complete_path(lpath); // ensure the path is complete

  // get the root
  boost::shared_ptr<CRoot> root =
      boost::dynamic_pointer_cast<CRoot>( m_root.lock() );

  return root->access_component(lpath);
}

/////////////////////////////////////////////////////////////////////////////////////


void Component::create_component ( XmlNode& node  )
{
  throw NotImplemented( FromHere(), "" );
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::list_tree( XmlNode& node )
{
  throw NotImplemented( FromHere(), "" );

  //  XMLNode this_node = parent.addChild( name().c_str() );

//  BOOST_FOREACH( CompStorage_t::value_type c, m_components )
//  {
//    c.second->list_tree( this_node );
//  }
}

/////////////////////////////////////////////////////////////////////////////////////

void Component::list_options ( XmlNode& node )
{
  throw NotImplemented( FromHere(), "" );
}

/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF
