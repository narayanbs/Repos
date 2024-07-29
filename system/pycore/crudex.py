import pymysql
import pymysql.cursors
from pymysql import Error


def main():
    connection = pymysql.connect(
        host="localhost",
        user="rev_user",
        password="slayer",
        database="revision",
        cursorclass=pymysql.cursors.DictCursor,
    )

    with connection:
        with connection.cursor() as cursor:
            try:
                sql = "select * from Laptop"
                cursor.execute(sql)
                result = cursor.fetchall()
                print(result)
            except Error as e:
                print(e)


if __name__ == "__main__":
    main()
