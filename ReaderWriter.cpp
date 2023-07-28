#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<thread>
#include<vector>
#include<cstdlib>
#include<chrono>
#include<mutex>
#include<condition_variable>


#pragma warning(disable: 4996)

using namespace std;

mutex MY_MUTEX;
condition_variable MY_COND_VAR;

vector<int> ARR;
size_t SIZE_ARR = 100;
size_t SIZE_FILL = 20;
size_t SIZE_SHOW = 20;

//atomic<size_t> whatingWriter = 0;
//atomic<size_t> workingWriter = 0;
//atomic<size_t> workingReader = 0;

class Locker {
    unique_lock<mutex> lock;
    atomic<size_t> whatingWriter;
    atomic<size_t> workingWriter;
    atomic<size_t> workingReader;

public:
    Locker () : lock(MY_MUTEX), whatingWriter(0), workingWriter(0), workingReader(0) {}
    
    void RaderLock() {
        //cout << "Reader START wating " << workingWriter << whatingWriter << endl;
        MY_COND_VAR.wait(lock, [this]() { return workingWriter == 0 && whatingWriter == 0; });
        //cout << "Reader STOP wating" << endl;
        workingReader++;
    }

    void ReaderUnlock() {
        workingReader--;
        MY_COND_VAR.notify_all();
    }

    void WriterLock() {
        whatingWriter++;
        MY_COND_VAR.wait(lock, [this]() { return workingWriter == 0 && workingReader == 0; });
        workingWriter++;
        whatingWriter--;
    }

    void WriterUnlock() {
        workingWriter--;
        MY_COND_VAR.notify_all();
        //cout << "workingWriter " << workingWriter << " whatingWriter " << whatingWriter << endl;
        this_thread::sleep_for(1ms);
    }
};

Locker MY_LOCKER;

void print_time() {
    const auto now = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

    cout << ms.count() << "ms: ";
}

void FillVec() {
    for (int i = 0; i < SIZE_FILL; i++) {
        int index = rand() % SIZE_ARR;
        
        MY_LOCKER.WriterLock();
        print_time();
        cout << i << "!!!Изменение " << index << " элемента с " << ARR[index];
        ARR[index] = rand() % 10;
        cout << " на " << ARR[index] << endl;
        MY_LOCKER.WriterUnlock();
    }
    //MY_COND_VAR.notify_all();

}

void ShowVec() {
    vector<int> show_arr(SIZE_SHOW, -1);
    for (int i = 0; i < SIZE_SHOW; i++) {
        int index = rand() % SIZE_ARR;

        MY_LOCKER.RaderLock();
        print_time();
        cout << "Чтение " << index << " элемента: " << ARR[index] << endl;
        show_arr[i] = ARR[index];
        MY_LOCKER.ReaderUnlock();
    }
    for (int i = 0; i < SIZE_SHOW; i++) cout << show_arr[i] << " ";
    cout << endl;
}

#include"1.h"

int main()
{
    system("chcp 1251");
    system("cls");

    ARR.reserve(SIZE_ARR);

    for (int i = 0; i < SIZE_ARR; i++) {
        ARR.push_back(rand() % 10);
        cout << "[" << i << "]" << ARR[i] << " ";
    }
    cout << endl;

    srand(time(nullptr));

    thread Reader1(ShowVec_V2);
    thread Reader2(ShowVec_V2);
    thread Writer1(FillVec_V2);

    Reader1.join();
    Reader2.join();
    Writer1.join();

    return 0;
}