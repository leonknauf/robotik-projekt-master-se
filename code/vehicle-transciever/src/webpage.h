#include <avr/pgmspace.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>NUT Control</TITLE>
            <script>
                function sendrequest(command, value) {
                    const Http = new XMLHttpRequest();
                    const url = 'http://'+location.hostname+'/control?command='+command+'&value='+value;
                    console.log('Sending request: ' + url);
                    Http.open("GET", url);
                    Http.send();
                }

                window.addEventListener("load", (event) => {
                    var buttons = document.getElementsByTagName('button');
                    for(let i = 0; i < buttons.length; i++) {
                        buttons[i].onmousedown = function(){
                            sendrequest(this.id,  1);
                        }
                        buttons[i].onmouseup = function(){
                            sendrequest(this.id,0);
                        }
                    }

                    var sliders = document.querySelectorAll('input[type=range]');
                    for(let i = 0; i < sliders.length; i++) {
                        sliders[i].onmouseup = function(){
                            sendrequest(this.id, this.value);
                        }
                    }
                });
                
            </script>
	</HEAD>
<BODY>
    <button style="width: 100%; height: 10%;" id="forward">Forward</button>
	<button style="width: 100%; height: 10%;" id="backward">Backwards</button>
	<button style="width: 100%; height: 10%;" id="left">Left</button>
	<button style="width: 100%; height: 10%;" id="right">Right</button>
    <button style="width: 100%; height: 10%;" id="hard_left">Hard Left</button>
	<button style="width: 100%; height: 10%;" id="hard_right">Hard Right</button>
    <div class="slidecontainer">
        <input style="width: 100%" type="range" min="0" max="255" value="100" class="slider" id="velocity">
    </div>
</BODY>
</HTML>
)=====";