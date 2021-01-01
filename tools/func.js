/*Funtionen für LED*/

function init_page()
{
  var stb_button = document.getElementById("led_standby_button");
  if(stb_button)
  {
    stb_button.onclick = onStandbyButtonClick ;
  }
  stb_button = document.getElementById("led_operate_button");
  if(stb_button)
  {
    stb_button.onclick = onOperateButtonClick;
  }
}

function onStandbyButtonClick()
{
  setStandby(true);
}

function onOperateButtonClick()
{
  setStandby(false);
}

function setStandby(is_stb)
{
  var xhr = new XMLHttpRequest();
  var url = "http://ota-server.fritz.box/rest/led";
  var stb = "true";
  if(is_stb==false)
  {
    stb = "false";
  }
  xhr.open("POST", url, true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.onreadystatechange = function () {
      if (xhr.readyState === 4 && xhr.status === 200) {
          var json = JSON.parse(xhr.responseText);
          console.log(json.email + ", " + json.password);
      }
  };
  var data = JSON.stringify({"set_standby:": stb});
  console.debug(data);
  xhr.send(data);
}
