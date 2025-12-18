#pragma once
#include <string>
#include <memory>
#include <cmath>
#include <mutex>

class Visitor;

// Базовый класс для всех NPC
class NPC
{
protected:
    std::string name;
    double x, y;
    int health;
    int damage;
    bool alive;
    mutable std::mutex mtx; // Мьютекс для защиты доступа к данным NPC

public:
    NPC(const std::string &name, double x, double y, int health, int damage)
        : name(name), x(x), y(y), health(health), damage(damage), alive(true) {}

    virtual ~NPC() = default;

    // Геттеры (потокобезопасные)
    std::string getName() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return name;
    }

    double getX() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return x;
    }

    double getY() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return y;
    }

    int getHealth() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return health;
    }

    bool isAlive() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return alive;
    }

    // Метод для получения типа NPC
    virtual std::string getType() const = 0;

    // Получение характеристик движения
    virtual int getMoveRange() const = 0;
    virtual int getKillRange() const = 0;

    // Метод для вычисления расстояния до другого NPC
    double distanceTo(const NPC &other) const
    {
        // Если это тот же объект, вернуть 0 без блокировки
        if (this == &other)
        {
            return 0.0;
        }

        // Используем геттеры вместо прямого доступа к полям
        double x1 = getX();
        double y1 = getY();
        double x2 = other.getX();
        double y2 = other.getY();

        double dx = x1 - x2;
        double dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Движение NPC
    void move(double dx, double dy, double mapWidth, double mapHeight)
    {
        std::lock_guard<std::mutex> lock(mtx);
        x += dx;
        y += dy;
        // Ограничение на границы карты
        if (x < 0)
            x = 0;
        if (x > mapWidth)
            x = mapWidth;
        if (y < 0)
            y = 0;
        if (y > mapHeight)
            y = mapHeight;
    }

    // Получение урона
    void takeDamage(int dmg)
    {
        std::lock_guard<std::mutex> lock(mtx);
        health -= dmg;
        if (health <= 0)
        {
            alive = false;
        }
    }

    // Убийство NPC (отметить как мертвого)
    void kill()
    {
        std::lock_guard<std::mutex> lock(mtx);
        alive = false;
    }

    // Нанесение урона другому NPC
    int getDamage() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return damage;
    }

    // Метод для паттерна Visitor
    virtual void accept(Visitor &visitor, NPC &other) = 0;

    // Сериализация
    virtual std::string serialize() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return getType() + " " + name + " " + std::to_string(x) + " " + std::to_string(y);
    }

    // Получить мьютекс для внешней синхронизации
    std::mutex &getMutex() const { return mtx; }
};
