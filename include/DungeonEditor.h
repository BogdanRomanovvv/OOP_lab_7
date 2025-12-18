#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include "NPC.h"
#include "NPCFactory.h"
#include "BattleVisitor.h"
#include "Observer.h"

class DungeonEditor
{
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    Subject subject;

    void startBattleImpl(double range, BattleVisitor &battleVisitor)
    {
        std::cout << "\n=== НАЧАЛО БОЕВОГО РЕЖИМА ===" << std::endl;
        std::cout << "Дальность боя: " << range << " метров\n"
                  << std::endl;

        bool hadBattle = false;

        // Проходим по всем парам NPC
        for (size_t i = 0; i < npcs.size(); ++i)
        {
            for (size_t j = i + 1; j < npcs.size(); ++j)
            {
                if (!npcs[i]->isAlive() || !npcs[j]->isAlive())
                {
                    continue;
                }

                double distance = npcs[i]->distanceTo(*npcs[j]);
                if (distance <= range)
                {
                    hadBattle = true;
                    // Используем паттерн Visitor для боя
                    npcs[i]->accept(battleVisitor, *npcs[j]);
                }
            }
        }

        // Удаляем мёртвых NPC
        npcs.erase(
            std::remove_if(npcs.begin(), npcs.end(),
                           [](const std::shared_ptr<NPC> &npc)
                           { return !npc->isAlive(); }),
            npcs.end());

        if (!hadBattle)
        {
            std::cout << "Не было боёв в указанной дальности." << std::endl;
        }

        std::cout << "\n=== КОНЕЦ БОЕВОГО РЕЖИМА ===" << std::endl;
        std::cout << "Выживших NPC: " << npcs.size() << "\n"
                  << std::endl;
    }

public:
    DungeonEditor()
    {
        // Добавляем наблюдателей
        subject.attach(std::make_shared<ConsoleObserver>());
        subject.attach(std::make_shared<FileObserver>("log.txt"));
    }

    // Добавление NPC
    bool addNPC(const std::string &type, const std::string &name, double x, double y)
    {
        // Проверка координат
        if (x < 0 || x > 500 || y < 0 || y > 500)
        {
            return false;
        }

        // Проверка уникальности имени
        for (const auto &npc : npcs)
        {
            if (npc->getName() == name)
            {
                return false;
            }
        }

        auto npc = NPCFactory::createNPC(type, name, x, y);
        if (npc)
        {
            npcs.push_back(npc);
            return true;
        }
        return false;
    }

    // Сохранение в файл
    bool saveToFile(const std::string &filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        for (const auto &npc : npcs)
        {
            if (npc->isAlive())
            {
                file << npc->serialize() << std::endl;
            }
        }

        file.close();
        return true;
    }

    // Загрузка из файла
    bool loadFromFile(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        npcs.clear();
        std::string line;
        while (std::getline(file, line))
        {
            auto npc = NPCFactory::createFromString(line);
            if (npc)
            {
                npcs.push_back(npc);
            }
        }

        file.close();
        return true;
    }

    // Печать перечня объектов
    void printNPCs() const
    {
        if (npcs.empty())
        {
            std::cout << "Нет NPC на карте." << std::endl;
            return;
        }

        std::cout << "\n=== Список NPC ===" << std::endl;
        std::cout << "Всего NPC: " << npcs.size() << std::endl;
        for (const auto &npc : npcs)
        {
            if (npc->isAlive())
            {
                std::cout << "- " << npc->getType()
                          << " \"" << npc->getName() << "\" "
                          << "в позиции (" << npc->getX() << ", " << npc->getY() << ")"
                          << " [HP: " << npc->getHealth() << "]"
                          << std::endl;
            }
        }
        std::cout << "==================\n"
                  << std::endl;
    }

    // Боевой режим
    void startBattle(double range)
    {
        BattleVisitor battleVisitor(subject);
        startBattleImpl(range, battleVisitor);
    }

    // Перегрузка для тестов/экспериментов: внешний Visitor
    void startBattle(double range, BattleVisitor &battleVisitor)
    {
        startBattleImpl(range, battleVisitor);
    }

    size_t getNPCCount() const
    {
        return npcs.size();
    }
};
