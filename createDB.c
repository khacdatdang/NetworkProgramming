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
          "VARCHAR(255) UNIQUE, password VARCHAR(255), highScore INT DEFAULT 0)")) {
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
            "CREATE TABLE using_accounts(id INT PRIMARY KEY AUTO_INCREMENT, username VARCHAR(255) UNIQUE)")) {
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
            "('Which of the following, is the best source of protein ?', 'A. Salad ', 'B. Fish', 'C.Cucumber', 'D.Grape','A','1'),"
            "('What kind of scientist was Euclid ?', 'A. Philosophist ', 'B. Chemist', 'C.Mathematic', 'D.Physician ', 'C','2'),"
            "('Which of the following, is the longest river in Europe ?', 'A. Ural ', 'B. Elbe', 'C.Oka', 'D.Volga', 'D','2'),"
            "('Which of the following, is the biggest moon of the Saturn ?', 'A. Titan ', 'B. Rhea', 'C.Hyperion', 'D.Iapetus ', 'A','2'),"
            "('Which of the following musicians is also known as King of POP ?', 'A. Frank zappa', 'B. Freddie mercury', 'C.George Michael', 'D.Michael Jackson', 'D' ,'2'),"
            "('What do you call someone who doesn''''t believe in god ?', 'A. Atheist ', 'B. Agnostic', 'C.Theist', 'D.Skeptic ', 'A','1'),"
            "('Which animal, is the fastest animal in the world ?', 'A. Lion ', 'B. Cheetah', 'C.Greyhound', 'D.Horse', 'B','3'),"
            "('What year did 9/11 happen ?', 'A. 1999 ', 'B. 2001', 'C.2004', 'D.2000', 'B' ,'3'),"
            "('Who were the defeated finalists at Euro 2008 ?', 'A. Germany ', 'B. England', 'C.France', 'D.Poland', 'A','3'),"
            "('How many independent countries border the Caspian sea ?', 'A. Four ', 'B. Six', 'C.Two', 'D.Five', 'D','3'),"
            "('What is the Closest Planet to Earth?', 'A. Venus ', 'B. Mars', 'C.Jupiter', 'D.Saturn', 'A','3'),"
            "('How many teeth do adult human have in their mouth ?', 'A. 30', 'B. 38', 'C. 32', 'D. 28', 'C', '1'),"
            "('What country declared independence from Serbia in 2008 ?',  'A. Albania', 'B. Kosovo', 'C. Montenegro', 'D. Macedonia', 'B','2'),"
            "('Who invented Mobile phone?', 'A. Martin Cooper', 'B. Alexander Graham Bell', 'C. Thomas Edison', 'D. Albert Einstein', 'A', '3'),"
            "('Which football club in England has won the most trophies ?', 'A. Liverpool', 'B. Chelsea', 'C. Arsenal', 'D. Manchester United', 'D', '1'),"
            "('Entomology is the science that studies ?','A. The formation of rocks', 'B. Behavior of human beings', 'C. Insects', 'D. Computers', 'C', '2'),"
            "('Grand Central Terminal, Park Avenue, New York is the world''s ?', 'A. Highest railway station', 'B. Largest railway station', 'C. Longest railway station', 'D. Largest airport', 'B', '3'),"
            "('Hitler party which came into power in 1933 is known as ?',  'A. Nazi Party', 'B. Labour Party', 'C. Ku-Klux-Klan', 'D. Democratic Party', 'A','1'),"
            "('Epsom (England) is the place associated with ?',  'A. Polo', 'B. Horse racing', 'C. Shooting', 'D. Snooker', 'B','2'),"
            "('First human heart transplant operation conducted by Dr. Christian Bernard on Louis Washkansky, was conducted in ?',  'A. 1965', 'B. 1964', 'C. 1969', 'D. 1967', 'D', '1'),"
            "('First Afghan War took place in ?', 'A. 1835', 'B. 1832', 'C. 1839', 'D. 1840', 'C', '2'),"
            "('First China War was fought between ?', 'A. China and France', 'B. China and Britain', 'C. China and Egypt', 'D. China and Greek', 'B', '3'),"
            "('Each year World Red Cross and Red Crescent Day is celebrated on ?',  'A. May 8', 'B. May 18', 'C. May 1', 'D. May 25', 'A','1'),"
            "('In which year of First World War Germany declared war on Russia and France ?', 'A. 1913', 'B. 1914', 'C. 1915', 'D. 1916', 'B', '2'),"
            "('In a normal human body, the total number of red blood cells is ?', 'A. 20 trillion', 'B. 10 trillion', 'C. 15 trillion', 'D. 30 trillion', 'D', '3'),"
            "('On a radio, stations are changed by using what control?', 'A. Tuning', 'B. Volume', 'C. Bass', 'D. Treble', 'A', '1'),"
            "('Which material is most dense?', 'A. Silver', 'B. Styrofoam', 'C. Butter', 'D. Gold', 'D', '2'),"
            "('Which state in the United States is largest by area?', 'A. Alaska', 'B. California', 'C. Texas', 'D. Hawaii', 'A', '3'),"
            "('What is Aurora Borealis commonly known as?', 'A. Fairy Dust', 'B. Northern Lights', 'C. Book of ages', 'D. a Game of Thrones main character', 'B', '1'),"
            "('Galileo was an Italian astronomer who ...', 'A. developed the telescope', 'B. discovered four satellites of Jupiter', 'C. discovered that the movement of pendulum produces a regular time measurement', 'D. All of the above', 'D', '2'),"
            "('Exposure to sunlight helps a person improve his health because ...', 'A. the infrared light kills bacteria in the body', 'B. resistance power increases', 'C. the pigment cells in the skin get stimulated and produce a healthy tan', 'D. the ultraviolet rays convert skin oil into Vitamin D', 'D', '3'),"
            "('Sir Thomas Fearnley Cup is awarded to ... ', 'A. a club or a local sport association for remarkable achievements', 'B. amateur athlete, not necessarily an Olympian', 'C. National Olympic Committee for outstanding work', 'D. None of the above', 'A', '1'),"
            "('When is the International Workers of Day?', 'A. 15th April', 'B. 12th December', 'C. 1st May', 'D. 1st August', 'C', '2'),"
            "('When did China test their first atomic device?', 'A. 1962', 'B. 1963', 'C. 1964', 'D. 1965', 'C', '3'),"
            "('When did Commander Robert Peary discover the North Pole?', 'A. 1904', 'B. 1905', 'C. 1908', 'D. 1909', 'D', '1'),"
            "('What is the population density of Kerala?', 'A. 819/sq. km', 'B. 602/sq. km', 'C. 415/sq. km', 'D. 500/sq. km', 'A', '2'),"
            "('What is the range of missile Akash?', 'A. 4 km', 'B. 25 km', 'C. 500 m to 9 km', 'D. 150 km', 'A', '3'),"
            "('In the U.S., if it is not Daylight Saving Time, is it what?', 'A. Borrowed time', 'B. Overtime', 'C. Standard time', 'D. Party time', 'B', '1'),"
            "('Which country is largest by area?', 'A. UK', 'B. USA', 'C. Russia', 'D. China', 'A', '2'),"
            "('What does the F stand for in FBI?', 'A. Foreign', 'B. Federal', 'C. Flappy', 'D. Face', 'B', '3'),"
            "('The US declared war on which country after the bombing of Pearl Harbor?', 'A. Japan', 'B. Russia', 'C. Germany', 'D. China', 'A', '1'),"
            "('When is the World of Diabetes Day?', 'A. 14th November', 'B. 11th December', 'C. 15th October', 'D. 1st July', 'A', '2'),"
            "('What is the S.I. unit of temperature?', 'A. Kelvin', 'B. Celsius', 'C. Centigrade', 'D. Fahrenheit', 'C', '3'),"
            "('When did US astronauts Neil Armstrong and Edwin E. Aldrin land on the moon?', 'A. July 21, 1969', 'B. July 21, 1970', 'C. July 21, 1963', 'D. July 21, 1972', 'A', '1'),"
            "('When did 19 NATO members and 11 Partners for Peace join hands for peace plan for Kosovo Crisis?', 'A. 1999', 'B. 1989', 'C. 1979', 'D. 1969', 'A', '2'),"
            "('What is the national emblem of Canada?', 'A. Maple Leaf', 'B. Brown Bear', 'C. Maple Syrup', 'D. Waffle', 'A', '3'),"
            "('Which of the following, is the largest company by revenue ?', 'A. Wal-Mart ', 'B. BP', 'C.Apple', 'D.Shell', 'A','2')")) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

  mysql_close(con);
  exit(0);
}
