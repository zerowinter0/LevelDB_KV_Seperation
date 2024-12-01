#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include "leveldb/db.h"
// 配置
const int TEST_EXPONENT = 5;
const int TEST_FREQUENCY = static_cast<int>(std::pow(10, TEST_EXPONENT));
const int MIN_STR_LEN = 255;
const int MAX_STR_LEN = 1024;
const std::string DB_PATH = "db_benchmark";
const std::string CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// 多语言
const std::string BASE_VALUE = "こんにちは世界！Hello World! Привет, мир! ¡Hola Mundo! 你好，世界！Bonjour le monde! Hallo Welt!";
// 莎士比亚
// const std::string BASE_VALUE = "To be, or not to be, that is the question: Whether 'tis nobler in the mind to suffer the slings and arrows of outrageous fortune, or to take arms against a sea of troubles and by opposing end them.";
// 超长字符
// const std::string BASE_VALUE = []() {
//     std::string base = "壹贰叁肆伍陆柒捌玖拾";
//     std::string long_text;
//     for (int i = 0; i < 100; ++i) { // 重复 100 次
//         long_text += base;
//     }
//     return long_text;
// }();

// 随机字符串生成
std::string randomStr() {
    int len = rand() % (MAX_STR_LEN - MIN_STR_LEN + 1) + MIN_STR_LEN;
    std::string str(len, '\0');
    for (int i = 0; i < len; ++i) {
        str[i] = CHARSET[rand() % CHARSET.size()];
    }
    return str;
}

// 计算并输出耗时的模板函数
template<typename Func>
void measureTime(const std::string& operation, Func func) {
    auto start = std::chrono::system_clock::now();
    func();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double seconds = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;

    // 输出格式化信息
    std::cout << "Operation: " << operation << "\n";
    std::cout << "Number of operations: " << TEST_FREQUENCY << "\n";
    std::cout << "Total time: " 
              << std::fixed << std::setprecision(6) << seconds << " seconds\n";
    std::cout << "Average time per operation: " 
              << std::fixed << std::setprecision(6) 
              << (seconds / TEST_FREQUENCY) * 1e6 << " microseconds\n";
    std::cout << "========================================\n";
}

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    // 打开数据库
    leveldb::Status status = leveldb::DB::Open(options, DB_PATH, &db);
    assert(status.ok());
    std::cout << "db open: " << DB_PATH << std::endl;

    srand(2017);

    // 生成测试数据
    std::string keys[TEST_FREQUENCY];
    for (int i = 0; i < TEST_FREQUENCY; ++i) {
        keys[i] = randomStr();
    }
    std::string value = BASE_VALUE;
    for (int i = 0; i < 4; ++i) {
        value += value; // 扩展 base value
    }

    // 测试添加
    measureTime("ADD", [&]() {
        for (int i = 0; i < TEST_FREQUENCY; ++i) {
            status = db->Put(leveldb::WriteOptions(), keys[i], value);
            assert(status.ok());
        }
    });

    // 测试获取
    measureTime("GET", [&]() {
        std::string retrievedValues[TEST_FREQUENCY];
        for (int i = 0; i < TEST_FREQUENCY; ++i) {
            status = db->Get(leveldb::ReadOptions(), keys[i], &retrievedValues[i]);
            assert(status.ok());
            assert(retrievedValues[i] == value); // 验证获取结果
        }
    });

    // 测试修改
    measureTime("UPDATE", [&]() {
        std::string newValue = value + value;
        for (int i = 0; i < TEST_FREQUENCY; ++i) {
            status = db->Put(leveldb::WriteOptions(), keys[i], newValue);
            assert(status.ok());
        }
    });

    // 测试删除
    measureTime("DELETE", [&]() {
        for (int i = 0; i < TEST_FREQUENCY; ++i) {
            status = db->Delete(leveldb::WriteOptions(), keys[i]);
            assert(status.ok());
        }
    });

    if (db) {
    delete db;
    db = nullptr;
    }
    std::cout << "Test completed, database has been closed." << std::endl;

    // Delete database directory
    #ifdef _WIN32
        std::string command = "rd /s /q \"" + DB_PATH + "\""; // Windows delete directory
    #else
        std::string command = "rm -rf \"" + DB_PATH + "\""; // Linux/macOS delete directory
    #endif
    if (std::system(command.c_str()) == 0) {
        std::cout << "Database directory has been successfully deleted" << std::endl;
    } else {
        std::cerr << "Warning: Failed to delete the database directory. Please check manually!" << std::endl;
    }

    
    return 0;
}
