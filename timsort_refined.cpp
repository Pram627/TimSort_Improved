#include <bits/stdc++.h>
using namespace std;

struct SortStats {
    long long comparisons = 0;
    long long moves = 0;
    void reset(){ comparisons = moves = 0; }
};

template<typename T>
void merge_merge(vector<T>& arr, vector<T>& aux, size_t left, size_t mid, size_t right, SortStats &stats){
    size_t i = left, j = mid+1, k = left;
    while(i <= mid && j <= right){
        stats.comparisons++;
        if(arr[i] <= arr[j]) aux[k++] = arr[i++];
        else aux[k++] = arr[j++];
        stats.moves++;
    }
    while(i <= mid){ aux[k++] = arr[i++]; stats.moves++; }
    while(j <= right){ aux[k++] = arr[j++]; stats.moves++; }
    for(size_t t = left; t <= right; ++t) arr[t] = aux[t];
}

template<typename T>
void merge_sort_rec(vector<T>& arr, vector<T>& aux, size_t left, size_t right, SortStats &stats){
    if(left >= right) return;
    size_t mid = left + (right - left)/2;
    merge_sort_rec(arr, aux, left, mid, stats);
    merge_sort_rec(arr, aux, mid+1, right, stats);
    merge_merge(arr, aux, left, mid, right, stats);
}

template<typename T>
void merge_sort(vector<T>& arr, SortStats &stats){
    if(arr.size() <= 1) return;
    vector<T> aux(arr.size());
    merge_sort_rec(arr, aux, 0, arr.size()-1, stats);
}


template<typename T>
void binary_insertion_sort(vector<T>& arr, size_t left, size_t right, SortStats &stats){
    for(size_t i = (left+1); i <= right; ++i){
        T key = arr[i];
        
        size_t lo = left, hi = i;
        while(lo < hi){
            size_t mid = lo + (hi - lo)/2;
            stats.comparisons++;
            if(arr[mid] <= key) lo = mid + 1;
            else hi = mid;
        }
        
        for(size_t j = i; j > lo; --j){
            arr[j] = arr[j-1];
            stats.moves++;
        }
        arr[lo] = key;
        stats.moves++;
    }
}

template<typename T>
size_t count_run_and_make_ascending(vector<T>& arr, size_t start){
    size_t n = arr.size();
    if(start >= n-1) return 1;
    size_t i = start;
    if(arr[i] <= arr[i+1]){
        while(i+1 < n && arr[i] <= arr[i+1]) ++i;
    } else {
        while(i+1 < n && arr[i] > arr[i+1]) ++i;
        reverse(arr.begin()+start, arr.begin()+i+1);
    }
    return (i - start + 1);
}

template<typename T>
void merge_runs(vector<T>& arr, size_t left, size_t mid, size_t right, SortStats &stats){
    vector<T> left_part(arr.begin()+left, arr.begin()+mid+1);
    vector<T> right_part(arr.begin()+mid+1, arr.begin()+right+1);
    size_t i = 0, j = 0, k = left;
    while(i < left_part.size() && j < right_part.size()){
        stats.comparisons++;
        if(left_part[i] <= right_part[j]) arr[k++] = left_part[i++];
        else arr[k++] = right_part[j++];
        stats.moves++;
    }
    while(i < left_part.size()){ arr[k++] = left_part[i++]; stats.moves++; }
    while(j < right_part.size()){ arr[k++] = right_part[j++]; stats.moves++; }
}

template<typename T>
void timsort_impl(vector<T>& arr, size_t minrun, SortStats &stats){
    size_t n = arr.size();
    if(n <= 1) return;

    vector<pair<size_t,size_t>> runs; 
    size_t i = 0;
    while(i < n){
        size_t run_len = count_run_and_make_ascending(arr, i);
        if(run_len < minrun){
            size_t extend = min(minrun, n - i);
            binary_insertion_sort(arr, i, i + extend - 1, stats);
            run_len = extend;
        } else {
        }
        runs.push_back({i, run_len});
        i += run_len;
    }

    auto collapse = [&](void){
        while(runs.size() >= 2){
            size_t m = runs.size();
            if(m >= 3){
                size_t A = runs[m-3].second, B = runs[m-2].second, C = runs[m-1].second;
                if(A <= B + C){
                    if(A < C){
                        size_t left = runs[m-3].first;
                        size_t mid = runs[m-3].first + runs[m-3].second - 1;
                        size_t right = runs[m-2].first + runs[m-2].second - 1;
                        merge_runs(arr, left, mid, right, stats);
                        runs[m-3].second = runs[m-3].second + runs[m-2].second;
                        runs.erase(runs.begin() + (m-2));
                        continue;
                    } else {
                        size_t left = runs[m-2].first;
                        size_t mid = runs[m-2].first + runs[m-2].second - 1;
                        size_t right = runs[m-1].first + runs[m-1].second - 1;
                        merge_runs(arr, left, mid, right, stats);
                        runs[m-2].second = runs[m-2].second + runs[m-1].second;
                        runs.erase(runs.begin() + (m-1));
                        continue;
                    }
                }
            } else {
                size_t left = runs[0].first;
                size_t mid = runs[0].first + runs[0].second - 1;
                size_t right = runs[1].first + runs[1].second - 1;
                merge_runs(arr, left, mid, right, stats);
                runs[0].second = runs[0].second + runs[1].second;
                runs.erase(runs.begin() + 1);
                continue;
            }
            break;
        }
    };

    size_t idx = 0;
    while(idx < runs.size()){
        collapse();
        ++idx;
    }
    while(runs.size() > 1){
        size_t left = runs[runs.size()-2].first;
        size_t mid = runs[runs.size()-2].first + runs[runs.size()-2].second - 1;
        size_t right = runs[runs.size()-1].first + runs[runs.size()-1].second - 1;
        merge_runs(arr, left, mid, right, stats);
        runs[runs.size()-2].second += runs[runs.size()-1].second;
        runs.pop_back();
    }
}

