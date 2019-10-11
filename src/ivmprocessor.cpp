#include <ivm/ivmprocessor.h>

#include <cassert>


/**
 * Constructor
 * =========== */

/**
 * Given a query order n, construct n relations with schemas as specified in the
 * exam problem statement.
 */
IVMProcessor::IVMProcessor(int n_) {
  n = n_;

  Schema schema(n - 1);
  for (int i = 0; i < n; ++i) {
    for (int j = 1; j < n; ++j) {
      // Compute the attribute index.
      int idx = i + j > n ? (i + j) % n : i + j;
      schema[j - 1] = "A" + std::to_string(idx);
    }
    // Create a new relation.
    rels.push_back(Relation(schema));
  }
}


/**
 * Operations
 * ========== */

// Update the given relation by adding the given multiplicity for the tuple t.
void IVMProcessor::update_rel(int rel_num, const Tuple &t, int multiplicity) {
  assert(0 <= rel_num && rel_num < n);

  rels[rel_num].update_tuple(t, multiplicity);
}


// Given an update to a relation, compute and return the new count.
LL IVMProcessor::update_and_count(int rel_num, const Tuple &t, 
    int multiplicity) {}