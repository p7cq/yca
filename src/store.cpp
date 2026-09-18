#include "store.h"

#include <chrono>
#include <optional>
#include <span>
#include <vector>

#include <sqlite3.h>

namespace store {

Statement::Statement(sqlite3 *db, std::string_view sql) : m_stmt(nullptr) {
  const int rc = ::sqlite3_prepare_v2(
      db, sql.data(), static_cast<int>(sql.size()), &m_stmt, nullptr);
  if (rc != SQLITE_OK)
    throw Error("sqlite3_prepare failed: " + std::string(::sqlite3_errmsg(db)));
}

Statement::~Statement() { ::sqlite3_finalize(m_stmt); }

void Statement::bind(int column, std::string_view val) {
  const int rc = ::sqlite3_bind_text64(m_stmt, column, val.data(), val.size(),
                                       SQLITE_TRANSIENT, SQLITE_UTF8);
  if (rc != SQLITE_OK)
    throw Error("sqlite3_bind_text failed");
}

void Statement::bind(int column, std::size_t val) {
  const int rc =
      ::sqlite3_bind_int64(m_stmt, column, static_cast<sqlite3_int64>(val));
  if (rc != SQLITE_OK)
    throw Error("sqlite3_bind_int failed");
}

std::string Statement::get_str(int column) {
  if (::sqlite3_column_type(m_stmt, column) != SQLITE_TEXT)
    throw Error("get_str: column is not TEXT");
  const auto *text = ::sqlite3_column_text(m_stmt, column);
  return std::string(reinterpret_cast<const char *>(text));
}

std::size_t Statement::get_size_t(int column) {
  if (::sqlite3_column_type(m_stmt, column) != SQLITE_INTEGER)
    throw Error("get_size_t: column is not INTEGER");
  return static_cast<std::size_t>(::sqlite3_column_int64(m_stmt, column));
}

std::pair<const uint8_t *, std::size_t> Statement::get_blob(int column) {
  if (::sqlite3_column_type(m_stmt, column) == SQLITE_NULL)
    return {nullptr, 0};
  const void *blob = ::sqlite3_column_blob(m_stmt, column);
  const int size = ::sqlite3_column_bytes(m_stmt, column);
  return {static_cast<const uint8_t *>(blob), static_cast<std::size_t>(size)};
}

bool Statement::step() { return ::sqlite3_step(m_stmt) == SQLITE_ROW; }

std::size_t Statement::spin() {
  std::size_t steps = 0;
  while (step())
    ++steps;
  return steps;
}

namespace {

// Implements Botan::SQL_Database::Statement, for Database::new_statement()
// (Botan::Certificate_Store_In_SQL's own queries) only - yca's own code
// never sees this type, it uses store::Statement above.
class BotanStatement final : public Botan::SQL_Database::Statement {
public:
  BotanStatement(sqlite3 *db, std::string_view sql) : m_stmt(nullptr) {
    const int rc = ::sqlite3_prepare_v2(
        db, sql.data(), static_cast<int>(sql.size()), &m_stmt, nullptr);
    if (rc != SQLITE_OK)
      throw Error("sqlite3_prepare failed: " +
                  std::string(::sqlite3_errmsg(db)));
  }

  ~BotanStatement() override { ::sqlite3_finalize(m_stmt); }

  BotanStatement(const BotanStatement &) = delete;
  BotanStatement &operator=(const BotanStatement &) = delete;

  void bind(int column, std::string_view val) override {
    if (val.data() == nullptr) {
      bind_null(column);
      return;
    }
    const int rc = ::sqlite3_bind_text64(m_stmt, column, val.data(), val.size(),
                                         SQLITE_TRANSIENT, SQLITE_UTF8);
    if (rc != SQLITE_OK)
      throw Error("sqlite3_bind_text failed");
  }

  void bind(int column, std::size_t val) override {
    const int rc =
        ::sqlite3_bind_int64(m_stmt, column, static_cast<sqlite3_int64>(val));
    if (rc != SQLITE_OK)
      throw Error("sqlite3_bind_int failed");
  }

  void bind(int column, std::chrono::system_clock::time_point time) override {
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(
                           time.time_since_epoch())
                           .count();
    bind(column, static_cast<std::size_t>(epoch));
  }

  void bind(int column, const std::vector<uint8_t> &val) override {
    bind(column, val.data(), val.size());
  }

