#include <ivm/helperfunctions.h>
#include <ivm/relation.h>

#include <algorithm>
#include <cassert>
#include <iostream>


/**
 * Constructor
 * =========== */

/**
 * Construct a new relation with the given schema. As per the instructions in
 * the exam, the schema must have at least two attributes, since n >= 3.
 */
Relation::Relation(const Schema &schema_) {
  // Check the schema size is at least 2.
  assert(schema_.size() > 1);

  schema = schema_;
  schema_size = schema.size();

  /**
   * Store the attributes in the schema in sorted order, to make it easier to
   * perform other operations later, and for consistency.
   */
  sort(schema.begin(), schema.end());

  // Map the attribute names to their positions in the schema array.
  for (int i = 0; i < schema.size(); ++i) {
    schema_map[schema[i]] = i;
  }
}


/**
 * Operations
 * ========== */

// Count the total number of tuples as given by the sum of all multiplicities.
LL Relation::count() const {
  LL total_count = 0;

  // Go through all entries and add their multiplicities.
  for (auto e : entries) {
    total_count += e.second;
  }

  return total_count;
}


/**
 * Join this relation with another relation, R. The join will be performed by
 * creating an index on R on the join attributes using a multimap, and then 
 * going through the entries in this relation to look up the join attributes in
 * the index.
 *
 * This join algorithm is made for relations stored in uniform representation,
 * so it also computes the multiplicities of the resulting tuples.
 *
 * Assuming both relations have size N, the creation of the hash index has
 * complexity O(N*logN), and the join also has complexity O(N*logN).
 *
 * The overall join complexity is O(N*logN).
 */
Relation Relation::join(const Relation &r) {
  // Get information about R.
  Schema r_schema = r.get_schema();
  std::unordered_map<Tuple, LL, container_hash<Tuple> > r_entries = 
      r.get_entries();

  // The join schema will be the union of the two schemas.
  Schema res_schema = HelperFunctions::schema_union(schema, r_schema);
  // The join attributes will be the intersection of the two schemas.
  Schema join_attr = HelperFunctions::schema_intersection(schema, r_schema);
  // Initialise the resulting relation.
  Relation res = Relation(res_schema);

  /**
   * Go through the attributes in R's schema and sort them into two vectors
   * depending on whether they are part of the join attributes or not. 
   *
   * The former will become the key of the index on R, while the latter will 
   * become the value (along with the tuple's multiplicity).
   */
  std::vector<int> r_key_indices;
  std::vector<int> r_val_indices;
  for (int i = 0, j = 0; i < r_schema.size(); ++i) {
    // Put each attribute index in either of the two vectors.
    if (r_schema[i] == join_attr[j]) {
      r_key_indices.push_back(i);
      j++;
    } else {
      r_val_indices.push_back(i);
    }
  }

  /**
   * Get information on where to look for the value needed to construct result
   * tuples. This will be used during the joining of the two relations, when
   * iterating through the relation's tuples and checking R's tuples through the
   * index.
   *
   * The values of the successfully joined tuples will be obtained either from 
   * the current tuple in this relation, or the <key, value> pair in R's index.
   * As such, a vector detailing the join attribute order will be created here,
   * specifying for each attribute where to look for its value (in the current
   * tuple of this relation or in the <key, value> pair of R's index), and at
   * which index.
   */
  std::vector<std::pair<int, int> > join_attr_order(res_schema.size());
  for (int i = 0; i < res_schema.size(); ++i) {
    bool found = false;
    // Check for the attribute in the schema of this relation.
    for (int j = 0; j < schema_size; ++j) {
      if (schema[j] == res_schema[i]) {
        join_attr_order[i] = std::make_pair(0, j);
        found = true;
        break;
      }
    }
    // Check for the attribute in R's index.
    if (!found) {
      for (int j = 0; j < r_val_indices.size(); ++j) {
        if (r_schema[r_val_indices[j]] == res_schema[i]) {
          join_attr_order[i] = std::make_pair(1, j);
          break;
        }
      }
    }
  }

  /**
   * The hash index on R will map each unique tuple of join attributes to a pair
   * consisting of a tuple of the remaining attributes, and the multiplicity of
   * the original full tuple.
   */
  std::unordered_multimap<Tuple, std::pair<Tuple, LL>, container_hash<Tuple> > 
      r_index;
  Tuple index_key(r_key_indices.size());
  Tuple index_val(r_val_indices.size());
  for (auto t : r_entries) {
    // Construct the key for tuple t.
    for (int i = 0; i < r_key_indices.size(); ++i) {
      index_key[i] = (t.first[r_key_indices[i]]);
    }
    // Construct the value for tuple t.
    for (int i = 0; i < r_val_indices.size(); ++i) {
      index_val[i] = (t.first[r_val_indices[i]]);
    }
    // Insert the mapping in the index.
    r_index.insert(std::make_pair(index_key, 
        std::make_pair(index_val, t.second)));
  }

  /**
   * Perform the actual join by going through all tuples in this relation,
   * constructing a key based on the tuple's values, and looking up the
   * corresponding value in R's index, if it exists.
   *
   * If a value is found, create the resulting join tuple and add it to the
   * resulting relation.
   */
  Tuple key(join_attr.size());
  Tuple join_t(res_schema.size());
  for (auto t : entries) {
    // Construct the key.
    for (int i = 0; i < join_attr.size(); ++i) {
      key[i] = t.first[schema_map[join_attr[i]]];
    }

    // Get the corresponding values.
    std::pair<std::unordered_multimap<Tuple, std::pair<Tuple, LL>, 
        container_hash<Tuple> >::iterator,
        std::unordered_multimap<Tuple, std::pair<Tuple, LL>, 
        container_hash<Tuple> >::iterator> vals = r_index.equal_range(key);

    // Go through the values and join the ones with non-zero multiplicity.
    for (std::unordered_multimap<Tuple, std::pair<Tuple, LL>, 
        container_hash<Tuple> >::iterator it = vals.first; it != vals.second;
        ++it) {
      if (it->second.second) {
        int j = 0;
        // Construct the join tuple.
        for (auto p : join_attr_order) {
          if (p.first) {
            join_t[j++] = it->second.first[p.second];
          } else {
            join_t[j++] = t.first[p.second];
          }
        }
        // Insert the join tuple in the resulting relation.
        res.update_tuple(join_t, t.second * it->second.second);
      }
    }
  }

  return res;
}


