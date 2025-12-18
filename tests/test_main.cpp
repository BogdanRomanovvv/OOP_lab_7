#include <gtest/gtest.h>
#include "../include/Knight.h"
#include "../include/Druid.h"
#include "../include/Elf.h"
#include "../include/NPCFactory.h"
#include "../include/BattleVisitor.h"
#include "../include/Observer.h"
#include "../include/DungeonEditor.h"
#include "../include/BattleQueue.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>

static std::function<int()> makeFixedRoller(std::vector<int> rolls)
{
    // Возвращает значения по очереди: r0, r1, r2...
    // Каждый бой потребляет 2 значения (атака, защита).
    auto state = std::make_shared<std::pair<std::vector<int>, size_t>>(std::move(rolls), 0);
    return [state]() mutable -> int
    {
        if (state->second >= state->first.size())
        {
            // Если тест не задал достаточно бросков — вернем нейтральное значение.
            return 3;
        }
        return state->first[state->second++];
    };
}

// Тесты создания NPC через Factory
class NPCFactoryTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(NPCFactoryTest, CreateKnight)
{
    auto knight = NPCFactory::createNPC("Knight", "TestKnight", 100, 100);
    ASSERT_NE(knight, nullptr);
    EXPECT_EQ(knight->getType(), "Knight");
    EXPECT_EQ(knight->getName(), "TestKnight");
    EXPECT_EQ(knight->getX(), 100);
    EXPECT_EQ(knight->getY(), 100);
    EXPECT_TRUE(knight->isAlive());
}

TEST_F(NPCFactoryTest, CreateDruid)
{
    auto druid = NPCFactory::createNPC("Druid", "TestDruid", 200, 200);
    ASSERT_NE(druid, nullptr);
    EXPECT_EQ(druid->getType(), "Druid");
    EXPECT_EQ(druid->getName(), "TestDruid");
    EXPECT_EQ(druid->getX(), 200);
    EXPECT_EQ(druid->getY(), 200);
    EXPECT_TRUE(druid->isAlive());
}

TEST_F(NPCFactoryTest, CreateElf)
{
    auto elf = NPCFactory::createNPC("Elf", "TestElf", 300, 300);
    ASSERT_NE(elf, nullptr);
    EXPECT_EQ(elf->getType(), "Elf");
    EXPECT_EQ(elf->getName(), "TestElf");
    EXPECT_EQ(elf->getX(), 300);
    EXPECT_EQ(elf->getY(), 300);
    EXPECT_TRUE(elf->isAlive());
}

TEST_F(NPCFactoryTest, CreateInvalidType)
{
    auto npc = NPCFactory::createNPC("InvalidType", "Test", 100, 100);
    EXPECT_EQ(npc, nullptr);
}

TEST_F(NPCFactoryTest, CreateFromString)
{
    std::string line = "Knight Arthur 150 250";
    auto npc = NPCFactory::createFromString(line);
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), "Knight");
    EXPECT_EQ(npc->getName(), "Arthur");
    EXPECT_EQ(npc->getX(), 150);
    EXPECT_EQ(npc->getY(), 250);
}

TEST_F(NPCFactoryTest, CreateFromInvalidString)
{
    std::string line = "InvalidLine";
    auto npc = NPCFactory::createFromString(line);
    EXPECT_EQ(npc, nullptr);
}

// Тесты расстояния между NPC
class NPCDistanceTest : public ::testing::Test
{
protected:
    std::shared_ptr<NPC> npc1;
    std::shared_ptr<NPC> npc2;

    void SetUp() override
    {
        npc1 = NPCFactory::createNPC("Knight", "Knight1", 0, 0);
        npc2 = NPCFactory::createNPC("Druid", "Druid1", 3, 4);
    }
};

TEST_F(NPCDistanceTest, CalculateDistance)
{
    double distance = npc1->distanceTo(*npc2);
    EXPECT_DOUBLE_EQ(distance, 5.0);
}

