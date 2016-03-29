#pragma once
//#include <prefixdb/domain/storage/aux/copy_dir.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/algorithm/string.hpp>
//#include <sstream>
#include <string>     
namespace wamba{ namespace prefixdb {
/*  
namespace 
{*/
  /*inline*/ /*bool copy_dir(
    boost::filesystem::path const & source,
    boost::filesystem::path const & destination,
    std::string& message
  );*//*
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
  }*/
/*}*/

bool copy_dir(const std::string& from, const std::string& to, std::string& message);/*
{
  return copy_dir( ::boost::filesystem::path(from),  ::boost::filesystem::path(to), message);
}*/

}}
