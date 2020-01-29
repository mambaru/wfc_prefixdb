#pragma once

namespace wamba { namespace prefixdb {

/**
 * @brief Общий результат выполнения операции
 * TODO: сделать отсылки в конфиг
 */
enum class common_status
{
  OK,                 ///< Успех  
  InvalidFieldValue,  ///< Недопустимое значение для merge-операций  
  EmptyFields,        ///< Пустой список полей в запросе 
  EmptyPrefix,        ///< Не задан префикс в запросе
  CreatePrefixFail,   ///< Ошибка создания нового префикса
  CreateSnapshotFail, ///< Ошибка создания снапшота
  CompactFail,        ///< Ошибка compactoins
  PrefixNotFound,     ///< Префикс не найден
  SnapshotNotFound,   ///< Не найден снапшот
  TooManyKeys,        ///< Слишком много ключей в запросе 
  KeyLengthExceeded,  ///< Превышен лимит символов имени в одном из ключей 
  ValueLengthExceeded, ///< Превышен лимит размера в одном из значений 
  PrefixLengthExceeded, ///< Превышен лимит символов имени префикса
  RangeLimitExceeded,   ///< Превышен лимит параметров range
  InvalidSeqNumber      ///< Ошибка репликации
};

}}
