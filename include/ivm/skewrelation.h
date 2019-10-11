#ifndef _SKEWRELATION_H
#define _SKEWRELATION_H

#include <ivm/relation.h>

#include <unordered_set>


/**
 * This class models a skew-aware relation used for IVM with skew-sensitive
 * processing. It partitions its tuples in two regular relations, depending on
 * a given skew attribute and a threshold determined by the size of the relation
 * and a given value epsilon.
 */
class SkewRelation : public Relation {
public:
  /**
   * Constructor
   * =========== */

  // Default constructor.
  SkewRelation() {}

  // The constructor gets the skew criteria and initialises the partitions.
  SkewRelation(const Schema &schema_, const std::string &skew_attr_, 
      double epsilon_);


  /**
   * Operations
   * ========== */

  // Print the heavy partition.
  void print_heavy_contents() const;

  // Print the light partition.
  void print_light_contents() const;

  // Update a tuple and re-balance its related tuples.
  void update_tuple(const Tuple &t, int multiplicity);


  /**
   * Accessors
   * ========= */

  // Return the heavy partition.
  Relation get_heavy_part() const;

  // Return the light partition.
  Relation get_light_part() const;


private:
  // The current number of tuples in the relation.
  int N;

  // The index in the schema of the skew attribute.
  int skew_attr_idx;

  double epsilon;
  Relation light_part;
  Relation heavy_part;
  
  // Store for each value of the skew attribute the tuples in which it is found.
  std::unordered_map<LL, std::unordered_set<Tuple, container_hash<Tuple> > > 
      skew_tuples;


  /**
   * Operations
   * ========== */

  // Re-balance the partitions after a given update in a given partition.
  void rebalance(LL val, bool in_light);
};

#endif