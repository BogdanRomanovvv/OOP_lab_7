#include "Druid.h"
#include "Knight.h"
#include "Elf.h"

void Druid::accept(Visitor &visitor, NPC &other)
{
    if (auto *knight = dynamic_cast<Knight *>(&other))
    {
        visitor.visitDruid(*this, *knight);
    }
    else if (auto *druid = dynamic_cast<Druid *>(&other))
    {
        visitor.visitDruid(*this, *druid);
    }
    else if (auto *elf = dynamic_cast<Elf *>(&other))
    {
        visitor.visitDruid(*this, *elf);
    }
}