TEST_F(NPCDistanceTest, DistanceToSelf)
{
    double distance = npc1->distanceTo(*npc1);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(NPCDistanceTest, LargeDistance)
{
    auto npc3 = NPCFactory::createNPC("Elf", "Elf1", 500, 500);
    double distance = npc1->distanceTo(*npc3);
    EXPECT_NEAR(distance, 707.1067, 0.001);
}

// Тесты боевой системы
class BattleVisitorTest : public ::testing::Test
{
protected:
    Subject subject;
    // visitor создаём в каждом тесте с нужной последовательностью бросков
};

TEST_F(BattleVisitorTest, KnightVsElf_AttackerKillsDefender_WhenAttackGreater)
{
    auto knight = std::make_shared<Knight>("Knight1", 100, 100);
    auto elf = std::make_shared<Elf>("Elf1", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1})); // атака > защита

    knight->accept(visitor, *elf);

    EXPECT_TRUE(knight->isAlive());
    EXPECT_FALSE(elf->isAlive());
}

TEST_F(BattleVisitorTest, KnightVsElf_DefenderKillsAttacker_WhenDefenseGreater)
{
    auto knight = std::make_shared<Knight>("Knight1", 100, 100);
    auto elf = std::make_shared<Elf>("Elf1", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({1, 6})); // защита > атака

    knight->accept(visitor, *elf);

    EXPECT_FALSE(knight->isAlive());
    EXPECT_TRUE(elf->isAlive());
}

TEST_F(BattleVisitorTest, ElfVsDruid_DruidDies_WhenAttackGreater)
{
    auto elf = std::make_shared<Elf>("Elf1", 100, 100);
    auto druid = std::make_shared<Druid>("Druid1", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1})); // атака > защита

    elf->accept(visitor, *druid);

    EXPECT_TRUE(elf->isAlive());
    EXPECT_FALSE(druid->isAlive());
}

TEST_F(BattleVisitorTest, ElfVsDruid_NoDeath_WhenAttackNotGreater)
{
    auto elf = std::make_shared<Elf>("Elf1", 100, 100);
    auto druid = std::make_shared<Druid>("Druid1", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({1, 6})); // атака <= защита

    elf->accept(visitor, *druid);

    EXPECT_TRUE(elf->isAlive());
    EXPECT_TRUE(druid->isAlive());
}

TEST_F(BattleVisitorTest, DruidVsDruid_AttackerKillsDefender_WhenAttackGreater)
{
    auto druid1 = std::make_shared<Druid>("Druid1", 100, 100);
    auto druid2 = std::make_shared<Druid>("Druid2", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1}));

    druid1->accept(visitor, *druid2);

    EXPECT_TRUE(druid1->isAlive());
    EXPECT_FALSE(druid2->isAlive());
}

TEST_F(BattleVisitorTest, KnightVsKnight_NoDeath)
{
    auto knight1 = std::make_shared<Knight>("Knight1", 100, 100);
    auto knight2 = std::make_shared<Knight>("Knight2", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1}));

    knight1->accept(visitor, *knight2);

    EXPECT_TRUE(knight1->isAlive());
    EXPECT_TRUE(knight2->isAlive());
}

TEST_F(BattleVisitorTest, ElfVsElf_NoDeath)
{
    auto elf1 = std::make_shared<Elf>("Elf1", 100, 100);
    auto elf2 = std::make_shared<Elf>("Elf2", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1}));

    elf1->accept(visitor, *elf2);

    EXPECT_TRUE(elf1->isAlive());
    EXPECT_TRUE(elf2->isAlive());
}

TEST_F(BattleVisitorTest, KnightVsDruid_NoDeath)
{
    auto knight = std::make_shared<Knight>("Knight1", 100, 100);
    auto druid = std::make_shared<Druid>("Druid1", 110, 110);

    BattleVisitor visitor(subject, makeFixedRoller({6, 1}));

    knight->accept(visitor, *druid);

    EXPECT_TRUE(knight->isAlive());
    EXPECT_TRUE(druid->isAlive());
}

// Тесты Observer
class ObserverTest : public ::testing::Test
{
protected:
    Subject subject;
    std::string testLogFile = "test_log.txt";

    void TearDown() override
    {
        std::remove(testLogFile.c_str());
    }
};

