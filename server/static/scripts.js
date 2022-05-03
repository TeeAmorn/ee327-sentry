var ws_stream0 = "";
var ws_stream1 = "";
var ws_stream2 = "";
var ws_stream3 = "";
var ws_stream4 = "";

function set_stream0(ws) {
  ws_stream0 = ws;
}
function set_stream1(ws) {
  ws_stream1 = ws;
}
function set_stream2(ws) {
  ws_stream2 = ws;
}
function set_stream3(ws) {
  ws_stream3 = ws;
}
function set_stream4(ws) {
  ws_stream4 = ws;
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

ws_stream0.onmessage = function (event) {
  var img = document.getElementById("stream0");
  img.src = "data:image/jpg;base64," + event.data;
};
ws_stream1.onmessage = function (event) {
  var img = document.getElementById("stream1");
  img.src = "data:image/jpg;base64," + event.data;
};
ws_stream2.onmessage = function (event) {
  var img = document.getElementById("stream2");
  img.src = "data:image/jpg;base64," + event.data;
};
ws_stream3.onmessage = function (event) {
  var img = document.getElementById("stream3");
  img.src = "data:image/jpg;base64," + event.data;
};
ws_stream4.onmessage = function (event) {
  var img = document.getElementById("stream4");
  img.src = "data:image/jpg;base64," + event.data;
};

function sendControl(msg) {
  ws_cam0.send(msg);
}
