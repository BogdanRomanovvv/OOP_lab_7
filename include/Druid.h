#pragma once
#include "NPC.h"
#include "Visitor.h"

class Druid : public NPC
{
public:
    Druid(const std::string &name, double x, double y)
        : NPC(name, x, y, 80, 25) {}

    std::string getType() const override
    {
        return "Druid";
    }

    int getMoveRange() const override { return 10; }
    int getKillRange() const override { return 10; }

    void accept(Visitor &visitor, NPC &other) override;
};
