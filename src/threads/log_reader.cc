#include "log_reader.h"
#include "log_record.h"

#include <iostream>
#include <json/json.h>
#include <memory>
#include <sstream>

LogReader::LogReader(const std::initializer_list<std::string> &log_sources)
{
  for (const std::string &source : log_sources) {
    log_files.emplace_front(source, std::ifstream::in);
  }
}

std::list<std::shared_ptr<LogRecord>> LogReader::read_logs()
{
  std::list<std::shared_ptr<LogRecord>> logs;

  std::string log_data;
  for (std::ifstream &log_file : log_files) {
    while (log_file.is_open() && std::getline(log_file, log_data)) {
      auto log = std::make_shared<LogRecord>(log_data);

      if (log->valid()) {
        logs.push_back(log);
      }
    }
  }

  return logs;
}

Json::Value parse_JSON(const std::string &raw_json)
{
  std::stringstream stream;
  stream << raw_json;

  Json::Value root;
  Json::CharReaderBuilder builder;
  JSONCPP_STRING errs;

  if (!parseFromStream(builder, stream, &root, &errs)) {
    std::cerr << errs << std::endl << "journalctl log not in valid JSON format: " << raw_json << std::endl;
  }

  return root;
}

std::list<std::shared_ptr<LogRecord>> LogReader::parse_journalctl_logs(const std::string &log_data)
{
  std::list<std::shared_ptr<LogRecord>> logs;
  std::stringstream log_stream(log_data);

  std::string log_line;
  while (std::getline(log_stream, log_line)) {
    Json::Value log_json = parse_JSON(log_line);
    if (log_json.empty()) {
      continue;
    }

    time_t timestamp = std::stol(log_json["__REALTIME_TIMESTAMP"].asString()) / (1000000);
    std::string msg  = log_json["MESSAGE"].asString();

    auto log = std::make_shared<LogRecord>(timestamp, msg);
    if (log->valid()) {
      logs.push_back(log);
    }
  }

  return logs;
}
