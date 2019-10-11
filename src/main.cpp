#include <ivm/deltaprocessor.h>
#include <ivm/helperfunctions.h>
#include <ivm/naiveprocessor.h>
#include <ivm/skewprocessor.h>
#include <ivm/viewprocessor.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <ctime>

typedef std::chrono::high_resolution_clock ClockT;


int main(int argc, char **argv) {
  // Ensure the command line arguments are valid.
  bool valid_args = HelperFunctions::validate_arguments(argc, argv);
  if (!valid_args) {
    return 1;
  }

  // Get the argument information, open the necessary files.
  int n = atoi(argv[1]);
  std::ifstream query_file;
  query_file.open(argv[2]);
  std::string join_type = argv[3];
  std::ofstream output_file(argv[4]);
  int update_limit = atoi(argv[5]);

  // Initialise the required IVM processor.
  IVMProcessor *ivm_proc;
  if (join_type == "naive") {
    ivm_proc = new NaiveProcessor(n);
  } else if (join_type == "delta") {
    ivm_proc = new DeltaProcessor(n);
  } else if (join_type == "view") {
    ivm_proc = new ViewProcessor(n);
  } else {
    ivm_proc = new SkewProcessor(n);
  }

  std::string input;
  int rel_num;
  int multiplicity;
  int update_count = 0;

  auto t1 = ClockT::now();
  // Read the lines and update the database.
  while (getline(query_file, input, ',') && 
      (update_count < update_limit || !update_limit)) {
    rel_num = stoi(input);
    std::vector<LL> tuple;

    for (int i = 0; i < n - 1; ++i) {
      getline(query_file, input, ',');
      tuple.push_back(stoll(input));
    }
    getline(query_file, input);
    multiplicity = stoi(input);
    
    LL result = ivm_proc->update_and_count(rel_num - 1, tuple, multiplicity);

    auto t2 = ClockT::now();
    double elapsed_time_so_far = (double)(std::chrono::duration_cast
        <std::chrono::seconds>(t2 - t1).count());

    output_file << result << "," << elapsed_time_so_far << "\n";

    // Print some progress dots from time to time.
    if (!(update_count % 500)) {
      std::cout << "." << std::flush;
    }
    update_count++;
  }
  std::cout << "\n";
  auto t2 = ClockT::now();
  double duration = (double)(std::chrono::duration_cast
      <std::chrono::seconds>(t2 - t1).count());
  std::cout << "Elapsed time: " << duration << " sec.\n";

  delete ivm_proc;
  query_file.close();
  output_file.close();
}