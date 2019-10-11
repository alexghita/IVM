#ifndef _VIEW_H
#define _VIEW_H

#include <ivm/relation.h>

#include <unordered_set>


/**
 * This class models a view used for IVM with materialised views. It is
 * basically a relation which also stores for each attribute the set of values
 * that appear in the relation.
 */
class View : public Relation {
public:
  /**
   * Constructor
   * =========== */

  // The constructor initialises the relation and the sets of values.
  View(const Schema &schema_);


  /**
   * Operations
   * ========== */

  // Add the values of a tuple to the sets of values.
  void add_values(const Tuple &t);

  // Update a tuple.
  void update_tuple(const Tuple &t, int multiplicity);


  /**
   * Accessors
   * ========= */

  // Get the set of values for a given attribute.
  std::unordered_set<LL> get_existing_values(int attr_index) const;


private:
  /**
   * For each attribute, store a set of the values for that attribute currently
   * found in the relation.
   */
  std::vector<std::unordered_set<LL> > existing_values;
};

#endif