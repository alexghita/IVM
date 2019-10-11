#include <ivm/deltaprocessor.h>
#include <ivm/helperfunctions.h>


/**
 * Constructor
 * =========== */

// The constructor initialises a delta processor for a query of order n.
DeltaProcessor::DeltaProcessor(int n_) : IVMProcessor(n_) {
  current_count = 0;
}


/**
 * Operations
 * ========== */

/**
 * Given an update to one of the relations, use delta processing to compute the
 * difference produced in the query result, then add it to the current count,
 * and return the new count.
 */
LL DeltaProcessor::update_and_count(int rel_num, const Tuple &t, 
    int multiplicity) {
  // Update the relation.
  update_rel(rel_num, t, multiplicity);

  // Construct the delta relation, which contains just the update tuple.
  Relation delta_rel(rels[rel_num].get_schema());
  delta_rel.update_tuple(t, multiplicity);

  /**
   * Join the delta relation with another one to get all values that need to be
   * checked for the attribute missing from the initial delta relation.
   */
  int next_rel_num = (rel_num + 1) % n;
  delta_rel = delta_rel.join(rels[next_rel_num]);

  /** 
   * If the join between the two is empty, then the update has no effect on the
   * total count, so stop now.
   */
  if (delta_rel.empty()) {
    return current_count;
  }

  // Get the attribute that is missing in the updated relation.
  std::string missing_attr = rel_num == 0 ? 
      "A" + std::to_string(n) : "A" + std::to_string(rel_num);
  /**
   * Generate keys used to search each relation for the multiplicity that would
   * correspond in a join to the updated tuple. For each relation other than the
   * updated one, there is a key consisting of a tuple with values taken from
   * the update tuple in the case of attributes shared by the updated relation
   * and the key's relation, along with an empty slot for the other attribute
   * that is not present in the updated relation, and its index in the tuple.
   */
  std::vector<std::pair<Tuple, int> > keys = HelperFunctions::generate_keys(
      rels, t, rel_num, missing_attr);
  // Get the entries in the join of the two relations.
  std::unordered_map<Tuple, LL, container_hash<Tuple> > entries = 
      delta_rel.get_entries();
  // Get the position in the relation's schema of the attribute not in t.
  int missing_idx = delta_rel.get_schema_map()[missing_attr];
  for (auto e : entries) {
    // Get the value for the attribute not in t, and the tuple count.
    LL missing_val = e.first[missing_idx];
    LL delta_count = e.second;

    // Go through each other relation than the two already considered.
    for (int i = 0; i < n; ++i) {
      if (i == rel_num || i == next_rel_num) {
        continue;
      }
      // Fill the empty slot in the key.
      keys[i].first[keys[i].second] = missing_val;
      // Get the count for the constructed tuple.
      delta_count *= rels[i].get_multiplicity(keys[i].first);
      // If the count becomes 0, then stop looking through the other relations.
      if (!delta_count) {
        break;
      }
    }
    current_count += delta_count;
  }

  return current_count;
}