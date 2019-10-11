# Batch test script for 30-minute tests per query per maintenance mechanism.
echo "delta-q3"
timeout 1800 ./ivm-bin 3 query3.csv delta delta-q3.csv 0
echo "delta-q4"
timeout 1800 ./ivm-bin 4 query4.csv delta delta-q4.csv 0
echo "delta-q5"
timeout 1800 ./ivm-bin 5 query5.csv delta delta-q5.csv 0
echo "view-q3"
timeout 1800 ./ivm-bin 3 query3.csv view view-q3.csv 0
echo "view-q4"
timeout 1800 ./ivm-bin 4 query4.csv view view-q4.csv 0
echo "view-q5"
timeout 1800 ./ivm-bin 5 query5.csv view view-q5.csv 0
