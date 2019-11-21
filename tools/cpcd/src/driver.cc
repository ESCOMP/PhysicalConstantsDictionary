/*  Driver - Parse dictionary and extract selected physical constant subsets
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

#include "config.h"
#include "cpcd.h"

#include <getopt.h>


static void
print_usage (int status)
{
  std::cerr << "Usage: " << PACKAGE << " [options] ..." << std::endl;
  std::cerr << "Main tool to validate, parse, and extract physical constant sets" << std::endl;
  std::cerr << "from the Community Physical Constant Dictionary" << std::endl;
  std::cerr << std::endl;
  std::cerr << "Mandatory arguments to long options are mandatory for short options too." << std::endl;
  std::cerr << "  -d, --dictionary YAML_FILE      Use YAML_FILE as dictionary" << std::endl;
  std::cerr << "  -r, --request    YAML_FILE      Extract constants listed in YAML_FILE" << std::endl;
  std::cerr << "  -o, --output     FILE           Save Fortran output to FILE" << std::endl;
  std::cerr << "  -x, --validate                  Validate dictionary file before proceeding" << std::endl;
  std::cerr << "  -v, --verbose                   Use verbose output" << std::endl;
  std::cerr << "  -V, --version                   Print version information" << std::endl;
  std::cerr << "  -h, --help                      Display available options" << std::endl;
  std::cerr << std::endl;
  std::cerr << "Exit status:" << std::endl;
  std::cerr << " " << CPCD_SUCCESS << "if successful, " << CPCD_FAILURE << " if an error occurs." << std::endl;
  std::cerr << std::endl;
  std::cerr << "For bugs reporting, please visit:" << std::endl;
  std::cerr << "<https://github.com/ESCOMP/PhysicalConstantsDictionary>" << std::endl;
  std::exit (status);
}


static void
print_version()
{
  std::cout << PACKAGE_NAME << ", version " << PACKAGE_VERSION << std::endl;
  std::cout << "Copyright (C) 2019  National ESPC/CSC" << std::endl;
  std::cout << std::endl;
  std::cout << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl;
  std::cout << "This is free software, and you are welcome to"   << std::endl;
  std::cout << "redistribute it under certain conditions."       << std::endl;
  std::cout << "See license terms in included COPYING file."     << std::endl;

  std::exit (CPCD_SUCCESS);
}


int
main (int argc, char** argv)
{

  // Defaults
  std::string pcd_file = "pcd.yaml";        // Physical constant dictionary YAML file
  std::string req_file = "req.yaml";        // User-provided YAML file with requested constants
  std::string out_file = "cpcd_mod.F90";    // Fortran module file

  // Control flags
  int validate = 0;
  int verbose  = 0;
  int print    = 0;

  // Define command-line options
  static struct option options[] =
  {
    // Add command-line options here
    { "help",        no_argument,        NULL,       'h' },
    { "version",     no_argument,        NULL,       'V' },
    { "verbose",     no_argument,        &verbose,    1  },
    { "validate",    no_argument,        &validate,   1  },
    { "print",       no_argument,        &print,      1  },
    { "request",     required_argument,  NULL,       'r' },
    { "output",      required_argument,  NULL,       'o' },
    { "dictionary",  required_argument,  NULL,       'd' },
    // Mark end of table
    { NULL,          0,                  NULL,       0   }
  };

  /* Parse command-line options */
  int c = 0;

  while ((c = getopt_long (argc, argv, "hvVvxpr:o:d:", options, NULL)) != -1)
    {
      switch(c)
        {
        case 'h':
          print_usage(CPCD_SUCCESS);
          /* Function will exit */
        case 'V':
          print_version();
          /* Function will exit */
        case 'v':
          verbose = 1;
          break;
        case 'x':
          validate = 1;
          break;
        case 'p':
          print = 1;
          break;
        case 'r':
          req_file = optarg;
          break;
        case 'o':
          out_file = optarg;
          break;
        case 'd':
          pcd_file = optarg;
          break;
        default:
          break;
  //      print_usage(CPCD_FAILURE);
          /* Function will exit */
        }
    }

  argc -= optind;
  argv += optind;

  // Abort if there are remaining arguments after parsing known command-line options
  if (optind == 1 || argc > 1) {
    print_usage(CPCD_FAILURE);
  }

  /* Create dictionary instance */
  CPCD::CPCD doc;

  /* Read physical constant dictionary */
  int rc = doc.read (pcd_file);
  if (rc != CPCD_SUCCESS) {
    return rc;
  }

  // Print physical constant dictionary if requested
  if (print) {
    rc = doc.write ();
    if (rc != CPCD_SUCCESS) {
      return rc;
    }
  }

  // Validate dictionary syntax if requested
  if (validate) {
    std::cout << std::endl;
    std::cout << "Validating physical constant dictionary ... ";
    rc = doc.validate ();
    if (rc != CPCD_SUCCESS) {
      std::cout << "FAILED" << std::endl;
      return rc;
    } else {
      std::cout << "passed" << std::endl;
    }
    return rc;
  }

  // Read YAML file containing user-requested constants
  rc = doc.readreq (req_file);
  if (rc != CPCD_SUCCESS) {
    return rc;
  }
  
  // Print original user request and corresponding dictionary query
  if (verbose) {
    std::cout << "================" << std::endl;
    std::cout << "User request:" << std::endl;
    std::cout << "================" << std::endl;
    rc = doc.showreq ();
    if (rc != CPCD_SUCCESS) {
      return rc;
    }
    std::cout << "================" << std::endl;
    std::cout << std::endl;
    std::cout << "Actual query:" << std::endl;
    std::cout << "================" << std::endl;
    doc.showsreq ();
    std::cout << "================" << std::endl; 
    std::cout << std::endl;
  }

  // Parse dictionary to extract user-requested constants
  rc = doc.parse ();
  if (rc != CPCD_SUCCESS) {
    return rc;
  }

  // Emit Fortran module defining requested physical constants
  rc = doc.femit (out_file);
  if (rc != CPCD_SUCCESS) {
    return rc;
  }
  
  return CPCD_SUCCESS;
}
