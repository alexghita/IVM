#include <ivm/helperfunctions.h>
#include <ivm/viewprocessor.h>

#include <iostream>


/**
 * Constructor
 * =========== */

/**
 * Initialise a view processor with n relations and n views, one per relation,
 * such that each view allows O(1) count re-computation under updates
 * to its corresponding relation.
 */
ViewProcessor::ViewProcessor(int n_) : DeltaProcessor(n_) {
  Schema schema(n - 1);
  for (int i = 0; i < n; ++i) {
    for (int j = 1; j < n; ++j) {
      int idx = i + j > n ? (i + j) % n : i + j;
      schema[j - 1] = "A" + std::to_string(idx);
    }
    views.push_back(View(schema));
  }
}


/**
 * Operations
 * ========== */

/**
 * Given an update to a relation, update the current count using the view that
 * corresponds to the updated relation, then also maintain all views affected
 * by the update, i.e. all other views, and return the updated count.
 */
LL ViewProcessor::update_and_count(int rel_num, const Tuple &t, 
    int multiplicity) {
  // Update the count using the relation's view.
  current_count += multiplicity * views[rel_num].get_multiplicity(t);
  // Update the relation.
  update_rel(rel_num, t, multiplicity);

  bool is_new_tuple = (rels[rel_num].get_multiplicity(t) == multiplicity);

  views[rel_num].add_values(t);
  update_views(rel_num, t, multiplicity, is_new_tuple);

  return current_count;
}


/**
 * Update all views affected by an update to a relation. In the case of the
 * query class described in the exam problem statement, this means updating all
 * n-1 views corresponding to the all relations other than the updated one.
 */
void ViewProcessor::update_views(int rel_num, const Tuple &t, 
    int multiplicity, bool is_new_tuple) {
  // Get the attribute that is missing in the updated relation.
  std::string missing_attr = rel_num == 0 ? 
      "A" + std::to_string(n) : "A" + std::to_string(rel_num);
  // Get the keys used to search in the other views.
  std::vector<std::pair<Tuple, int> > keys = HelperFunctions::generate_keys(
      rels, t, rel_num, missing_attr);

  // Get the values for the missing attribute for each other view.
  std::vector<std::unordered_set<LL> > vals(n);
  for (int i = 0; i < n; ++i) {
    if (i == rel_num) {
      continue;
    }
    vals[i] = views[i].get_existing_values(
        rels[i].get_schema_map()[missing_attr]);
  }

  // Go through each view and update it.
  for (int i = 0; i < n; ++i) {
    // Ignore the view corresponding to the updated relation.
    if (i == rel_num) {
      continue;
    }

    // Go through the values for the missing attribute in the current view.
    for (auto v : vals[i]) {
      // The multiplicity difference.
      LL delta_count = multiplicity;

      // Go through each other relation to perform a delta update to the view.
      for (int j = 0; j < n; ++j) {
        if (j == i || j == rel_num) {
          continue;
        }
        // Fill the empty slot in the key.
        keys[j].first[keys[j].second] = v;
        // Get the count for the constructed tuple.
        delta_count *= rels[j].get_multiplicity(keys[j].first);
        // Stop if the count becomes 0.
        if (!delta_count) {
          break;
        }
      }
      keys[i].first[keys[i].second] = v;
      if (delta_count) {
        views[i].update_tuple(keys[i].first, delta_count);
      }
      // If the update contained a new tuple, then update the value sets too.
      if (is_new_tuple) {
        views[i].add_values(keys[i].first);
      }
    }
  }
}