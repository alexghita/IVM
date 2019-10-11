IVM (Incremental View Maintenance) is a database system that supports efficient computation of a specific class of queries under frequent updates.

For *n* relations *R*<sub>*1*</sub>, ..., *R*<sub>*n*</sub> with corresponding schemas *(A*<sub>*1*</sub>, ..., *A*<sub>*n-1*</sub>)*, *(A*<sub>*2*</sub>, ..., *A*<sub>*n*</sub>)*, ..., *(A*<sub>*n*</sub>, *A*<sub>*1*</sub>, ..., *A*<sub>*n-2*</sub>)*, IVM computes their join and incrementally updates the join result under updates to any relation. It is assumed the relations are initially empty, and that only insertion updates are performed.

# How to Use
---
#### Prerequisites
IVM is written in **C++**, using the **STL** and **Boost** libraries. To install Boost on Unix systems, use the following command: **sudo apt-getinstall libboost-all-dev**.
### Compilation
1. Go to **ivm/build**.
2. Run "**cmake .\.**", then "**make**".
3. To run IVM, use the following command:
   **./ivm-bin N query-file mode output-file M**, where:
   - *N* is the number of relations.
   - *query-file* is the relative path to a CSV file containing the list of updates performed on each relation.
   - *mode* represents the result update technique, which can be one of the following: **naive**, **delta**, or **view**.
   - *output-file* represents the relative path to where the query result and total time elapsed after each update are printed.

### Plotting Results
The Python script **plot_throughput.py** can be used to plot the throughput for multiple IVM output files. To use it, run the following command:

    python plot_throughput.py output-1 legend-1 ... output-k legend-k
    
where:
- *output-i* is the relative path to a CSV file outputted by IVM.
- *legend-i* represents the associated plot legend for *output-file-i*.
