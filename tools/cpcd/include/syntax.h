#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#define _CPCD_FORTRAN_NAME   "cpcd"
#define _CPCD_FORTRAN_KIND   "cpcd_kind"
#define _CPCD_FORTRAN_INDENT "  "
  

namespace CPCD {

  std::string dict_syntax = "\
  physical_constants_dictionary: \
  \n    version_number: VALUE \
  \n    institution: VALUE    \
  \n    description: VALUE    \
  \n    contact: VALUE \
  \n    set: \
  \n      - VALUE: \
  \n          description: VALUE \
  \n          citation: VALUE \
  \n          entries: \
  \n            - name:  VALUE \
  \n              value: VALUE \
  \n              units: VALUE \
  \n              prec:  VALUE \
  \n              type:  VALUE \
  \n              uncertainty: VALUE \
  \n              description: VALUE \
  ";

/*
  std::string dict_syntax = "{ \
    physical_constants_dictionary: [ \
                                     version_number,  \
                                     institution   ,  \
                                     description   ,  \
                                     contact \
                                   ], \
    set: [ \
           description, \
           citation \
         ], \
    entries: [ \
               name, \
               value, \
               units, \
               prec, \
               type, \
               uncertainty, \
               description \
             ] \
  }";

  YAML::Node req_syntax = YAML::Load(dict_syntax);

  std::string dict_open = "physical_constants_dictionary";
  std::vector<std::string> dict_info = {
                                         "version_number", 
                                         "institution"   , 
                                         "description"   , 
                                         "contact"       ,
                                       };
  std::string dict_sets = "set";

  std::vector<std::string> dist_sets_info = {
                                              "description", 
                                              "citation"
                                            };


  std::string              dict_sets_entries      = "entries";
  std::vector<std::string> dict_sets_entries_info = {
                                                      "name"
                                                      "value"
                                                      "units"
                                                      "prec"
                                                      "type"
                                                      "uncertainty"
                                                      "description"
                                                    };
*/

} // namespace CPCD

#endif // _SYNTAX_H_
