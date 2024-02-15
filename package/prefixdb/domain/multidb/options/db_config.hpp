#pragma once

#include <string>
#include <prefixdb/domain/multidb/options/slave_config.hpp>
#include <prefixdb/domain/multidb/options/backup_config.hpp>
#include <prefixdb/domain/multidb/options/archive_config.hpp>
#include <prefixdb/domain/multidb/options/restore_config.hpp>
#include <prefixdb/domain/multidb/options/compact_config.hpp>
#include <prefixdb/domain/multidb/options/initial_config.hpp>

namespace wamba{ namespace prefixdb{

/**
  * @brief Конфигурация
  */
struct db_config
{
  // Путь к базе данных для всех префиксов
  std::string path = "";
  // Путь для WAL, если не указан то по умолчанью из rocksdb
  // если путь указано в ini, то wal_path + "prefix" + ini.wal_dir
  std::string wal_path = "";
  // для "отцепленных" префиксов
  std::string detach_path = "";
  // Файл опций в формате ini
  std::string ini = "";

  // Время жизни полей, после последнего измения для всех префиксов
  uint32_t TTL_seconds = 0;
  std::map<std::string, uint32_t> TTL_prefix;
  size_t packed_limit = 0;
  size_t array_limit  = 0;
  // range.limit + range.offset, для защиты от перебора. Нужно использовать from и to
  size_t range_limit  = 0;

  // Автоматически попытаться востановить базу при ошибке открытия
  bool auto_repair = false;
  // Восстанавливать даже если открывается (только через параметры командной строки)
  bool forced_repair = false;

  // Завершение работы, если база префикса не смогла быть открыта
  bool abort_if_open_error = true;


  /**
    * @brief Запись batch в отдельном потоке
    */
  bool enable_delayed_write = false;

  /**
    * @brief Сначала ответить на запрос (всегда ОК), потом сделать запись
    * @details Только для отладки или временный костыль.
    * Сработает только для запросов с sync=false, nores=false.
    * Может поломать логику клиента
    */
  bool answer_before_write = false;

  // Проверять на валидность параметры JSON в merge - операциях
  // (параметров в WAL идет сыром виде, если там мусор будет ошибка,
  //  которую будет трудно идентифицировать )
  bool check_incoming_merge_json = true;
  // Проверять на корректность JSON возвращаемые значения и, при необходимости,
  // исправлять. Отбрасывать мусор с хвоста, если голова прошла JSON-проверку либо
  // пересериализовывать в строку если там не понятно что
  bool repair_json_values = true;

  compact_config compact;
  slave_config slave;
  initial_config initial_load;
  backup_config backup;
  archive_config archive;
  restore_config restore;

  struct
  {
    std::shared_ptr<wflow::workflow> timers_workflow;
    std::shared_ptr<wflow::workflow> write_workflow;
  } args;
};

}}