TEST_F(ObserverTest, FileObserverWritesToFile)
{
    auto fileObserver = std::make_shared<FileObserver>(testLogFile);
    subject.attach(fileObserver);

    subject.notify("Knight", "Elf");

    std::ifstream file(testLogFile);
    ASSERT_TRUE(file.is_open());
    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "Knight убил(а) Elf");
    file.close();
}

TEST_F(ObserverTest, MultipleObservers)
{
    auto fileObserver = std::make_shared<FileObserver>(testLogFile);
    auto consoleObserver = std::make_shared<ConsoleObserver>();

    subject.attach(fileObserver);
    subject.attach(consoleObserver);

    subject.notify("Druid", "Druid");

    std::ifstream file(testLogFile);
    ASSERT_TRUE(file.is_open());
    std::string line;
    std::getline(file, line);
    EXPECT_EQ(line, "Druid убил(а) Druid");
    file.close();
}

// Тесты DungeonEditor
class DungeonEditorTest : public ::testing::Test
{
protected:
    DungeonEditor editor;
    std::string testFile = "test_dungeon.txt";

    void TearDown() override
    {
        std::remove(testFile.c_str());
        std::remove("log.txt");
    }
};

TEST_F(DungeonEditorTest, AddNPC_ValidCoordinates)
{
    EXPECT_TRUE(editor.addNPC("Knight", "TestKnight", 100, 100));
    EXPECT_EQ(editor.getNPCCount(), 1);
}

TEST_F(DungeonEditorTest, AddNPC_InvalidCoordinates_TooLarge)
{
    EXPECT_FALSE(editor.addNPC("Knight", "TestKnight", 600, 100));
    EXPECT_EQ(editor.getNPCCount(), 0);
}

TEST_F(DungeonEditorTest, AddNPC_InvalidCoordinates_Negative)
{
    EXPECT_FALSE(editor.addNPC("Knight", "TestKnight", -10, 100));
    EXPECT_EQ(editor.getNPCCount(), 0);
}

TEST_F(DungeonEditorTest, AddNPC_DuplicateName)
{
    EXPECT_TRUE(editor.addNPC("Knight", "TestKnight", 100, 100));
    EXPECT_FALSE(editor.addNPC("Druid", "TestKnight", 200, 200));
    EXPECT_EQ(editor.getNPCCount(), 1);
}

TEST_F(DungeonEditorTest, AddNPC_BoundaryCoordinates)
{
    EXPECT_TRUE(editor.addNPC("Knight", "K1", 0, 0));
    EXPECT_TRUE(editor.addNPC("Druid", "D1", 500, 500));
    EXPECT_EQ(editor.getNPCCount(), 2);
}

TEST_F(DungeonEditorTest, SaveAndLoadFromFile)
{
    editor.addNPC("Knight", "Arthur", 100, 100);
    editor.addNPC("Druid", "Merlin", 200, 200);
    editor.addNPC("Elf", "Legolas", 300, 300);

    EXPECT_TRUE(editor.saveToFile(testFile));

    DungeonEditor editor2;
    EXPECT_TRUE(editor2.loadFromFile(testFile));
    EXPECT_EQ(editor2.getNPCCount(), 3);
}

TEST_F(DungeonEditorTest, LoadFromNonExistentFile)
{
    EXPECT_FALSE(editor.loadFromFile("nonexistent.txt"));
}

TEST_F(DungeonEditorTest, BattleMode_NoNPCsInRange)
{
    editor.addNPC("Knight", "K1", 0, 0);
    editor.addNPC("Druid", "D1", 500, 500);

    editor.startBattle(100);

    // Оба должны выжить, так как далеко друг от друга
    EXPECT_EQ(editor.getNPCCount(), 2);
}

TEST_F(DungeonEditorTest, BattleMode_NPCsInRange)
{
    editor.addNPC("Knight", "K1", 100, 100);
    editor.addNPC("Elf", "E1", 110, 110);

    Subject subj;
    // Задаем атаку > защиту, чтобы K1 гарантированно убил E1
    BattleVisitor visitor(subj, makeFixedRoller({6, 1}));
    editor.startBattle(50, visitor);

    // Должен погибнуть только E1
    EXPECT_EQ(editor.getNPCCount(), 1);
}

