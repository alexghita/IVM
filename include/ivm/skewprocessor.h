#ifndef _SKEWPROCESSOR_H
#define _SKEWPROCESSOR_H

#include <ivm/ivmprocessor.h>
#include <ivm/skewrelation.h>


const double EPSILON = 0.5;

class SkewProcessor : public IVMProcessor {
public:
  SkewProcessor(int n_);
  LL update_and_count(int rel_num, const Tuple &t, int multiplicity);
  LL delta_update(int rel_num, const Tuple &t, int multiplicity,
    int next_rel_num, const std::vector<bool> &skew_config);

private:
  std::vector<SkewRelation> skew_rels;
  LL current_count;
};

#endif