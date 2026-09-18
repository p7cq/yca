#pragma once

// A SQLite wrapper for yca's own index tables (ca_cert_index, cert_index,
// config/purpose tables). yca used to route this through Botan::SQL_Database,
// but that interface has been reshaped across Botan releases (see
// tools/update-botan.sh history); talking to SQLite directly removes that
// coupling for yca's own queries.
//
// Database still implements Botan::SQL_Database, because
// Botan::Certificate_Store_In_SQL (the CA cert/key storage in ca.cpp)
// requires that interface in its constructor, and it must share yca's own
// connection - not a second one - so that its writes fall inside the same
// begin_write/commit_write transaction as everything else. Only
// Database::new_statement() (store.cpp) serves that interface; the rest of
// yca's code uses stmt()/create_table() below, so a future change to
// Botan's SQL_Database shape only touches store.cpp.

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <botan/botan_all.h>

struct sqlite3;
struct sqlite3_stmt;

namespace store {

// Thrown on any SQLite error (open, prepare, bind, exec).
class Error : public std::runtime_error {
public:
  explicit Error(const std::string &what) : std::runtime_error(what) {}
};

// yca's own statement type, independent of Botan's SQL_Database ABC.
class Statement {
public:
  Statement(sqlite3 *db, std::string_view sql);
  ~Statement();

  Statement(const Statement &) = delete;
  Statement &operator=(const Statement &) = delete;

  void bind(int column, std::string_view val);
  void bind(int column, std::size_t val);

  std::string get_str(int column);
  std::size_t get_size_t(int column);
  std::pair<const uint8_t *, std::size_t> get_blob(int column);

  // Advances to the next row; true while one is available.
  bool step();
  // Runs to completion (for statements with no rows to read); returns the
  // number of rows stepped over.
  std::size_t spin();

private:
  sqlite3_stmt *m_stmt;
};

class Database : public Botan::SQL_Database {
public:
  explicit Database(std::string_view path);
  ~Database() override;

  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  // Qualified as store::Statement, not Statement: Database inherits from
  // Botan::SQL_Database, whose own nested Statement type would otherwise
  // shadow ours in this class's scope.
  std::shared_ptr<store::Statement> stmt(std::string_view sql) const;

  // Botan::SQL_Database overrides - for Certificate_Store_In_SQL only, see
  // above. Not called anywhere else in yca.
  void create_table(std::string_view schema) override;
  std::shared_ptr<Botan::SQL_Database::Statement>
  new_statement(std::string_view sql) const override;
  std::size_t row_count(std::string_view table_name) override;
  std::size_t rows_changed_by_last_statement() override;

private:
  sqlite3 *m_db;
};

} // namespace store
