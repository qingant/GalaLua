CC=gcc
CFLAGS=-g -pedantic -std=c99 -Wall -O2 -shared -fpic -I /usr/local/include/luajit-2.0 -I /usr/include/mysql -I $(HOME)/Sqlite3/include -I /usr/include/pgsql/server/ -I /opt/ibm/db2/V9.7/include/ -I /u01/app/oracle/product/10.2.0/rdbms/public/ -I .
AR=ar rcu
RANLIB=ranlib
RM=rm -rf
MKDIR=mkdir -p

COMMON_LDFLAGS=
MYSQL_LDFLAGS=$(COMMON_LDFLAGS) -lmysqlclient -L /usr/lib64/mysql
PSQL_LDFLAGS=$(COMMON_LDFLAGS) -lpq
SQLITE3_LDFLAGS=$(COMMON_LDFLAGS) -lsqlite3 -L $(HOME)/Sqlite3/lib 
DB2_LDFLAGS=$(COMMON_LDFLAGS) -L/opt/ibm/db2/V9.7/lib64 -L/opt/ibm/db2/V9.7/lib32 -ldb2
ORACLE_LDFLAGS=$(COMMON_LDFLAGS) -L/u01/app/oracle/product/10.2.0/lib -locijdbc11

BUILDDIR=build

DBDMYSQL=dbdmysql.so
DBDPSQL=dbdpostgresql.so
DBDSQLITE3=dbdsqlite3.so
DBDDB2=dbddb2.so
DBDORACLE=dbdoracle.so

OBJS=build/dbd_common.o
MYSQL_OBJS=$(OBJS) build/dbd_mysql_main.o build/dbd_mysql_connection.o build/dbd_mysql_statement.o
PSQL_OBJS=$(OBJS) build/dbd_postgresql_main.o build/dbd_postgresql_connection.o build/dbd_postgresql_statement.o
SQLITE3_OBJS=$(OBJS) build/dbd_sqlite3_main.o build/dbd_sqlite3_connection.o build/dbd_sqlite3_statement.o
DB2_OBJS=$(OBJS) build/dbd_db2_main.o build/dbd_db2_connection.o build/dbd_db2_statement.o
ORACLE_OBJS=$(OBJS) build/dbd_oracle_main.o build/dbd_oracle_connection.o build/dbd_oracle_statement.o

free: mysql psql sqlite3

all:  mysql psql sqlite3 db2 oracle

mysql: $(BUILDDIR) $(MYSQL_OBJS)
	$(CC) $(CFLAGS) $(MYSQL_OBJS) -o $(DBDMYSQL) $(MYSQL_LDFLAGS)

psql: $(BUILDDIR) $(PSQL_OBJS)
	$(CC) $(CFLAGS) $(PSQL_OBJS) -o $(DBDPSQL) $(PSQL_LDFLAGS)

sqlite3: $(BUILDDIR) $(SQLITE3_OBJS)
	$(CC) $(CFLAGS) $(SQLITE3_OBJS) -o $(DBDSQLITE3) $(SQLITE3_LDFLAGS)

db2: $(BUILDDIR) $(DB2_OBJS)
	$(CC) $(CFLAGS) $(DB2_OBJS) -o $(DBDDB2) $(DB2_LDFLAGS)

oracle: $(BUILDDIR) $(ORACLE_OBJS)
	$(CC) $(CFLAGS) $(ORACLE_OBJS) -o $(DBDORACLE) $(ORACLE_LDFLAGS)

clean:
	$(RM) $(BUILDDIR) $(MYSQL_OBJS) $(PSQL_OBJS) $(SQLITE3_OBJS) $(DB2_OBJS) $(ORACLE_OBJS) $(DBDMYSQL) $(DBDPSQL) $(DBDSQLITE3) $(DBDDB2) $(DBDORACLE)

build/dbd_common.o: dbd/common.c dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build/dbd_mysql_connection.o: dbd/mysql/connection.c dbd/mysql/dbd_mysql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_mysql_main.o: dbd/mysql/main.c dbd/mysql/dbd_mysql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_mysql_statement.o: dbd/mysql/statement.c dbd/mysql/dbd_mysql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build/dbd_postgresql_connection.o: dbd/postgresql/connection.c dbd/postgresql/dbd_postgresql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_postgresql_main.o: dbd/postgresql/main.c dbd/postgresql/dbd_postgresql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_postgresql_statement.o: dbd/postgresql/statement.c dbd/postgresql/dbd_postgresql.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build/dbd_sqlite3_connection.o: dbd/sqlite3/connection.c dbd/sqlite3/dbd_sqlite3.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_sqlite3_main.o: dbd/sqlite3/main.c dbd/sqlite3/dbd_sqlite3.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_sqlite3_statement.o: dbd/sqlite3/statement.c dbd/sqlite3/dbd_sqlite3.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build/dbd_db2_connection.o: dbd/db2/connection.c dbd/db2/dbd_db2.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_db2_main.o: dbd/db2/main.c dbd/db2/dbd_db2.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_db2_statement.o: dbd/db2/statement.c dbd/db2/dbd_db2.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build/dbd_oracle_connection.o: dbd/oracle/connection.c dbd/oracle/dbd_oracle.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_oracle_main.o: dbd/oracle/main.c dbd/oracle/dbd_oracle.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)
build/dbd_oracle_statement.o: dbd/oracle/statement.c dbd/oracle/dbd_oracle.h dbd/common.h
	$(CC) -c -o $@ $< $(CFLAGS)

build:
	$(MKDIR) ${BUILDDIR}

