#pragma once
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

class ClubSystem {
public:
  ClubSystem(int tables, int openT, int closeT, int price);
  void processInputFile(const std::string &filename);

private:
  int nTables;
  int openTime;
  int closeTime;
  int pricePerHour;

  // Состояние
  std::set<std::string> inClub; // все клиенты, которые в клубе
  std::map<std::string, int>
      seatStart; // время начала текущей сессии по клиенту
  std::map<std::string, int> clientTable; // за каким столом сидит клиент
  std::queue<std::string> waitQueue; // очередь ожидания

  // Статистика по столам
  std::vector<int> occupiedMinutes; // суммарное занятое время (в мин) по столу
  std::vector<int> revenue; // выручка (в у.е.) по столу

  // Выходные события
  std::vector<std::string> eventsOut;

  // Обработка
  void handleEvent(int time, int id, const std::vector<std::string> &args);
  void emitIncoming(int time, int id, const std::vector<std::string> &args);
  void emitError(int time, const std::string &err);
  void seatClient(int time, const std::string &client, int table);
  void freeSeat(int time, const std::string &client);
  void seatFromQueue(int time);
  void finalizeDay();
};
