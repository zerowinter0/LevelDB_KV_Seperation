#include <thread>
#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <mutex>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "leveldb/iterator.h"
#include <sys/stat.h>  // For stat to get file size on Unix-like systems
#include <dirent.h>    // For directory reading on Unix-like systems

#define THREAD_COUNT 16          // 线程数量
#define PUT_THREAD_COUNT (THREAD_COUNT / 3)  // Put线程数量
#define DELETE_THREAD_COUNT (THREAD_COUNT / 3)  // Delete线程数量
#define ITERATE_THREAD_COUNT (THREAD_COUNT - PUT_THREAD_COUNT - DELETE_THREAD_COUNT)  // Iterate线程数量
#define VALUE_SIZE 1000         // Value的默认大小
#define DATABASE_PATH "db_benchmark"  // 数据库路径

std::mutex put_mutex;
std::mutex delete_mutex;
std::mutex iterate_mutex;

std::pair<int,int> put_time_count={0,0};
std::pair<int,int> delete_time_count={0,0};
std::pair<int,int> iterate_time_count={0,0};

// Helper function to generate a random string of a given length
std::string GenerateRandomString(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);
    
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }
    return result;
}

void PutData(leveldb::DB* db, int thread_id, int num_entries, size_t value_size) {
    leveldb::WriteOptions write_options;
    write_options.sync = false;

    auto start_time = std::chrono::high_resolution_clock::now(); // 记录开始时间

    for (int i = 0; i < num_entries; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
        std::string value = GenerateRandomString(value_size);

        leveldb::WriteBatch batch;
        batch.Put(key, value);
        db->Write(write_options, &batch);
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    put_mutex.lock();
    put_time_count.first+=duration;
    put_time_count.second+=num_entries;
    put_mutex.unlock();
}

void DeleteData(leveldb::DB* db, int thread_id, int num_entries) {
    leveldb::WriteOptions write_options;
    write_options.sync = false;

    auto start_time = std::chrono::high_resolution_clock::now(); // 记录开始时间
    
    for (int i = 0; i < num_entries; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);

        leveldb::WriteBatch batch;
        batch.Delete(key);
        db->Write(write_options, &batch);
    }
    auto end_time = std::chrono::high_resolution_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    delete_mutex.lock();
    delete_time_count.first+=duration;
    delete_time_count.second+=num_entries;
    delete_mutex.unlock();
}

void IterateData(leveldb::DB* db, leveldb::ReadOptions& read_options) {
    std::unique_ptr<leveldb::Iterator> it(db->NewIterator(read_options));

    auto start_time = std::chrono::high_resolution_clock::now(); // 记录开始时间
    
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        // 这里可以选择是否打印键值对，或者仅遍历不做任何操作
        std::cout << "Key: " << it->key().ToString() << ", Value: " << it->value().ToString() << "\n";
    }

    if (!it->status().ok()) {
        std::cerr << "Error during iteration: " << it->status().ToString() << "\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    iterate_mutex.lock();
    iterate_time_count.first+=duration;
    iterate_time_count.second++;
    iterate_mutex.unlock();
}

// Function to calculate the total size of all files in the database directory
uint64_t CalculateDatabaseSize(const std::string& db_path) {
    uint64_t total_size = 0;
    DIR* dir = opendir(db_path.c_str());
    if (dir == nullptr) {
        std::cerr << "Failed to open directory: " << db_path << "\n";
        return total_size;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) { // Only consider regular files
            std::string full_path = db_path + "/" + entry->d_name;
            struct stat file_stat;
            if (stat(full_path.c_str(), &file_stat) == 0) {
                total_size += file_stat.st_size;
            }
        }
    }

    closedir(dir);
    return total_size;
}

void CleanupDatabase(const std::string& db_path) {
    /// Delete database directory
    #ifdef _WIN32
        std::string command = "rd /s /q \"" + db_path + "\""; // Windows delete directory
    #else
        std::string command = "rm -rf \"" + db_path + "\""; // Linux/macOS delete directory
    #endif
    if (std::system(command.c_str()) == 0) {
        std::cout << "Database directory has been successfully deleted" << std::endl;
    } else {
        std::cerr << "Warning: Failed to delete the database directory. Please check manually!" << std::endl;
    }
}

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, DATABASE_PATH, &db);
    if (!status.ok()) {
        std::cerr << "Unable to open/create database: " << status.ToString() << "\n";
        return 1;
    }

    const int entries_per_thread = 1000000; // 每个线程执行的操作次数
    std::vector<std::thread> threads;

    // Create snapshot for iterate threads
    leveldb::ReadOptions read_options;
    read_options.snapshot = db->GetSnapshot();

    // Start threads for Put operations
    for (int i = 0; i < PUT_THREAD_COUNT; ++i) {
        threads.emplace_back(PutData, db, i, entries_per_thread, VALUE_SIZE);
    }

    // Start threads for Delete operations
    for (int i = 0; i < DELETE_THREAD_COUNT; ++i) {
        threads.emplace_back(DeleteData, db, i, entries_per_thread);
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // Start threads for Iterate operations
    for (int i = 0; i < ITERATE_THREAD_COUNT; ++i) {
        threads.emplace_back(IterateData, db, std::ref(read_options));
    }

    // Wait for all threads to finish
    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }
    threads.clear();

    // Release the snapshot after all threads have finished
    db->ReleaseSnapshot(read_options.snapshot);

    // Close the database
    delete db;
    std::cout<<"Put average time(per second):"<<put_time_count.first<<" "<<put_time_count.second<<std::endl;
    std::cout<<"Delete average time(per second):"<<delete_time_count.first<<" "<<delete_time_count.second<<std::endl;
    std::cout<<"Iterate average time(per second):"<<iterate_time_count.first<<" "<<iterate_time_count.second<<std::endl;
    // Calculate and print the total size of the database files
    uint64_t db_size = CalculateDatabaseSize(DATABASE_PATH);
    std::cout << "Total size of database files: " << db_size << " bytes\n";

    // Cleanup the database
    CleanupDatabase(DATABASE_PATH);

    return 0;
}