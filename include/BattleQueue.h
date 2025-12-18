#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

// Структура для задачи боя
struct BattleTask
{
    std::shared_ptr<class NPC> attacker;
    std::shared_ptr<class NPC> defender;

    BattleTask(std::shared_ptr<class NPC> atk, std::shared_ptr<class NPC> def)
        : attacker(atk), defender(def) {}
};

// Потокобезопасная очередь задач для боев
class BattleQueue
{
private:
    std::queue<BattleTask> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;

public:
    // Добавить задачу в очередь
    void push(const BattleTask &task)
    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(task);
        cv.notify_one();
    }

    // Извлечь задачу из очереди (блокирующая операция)
    bool pop(BattleTask &task)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !tasks.empty() || stopped; });

        if (stopped && tasks.empty())
        {
            return false;
        }

        task = tasks.front();
        tasks.pop();
        return true;
    }

    // Остановить очередь
    void stop()
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopped = true;
        cv.notify_all();
    }

    // Проверка, пуста ли очередь
    bool empty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return tasks.empty();
    }
};
