<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0", viewpoint-fir="cover">
    
    <title>Security system WebServer</title>
</head>
  <style>
    body {
      background-image: url(background.jpg);
      background-repeat: no-repeat;
      background-attachment: fixed;
      background-size: 100% 100%;
    }
    .header {
      display: flex;
      flex-direction: column;
      background-color: color-mix(in oklab, black 25%, white);
      align-items: center;
      border: 2px solid black; 
    }
    .footer {
      display: flex;
      background-color: color-mix(in oklab, black 25%, white);
      align-items: center;
      border: 2px solid black;
      height: 25px;
      justify-content: center;
      z-index: 1000;
      position: fixed;
      bottom: 10px;
      padding: 10px;
    }
    #copy {
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
      width: 400px;
      height: 500px;
      background-color: rgba(235, 38, 12, 0.5);
      margin-bottom: 75px;
    }
    .item2 {
      border: 3px solid black;
      margin: 10px;
      border-radius: 5px;
      background-color: rgba(235, 38, 12, 0.5);
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
    button {
      padding: 30px 55.5px;
      border-radius: 10px;
      border-color: rgb(145, 24, 24);
      font-size: x-large;
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
        <h1>- - - Security System Keypad - - -</h1>
    </div>

    <br><br><br><br><br><br><br>

    <div class="container">
      <div class="item">
        <table>
          <tr>
            <th><button>1</button></th>
            <th><button>2</button></th>
            <th><button>3</button></th>
          </tr>
          <tr>
            <th><button>4</button></th>
            <th><button>5</button></th>
            <th><button>6</button></th>
          </tr>
          <tr>
            <th><button>7</button></th>
            <th><button>8</button></th>
            <th><button>9</button></th>
          </tr>
          <tr>
            <th><button>*</button></th>
            <th><button>0</button></th>
            <th><button>#</button></th>
          </tr>
        </table>
        <div class="item2">
          <p>1 2 3 4</p>
        </div>
      </div>

      <div id="back">
        <a href="webpage.h" target="_self">
          <button>Go Home</button>
        </a>
      </div>
    </div>
    
    <div class="footer">
      <p id="copy">Copyright &copy; 2025 Seth Butler</p>
    </div>

  </body>
</html>