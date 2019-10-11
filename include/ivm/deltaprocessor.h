#ifndef _DELTAPROCESSOR_H
#define _DELTAPROCESSOR_H

#include <ivm/ivmprocessor.h>


/**
 * This class models a delta processor, which uses delta processing to compute
 * the query result after each update instead of simply re-evaluating the query.
 */
class DeltaProcessor : public IVMProcessor {
public:
  /**
   * Constructor
   * =========== */

  // The constructor initialises a delta processor for a query of order n.
  DeltaProcessor(int n_);


  /**
   * Operations
   * ========== */

  // Given an update to a relation, use delta processing to get the new count.
  LL update_and_count(int rel_num, const Tuple &t, int multiplicity);


protected:
  // Keep track of the current query result.
  LL current_count;
};

#endif