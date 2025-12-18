#pragma once
#include "NPC.h"
#include "Visitor.h"

class Elf : public NPC
{
public:
    Elf(const std::string &name, double x, double y)
        : NPC(name, x, y, 70, 35) {}

    std::string getType() const override
    {
        return "Elf";
    }

    int getMoveRange() const override { return 10; }
    int getKillRange() const override { return 50; }

    void accept(Visitor &visitor, NPC &other) override;
};
