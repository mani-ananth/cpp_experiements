#include <bits/stdc++.h>
#include <thread>
#include <benchmark/benchmark.h>

using sum_st = unsigned long long int;
using sum_mt = std::atomic<unsigned long long int>;
using vec_type = std::vector<unsigned int>;
using vector_of_threads = std::vector<std::thread>;

struct Args {
    int tid, THREADS;
    std::reference_wrapper<vec_type> v;
	sum_st ref_sum;
};

////////////////////////////////////////////////////////////////////////////////
/////////               Single Threaded Implementation              ////////////
////////////////////////////////////////////////////////////////////////////////

void vector_add_ST(vec_type &v, sum_st &sum, int tid, int THREADS) {
    if (THREADS == 1) {
        // sequential implementation
        for (auto &i : v) {
            sum += i;
        }
    } else {
        exit(-1);
    }
}

auto vector_add_BM_ST = [](benchmark::State &state, Args &args) {
    while (state.KeepRunning()) {
        sum_st sum = 0;
        vector_add_ST(args.v, sum, args.tid, args.THREADS);
        std::cout << "Sum = " << sum << " computed across by single thread\n";
		assert(args.ref_sum == sum);
    }
};

////////////////////////////////////////////////////////////////////////////////
/////////               Multi Threaded Implementation               ////////////
////////////////////////////////////////////////////////////////////////////////

void vector_add_MT(vec_type &v, sum_mt &sum, int tid, int THREADS) {
    auto TOTAL = v.size();
    auto per_thread = (long unsigned int)(ceil(TOTAL * 1.0 / THREADS));
    auto start = tid * per_thread;
    auto end = std::min((tid+1)*per_thread, TOTAL);
    sum_st local_sum = 0;
    for (auto i = start; i < end; i++) {
        local_sum += v[i];
    }
    sum += local_sum;
    printf("tid %d: local_sum = %u", tid, local_sum);
    std::cout << " sum = " << sum << std::endl;
}

auto vector_add_BM_MT = [](benchmark::State &state, Args &args) {
    while (state.KeepRunning()) {
        sum_mt sum;
        sum = 0;
		vector_of_threads threads;
		for (auto i=0; i < args.THREADS; ++i)
		    threads.emplace_back(std::thread(vector_add_MT, std::ref(args.v), std::ref(sum), i, args.THREADS));
		for (auto &t : threads)
		    t.join();
        std::cout << "Sum = " << sum.load() << " computed across " << args.THREADS << " threads\n";
		assert(args.ref_sum == sum.load());
    }
};

////////////////////////////////////////////////////////////////////////////////
/////////                     MAIN FUNCTION                         ////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

    if (argc < 4) {
        std::cout << "Usage: ./multithreaded_vector_add <mode> <N_ELEMS> <N_THREADS>\n";
        return -1;
    }
 
    int mode = std::stoi(argv[1]);
    auto num_elements = std::stoi(argv[2]);
    auto nthreads = std::stoi(argv[3]);
    vec_type v(num_elements, 0);
 
    std::generate(v.begin(), v.end(), [](){
        return rand() % 5;
    });

    // Reference code
    sum_st ref_sum = 0;
    vector_add_ST(v, ref_sum, 0, 1);
    std::cout << "Reference: Sum = " << ref_sum << std::endl;
    // std::cout << "Reference: Values = ";
    // for (auto &i : v)
    //     std:: cout << i << " ";
    // std::cout << std::endl;
	
	// Actual
    if (mode == 1) {
        Args args{0, 1, std::ref(v), ref_sum};
        benchmark::RegisterBenchmark("Single Threaded Vec Add", vector_add_BM_ST, std::ref(args)) \
            -> Unit(benchmark::kMillisecond) \
            -> Iterations(5);
        benchmark::Initialize(&argc, argv);
        benchmark::RunSpecifiedBenchmarks();
    } else if (mode == 2) {
        Args args{0/*doesnt matter*/, nthreads, std::ref(v), ref_sum};
        benchmark::RegisterBenchmark("Multi Threaded Vec Add", vector_add_BM_MT, std::ref(args)) \
            -> Unit(benchmark::kMillisecond) \
            -> Iterations(5) \
			-> UseRealTime();
        benchmark::Initialize(&argc, argv);
        benchmark::RunSpecifiedBenchmarks();
    } else if (mode == 3) {
        sum_mt sum; sum = 0;
        std::vector<std::thread> threads;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i=0; i<nthreads; i++)
            threads.push_back(std::thread(vector_add_MT, std::ref(v), std::ref(sum), i, nthreads));
        for (int i=0; i<nthreads; i++)
            threads[i].join();
        auto end = std::chrono::high_resolution_clock::now();
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Sum = " << sum << " computed across " << nthreads << " threads in " << t.count() << " ms\n";
    } else {
        exit(-1);
    }
    
    return 0;
 
}

