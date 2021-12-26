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

  if (mysql_query(con, "CREATE DATABASE test")) {
    if (strcmp(mysql_error(con),
               "Can't create database 'test'; database exists") == 0) {
      printf("Database is exists");
    } else {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
    }
  }
  mysql_close(con);
  con = mysql_init(NULL);

  if (mysql_real_connect(con, "localhost", "root", "", "test", 0, NULL, 0) ==
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

    if (mysql_query(
            con,
            "CREATE TABLE questions(id INT PRIMARY KEY AUTO_INCREMENT, question VARCHAR(255), answer1 VARCHAR(255), answer2 VARCHAR(255), answer3 VARCHAR(255), answer4 VARCHAR(255), trueanswer CHAR(1), level INT)")) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

  if (mysql_query(
          con,
          "INSERT INTO users(username, password) VALUES('Audi', '52642')")) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
  }

    if (mysql_query(
            con,
            "INSERT INTO questions(question, answer1, answer2, answer3, answer4, trueanswer, level) VALUES('This is question 1', 'A. Answer1 ', 'B. Answer2', 'C.Answer3', 'D.Answer 4', 'B','1'),('This is question 2', 'A. Answer1 ', 'B. Answer2', 'C.Answer3', 'D.Answer 4', 'C','2') ")) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

  mysql_close(con);
  exit(0);
}
