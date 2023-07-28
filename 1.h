#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<thread>
#include<vector>
#include<cstdlib>
#include<chrono>
#include<mutex>
#include<condition_variable>
#include<shared_mutex>

#pragma warning(disable: 4996)

using namespace std;

shared_mutex MY_SHARED_MUTEX;

void FillVec_V2() {
    for (int i = 0; i < SIZE_FILL; i++) {
        int index = rand() % SIZE_ARR;
        {
            unique_lock<shared_mutex> lock(MY_SHARED_MUTEX);
            print_time();
            cout << i << "Изменение " << index << " элемента с " << ARR[index];
            ARR[index] = rand() % 10;
            cout << " на " << ARR[index] << endl;
        }
        this_thread::sleep_for(1ms);
    }
}

void ShowVec_V2() {
    vector<int> show_arr(SIZE_SHOW, -1);
    for (int i = 0; i < SIZE_SHOW; i++) {
        int index = rand() % SIZE_ARR;
        shared_lock<shared_mutex> lock(MY_SHARED_MUTEX);
        print_time();
        cout << "Чтение " << index << " элемента: " << ARR[index] << endl;
        show_arr[i] = ARR[index];
    }
    for (int i = 0; i < SIZE_SHOW; i++) cout << show_arr[i] << " ";
    cout << endl;
}

