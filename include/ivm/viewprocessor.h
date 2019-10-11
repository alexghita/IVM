#ifndef _VIEWPROCESSOR_H
#define _VIEWPROCESSOR_H

#include <ivm/deltaprocessor.h>
#include <ivm/view.h>


/**
 * This class models a view processor, which uses materialised views and delta
 * processing to compute the query result after each update instead of simply
 * re-evaluating the query.
 */
class ViewProcessor : public DeltaProcessor {
public:
	/**
	 * Constructor
	 * =========== */

	// The constructor initialises a view processor for a query of order n.
  ViewProcessor(int n_);


  /**
   * Operations
   * ========== */

  // Given an update to a relation, use materialised views to get the new count.
  LL update_and_count(int rel_num, const Tuple &t, int multiplicity);


protected:
	// Store a vector of views, one per relation.
  std::vector<View> views;


  /**
   * Operations
   * ========== */

  // Maintain the views after updating a relation.
  void update_views(int rel_num, const Tuple &t, int multiplicity, 
  		bool new_tuple);
};

#endif