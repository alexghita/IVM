#include <ivm/helperfunctions.h>
#include <ivm/skewprocessor.h>
#include <ivm/skewrelation.h>

#include <iostream>


SkewProcessor::SkewProcessor(int n_) {
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
    skew_rels.push_back(SkewRelation(schema, "A" + std::to_string(i + 1), 
        EPSILON));
  }
}


LL SkewProcessor::delta_update(int rel_num, const Tuple &t, int multiplicity,
    int next_rel_num, const std::vector<bool> &skew_config) {
  std::string missing_attr = rel_num == 0 ? 
      "A" + std::to_string(n) : "A" + std::to_string(rel_num);
  int missing_idx = rels[next_rel_num].get_schema_map()[missing_attr];

  std::vector<std::pair<Tuple, int> > keys = HelperFunctions::generate_keys(
      rels, t, rel_num, missing_attr);

  std::unordered_map<Tuple, LL, container_hash<Tuple> > entries = 
      skew_config[next_rel_num] ? 
      skew_rels[next_rel_num].get_heavy_part().get_entries() :
      skew_rels[next_rel_num].get_light_part().get_entries();
  std::unordered_set<LL> missing_vals;
  for (auto e : entries) {
    if (e.second)
      missing_vals.insert(e.first[missing_idx]);
  }
  for (auto v : missing_vals) {
    LL delta_count = multiplicity;

    for (int i = 0; i < n; ++i) {
      if (i == rel_num) {
        continue;
      }
      // Fill the empty slot in the key.
      keys[i].first[keys[i].second] = v;
      // Get the count for the constructed tuple.
      Relation rel_to_get = skew_config[i] ? 
          skew_rels[i].get_heavy_part() :
          skew_rels[i].get_light_part();
      if (!skew_config[i]) {
        // if (skew_rels[i].get_light_part().has_tuple(keys[i].first))
        delta_count *= skew_rels[i].get_light_part().get_multiplicity(keys[i].first);
        // if (skew_rels[i].get_light_part().get_multiplicity(keys[i].first))
          //std::cout<<skew_rels[i].get_light_part().get_multiplicity(keys[i].first)<<'\n';
      } else {
        // if (skew_rels[i].get_heavy_part().has_tuple(keys[i].first))
        delta_count *= skew_rels[i].get_heavy_part().get_multiplicity(keys[i].first);
        // if (skew_rels[i].get_heavy_part().get_multiplicity(keys[i].first))
          //std::cout<<skew_rels[i].get_heavy_part().get_multiplicity(keys[i].first)<<'\n';
      }
      // If the count becomes 0, then stop looking through the other relations.
      if (!delta_count) {
        break;
      }
    }
    current_count += delta_count;
  }

  return current_count;
}


LL SkewProcessor::update_and_count(int rel_num, const Tuple &t, 
    int multiplicity) {
  skew_rels[rel_num].update_tuple(t, multiplicity);

  std::vector<bool> skew_config(n, false);
  int num_of_configs = pow(2, n - 1);
  int missing_attr_idx = rel_num == 0 ? n : rel_num - 1;

  for (int i = 0; i < num_of_configs; ++i) {
    bool has_light = false;
    int source_rel = -1;

    for (int j = 0; j < skew_config.size(); ++j) {
      if (j == missing_attr_idx && j == rel_num) continue;
      if (skew_config[j] == false) {
        has_light = true;
        source_rel = j;
        break;
      }
    }
    current_count += delta_update(rel_num, t, multiplicity, (rel_num + 1) % n, skew_config);
    // if (has_light || skew_config[missing_attr_idx]) {
    //   if (source_rel == -1) source_rel = missing_attr_idx - 1;
    //   current_count += delta_update(rel_num, t, multiplicity, source_rel, skew_config);
    // } else {
    //   //view_update();
    // }
  }

  return current_count;
}