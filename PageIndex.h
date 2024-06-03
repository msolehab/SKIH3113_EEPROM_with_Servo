const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      
      .slidecontainer {
        width: 100%;
      }

      .slider {
        -webkit-appearance: none;
        width: 50%;
        height: 15px;
        border-radius: 5px;
        background: #d3d3d3;
        outline: none;
        opacity: 0.7;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }

      .slider:hover {
        opacity: 1;
      }

      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background: #4CAF50;
        cursor: pointer;
      }

      .slider::-moz-range-thumb {
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background: #4CAF50;
        cursor: pointer;
      }
    </style>
  </head>
  <body>

    <h1>NodeMCU ESP8266 / ESP12E Control Servo SG90</h1>
    <br><br>
    
    <div class="slidecontainer">
      <input type="range" min="0" max="180" value="50" class="slider" id="myRange">
      <p>Value : <span id="demo"></span></p>
    </div>

    <div>
      <h2>WiFi Configuration</h2>
      <form id="configForm">
        <label for="ssid">SSID:</label><br>
        <input type="text" id="ssid" name="ssid" required><br>
        <label for="password">Password:</label><br>
        <input type="password" id="password" name="password" required><br>
        <label for="servoStatus">Servo Status:</label><br>
        <input type="text" id="servoStatus" name="servoStatus" required><br>
        <input type="submit" value="Save">
      </form>
    </div>

    <script>
      document.getElementById("configForm").addEventListener("submit", function(event) {
        event.preventDefault();
        var ssid = document.getElementById("ssid").value;
        var password = document.getElementById("password").value;
        var servoStatus = document.getElementById("servoStatus").value;
        saveConfig(ssid, password, servoStatus);
      });

      function saveConfig(ssid, password, servoStatus) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            alert("Configuration saved! Rebooting...");
            setTimeout(function() {
              window.location.reload();
            }, 2000);
          }
        };
        xhttp.open("GET", "/saveConfig?ssid=" + ssid + "&password=" + password + "&servoStatus=" + servoStatus, true);
        xhttp.send();
      }

      function sendData(pos) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            console.log(this.responseText);
          }
        };
        xhttp.open("GET", "setPOS?servoPOS=" + pos, true);
        xhttp.send();
      } 

      var slider = document.getElementById("myRange");
      var output = document.getElementById("demo");
      output.innerHTML = slider.value;

      slider.oninput = function() {
        output.innerHTML = this.value;
        sendData(output.innerHTML);
      }
    </script>

  </body>
</html>
)=====";
