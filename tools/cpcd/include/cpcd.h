/*  CPCD class definitions
    Copyright (C) 2019  National Earth System Prediction Capability/CSC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _CPCD_H_
#define _CPCD_H_

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>

#include "yaml-cpp/yaml.h"

// return codes
#define CPCD_SUCCESS 0
#define CPCD_FAILURE 1

namespace CPCD {

  // interface with external YAML C++ library
  typedef YAML::Node           Node;
  typedef YAML::NodeType       NodeType;
  typedef YAML::Exception      Exception;
  typedef YAML::const_iterator Iterator;

  static Node
  YAMLLoad (const std::string& string)
  {
    return YAML::Load(string);
  }

  static Node
  YAMLLoadFile (const std::string& string)
  {
    return YAML::LoadFile(string);
  }

  
  // class declaration
  class CPCD;

  class CPCD {

    public:

      // constructor
      CPCD ();

      // destructor
      ~CPCD ();

      // public basic I/O methods
      int read (const std::string& filename);

      int write () const ;
      int write (const std::string& filename) const;
      int write (std::ostream& ostream) const;

      // read and set user requests for physical constant subsets
      int readreq (const std::string& filename);
      int loadreq (const std::string& request);
      // write original request to standard output
      int showreq () const;
      // write stored request to standard output
      int showsreq () const;

      // parse dictionary -- requires a user request to be set
      int parse ();

      // validate syntax of the physical constant dictionary
      int validate ();

      // emit requested physical constants as Fortran module
      int femit (const std::string& filename) const;


      // public data members
      int verbose;
      int depth;

    private:

      // load reference syntax for physical constant dictionary
      static Node LoadSyntax (const std::string& rules);

      // parse user request
      int ParseReq (const Node& req, Node& preq);

      // validate
      int ValidateNode (const Node& node, const Node& syntax);

      // parse
      int ParseNode (const Node& node, const Node& req, Node& map);

      // emit
      int emitF (std::ostream& os, const Node& map) const;
      
      // private data members
      Node doc;    // stores YAML physical constant dictionary
      Node req;    // stores original YAML user request for physical constants
      Node sel;    // stores physical constant list parsed from input user request
      Node syntax; // stores syntax reference for physical constant dictionary for validation purposes
      Node map;    // work map

  }; // class CPCD

} // namespace CPCD

#endif // _CPCD_H_