  void bind(int column, const uint8_t *data, std::size_t len) override {
    if (data == nullptr) {
      bind_null(column);
      return;
    }
    const int rc =
        ::sqlite3_bind_blob64(m_stmt, column, data, len, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
      throw Error("sqlite3_bind_blob failed");
  }

  void bind_null(int column) override {
    const int rc = ::sqlite3_bind_null(m_stmt, column);
    if (rc != SQLITE_OK)
      throw Error("sqlite3_bind_null failed");
  }

  std::span<const uint8_t> get_blob(int column) override {
    if (::sqlite3_column_type(m_stmt, column) == SQLITE_NULL)
      return {};
    const void *blob = ::sqlite3_column_blob(m_stmt, column);
    const int size = ::sqlite3_column_bytes(m_stmt, column);
    return {static_cast<const uint8_t *>(blob), static_cast<std::size_t>(size)};
  }

  std::optional<std::string> get_str(int column) override {
    if (::sqlite3_column_type(m_stmt, column) == SQLITE_NULL)
      return std::nullopt;
    const auto *text = ::sqlite3_column_text(m_stmt, column);
    const int len = ::sqlite3_column_bytes(m_stmt, column);
    return std::string(reinterpret_cast<const char *>(text),
                       static_cast<std::size_t>(len));
  }

  std::size_t get_size_t(int column) override {
    if (::sqlite3_column_type(m_stmt, column) != SQLITE_INTEGER)
      throw Error("get_size_t: column is not INTEGER");
    return static_cast<std::size_t>(::sqlite3_column_int64(m_stmt, column));
  }

  std::size_t spin() override {
    std::size_t steps = 0;
    while (step())
      ++steps;
    return steps;
  }

  bool step() override {
    const int rc = ::sqlite3_step(m_stmt);
    if (rc == SQLITE_ROW)
      return true;
    if (rc == SQLITE_DONE)
      return false;
    throw Error(std::string("sqlite3_step failed: ") +
                ::sqlite3_errmsg(::sqlite3_db_handle(m_stmt)));
  }

private:
  sqlite3_stmt *m_stmt;
};

} // namespace

Database::Database(std::string_view path) : m_db(nullptr) {
  const int flags =
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
  const int rc =
      ::sqlite3_open_v2(std::string(path).c_str(), &m_db, flags, nullptr);
  if (rc != SQLITE_OK) {
    const std::string msg = ::sqlite3_errmsg(m_db);
    ::sqlite3_close(m_db);
    m_db = nullptr;
    throw Error("sqlite3_open failed: " + msg);
  }
}

Database::~Database() {
  if (m_db != nullptr)
    ::sqlite3_close(m_db);
}

std::shared_ptr<store::Statement> Database::stmt(std::string_view sql) const {
  return std::make_shared<store::Statement>(m_db, sql);
}

void Database::create_table(std::string_view schema) {
  char *errmsg = nullptr;
  const int rc = ::sqlite3_exec(m_db, std::string(schema).c_str(), nullptr,
                                nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    const std::string msg = errmsg != nullptr ? errmsg : "unknown error";
    ::sqlite3_free(errmsg);
    throw Error("sqlite3_exec for create_table failed: " + msg);
  }
}

void Database::create_table(const Botan::SQL_Database::Table_Schema &schema) {
  using Column_Type = Botan::SQL_Database::Column_Type;

  std::string sql = "CREATE TABLE ";
  if (schema.is_if_not_exists())
    sql += "IF NOT EXISTS ";
  sql += schema.name();
  sql += " (";
  bool first = true;
  for (const auto &col : schema.columns()) {
    if (!first)
      sql += ", ";
    sql += col.name();
    sql += ' ';
    switch (col.type()) {
    case Column_Type::Blob:
      sql += "BLOB";
      break;
    case Column_Type::String:
      sql += "TEXT";
      break;
    case Column_Type::Integer:
      sql += "INTEGER";
      break;
    }
    if (col.is_primary_key())
      sql += " PRIMARY KEY";
    if (col.is_unique())
      sql += " UNIQUE";
    if (col.is_not_null())
      sql += " NOT NULL";
    first = false;
  }
  sql += ")";
  create_table(sql); // delegates to yca's own overload, above
}

std::shared_ptr<Botan::SQL_Database::Statement>
Database::new_statement(std::string_view sql) const {
  return std::make_shared<BotanStatement>(m_db, sql);
}

std::shared_ptr<Botan::SQL_Database::Statement>
Database::upsert(std::string_view table,
                 std::initializer_list<std::string_view> columns) const {
  std::string sql = "INSERT OR REPLACE INTO ";
  sql += table;
  sql += " (";
  bool first = true;
  for (const auto &col : columns) {
    if (!first)
      sql += ", ";
    sql += col;
    first = false;
  }
  sql += ") VALUES (";
  for (std::size_t i = 1; i <= columns.size(); ++i) {
    if (i > 1)
      sql += ", ";
    sql += "?" + std::to_string(i);
  }
  sql += ")";
  return new_statement(sql);
}

std::size_t Database::row_count(std::string_view table_name) {
  auto s = stmt("SELECT COUNT(*) FROM " + std::string(table_name));
  if (!s->step())
    throw Error("row_count: query failed for table '" +
                std::string(table_name) + "'");
  return s->get_size_t(0);
}

std::size_t Database::rows_changed_by_last_statement() {
  return static_cast<std::size_t>(::sqlite3_changes64(m_db));
}

} // namespace store