// Update a tuple with a given multiplicity (or add it if it does not exist).
void Relation::update_tuple(const Tuple &t, int multiplicity) {
  assert(t.size() == schema.size());
  assert(multiplicity);

  entries[t] += multiplicity;
}


// Print a list of tuples in the relation and their multiplicities.
void Relation::print_contents() const {
  // Print a header made of the names of the attributes, in order.
  for (auto attr : schema) {
    std::cout << attr << " ";
  }
  std::cout << "[#]\n";

  // Go through the entries and print those with non-zero multiplicity.
  for (auto e : entries) {
    if (e.second == 0) {
      continue;
    }
    // Print the attribute values.
    for (int i = 0; i < e.first.size(); ++i) {
      std::cout << e.first[i] << " ";
    }
    // Print the multiplicity.
    std::cout << "[" << e.second << "]\n";
  }
}


/**
 * Accessors
 * ========= */

// Return true if the relation contains no tuples with non-zero multiplicity.
bool Relation::empty() const {
  return entries.empty();
}


// Get the entries in the relation as a map of tuples to multiplicities.
std::unordered_map<Tuple, LL, container_hash<Tuple> > 
    Relation::get_entries() const {
  return entries;
}


// Get the multiplicity of a given tuple.
LL Relation::get_multiplicity(const Tuple &t) const {
  assert(t.size() == schema.size());

  LL res;
  // Check if the tuple exists in the map.
  try {
    res = entries.at(t);
  } catch (std::out_of_range e) {
    res = 0;
  }

  return res;
}


// Get the schema of the relation.
Schema Relation::get_schema() const {
  return schema;
}


// Get the schema map of the relation.
std::map<std::string, int> Relation::get_schema_map() const {
  return schema_map;
}


// Get the number of attributes in the relation.
int Relation::get_schema_size() const {
  return schema_size;
}