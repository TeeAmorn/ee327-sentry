var wsStream = "";
var wsCam1 = "";
var wsCam2 = "";
var wsCam3 = "";
var wsCam4 = "";

function set_stream(ws) {
  wsStream = ws;
}
function set_cam1(ws) {
  wsCam1 = ws;
}
function set_cam2(ws) {
  wsCam2 = ws;
}
function set_cam3(ws) {
  wsCam3 = ws;
}
function set_cam4(ws) {
  wsCam4 = ws;
}

function onClickManual(event) {
  if (event.target.checked) {
    let auto_switches = document.getElementsByClassName("auto");
    for (var i = 0, max = auto_switches.length; i < max; i++) {
      auto_switches[i].children[1].children[0].disabled = true;
      auto_switches[i].children[1].children[0].classList.add("disabled-switch");
    }
    let direction_buttons = document.getElementsByClassName("direction");
    for (var i = 0, max = direction_buttons.length; i < max; i++) {
      direction_buttons[i].classList.remove("disabled-button");
      direction_buttons[i].disabled = false;
    }
  } else {
    let auto_switches = document.getElementsByClassName("auto");
    for (var i = 0, max = auto_switches.length; i < max; i++) {
      auto_switches[i].children[1].children[0].disabled = false;
      auto_switches[i].children[1].children[0].classList.remove(
        "disabled-switch"
      );
    }
    let direction_buttons = document.getElementsByClassName("direction");
    for (var i = 0, max = direction_buttons.length; i < max; i++) {
      direction_buttons[i].classList.add("disabled-button");
      direction_buttons[i].disabled = true;
    }
  }
}

wsStream.onmessage = function (event) {
  var img = document.getElementById("stream");
  img.src = "data:image/jpg;base64," + event.data;
};
wsCam1.onmessage = function (event) {
  var img = document.getElementById("cam1");
  img.src = "data:image/jpg;base64," + event.data;
};
wsCam2.onmessage = function (event) {
  var img = document.getElementById("cam2");
  img.src = "data:image/jpg;base64," + event.data;
};
wsCam3.onmessage = function (event) {
  var img = document.getElementById("cam3");
  img.src = "data:image/jpg;base64," + event.data;
};
wsCam4.onmessage = function (event) {
  var img = document.getElementById("cam4");
  img.src = "data:image/jpg;base64," + event.data;
};

function sendControl(msg) {
  wsStream.send(msg);
}
