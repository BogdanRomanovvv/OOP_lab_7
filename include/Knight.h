#pragma once
#include "NPC.h"
#include "Visitor.h"

class Knight : public NPC
{
public:
    Knight(const std::string &name, double x, double y)
        : NPC(name, x, y, 100, 30) {}

    std::string getType() const override
    {
        return "Knight";
    }

    int getMoveRange() const override { return 30; }
    int getKillRange() const override { return 10; }

    void accept(Visitor &visitor, NPC &other) override;
};
