#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <condition_variable>
#include <deque>
#include <vector>
#include <memory>

#include "nocopyable.h"
namespace thp
{
    class ThreadPool :public nocopyable
    {
    public:
        typedef std::function<void()> Task;

        explicit ThreadPool(const std::string& name = std::string());
        ~ThreadPool();


        void start(int numThreads);//�����߳���������numThreads���߳�
        void stop();//�̳߳ؽ���
        void run(const Task& f);//����f���̳߳�������
        void setMaxQueueSize(int maxSize) { _maxQueueSize = maxSize; }//����������пɴ�����������

    private:
        bool isFull();//��������Ƿ�����
        void runInThread();//�̳߳���ÿ��threadִ�е�function
        Task take();//�����������ȡ��һ������

        std::mutex _mutex;
        std::condition_variable _notEmpty;
        std::condition_variable _notFull;
        std::string _name;
        std::vector<std::thread> _threads;
        std::deque<Task> _queue;
        size_t _maxQueueSize;
        bool _running;
    };
}

#endif // THREADPOOL_H