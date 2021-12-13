#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const* argv[]) {
  MYSQL* con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "", NULL, 0, NULL, 0) ==
      NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  if (mysql_query(con, "CREATE DATABASE project")) {
    if (strcmp(mysql_error(con),
               "Can't create database 'project'; database exists") == 0) {
      printf("Database is exists");
    } else {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
    }
  }
  mysql_close(con);
  con = mysql_init(NULL);

  if (mysql_real_connect(con, "localhost", "root", "", "project", 0, NULL, 0) ==
      NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  if (mysql_query(con, "DROP TABLE IF EXISTS users")) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  if (mysql_query(
          con,
          "CREATE TABLE users(id INT PRIMARY KEY AUTO_INCREMENT, username "
          "VARCHAR(255) UNIQUE, password VARCHAR(255), highScore INT)")) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

//    if (mysql_query(
//            con,
//            "CREATE TABLE questions(id INT PRIMARY KEY AUTO_INCREMENT, question VARCHAR(500),"
//            "ans1 VARCHAR(255),ans1 VARCHAR(255),ans1 VARCHAR(255),ans1 VARCHAR(255),password VARCHAR(255), highScore INT)")) {
//        fprintf(stderr, "%s\n", mysql_error(con));
//        mysql_close(con);
//        exit(1);
//    }

  if (mysql_query(
          con,
          "INSERT INTO users(username, password) VALUES('Audi', '52642')")) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

  mysql_close(con);
  exit(0);
}
