#include <iostream>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <windows.h>

namespace fs = std::filesystem;

long long countLinesFast(const std::wstring& path) {
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ,
                            FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::printf("Invalid file handler"); 
        return 0;
    }
    
    char buffer[65536];
    long long lines = 0;
    DWORD bytesRead;
    while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0){
        for (DWORD i = 0; i < bytesRead; ++i){
            if(buffer[i] == '\n') lines++;
        }
    }
    CloseHandle(hFile);
    return lines;
}

void processDirectory(std::wstring path, std::map<std::wstring, long long>& stats) {
    WIN32_FIND_DATAW data;
    std::wstring searchPath = path + L"\\*";
    
    // LARGE_FETCH is the secret sauce for Windows performance
    HANDLE hFind = FindFirstFileExW(searchPath.c_str(), FindExInfoBasic, &data, 
                                    FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        std::wstring name = data.cFileName;
        if (name == L"." || name == L"..") continue;

        std::wstring fullPath = path + L"\\" + name;

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            processDirectory(fullPath, stats);
        } else {
            size_t dotPos = name.find_last_of(L'.');
            if (dotPos != std::wstring::npos) {
                std::wstring ext = name.substr(dotPos + 1);
                stats[ext] += countLinesFast(fullPath);
            }
        }
    } while (FindNextFileW(hFind, &data));

    FindClose(hFind);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: LocCounter.exe <path>\n");
        return 1;
    }
    if(!fs::is_directory(argv[1]) || !fs::exists(argv[1])) {
        printf("Invalid path: %s\n", argv[1]);
        return 1;
    }
    fs::path p(argv[1]);
    std::wstring path = fs::absolute(p).wstring();
    // Ensure theres no trailing backslash before we start
    if (!path.empty() && (path.back() == L'\\' || path.back() == L'/')) path.pop_back();

    auto startTime = std::chrono::steady_clock::now();

    std::map<std::wstring, long long> stats;
    processDirectory(path, stats);

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    double elapsedSec = elapsedMs / 1000.0;

    std::vector<std::pair<std::wstring, long long>> sorted(stats.begin(), stats.end());
    std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){
        return a.second > b.second;
    });

    std::printf("\nLines of code by file type:\n ----------------\n");
    long long total = 0;
    for (auto& [ext, count] : sorted){
        total += count;
        std::string s_ext;
        s_ext.reserve(ext.size());
        for (wchar_t wc : ext) s_ext.push_back(static_cast<char>(wc));
        std::println("{:<10} : {:>12} lines", s_ext, count);
    }
    std::println("\n ----------------\n");
    std::println("\nTotal: {:>12} Lines", total);
    std::println("Time:  {:>12.3f} s", elapsedSec);

    // Log to timestamp.log in execution (current) directory
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &t);
    std::ostringstream logName;
    logName << std::put_time(&tm_buf, "%Y-%m-%d_%H-%M-%S") << ".log";
    fs::path logPath = fs::current_path() / logName.str();

    std::ofstream log(logPath);
    if (log) {
        log << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " | path: " << p.string() << "\n";
        log << "elapsed: " << std::fixed << std::setprecision(3) << elapsedSec << " s\n";
        log << "total lines: " << total << "\n";
        log << "by extension:\n";
        for (auto& [ext, count] : sorted) {
            std::string s_ext;
            s_ext.reserve(ext.size());
            for (wchar_t wc : ext) s_ext.push_back(static_cast<char>(wc));
            log << "  " << s_ext << ": " << count << "\n";
        }
    }

    return 0;
}