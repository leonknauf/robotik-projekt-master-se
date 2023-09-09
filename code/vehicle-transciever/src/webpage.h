#include <avr/pgmspace.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>NUT Control</TITLE>
            <script>
                function sendcontrolrequest(command, value) {
                    const Http = new XMLHttpRequest();
                    const url = 'http://'+location.hostname+'/control?command='+command+'&value='+value;
                    console.log('Sending request: ' + url);
                    Http.open("GET", url);
                    Http.send();
                }

                function sendvalverequest(valveNum, state) {
                    const Http = new XMLHttpRequest();
                    const url = 'http://192.168.4.200/valve?valveNum='+valveNum+'&state='+state;
                    console.log('Sending request: ' + url);
                    Http.open("GET", url);
                    Http.send();
                }

                window.addEventListener("load", (event) => {
                    var buttons = document.getElementsByClassName('manualControl');
                    for(let i = 0; i < buttons.length; i++) {
                        buttons[i].onmousedown = function(){
                            sendcontrolrequest(this.id,  1);
                        }
                        buttons[i].onmouseup = function(){
                            sendcontrolrequest(this.id,0);
                        }
                    }

                    var sliders = document.querySelectorAll('input[type=range]');
                    for(let i = 0; i < sliders.length; i++) {
                        sliders[i].onmouseup = function(){
                            sendcontrolrequest(this.id, this.value);
                        }
                    }

                    var buttons = document.getElementsByClassName('valveControl');
                    for(let i = 0; i < buttons.length; i++) {
                        buttons[i].onmousedown = function(){
                            sendvalverequest(this.id,  "open");
                        }
                        buttons[i].onmouseup = function(){
                            sendvalverequest(this.id, "close");
                        }
                    }
                });
                
            </script>
	</HEAD>
<BODY>
    <button style="width: 100%; height: 10%;" class="manualControl" id="forward">Forward</button>
	<button style="width: 100%; height: 10%;" class="manualControl" id="backward">Backwards</button>
	<button style="width: 100%; height: 10%;" class="manualControl" id="left">Left</button>
	<button style="width: 100%; height: 10%;" class="manualControl" id="right">Right</button>
    <button style="width: 100%; height: 10%;" class="manualControl" id="hard_left">Hard Left</button>
	<button style="width: 100%; height: 10%;" class="manualControl" id="hard_right">Hard Right</button>
    <div class="slidecontainer">
        <input style="width: 100%" type="range" min="0" max="255" value="100" class="slider" id="velocity">
    </div>
    <button style="width: 100%; height: 10%;" class="valveControl" id="valve1">Valve1</button>
    <button style="width: 100%; height: 10%;" class="valveControl" id="valve2">Valve2</button>
    <button style="width: 100%; height: 10%;" class="valveControl" id="valve3">Valve3</button>
    <button style="width: 100%; height: 10%;" class="valveControl" id="valve4">Valve4</button>
</BODY>
</HTML>
)=====";