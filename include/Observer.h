#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <mutex>

// Глобальный мьютекс для вывода в консоль
extern std::mutex cout_mutex;

// Интерфейс Observer
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void onKill(const std::string &killer, const std::string &victim) = 0;
};

// Observer для записи в файл (потокобезопасный)
class FileObserver : public Observer
{
private:
    std::string filename;
    std::mutex file_mutex;

public:
    FileObserver(const std::string &filename) : filename(filename) {}

    void onKill(const std::string &killer, const std::string &victim) override
    {
        std::lock_guard<std::mutex> lock(file_mutex);
        std::ofstream file(filename, std::ios::app);
        if (file.is_open())
        {
            file << killer << " убил(а) " << victim << std::endl;
            file.close();
        }
    }
};

// Observer для вывода на экран (потокобезопасный)
class ConsoleObserver : public Observer
{
public:
    void onKill(const std::string &killer, const std::string &victim) override
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[БОЕВОЙ ЛОГ] " << killer << " убил(а) " << victim << std::endl;
    }
};

// Класс Subject для управления наблюдателями
class Subject
{
private:
    std::vector<std::shared_ptr<Observer>> observers;
    std::mutex observers_mutex;

public:
    void attach(std::shared_ptr<Observer> observer)
    {
        std::lock_guard<std::mutex> lock(observers_mutex);
        observers.push_back(observer);
    }

    void notify(const std::string &killer, const std::string &victim)
    {
        std::lock_guard<std::mutex> lock(observers_mutex);
        for (auto &observer : observers)
        {
            observer->onKill(killer, victim);
        }
    }
};
