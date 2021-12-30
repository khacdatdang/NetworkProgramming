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
            "CREATE TABLE questions(id INT PRIMARY KEY AUTO_INCREMENT, question VARCHAR(255), answer1 VARCHAR(255), answer2 VARCHAR(255), answer3 VARCHAR(255), answer4 VARCHAR(255), trueanswer VARCHAR(255), level INT)")) {
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
            "INSERT INTO questions(question, answer1, answer2, answer3, answer4, trueanswer, level) "
            "VALUES('How many rings does Olympic flag have ?', 'A. Five', 'B. Six', 'C.Four', 'D.Ten', 'A','1'),"
            "('What did Erno Rubik invent in 1975 ?', 'A. Microscope ', 'B. Telegraph', 'C.Rubik''s Cube', 'D.Polygraph', 'D','1'),"
            "('Which country has won the most FIFA World Cups in football?', 'A. Argentina ', 'B. Spain', 'C.Japan', 'D.Brazil', 'B','1'),"
            "('Which of the following, is the best source of protein ?', 'A. Salad ', 'B. Fish', 'C.Cucumber', 'D.Grape','C','1'),"
            "('What kind of scientist was Euclid ?', 'A. Philosophist ', 'B. Chemist', 'C.Mathematic', 'D.Physician ', answer3,'2'),"
            "('Which of the following, is the longest river in Europe ?', 'A. Ural ', 'B. Elbe', 'C.Oka', 'D.Volga', answer4,'2'),"
            "('Which of the following, is the biggest moon of the Saturn ?', 'A. Titan ', 'B. Rhea', 'C.Hyperion', 'D.Iapetus ', answer1,'2'),"
            "('Which of the following musicians is also known as King of POP ?', 'A. Frank zappa', 'B. Freddie mercury', 'C.George Michael', 'D.Michael Jackson', answer4 ,'2'),"
            "('What do you call someone who doesn''''t believe in god ?', 'A. Atheist ', 'B. Agnostic', 'C.Theist', 'D.Skeptic ', answer1,'1'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', answer2,'3'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', answer2,'3'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', answer2,'3'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', answer2,'3'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', answer2,'3'),"
            "('Which of the following, is the largest company by revenue ?', 'A. Wal-Mart ', 'B. BP', 'C.Apple', 'D.Shell', answer1,'2')")) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

  mysql_close(con);
  exit(0);
}
