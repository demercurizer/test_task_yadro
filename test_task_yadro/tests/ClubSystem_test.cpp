#include "gtest/gtest.h"
#include "ClubSystem.h"
#include "utils.h"
#include <sstream>
#include <string>
#include <vector>

// Helper: запускает ClubSystem на фикстуре и захватывает stdout
static std::string runFixture(int tables, const std::string &path) {
    std::stringstream buf;
    auto* old = std::cout.rdbuf(buf.rdbuf());
    ClubSystem club{tables, 0, 0, 0};
    club.processInputFile(path);
    std::cout.rdbuf(old);
    return buf.str();
}

// 1) Простая посадка и уход
TEST(ClubSystem, SimpleFlow) {
    std::string out = runFixture(2, "tests/fixtures/simple.txt");
    EXPECT_NE(out.find("09:00 1 alice"), std::string::npos);
    EXPECT_NE(out.find("09:10 2 alice 1"), std::string::npos);
    EXPECT_NE(out.find("10:00 11 alice"), std::string::npos);
}

// 2) Попытка входа до открытия
TEST(ClubSystem, BeforeOpen) {
    std::string out = runFixture(1, "tests/fixtures/before_open.txt");
    EXPECT_NE(out.find("09:00 13 NotOpenYet"), std::string::npos);
}

// 3) PlaceIsBusy при занятом столе
TEST(ClubSystem, PlaceIsBusy) {
    std::string out = runFixture(1, "tests/fixtures/busy.txt");
    EXPECT_NE(out.find("08:15 13 PlaceIsBusy"), std::string::npos);
}


// 5) Авто-рассадка из очереди
TEST(ClubSystem, AutoSeatFromQueue) {
    std::string out = runFixture(1, "tests/fixtures/auto_seat.txt");
    EXPECT_NE(out.find("08:20 12 y 1"), std::string::npos);
}

// 6) Проверка округления оплаты
TEST(ClubSystem, BillingCeil) {
    std::string out = runFixture(1, "tests/fixtures/billing.txt");
    EXPECT_NE(out.find("1 20 01:30"), std::string::npos);
}

// 7) Выход всех к концу дня
TEST(ClubSystem, EndOfDayLogout) {
    std::string out = runFixture(2, "tests/fixtures/end_day.txt");
    EXPECT_NE(out.find("17:00 11 u1"), std::string::npos);
    EXPECT_NE(out.find("17:00 11 u2"), std::string::npos);
}

// 8) Полный пример из задания
TEST(ClubSystem, FullExample) {
    std::string out = runFixture(3, "tests/fixtures/example.txt");
    std::vector<std::string> expected = {
        "09:00",
        "08:48 1 client1",
        "08:48 13 NotOpenYet",
        "09:41 1 client1",
        "09:48 1 client2",
        "09:52 3 client1",
        "09:52 13 ICanWaitNoLonger!",
        "09:54 2 client1 1",
        "10:25 2 client2 2",
        "10:58 1 client3",
        "10:59 2 client3 3",
        "11:30 1 client4",
        "11:35 2 client4 2",
        "11:35 13 PlaceIsBusy",
        "11:45 3 client4",
        "12:33 4 client1",
        "12:33 12 client4 1",
        "12:43 4 client2",
        "15:52 4 client4",
        "19:00 11 client3",
        "19:00",
        "1 70 05:58",
        "2 30 02:18",
        "3 90 08:01"
    };
    size_t pos = 0;
    for (auto &line : expected) {
        pos = out.find(line, pos);
        EXPECT_NE(pos, std::string::npos) << "Line not found: " << line;
    }
}

// 9) Дублирующий приход -> YouShallNotPass
TEST(ClubSystem, DuplicateArrival) {
    std::string out = runFixture(1, "tests/fixtures/dup_arrival.txt");
    EXPECT_NE(out.find("09:06 13 YouShallNotPass"), std::string::npos);
}

// 10) Садится без прихода -> ClientUnknown
TEST(ClubSystem, SeatWithoutArrival) {
    std::string out = runFixture(1, "tests/fixtures/seat_unknown.txt");
    EXPECT_NE(out.find("08:10 13 ClientUnknown"), std::string::npos);
}

// 11) ICanWaitNoLonger когда свободен стол
TEST(ClubSystem, CannotWaitWhenFree) {
    std::string out = runFixture(2, "tests/fixtures/wait_with_free.txt");
    EXPECT_NE(out.find("09:05 13 ICanWaitNoLonger!"), std::string::npos);
}

// 12) Авто-рассадка после ухода из очереди
TEST(ClubSystem, AutoSeatAfterLeave) {
    std::string out = runFixture(1, "tests/fixtures/auto_seat_after_leave.txt");
    EXPECT_NE(out.find("08:30 12 carol 1"), std::string::npos);
}

// 13) Приход ровно в момент открытия
TEST(ClubSystem, ArrivalAtOpening) {
    std::string out = runFixture(1, "tests/fixtures/at_open.txt");
    EXPECT_NE(out.find("09:00 1 alice"), std::string::npos);
}

// 14) Минимальная сессия (<1 мин) -> округление до часа
TEST(ClubSystem, MinimumCharge) {
    std::string out = runFixture(1, "tests/fixtures/minute_session.txt");
    EXPECT_NE(out.find("1 10 00:01"), std::string::npos);
}

// 15) Точная двухчасовая сессия
TEST(ClubSystem, ExactTwoHours) {
    std::string out = runFixture(1, "tests/fixtures/two_hour_session.txt");
    EXPECT_NE(out.find("1 20 02:00"), std::string::npos);
}
