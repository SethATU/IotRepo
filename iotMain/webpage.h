<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0", viewpoint-fir="cover">
    
    <title>Security system WebServer</title>
</head>
    <style>
        body {
                background-color: grey;
        }
        .header {
            display: flex;
            flex-direction: column;
            background-color: whitesmoke;
            align-items: center;
            border: 2px solid black;
            
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
            background-color: blanchedalmond;
        }
        .item1 {
            border-bottom: 2px solid black;
            width: 250px;
            height: 172px;
            background-color: blanchedalmond;
        }
        h1 {
            font: bold;
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
    </style>

  <body>
    <div class="header">
        <h1>Security System WebServer</h1>
    </div>

    <br><br><br><br><br><br><br>

    <div class="container">
      <div class="item">
        <h1>Message</h1>
        <br><br><br><br>
        <p>Arm the alarm</p>
      </div>

      <div class="item">
        <h1>Distance</h1>
        <br>
        <p>123.45Cm</p>
        <br><br><br>
        <p>Safe</p>
      </div>

      <div class="item">
        <h1>Keypad</h1>
        <br>
        <p>Numbers entered</p>
        <br><br><br><br>
        <p>Enter Pin online <!-- botton to virtual Keypad --></p>
      </div>

      <div class="item">
        <div class="item1">
          <h1>Alarm</h1>
          <p>Acive/Deactivated</p>
        </div>
        <div class="item1">
            <h1>Camera</h1>
            <p>View Camera <!-- Button --></p>
        </div>
      </div>
    </div>
  </body>
</html>