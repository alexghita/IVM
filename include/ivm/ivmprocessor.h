#ifndef _IVMPROCESSOR_H
#define _IVMPROCESSOR_H

#include <ivm/relation.h>


/**
 * This class models an IVM processor, which processes the query class described
 * in the exam problem statement using a specific strategy.
 */
class IVMProcessor {
public:
	/**
	 * Constructors
	 * ============ */

  // Default constructor.
  IVMProcessor() {}

	// The constructor initialises an IVM processor for a query of order n.
  IVMProcessor(int n_);


  /**
   * Operations
   * ========== */

  // Given an update to a relation, compute and return the new count.
  virtual LL update_and_count(int rel_num, const Tuple &t, int multiplicity);


protected:
	// The query order.
  int n;
  // The relations involved in the query.
  std::vector<Relation> rels;

  /**
   * Operations
   * ========== */

  // Perform an update to a specified relation.
  void update_rel(int rel_num, const Tuple &t, int multiplicity);
};

#endif