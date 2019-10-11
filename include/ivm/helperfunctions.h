#ifndef _HELPERFUNCTIONS_H
#define _HELPERFUNCTIONS_H

#include <ivm/relation.h>


/**
 * This class represents a collection of static helper functions, which are used
 * in various classes throughout the IVM system.
 */
class HelperFunctions {
public:
  // Return one tuple for each relation filled with values from a given tuple.
  static std::vector<std::pair<Tuple, int> > generate_keys(
      const std::vector<Relation> &rels, const Tuple &vals, 
      int excluded_rel_num, const std::string &excluded_attr);

  // Return the next partition configuration for a given number of relations.
  static std::vector<bool> get_next_config(const std::vector<bool> &config,
      int skip_idx);

  // Return the intersection of two schemas.
  static Schema schema_intersection(const Schema &a, const Schema &b);

  // Return the union of two schemas.
  static Schema schema_union(const Schema &a, const Schema &b);

  // Validate the command line arguments passed to the system.
  static bool validate_arguments(int argc, char **argv);


private:
  HelperFunctions() {}
};

#endif