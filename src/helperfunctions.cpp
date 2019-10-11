#include <ivm/helperfunctions.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>


/**
 * For a given set of relations corresponding to a query of order n, and a given
 * tuple from a certain relation (denoted by its number and the excluded 
 * attribute), generate a vector of keys for the other relations.
 *
 * For a relation S, other than the updated relation R, a key is a tuple where 
 * the values of the attributes shared by R and S have been filled with the
 * values from vals, the updated tuple in R. The last value in the key is empty,
 * and its position in the tuple is stored along with the tuple.
 */
std::vector<std::pair<Tuple, int> > HelperFunctions::generate_keys(
    const std::vector<Relation> &rels, const Tuple &vals,
    int excluded_rel_num, const std::string &excluded_attr) {
  // Initialise the key vector.
  std::vector<std::pair<Tuple, int> > keys(rels.size());
  // Get the schema map of the excluded relation.
  std::map<std::string, int> sm = rels[excluded_rel_num].get_schema_map();
  Tuple key(rels.size() - 1);
  // Go through all relations other than the excluded one.
  for (int i = 0; i < rels.size(); ++i) {
    if (i == excluded_rel_num) {
      continue;
    }

    int idx;
    Schema schema = rels[i].get_schema();
    // Generate the key.
    for (int j = 0; j < schema.size(); ++j) {
      /**
       * Put a "blank" slot for the excluded attribute, and the tuple's values
       * for the other attributes.
       */
      if (schema[j] == excluded_attr) {
        idx = j;
        key[j] = 0;
      } else {
        key[j] = vals[sm[schema[j]]];
      }
    }
    keys[i] = std::make_pair(key, idx);
  }
  
  return keys;
}


/**
 * Return the next partition configuration, given a specific configuration. For
 * example, for the configuration LLL, the next one would be HLL, followed by
 * LHL, HHL, and so on, where L means "light" and H means "heavy".
 */
std::vector<bool> HelperFunctions::get_next_config(
    const std::vector<bool> &config, int skip_idx) {
  bool found_zero = false;
  std::vector<bool> next_config = config;

  // Find the first light/zero that can be turned in a heavy/one.
  for (int i = 0; i < next_config.size(); ++i) {
    if (i == skip_idx) {
      continue;
    }
    if (next_config[i] == 0) {
      found_zero = true;
      next_config[i] = 1;
      for (int j = 0; j < i; ++j) {
        if (j == skip_idx) {
          continue;
        }
        next_config[j] = 0;
      }
      break;
    }
  }

  // Start over with all light/0 if none is found previously
  if (!found_zero) {
    for (int i = 0; i < next_config.size(); ++i) {
      if (i == skip_idx) {
        continue;
      }
      next_config[i] = 0;
    }
  }

  return next_config;
}


// Return the intersection of two schemas.
Schema HelperFunctions::schema_intersection(const Schema &a, const Schema &b) {
  Schema res(a.size());
  Schema::iterator it;

  it = set_intersection(a.begin(), a.end(), b.begin(), b.end(), res.begin());
  res.resize(it - res.begin());

  return res;
}


// Return the union of two schemas.
Schema HelperFunctions::schema_union(const Schema &a, const Schema &b) {
  Schema res(a.size() + b.size());
  Schema::iterator it;

  it = set_union(a.begin(), a.end(), b.begin(), b.end(), res.begin());
  res.resize(it - res.begin());

  return res;
}


/**
 * Validate the command line arguments passed to the system. Return true if the
 * arguments are valid, or false otherwise.
 */
bool HelperFunctions::validate_arguments(int argc, char **argv) {
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] << " N " <<
        "/relative/path/to/query-file (naive|delta|view|skew) " << 
        "/relative/path/to/output-file max-number-of-updates\n";
    return false;
  }

  if (argv[1][0] < '1' || '9' < argv[1][0]) {
    std::cerr << "Invalid value for N!\n";
    return false;
  }
  for (int i = 0; argv[1][i] != 0; ++i) {
    if (!isdigit(argv[1][i])) {
      std::cerr << "Invalid value for N!\n";
      return false;
    }
  }
  if (atoi(argv[1]) < 3) {
    std::cerr << "N must be at least 3!\n";
    return false;
  }

  std::ifstream f(argv[2]);
  if (!f.good()) {
    std::cerr << "File not found! Make sure the path is relative to the" <<
        " location of the binary file.\n";
    return false;
  }

  if (strcmp(argv[3], "naive") && strcmp(argv[3], "delta") && 
      strcmp(argv[3], "view") && strcmp(argv[3], "skew")) {
    std::cerr << "Invalid join type! Choose between: naive, delta, view, " << 
        "skew.\n";
    return false;
  }

  if (argv[5][0] < '0' || '9' < argv[5][0]) {
    std::cerr << "Invalid value for max-number-of-updates! Use either " <<
        " 0 to process all updates until the end of file, or a positive " <<
        "number.\n";
    return false;
  }
  for (int i = 0; argv[5][i] != 0; ++i) {
    if (!isdigit(argv[5][i])) {
      std::cerr << "Invalid value for max-number-of-updates! Use either " <<
          " 0 to process all updates until the end of file, or a positive " <<
          "number.\n";
      return false;
    }
  }

  return true;
}