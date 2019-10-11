#include <ivm/naiveprocessor.h>


/**
 * Constructor
 * =========== */

// Construct the relations for the given query order n.
NaiveProcessor::NaiveProcessor(int n_) : IVMProcessor(n_) {}


/**
 * Operations
 * ========== */

// Given an update to a relation, re-evaluate the query and get the new count.
LL NaiveProcessor::update_and_count(int rel_num, const Tuple &t, 
    int multiplicity) {
  // Update the relation.
  update_rel(rel_num, t, multiplicity);

  // Compute the entire join.
  Relation rel_join = rels[0].join(rels[1]);
  for (int i = 2; i < n; ++i) {
    rel_join = rel_join.join(rels[i]);
  }

  // Count the total multiplicity and return it.
  return rel_join.count();
}