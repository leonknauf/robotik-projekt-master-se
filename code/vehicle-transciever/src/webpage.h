#include <avr/pgmspace.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>NUT Control</TITLE>
            <script>
                function sendrequest(suburl) {
                    const Http = new XMLHttpRequest();
                    const url = 'http://'+location.hostname+'/'+suburl;
                    Http.open("GET", url);
                    Http.send();
                }
            </script>
	</HEAD>
<BODY>
    <button style="width: 100%; height: 10%;" onclick="sendrequest('forward')">Forward</button>
	<button style="width: 100%; height: 10%;" onclick="sendrequest('stop')">Stop</button>
	<button style="width: 100%; height: 10%;" onclick="sendrequest('left')">Left</button>
	<button style="width: 100%; height: 10%;" onclick="sendrequest('right')">Right</button>
</BODY>
</HTML>
)=====";