#ifndef _RELATION_H
#define _RELATION_H

#include <map>
#include <unordered_map>
#include <vector>

#include <boost/functional/hash.hpp>


// Define a generic hash function for containers such as vectors.
template <typename Container>
struct container_hash {
  std::size_t operator()(Container const& c) const {
    return boost::hash_range(c.begin(), c.end());
  }
};


// Shorthand for the 64-bit integer data type used in all relations.
typedef long long LL;
// Shorthand for the schema of a relation, represented as an array of strings.
typedef std::vector<std::string> Schema;
// Shorthand for a tuple in a relation, represented as an array of numbers.
typedef std::vector<LL> Tuple;


/**
 * This class models a relation in uniform update representation, such that each
 * tuple is mapped to a multiplicity count.
 */
class Relation {
public:
  /**
   * Constructors
   * ============ */

  // Default constructor.
  Relation() {}
  
  // The constructor simply initialises a new relation with the given schema.
  Relation(const Schema &schema_);


  /**
   * Operations
   * ========== */

  // Count the total number of tuples as given by the sum of all multiplicities.
  LL count() const;

  // Join the relation with another, and return the resulting relation.
  Relation join(const Relation &r);

  // Update a tuple with a given multiplicity (or add it if it does not exist).
  void update_tuple(const Tuple &t, int multiplicity);

  // Print a list of tuples in the relation and their multiplicities.
  void print_contents() const;


  /**
   * Accessors
   * ========= */

  // Return true if the relation contains no tuples with non-zero multiplicity.
  bool empty() const;

  // Get the entries in the relation as a map of tuples to multiplicities.
  std::unordered_map<Tuple, LL, container_hash<Tuple> > get_entries() const;

  // Get the multiplicity of a given tuple.
  LL get_multiplicity(const Tuple &t) const;

  // Get the schema of the relation.
  Schema get_schema() const;

  // Get the schema map of the relation.
  std::map<std::string, int> get_schema_map() const;

  // Get the number of attributes in the relation.
  int get_schema_size() const;


protected:
  // The schema of the relation.
  Schema schema;

  // The number of attributes in the schema.
  int schema_size;

  /**
   * Mapping between attribute names and their positions in the schema array,
   * e.g. in schema (A1, A2, A3), A1 is mapped to 0, A2 to 1, and A3 to 2.
   */
  std::map<std::string, int> schema_map;

  // The entries in the relation.
  std::unordered_map<Tuple, LL, container_hash<Tuple> > entries;
};

#endif