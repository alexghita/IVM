#include <ivm/view.h>


/**
 * Constructor
 * =========== */

// Initialise the relation and the set of values.
View::View(const Schema &schema_) : Relation(schema_), 
    existing_values(schema_.size(), std::unordered_set<LL>()) {}


/**
 * Operations
 * ========== */

// Add the values of a tuple to the sets of values.
void View::add_values(const Tuple &t) {
  for (int i = 0; i < schema_size; ++i) {
    existing_values[i].insert(t[i]);
  }
}


// Update a tuple.
void View::update_tuple(const Tuple &t, int multiplicity) {
  Relation::update_tuple(t, multiplicity);
}


/**
 * Accessors
 * ========= */

// Get the set of values for a given attribute.
std::unordered_set<LL> View::get_existing_values(int attr_index) const {
  return existing_values[attr_index];
}