import pymysql.cursors
from pymysql import Error

try:
    connection = pymysql.connect(
        host="localhost",
        user="rev_user",
        password="slayer",
        database="revision",
        cursorclass=pymysql.cursors.DictCursor,
    )

    with connection:
        db_info = connection.get_server_info()
        print(f"Connected to mysql server version {db_info}")
        with connection.cursor() as cursor:
            cursor.execute("select database();")
            result = cursor.fetchone()
            print(result)
except Error as e:
    print(f"Error while connection to MySQL {e}")
finally:
    print("MySQL connection is closed")
