from flask import Flask, request

app = Flask(__name__)


@app.route("/BEK_logger", methods=['GET', 'POST'])
def bek_logger():
    with open('logged.txt', 'a') as file:
        file.write(str(request.headers))
        file.write('\n')
        file.write(str(request.get_data()))
        file.write('\n\n')

    return "OK"
