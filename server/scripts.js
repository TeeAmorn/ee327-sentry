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
    console.log(auto_switches[0].children[1].children[0].classList);
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
