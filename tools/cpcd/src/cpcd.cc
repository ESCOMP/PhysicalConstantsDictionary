/*  The Community Physical Constant Dictionary (CPCD) class methods
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

#include "cpcd.h"
#include "syntax.h"

namespace CPCD {

  static int
  SetError (const std::string& message)
  {
    // set failure error code and write out
    // input error message to standard error
    // -- private class method
     std::cerr << "Error: " << message << std::endl;
     return CPCD_FAILURE;
   }

  // CPCD class member function definition

  // - constructor
  CPCD::CPCD() : syntax(YAMLLoad(dict_syntax)) {};

  // - standard destructor
  CPCD::~CPCD() {};


  // public functions

  // - I/O

  int
  CPCD::read (const std::string& filename)
  {
    // read physical constant dictionary and store
    // its content to private class member
    // -- public class method
    try {
      this->doc = YAMLLoadFile(filename);
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::write () const
  {
    // write stored content of physical constant
    // dictionary to standard output
    // -- public class method
    try {
      std::cout << this->doc << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::write (const std::string& filename) const
  {
    // write stored content of physical constant
    // dictionary to file
    // -- public class method
    try {
      std::ofstream of(filename);
      of << this->doc << std::endl;
      of.close();
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::write (std::ostream& os) const
  {
    // write stored content of physical constant
    // dictionary to input output stream object
    // -- public class method
    try {
      os << this->doc << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }


  // - control

  int
  CPCD::readreq (const std::string& filename)
  {
    // read user-requested physical constants
    // from YAML file, then rearrange (parse)
    // request in a more convenient YAML format
    // -- public class method
    try {
      this->req = YAMLLoadFile(filename);
      std::cout << this->req << std::endl;
      if (this->ParseReq(this->req, this->sel)) {
        return SetError("failure parsing dictionary request");
      }
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }
    
  int
  CPCD::loadreq (const std::string& request)
  {
    // load user-requested physical constants
    // from YAML string, then rearrange (parse)
    // request in a more convenient YAML format
    // -- public class method
    try {
      this->req = YAMLLoad(request);
      if (this->ParseReq(this->req, this->sel))
        return SetError("failure parsing dictionary request");
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::showreq () const
  {
    // write YAML file containing user-requested
    // physical constants to standard output
    // -- public class method
    try {
      std::cout << this->req << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::showsreq () const
  {
    // write stored list of user-requested
    // physical constants to standard output
    // in YAML format
    // -- public class method
    try {
      std::cout << this->sel << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }


  // - sanity check

  int
  CPCD::ParseReq (const Node& req, Node& preq)
  {
    // parse YAML user request and return sorted request
    // -- private class method
    try {
      preq = Node(NodeType::Null);  // reset parsed request to empty
      
      typedef std::list<std::string> rList;
      typedef std::map<std::string, rList> rMap;
      rMap m;
      
      // first, order by key
      for (Iterator it=req.begin(); it!=req.end(); it++){
        switch(it->second.Type()) {
          case NodeType::Null:
          case NodeType::Undefined:
          case NodeType::Map:
            return SetError("Requests should not include nested maps");
          case NodeType::Scalar:
            m[it->first.as<std::string>()].push_back(it->second.as<std::string>());
          case NodeType::Sequence:
            for (int i=0; i<it->second.size(); i++)
              m[it->first.as<std::string>()].push_back(it->second[i].as<std::string>());
        }
      }
      
      // then order by value
      for (rMap::iterator it=m.begin(); it!=m.end(); it++) {
        it->second.sort();
        it->second.unique(); 
        for (rList::iterator il=it->second.begin(); il!=it->second.end(); il++)
          preq[it->first].push_back(*il);
      }
    
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  Node
  LoadSyntax (const std::string& rules)
  {
    // load reference syntax for physical constant
    // dictionary from string
    // -- private class method
    try {
      return YAMLLoad(rules);
    } catch (const Exception& e) {
      std::string errmsg = "Unable to load syntax rules: ";
      SetError(errmsg.append(e.what()));
      return Node();
    }
  }

  int
  CPCD::ValidateNode (const Node& node, const Node& syntax)
  {
    // validate syntax for each physical constant dictionary node
    // -- private class method
    try {
      if (node.Type() != syntax.Type())
        return SetError("unmatched node type");

      switch (node.Type()) {
        case NodeType::Null:
        case NodeType::Undefined:
        case NodeType::Scalar:
          break;
        case NodeType::Sequence:
          for (int i=0; i<syntax.size(); i++) {
            for (int l=0; l<node.size(); l++) {
              if (this->ValidateNode(node[l], syntax[i]))
                return SetError("unmatched list syntax");
            }
          }
          break;
        case NodeType::Map:
          for (Iterator is=syntax.begin(); is!=syntax.end(); is++) {
            std::string key = is->first.as<std::string>();
            if (key.compare("VALUE") && node[key]) {
              if (this->ValidateNode(node[key], is->second))
                return SetError("map: internal error");
            } else {
              Iterator it = node.begin();
              if (this->ValidateNode(it->second, is->second))
                return SetError("map: internal error");
            }
          }
          break;
      }
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::validate ()
  {
    // validate syntax of stored physical constant dictionary
    // -- public class method
    try {
      this->ValidateNode(this->doc, this->syntax);
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }


  // - parse

  int
  CPCD::ParseNode (const Node& node, const Node& req, Node& map)
  {
    // recursive method parsing stored physical constant dictionary
    // to extract user-requested constants into a key:value map object
    // -- private class method
    try {
      if (!node) {
        return CPCD_SUCCESS;
      }
      if (req.IsSequence()) {
        if (!node.IsSequence())
          return SetError("parsing mismatch");
        for (int l=0; l<node.size(); l++) {
          if (node[l]["name"] && node[l]["value"]) {
            std::string str = node[l]["name"].as<std::string>();
            std::string val = node[l]["value"].as<std::string>();
            for (int i=0; i<req.size(); i++) {
              if (!str.compare(req[i].as<std::string>())) {
                Node pair;
                pair["name"]  = str;
                pair["value"] = val;
                std::cerr << ">>> " << str << " = " << val << std::endl;
                map.push_back(pair);
              }
            }
          }
        }
      } else if (req.IsMap()) {
        if (!node.IsSequence())
          return SetError("parsing mismatch");
        for (int i=0; i<node.size(); i++) {
          for (Iterator it=req.begin(); it!=req.end(); it++) {
            std::string str = it->first.as<std::string>();
            if (node[i][str]) {
              Node m = map[str];
              if (this->ParseNode(node[i][str]["entries"], it->second, m))
                return SetError("error parsing set");
            }
          }
        }
      }
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::parse ()
  {
    // parse stored physical constant dictionary to extract
    // user-requested constants into a key:value map object
    // -- public class method
    try {
      std::cout << "Parsing ..." << std::endl;
      if (this->ParseNode(this->doc["physical_constants_dictionary"]["set"], this->sel, this->map))
        return SetError("parse error");
      std::cout << this->map << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }


  // - emit

  int
  CPCD::emitF (std::ostream& os, const Node& map) const
  {
    // emit Fortran module file including user-requested
    // physical constants to output stream object
    // -- private class method
    try {
      os << "module "
         << _CPCD_FORTRAN_NAME
         << std::endl
         << std::endl;
      os << _CPCD_FORTRAN_INDENT
         << "integer, parameter :: "
         << _CPCD_FORTRAN_KIND
         << " = kind(1.d0)"
         << std::endl
         << std::endl;
      for (Iterator is=map.begin(); is!=map.end(); is++) {
        os << "! - from set " << is->first << std::endl;
        for (int i=0; i<is->second.size(); i++) {
          os << _CPCD_FORTRAN_INDENT
             << "real("
             << _CPCD_FORTRAN_KIND
             << "), parameter :: "
             << is->first << "_"
             << is->second[i]["name"]
             << " = " 
             << is->second[i]["value"]
             << "_" << _CPCD_FORTRAN_KIND
             << std::endl;
        }
      }
      os << std::endl;
      os << "end module "
         << _CPCD_FORTRAN_NAME
         << std::endl;
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return CPCD_SUCCESS;
  }

  int
  CPCD::femit (const std::string& filename) const
  {
    // emit Fortran module file including user-requested
    // physical constants to file
    // -- public class method
    int rc = CPCD_SUCCESS;
    try {
      std::ofstream of(filename);
      rc = this->emitF(of, this->map);
      of.close();
    } catch (const Exception& e) {
      return SetError(e.what());
    }
    return rc;
  }

} // namespace CPCD