template<typename T>
void timsort(vector<T>& arr, size_t minrun, SortStats &stats){
    timsort_impl(arr, minrun, stats);
}

vector<int> generate_dataset(const string &type, size_t n, unsigned seed){
    mt19937 rng(seed);
    uniform_int_distribution<int> dist(0, 1000000);
    vector<int> v(n);
    for(size_t i=0;i<n;++i) v[i] = dist(rng);
    if(type == "sorted") sort(v.begin(), v.end());
    else if(type == "reversed") sort(v.begin(), v.end(), greater<int>());
    else if(type == "nearly_sorted"){
        sort(v.begin(), v.end());
        size_t swaps = max((size_t)1, n/20);
        for(size_t k=0;k<swaps;++k){
            size_t a = rng() % n;
            size_t b = rng() % n;
            swap(v[a], v[b]);
        }
    }
    return v;
}

double measure_ms(function<void()> f){
    auto t0 = chrono::high_resolution_clock::now();
    f();
    auto t1 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> d = t1 - t0;
    return d.count();
}

bool is_sorted_vec(const vector<int>& v){
    return is_sorted(v.begin(), v.end());
}

void ensure_csv_header(ofstream &ofs){
    ofs << "algorithm,minrun,dataset_type,n,run_id,time_ms,comparisons,moves\n";
}

int main(int argc, char** argv){
    vector<string> dataset_types = {"random", "sorted", "reversed", "nearly_sorted"};
    vector<size_t> sizes = {10000, 50000, 100000, 500000};
    vector<size_t> minruns = {16, 32, 64, 128};
    size_t repetitions = 5;
    unsigned seed_base = 42;

    string out_csv = "results.csv";
    ofstream ofs(out_csv);
    if(!ofs.is_open()){
        cerr << "Cannot open " << out_csv << " for writing\n";
        return 1;
    }
    ensure_csv_header(ofs);

    cout << "Starting experiments. Results will be saved to " << out_csv << endl;

    for(const string &dtype : dataset_types){
        for(size_t n : sizes){
            for(size_t rep = 0; rep < repetitions; ++rep){
                unsigned seed = seed_base + rep;
                vector<int> base = generate_dataset(dtype, n, seed);

                {
                    vector<int> arr = base;
                    SortStats stats; stats.reset();
                    double t = measure_ms([&](){ merge_sort(arr, stats); });
                    bool ok = is_sorted_vec(arr);
                    if(!ok) cerr << "MergeSort failed to sort for " << dtype << " n="<<n<<"\n";
                    ofs << "MergeSort,NA,"<<dtype<<","<<n<<","<<rep<<","<<t<<","<<stats.comparisons<<","<<stats.moves<<"\n";
                    cout << "MergeSort: " << dtype << " n="<<n<<" run="<<rep<<" time="<<t<<" ms\n";
                }
                for(size_t mr : minruns){
                    vector<int> arr = base;
                    SortStats stats; stats.reset();
                    double t = measure_ms([&](){ timsort(arr, mr, stats); });
                    bool ok = is_sorted_vec(arr);
                    if(!ok) cerr << "TimSort failed to sort for minrun="<<mr<<" "<<dtype<<" n="<<n<<"\n";
                    ofs << "TimSort,"<<mr<<","<<dtype<<","<<n<<","<<rep<<","<<t<<","<<stats.comparisons<<","<<stats.moves<<"\n";
                    cout << "TimSort(minrun="<<mr<<"): " << dtype << " n="<<n<<" run="<<rep<<" time="<<t<<" ms\n";
                }
            }
        } 
    }

    ofs.close();
    cout << "All experiments completed. CSV: " << out_csv << endl;
    return 0;
}
