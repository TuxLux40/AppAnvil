#ifndef SRC_THREADS_LOG_READER_H
#define SRC_THREADS_LOG_READER_H

#include <forward_list>
#include <fstream>
#include <memory>
#include <string>

#include "log_record.h"

class LogReader
{
public:
  LogReader(const std::initializer_list<std::string> &log_sources = { "/var/log/kern.log", "/var/log/dmesg", "/var/log/syslog" });

  std::list<std::shared_ptr<LogRecord>> read_logs();

  /**
   * @brief Parses journalctl logs into a list of LogRecords
   * 
   * @param log_data 
   * @return std::list<std::shared_ptr<LogRecord>> 
   */
  static std::list<std::shared_ptr<LogRecord>> parse_journalctl_logs(const std::string &log_data);

private:
  std::forward_list<std::ifstream> log_files;
  std::string checkpoint_filepath;
};

#endif