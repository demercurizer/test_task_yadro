#include "ClubSystem.h"
#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

ClubSystem::ClubSystem(int tables, int openT, int closeT, int price)
    : nTables(tables), openTime(openT), closeTime(closeT), pricePerHour(price),
      occupiedMinutes(tables + 1, 0), revenue(tables + 1, 0) {}

void ClubSystem::processInputFile(const std::string &filename) {
  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Cannot open file\n";
    return;
  }

  std::string line;
  // 1) число столов:
  std::getline(in, line);
  nTables = std::stoi(line);
  occupiedMinutes.assign(nTables + 1, 0);
  revenue.assign(nTables + 1, 0);

  // 2) время открытия/закрытия
  std::getline(in, line);
  {
    std::istringstream ss(line);
    std::string o, c;
    ss >> o >> c;
    openTime = parseTime(o);
    closeTime = parseTime(c);
  }
  std::getline(in, line);
  pricePerHour = std::stoi(line);

  // Обработка входящих событий
  while (std::getline(in, line)) {
    if (line.empty())
      continue;
    std::istringstream ss(line);
    std::string tS;
    int id;
    ss >> tS >> id;
    int t = parseTime(tS);
    std::vector<std::string> args;
    std::string a;
    while (ss >> a)
      args.push_back(a);
    handleEvent(t, id, args);
  }

  // --- Генерируем исходящие ID11 перед выводом списка событий ---
  finalizeDay();

  // Вывод
  std::cout << formatTime(openTime) << "\n";
  for (auto &e : eventsOut) {
    std::cout << e << "\n";
  }
  std::cout << formatTime(closeTime) << "\n";

  // Статистика по столам (используем setw/setfill для выравнивания)
  // :contentReference[oaicite:2]{index=2}
  for (int i = 1; i <= nTables; ++i) {
    int mins = occupiedMinutes[i];
    int h = mins / 60, m = mins % 60;
    std::ostringstream dur;
    dur << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2)
        << std::setfill('0') << m;
    std::cout << i << " " << revenue[i] << " " << dur.str() << "\n";
  }
}

void ClubSystem::handleEvent(int time, int id,
                             const std::vector<std::string> &args) {
  // Сначала эхо входящего
  emitIncoming(time, id, args);

  if (id == 1) {
    // Клиент пришел: args[0]=name
    const auto &name = args[0];
    if (inClub.count(name)) {
      emitError(time, "YouShallNotPass");
      return;
    }
    if (time < openTime || time > closeTime) {
      emitError(time, "NotOpenYet");
      return;
    }
    inClub.insert(name);
  } else if (id == 2) {
    // Клиент сел: args[0]=name, args[1]=table
    const auto &name = args[0];
    int table = std::stoi(args[1]);
    if (!inClub.count(name)) {
      emitError(time, "ClientUnknown");
      return;
    }
    if (table < 1 || table > nTables) {
      emitError(time, "PlaceIsBusy");
      return;
    }
    // если сидел — пересадка: освобождаем старое место
    if (clientTable.count(name)) {
      freeSeat(time, name);
    }
    if (clientTable.count(name) == 0 &&
        std::find_if(clientTable.begin(), clientTable.end(), [&](auto &p) {
          return p.second == table;
        }) != clientTable.end()) {
      emitError(time, "PlaceIsBusy");
      return;
    }
    // садим
    seatClient(time, name, table);
  } else if (id == 3) {
    // Ожидание: args[0]=name
    const auto &name = args[0];
    if (!inClub.count(name)) {
      emitError(time, "ClientUnknown");
      return;
    }
    // свободных столов?
    int occupiedCnt = clientTable.size();
    if (occupiedCnt < nTables) {
      emitError(time, "ICanWaitNoLonger!");
      return;
    }
    // очередь полна?
    if ((int)waitQueue.size() >= nTables) {
      // уходит
      waitQueue.push(
          name); // временно добавляем, чтобы всест правильно убрать ниже
      waitQueue.pop();
      inClub.erase(name);
      emitError(time, ""); // эхо пустой входящий, но дальше событие ID11
      // генерируем ID11
      eventsOut.push_back(formatTime(time) + " 11 " + name);
      return;
    }
    waitQueue.push(name);
  } else if (id == 4) {
    // Клиент ушел: args[0]=name
    const auto &name = args[0];
    if (!inClub.count(name)) {
      emitError(time, "ClientUnknown");
      return;
    }
    // если сидел — освобождаем стол и ведём к очереди
    if (clientTable.count(name)) {
      freeSeat(time, name);
      // сел из очереди, если есть
      seatFromQueue(time);
    } else {
      // если был только в очереди — убираем
      std::queue<std::string> tmp;
      while (!waitQueue.empty()) {
        if (waitQueue.front() != name)
          tmp.push(waitQueue.front());
        waitQueue.pop();
      }
      waitQueue = std::move(tmp);
    }
    inClub.erase(name);
  }
  // другие id — не обрабатываем
}

void ClubSystem::emitIncoming(int time, int id,
                              const std::vector<std::string> &args) {
  std::ostringstream ss;
  ss << formatTime(time) << " " << id;
  for (auto &a : args)
    ss << " " << a;
  eventsOut.push_back(ss.str());
}

void ClubSystem::emitError(int time, const std::string &err) {
  if (err.empty())
    return;
  eventsOut.push_back(formatTime(time) + " 13 " + err);
}

void ClubSystem::seatClient(int time, const std::string &client, int table) {
  clientTable[client] = table;
  seatStart[client] = time;
}

void ClubSystem::freeSeat(int time, const std::string &client) {
  int table = clientTable[client];
  int start = seatStart[client];
  int dur = time - start;
  int hrs = ceilHours(dur);
  occupiedMinutes[table] += dur;
  revenue[table] += hrs * pricePerHour;
  clientTable.erase(client);
  seatStart.erase(client);
}

void ClubSystem::seatFromQueue(int time) {
  if (waitQueue.empty())
    return;
  std::string next = waitQueue.front();
  waitQueue.pop();
  // ищем свободный стол (минимальный номер)
  for (int t = 1; t <= nTables; ++t) {
    bool busy = false;
    for (auto &p : clientTable) {
      if (p.second == t) {
        busy = true;
        break;
      }
    }
    if (!busy) {
      // садим его
      clientTable[next] = t;
      seatStart[next] = time;
      eventsOut.push_back(formatTime(time) + " 12 " + next + " " +
                          std::to_string(t));
      return;
    }
  }
}

void ClubSystem::finalizeDay() {
  // перед закрытием: для всех оставшихся сидящих клиентов — считаем оплату до
  // closeTime
  std::vector<std::string> still(inClub.begin(), inClub.end());
  // сначала освободим все сидячие
  for (auto &c : still) {
    if (clientTable.count(c)) {
      freeSeat(closeTime, c);
    }
  }
  // генерируем ID11 для всех оставшихся (и тех, кто сидел, и кто в очереди)
  std::sort(still.begin(), still.end());
  for (auto &c : still) {
    eventsOut.push_back(formatTime(openTime) == ""
                            ? formatTime(closeTime) + " 11 " + c
                            : formatTime(closeTime) + " 11 " + c);
  }
}
