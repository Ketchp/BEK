from flask import Flask, request
from werkzeug.middleware.proxy_fix import ProxyFix


app = Flask(__name__)
app.wsgi_app = ProxyFix(
    app.wsgi_app, x_for=1, x_proto=1
)


@app.route("/", methods=['GET', 'POST'])
def bek_logger():
    with open('logged.txt', 'a') as file:
        file.write(request.get_data().decode('ascii'))
        file.write('\n\n')

    return "OK"


if __name__ == '__main__':
    app.run(host='0.0.0.0')

