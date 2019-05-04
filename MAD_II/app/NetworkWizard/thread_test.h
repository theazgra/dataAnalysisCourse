#pragma once
#include <thread>
#include <chrono>
#include <QDebug>

inline int test_thread_concurrency(int a)
{
    qDebug() << "before thread job with argument: " << a;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    qDebug() << "thread job completed";
    return 8498;
}
