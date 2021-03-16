#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <prefixdb/prefixdb/multidb/aux/copy_dir.hpp>
#include <boost/algorithm/string.hpp>
#include <wfc/boost.hpp>
#include <sstream>
#include <cstdlib>
namespace wamba{ namespace prefixdb {

namespace
{
  bool copy_dir(
    boost::filesystem::path const & source,
    boost::filesystem::path const & destination,
    std::string& message
  )
  {
    try
    {
      // Check whether the function call is valid
      if( !::boost::filesystem::exists(source) || !::boost::filesystem::is_directory(source) )
      {
        std::stringstream ss;
        ss << "Source directory " << source.string() << " does not exist or is not a directory." << '\n';
        message = ss.str();
        return false;
      }

      if ( ::boost::filesystem::exists(destination) )
      {
        std::stringstream ss;
        ss << "Destination directory " << destination.string() << " already exists." << '\n';
        message = ss.str();
        return false;
      }

      // Create the destination directory
      if( !::boost::filesystem::create_directory(destination) )
      {
        std::stringstream ss;
        ss << "Unable to create destination directory " << destination.string() << '\n';
        message = ss.str();
        return false;
      }
    }
    catch(::boost::filesystem::filesystem_error const & e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }

    // Iterate through the source directory
    for( ::boost::filesystem::directory_iterator file(source); file != ::boost::filesystem::directory_iterator(); ++file) try
    {
      ::boost::filesystem::path current(file->path());
      if( ::boost::filesystem::is_directory(current) )
      {
        // Found directory: Recursion
        if( !copy_dir(current, destination / current.filename(), message) )
          return false;
      }
      else
      {
        // Found file: Copy
        ::boost::system::error_code ec;
        ::boost::filesystem::copy_file( current, destination / current.filename(), ec);
        if (ec)
        {
          // TODO: Ошибка
        }
      }
    }
    catch(const ::boost::filesystem::filesystem_error& e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }
    return true;
  }

  bool move_dir(
    ::boost::filesystem::path const & source,
    ::boost::filesystem::path const & destination,
    std::string& message
  )
  try
  {
    if ( boost::filesystem::exists(destination) )
      boost::filesystem::remove_all(destination);

    if ( std::system(nullptr) != 0 )
    {
      std::stringstream ss;
      ss << "mv '" << source << "' '" << destination << "'";
      int ret = std::system(ss.str().c_str());
      if ( WIFEXITED(ret) )
      {
        int err = WEXITSTATUS(ret);
        if ( err != 0 )
        {
          message=std::strerror(err);
          return false;
        }
      }
    }
    else
    {
      message="command processor is available";
      return false;
    }
    //boost::filesystem::rename(source, destination);
    return true;
  }
  catch(const ::boost::filesystem::filesystem_error& e)
  {
    std::stringstream ss;
    ss << e.what() << '\n';
    message = ss.str();
    return false;
  }

  bool delete_dir( const ::boost::filesystem::path& path, std::string& message )
  {
    boost::system::error_code ec;
    ::boost::filesystem::remove_all(path, ec);
    if (ec)
    {
      message = ec.message();
      return false;
    }
    return true;
  }
}

bool copy_dir(const std::string& from, const std::string& to, std::string& message)
{
  return copy_dir( ::boost::filesystem::path(from),  ::boost::filesystem::path(to), message);
}

bool move_dir(const std::string& from, const std::string& to, std::string& message)
{
  return move_dir( ::boost::filesystem::path(from),  ::boost::filesystem::path(to), message);
}

bool delete_dir(const std::string& path, std::string& message)
{
  return delete_dir( ::boost::filesystem::path(path), message );
}

}}
