ipcrm -a
cd ../build
gcc $1 ../code/process_generator.c -o process_generator.out
gcc $1 ../code/clk.c -o clk.out
gcc $1 ../code/scheduler.c -o scheduler.out $2
gcc $1 ../code/process.c -o process.out
gcc $1 ../code/test_generator.c -o test_generator.out
gcc $1 ../code/data_structures/main.c -o ds.out