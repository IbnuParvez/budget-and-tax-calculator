# Budget & Tax System — MySQL edition

This app used to persist data in flat text files (`records.txt`,
`history_<username>.txt`). It now stores everything in MySQL instead. The
menus, prompts, and program behavior are unchanged — only the storage layer
moved.

## What changed

- **New `Database` class** (`Database.h`/`.cpp`) wraps a single MySQL
  connection (via the MySQL C API / `libmysqlclient`). On startup it
  connects, creates the database if missing, and creates the `users` and
  `history` tables if missing — so a first run "just works" against an
  empty MySQL server.
- **`AuthManager`** now takes a `Database&` instead of a filename. Every
  method (`registerUser`, `loginUser`, `resetPassword`, `listUsernames`,
  `deleteUser`) runs a SQL query against the `users` table instead of
  reading/rewriting `records.txt`.
- **`HistoryManager`** now takes a `Database&` + username instead of a
  filename. `addEntry`/`showHistoryGraphs`/`clearHistory` run SQL against
  the `history` table instead of `history_<username>.txt`.
- **Cascading delete**: `history.username` has a foreign key to
  `users.username` with `ON DELETE CASCADE`, so deleting a user
  automatically deletes their history — `main.cpp` no longer manually
  deletes a `history_<username>.txt` file.
- All user input is passed through `Database::escape()` (which wraps
  `mysql_real_escape_string`) before being interpolated into SQL, so
  usernames/passwords/passphrases can't break out of a query.

Passwords and passphrases are still stored as **plain text**, exactly as
before — this change is scoped to storage (files → MySQL), not to security.
`Hash.h`/`Hash.cpp` are still unused, same as in the original project;
wiring `hashPassword()` into `registerUser`/`loginUser`/`resetPassword`
would be a good follow-up if you want it.

## Schema

```sql
CREATE TABLE users (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(100) NOT NULL UNIQUE,
  password VARCHAR(255) NOT NULL,
  passphrase VARCHAR(255) NOT NULL
);

CREATE TABLE history (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(100) NOT NULL,
  income DOUBLE NOT NULL,
  tax DOUBLE NOT NULL,
  remaining_income DOUBLE NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);
```

You don't need to run this by hand — `Database::connect()` creates both
tables automatically the first time the program runs against a fresh
database.

## Setup

### 1. Install MySQL + the C client library

```bash
# Ubuntu/Debian
sudo apt-get install mysql-server libmysqlclient-dev
sudo service mysql start   # or: sudo systemctl start mysql
```

### 2. Configure connection settings (optional)

The app reads connection settings from environment variables, all
optional:

| Variable      | Default               |
|---------------|------------------------|
| `DB_HOST`     | `localhost`            |
| `DB_PORT`     | `3306`                 |
| `DB_USER`     | `root`                 |
| `DB_PASSWORD` | *(empty)*               |
| `DB_NAME`     | `budget_tax_system`     |

If your MySQL root account needs a password, or you'd rather use a
dedicated app user:

```sql
CREATE USER 'budget_app'@'localhost' IDENTIFIED BY 'choose-a-password';
GRANT ALL PRIVILEGES ON budget_tax_system.* TO 'budget_app'@'localhost';
FLUSH PRIVILEGES;
```

```bash
export DB_USER=budget_app
export DB_PASSWORD=choose-a-password
```

### 3. Build

`make` uses `mysql_config` to find MySQL's headers and libraries
automatically, so make sure it's on your `PATH` (installing
`libmysqlclient-dev` puts it there):

```bash
make
```

### 4. Run

```bash
./budget_tax_system
```

On first run against an empty database you'll see no errors — the
`users`/`history` tables are created silently before the login menu
appears. If MySQL isn't reachable, the program prints a clear error and
exits instead of crashing partway through a menu.
