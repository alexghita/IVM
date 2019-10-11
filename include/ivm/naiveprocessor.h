#ifndef _NAIVEPROCESSOR_H
#define _NAIVEPROCESSOR_H

#include <ivm/ivmprocessor.h>


/**
 * This class models a naive processor, which simply re-evaluates the query
 * after each update.
 */
class NaiveProcessor : public IVMProcessor {
public:
  /**
   * Constructor
   * =========== */

  // The constructor initialises a naive processor for a query of order n.
  NaiveProcessor(int n_);


  /**
   * Operations
   * ========== */

  // Given an update to a relation, re-evaluate the query and get the new count.
  LL update_and_count(int rel_num, const Tuple &t, int multiplicity);
};

#endif