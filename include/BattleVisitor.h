#pragma once
#include "Visitor.h"
#include "Knight.h"
#include "Druid.h"
#include "Elf.h"
#include "Observer.h"
#include <functional>
#include <random>
#include <mutex>

// Реализация Visitor для боевой системы с бросками кубика
// Правила боя:
// - Рыцарь убивает эльфа
// - Эльф убивает друида и рыцаря
// - Друид убивает друидов
// - В бою каждый NPC бросает 6-гранный кубик
// - Если атака > защита, происходит убийство

class BattleVisitor : public Visitor
{
private:
    Subject &subject;
    std::function<int()> rollFn;
    mutable std::mutex rng_mutex; // Мьютекс для генератора случайных чисел
    mutable std::mt19937 rng;
    mutable std::uniform_int_distribution<int> dice;

    // Бросок кубика (1-6)
    int rollDice() const
    {
        std::lock_guard<std::mutex> lock(rng_mutex);
        if (rollFn)
        {
            return rollFn();
        }
        return dice(rng);
    }

    void fight(NPC &attacker, NPC &defender, bool canAttackerKill, bool canDefenderKill)
    {
        if (!canAttackerKill && !canDefenderKill)
        {
            // Никто не может убить друг друга
            return;
        }

        int attackRoll = rollDice();
        int defenseRoll = rollDice();

        if (canAttackerKill && canDefenderKill)
        {
            // Оба могут убивать друг друга
            if (attackRoll > defenseRoll)
            {
                // Атакующий побеждает
                defender.kill();
                subject.notify(attacker.getName() + " (" + attacker.getType() + ")",
                               defender.getName() + " (" + defender.getType() + ") [Атака:" + std::to_string(attackRoll) + " > Защита:" + std::to_string(defenseRoll) + "]");
            }
            else if (defenseRoll > attackRoll)
            {
                // Защищающийся побеждает
                attacker.kill();
                subject.notify(defender.getName() + " (" + defender.getType() + ")",
                               attacker.getName() + " (" + attacker.getType() + ") [Защита:" + std::to_string(defenseRoll) + " > Атака:" + std::to_string(attackRoll) + "]");
            }
            // Если равны - ничья
        }
        else if (canAttackerKill)
        {
            // Только атакующий может убить
            if (attackRoll > defenseRoll)
            {
                defender.kill();
                subject.notify(attacker.getName() + " (" + attacker.getType() + ")",
                               defender.getName() + " (" + defender.getType() + ") [Атака:" + std::to_string(attackRoll) + " > Защита:" + std::to_string(defenseRoll) + "]");
            }
        }
        else if (canDefenderKill)
        {
            // Только защищающийся может убить
            if (defenseRoll > attackRoll)
            {
                attacker.kill();
                subject.notify(defender.getName() + " (" + defender.getType() + ")",
                               attacker.getName() + " (" + attacker.getType() + ") [Защита:" + std::to_string(defenseRoll) + " > Атака:" + std::to_string(attackRoll) + "]");
            }
        }
    }

public:
    BattleVisitor(Subject &subject)
        : subject(subject), rng(std::random_device{}()), dice(1, 6) {}

    // Конструктор для тестов: позволяет задать детерминированный бросок кубика
    BattleVisitor(Subject &subject, std::function<int()> rollFn)
        : subject(subject), rollFn(std::move(rollFn)), rng(std::random_device{}()), dice(1, 6) {}

    // Knight vs ...
    void visitKnight(Knight &attacker, Knight &defender) override
    {
        // Рыцарь не убивает рыцаря
        fight(attacker, defender, false, false);
    }

    void visitKnight(Knight &attacker, Druid &defender) override
    {
        // Рыцарь не убивает друида
        fight(attacker, defender, false, false);
    }

    void visitKnight(Knight &attacker, Elf &defender) override
    {
        // Рыцарь убивает эльфа, но эльф тоже убивает рыцаря
        fight(attacker, defender, true, true);
    }

    // Druid vs ...
    void visitDruid(Druid &attacker, Knight &defender) override
    {
        // Друид не убивает рыцаря
        fight(attacker, defender, false, false);
    }

    void visitDruid(Druid &attacker, Druid &defender) override
    {
        // Друид убивает друида
        fight(attacker, defender, true, true);
    }

    void visitDruid(Druid &attacker, Elf &defender) override
    {
        // Друид не убивает эльфа, но эльф убивает друида
        fight(attacker, defender, false, true);
    }

    // Elf vs ...
    void visitElf(Elf &attacker, Knight &defender) override
    {
        // Эльф убивает рыцаря, но рыцарь тоже убивает эльфа
        fight(attacker, defender, true, true);
    }

    void visitElf(Elf &attacker, Druid &defender) override
    {
        // Эльф убивает друида
        fight(attacker, defender, true, false);
    }

    void visitElf(Elf &attacker, Elf &defender) override
    {
        // Эльф не убивает эльфа
        fight(attacker, defender, false, false);
    }
};