TEST_F(DungeonEditorTest, BattleMode_ComplexScenario)
{
    editor.addNPC("Knight", "K1", 100, 100);
    editor.addNPC("Elf", "E1", 110, 110);
    editor.addNPC("Druid", "D1", 120, 120);
    editor.addNPC("Knight", "K2", 400, 400);

    Subject subj;
    // Пары обрабатываются i<j и один раз:
    // K1 vs E1: атака > защита => E1 умирает
    // K1 vs D1: никто никого
    // E1 уже мертв => дальше пропускается
    // K2 далеко
    BattleVisitor visitor(subj, makeFixedRoller({6, 1}));
    editor.startBattle(50, visitor);

    EXPECT_EQ(editor.getNPCCount(), 3); // K1, D1, K2
}

// Тесты характеристик ЛР7 (ход/убийство)
TEST(NPCRangesTest, MoveAndKillRanges)
{
    Knight k("K", 0, 0);
    Druid d("D", 0, 0);
    Elf e("E", 0, 0);

    EXPECT_EQ(k.getMoveRange(), 30);
    EXPECT_EQ(k.getKillRange(), 10);
    EXPECT_EQ(d.getMoveRange(), 10);
    EXPECT_EQ(d.getKillRange(), 10);
    EXPECT_EQ(e.getMoveRange(), 10);
    EXPECT_EQ(e.getKillRange(), 50);
}

TEST(NPCMoveTest, ClampsToMapBounds)
{
    Knight k("K", 50, 50);
    k.move(1000, 1000, 100, 100);
    EXPECT_DOUBLE_EQ(k.getX(), 100);
    EXPECT_DOUBLE_EQ(k.getY(), 100);

    k.move(-1000, -1000, 100, 100);
    EXPECT_DOUBLE_EQ(k.getX(), 0);
    EXPECT_DOUBLE_EQ(k.getY(), 0);
}

// Тесты BattleQueue
TEST(BattleQueueTest, PushPopOrder)
{
    BattleQueue q;
    auto a = NPCFactory::createNPC("Knight", "A", 0, 0);
    auto b = NPCFactory::createNPC("Druid", "B", 0, 0);
    auto c = NPCFactory::createNPC("Elf", "C", 0, 0);

    q.push(BattleTask(a, b));
    q.push(BattleTask(b, c));

    BattleTask t1(nullptr, nullptr);
    ASSERT_TRUE(q.pop(t1));
    EXPECT_EQ(t1.attacker->getName(), "A");
    EXPECT_EQ(t1.defender->getName(), "B");

    BattleTask t2(nullptr, nullptr);
    ASSERT_TRUE(q.pop(t2));
    EXPECT_EQ(t2.attacker->getName(), "B");
    EXPECT_EQ(t2.defender->getName(), "C");
}

TEST(BattleQueueTest, StopUnblocksAndPopReturnsFalseWhenEmpty)
{
    BattleQueue q;
    q.stop();
    BattleTask t(nullptr, nullptr);
    EXPECT_FALSE(q.pop(t));
}

// Тесты сериализации
class SerializationTest : public ::testing::Test
{
};

TEST_F(SerializationTest, NPCSerialization)
{
    auto knight = NPCFactory::createNPC("Knight", "Arthur", 123.5, 456.7);
    std::string serialized = knight->serialize();
    EXPECT_EQ(serialized, "Knight Arthur 123.500000 456.700000");
}

TEST_F(SerializationTest, SerializeAndDeserialize)
{
    auto original = NPCFactory::createNPC("Druid", "Merlin", 200, 300);
    std::string serialized = original->serialize();

    auto deserialized = NPCFactory::createFromString(serialized);

    ASSERT_NE(deserialized, nullptr);
    EXPECT_EQ(deserialized->getType(), original->getType());
    EXPECT_EQ(deserialized->getName(), original->getName());
    EXPECT_DOUBLE_EQ(deserialized->getX(), original->getX());
    EXPECT_DOUBLE_EQ(deserialized->getY(), original->getY());
}

// Главная функция для запуска тестов
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
