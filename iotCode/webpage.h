String homepage1 = R"=====( 
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0", viewpoint-fir="cover">
    
    <title>Security system WebServer</title>
</head>
  <style>
    body {
      background-color: lightgrey;
    }
    .header {
      display: flex;
      flex-direction: column;
      background-color: white;
      align-items: center;
      border: 2px solid black; 
    }
    .footer {
      display: flex;
      background-color: white;
      align-items: center;
      border: 2px solid black;
      height: 25px;
      justify-content: center;
      z-index: 1000;
      position: fixed;
      bottom: 10px;
      padding: 10px;
    }
    .copy {
      font-size: 20px;
    }
    .container {
      display: flex;
      flex-direction: row;
      align-items:center;
      justify-content: space-evenly;
      flex-wrap: wrap;
    }
    .item {
      border: 2px solid black;
      width: 250px;
      height: 400px;
      background-color: rgba(235, 38, 12, 0.5);
      margin-bottom: 75px;
    }
    .item1 {
      border-bottom: 2px solid black;
      width: 250px;
      height: 199px;
      display: flex;
      flex-direction: column;
      align-items: center;
    }
    h1 {
      font-weight: bold;
      font-size: 40px;
      font-family: arial;
      color: black;
      text-align: center;
    }
    p {
      font-size: 25px;
      font-family: arial;
      color: black;
      text-align: center;
    }
    button {
      padding: 15px 30px;
      border-radius: 10px;
      border-color: rgb(145, 24, 24);
      font-size: medium;
      font-weight: bold;
      text-decoration: none;
      background-color: rgba(163, 44, 1, 0.5);
    }
    button:hover {
      background-color: rgba(163, 44, 1, 1.0);
      cursor: pointer;
    }
  </style>

  <body>
    <div class="header">
        <h1>- - - Security System WebServer - - -</h1>
    </div>

    <br><br><br><br><br><br><br>

    <div class="container">
      <div class="item">
        <h1>- Message -</h1>
        <br><br><br><br>
        <p>
)=====";
String homepage2 = R"=====(
        </p>
      </div>

      <div class="item">
        <h1>- Distance -</h1>
        <br>
        <p>
)=====";
String homepage3 = R"=====(        
        </p>
        <br><br><br>
        <p>
)=====";
String homepage4 = R"=====(
        </p>
      </div>

      <div class="item">
        <div class="item1">
          <h1>- Keypad -</h1>
          <a href="/keypadWeb">
            <button>Enter Pin</button>
          </a>
        </div>
        <div class="item1">
            <h1>- Key -</h1>
            <p>
)=====";
String homepage5 = R"=====(
            </p>
        </div>
      </div>

      <div class="item">
        <div class="item1">
          <h1>- Location -</h1>
          <a href="/locationWeb">
            <button>View Location</button>
          </a>
        </div>
        <div class="item1">
            <h1>- Camera -</h1>
            <a href="/cameraWeb">
            <button>View Camera</button>
          </a>
        </div>
      </div>
    </div>

    <div class="footer">
        <p class="copy">Copyright &copy; 2025 Seth Butler</p>
    </div>

  </body>
</html>
)=====";