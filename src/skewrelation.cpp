#include <ivm/skewrelation.h>

#include <cmath>


/**
 * Constructor
 * =========== */

// Initialise the partitions and the skew criteria.
SkewRelation::SkewRelation(const Schema &schema_, const std::string &skew_attr_,
    double epsilon_) : Relation(schema_), light_part(schema_), 
    heavy_part(schema_) {
  // Get the index in the schema of the skew attribute.
  for (int i = 0; i < schema.size(); ++i) {
    if (schema[i] == skew_attr_) {
      skew_attr_idx = i;
      break;
    }
  }
  epsilon = epsilon_;
}


/**
 * Operations
 * ========== */

// Update a tuple in a partition, then re-balance the partitions for that tuple.
void SkewRelation::update_tuple(const Tuple &t, int multiplicity) {
  assert(t.size() == schema.size());
  assert(multiplicity);

  // Get the tuple's multiplicity in each partition to determine where it is.
  LL heavy_mult = heavy_part.get_multiplicity(t);
  LL light_mult = light_part.get_multiplicity(t);
  // Track where the tuple is put. 0 = in light, 1 = in heavy, 2 = nowhere.
  int placement;

  // If it's in the heavy partition, update it there.
  if (heavy_mult) {
    heavy_part.update_tuple(t, multiplicity);
    placement = 1;
    // Check if it has just been deleted.
    if (!(heavy_mult + multiplicity)) {
      // Remove it from the tuples associated with this value.
      skew_tuples[t[skew_attr_idx]].erase(t);
      // Decrement the size.
      N--;
      // If it has been deleted, it doesn't need to be re-balanced.
      placement = 2;
    }
  } else {
    // If it's in the light partition, update it there.
    light_part.update_tuple(t, multiplicity);
    placement = 0;
    // Check if it has just been deleted.
    if (!(light_mult + multiplicity)) {
      // Remove it from the tuples associated with this value.
      skew_tuples[t[skew_attr_idx]].erase(t);
      // Decrement the size.
      N--;
      // If it has been deleted, it doesn't need to be re-balanced.
      placement = 2;
    } else if (!light_mult) {
      // If it's a new tuple, put it in the light partition.
      skew_tuples[t[skew_attr_idx]].insert(t);
      // Increment the size.
      N++;
    }
  }
  // If the tuple still exists in the relation, re-balanceit.
  if (placement != 2) {
    rebalance(t[skew_attr_idx], !placement);
  }
}


/**
 * Given a value for the skew attribute and the partition where the tuples
 * associated with it are currently located, re-balance these tuples if needed.
 */
void SkewRelation::rebalance(LL val, bool in_light) {
  // Check whether the tuples should be in the light partition or not.
  bool light = skew_tuples[val].size() <= pow(N, epsilon);

  // If the tuples are where they should be, then do nothing.
  if (in_light && light || !in_light && !light) {
    return;
  }

  // Otherwise, move them to the other partition.
  if (in_light) {
    for (auto t : skew_tuples[val]) {
      int mult = light_part.get_multiplicity(t);
      if (!mult) continue;
      heavy_part.update_tuple(t, mult);
      light_part.update_tuple(t, -mult);
    }
  } else {
    for (auto t : skew_tuples[val]) {
      int mult = heavy_part.get_multiplicity(t);
      if (!mult) continue;
      light_part.update_tuple(t, mult);
      heavy_part.update_tuple(t, -mult);
    }
  }
}


// Print the contents of the heavy partition.
void SkewRelation::print_heavy_contents() const {
  heavy_part.print_contents();
}

// Print the contents of the light partition.
void SkewRelation::print_light_contents() const {
  light_part.print_contents();
}


/**
 * Accessors
 * ========= */

// Return the heavy partition.
Relation SkewRelation::get_heavy_part() const {
  return heavy_part;
}


// Return the light partition.
Relation SkewRelation::get_light_part() const {
  return light_part;
}