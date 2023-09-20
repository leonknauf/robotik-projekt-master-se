#include <avr/pgmspace.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>NUT Control</TITLE>
            <script>

                var auto_refresh = 1;

                function buttonEvent(button, isPressDown) {
                    const Http = new XMLHttpRequest();
                    var url = '';
                    if (button.dataset.target == 'vehicle'){
                        url = 'http://'+location.hostname+'/control?command='+button.dataset.command+'&value='+isPressDown;
                    }
                    else if (button.dataset.target == 'valve' && isPressDown > 0) {
                        url = 'http://192.168.4.200/valve?valveNum='+button.dataset.valvenum+'&state='+button.dataset.value;
                    } else if (button.dataset.target == 'variable_base' && isPressDown > 0) {
                        var value = document.getElementById(button.dataset.valuename).value;
                        url = 'http://192.168.4.200/value?valueName='+button.dataset.valuename+'&value='+value;
                    } else if (button.dataset.target == 'internal_refresh' && isPressDown > 0) {
                        if (auto_refresh > 0) {
                            auto_refresh = 0;
                            button.innerHTML = "Auto Refresh: OFF"
                            document.querySelectorAll('.deacOnRefresh').forEach(element => element.disabled = false);
                        } else {
                            auto_refresh = 1;
                            button.innerHTML = "Auto Refresh: ON"
                            document.querySelectorAll('.deacOnRefresh').forEach(element => element.disabled = true);
                        }
                    }
                    else {
                        return;
                    }

                    console.log('Sending request: ' + url);

                    Http.open("GET", url);
                    Http.send();

                }

                window.addEventListener("load", (event) => {
                    var buttons = document.getElementsByTagName('button');
                    for(let i = 0; i < buttons.length; i++) {
                        buttons[i].onmousedown = function(){
                            buttonEvent(this, 1);
                        }
                        buttons[i].onmouseup = function(){
                            buttonEvent(this, 0);
                        }
                    }
                    document.querySelectorAll('.deacOnRefresh').forEach(element => element.disabled = true);

                    window.setInterval(function(){
                        if (auto_refresh > 0) {
                            var variables_base = ["enableAutomaticMode", "automaticState", "numConfiguredValves"];

                            variables_base.forEach(function(variable){
                                const Http = new XMLHttpRequest();
                                Http.onreadystatechange = function() {
                                    if (Http.readyState == XMLHttpRequest.DONE) {
                                        document.getElementById(variable).value = Http.responseText;
                                    }
                                }
                                var url = 'http://192.168.4.200/value?valueName='+variable+'&value=request';
                                console.log('Sending request: ' + url);
                                Http.open("GET", url);
                                Http.send();
                            });
                        }
                        
                    }, 500);
                });
                
            </script>
	</HEAD>
<BODY style="font-family: Consolas, Menlo, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace, serif;">
    <h2>Manual Control:</h2>
    <button data-target="vehicle" data-command="forward">Forward</button>
	<button data-target="vehicle" data-command="backward">Backwards</button>
	<button data-target="vehicle" data-command="left">Left</button>
	<button data-target="vehicle" data-command="right">Right</button>
    <button data-target="vehicle" data-command="hard_left">Hard Left</button>
	<button data-target="vehicle" data-command="hard_right">Hard Right</button>
    
    <h2>Valve Control:</h2>
    <h4>Trigger Valves</h4>
    <button data-target="valve" data-value="2" data-valvenum="1">Trigger Valve1</button>
    <button data-target="valve" data-value="2" data-valvenum="2">Trigger Valve2</button>
    <button data-target="valve" data-value="2" data-valvenum="3">Trigger Valve3</button>
    <button data-target="valve" data-value="2" data-valvenum="4">Trigger Valve4</button>
    <h4>Open Valves</h4>
    <button data-target="valve" data-value="1" data-valvenum="1">Open Valve1</button>
    <button data-target="valve" data-value="1" data-valvenum="2">Open Valve2</button>
    <button data-target="valve" data-value="1" data-valvenum="3">Open Valve3</button>
    <button data-target="valve" data-value="1" data-valvenum="4">Open Valve4</button>
    <h4>Close Valves</h4>
    <button data-target="valve" data-value="0" data-valvenum="1">Close Valve1</button>
    <button data-target="valve" data-value="0" data-valvenum="2">Close Valve2</button>
    <button data-target="valve" data-value="0" data-valvenum="3">Close Valve3</button>
    <button data-target="valve" data-value="0" data-valvenum="4">Close Valve4</button>

    <h2>Variables:</h2>
    <button data-target="internal_refresh">Auto Refresh: ON</button>
    <h4>Base Transciever</h4>
    <label for="enableAutomaticMode">enableAutomaticMode :</label>
    <input type="text" id="enableAutomaticMode" class="deacOnRefresh">
    <button data-target="variable_base" data-valuename="enableAutomaticMode" class="deacOnRefresh">Set Value</button><br>
    <label for="automaticState">automaticState&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:</label>
    <input type="text" id="automaticState" disabled><br>
    <label for="numConfiguredValves">numConfiguredValves :</label>
    <input type="text" id="numConfiguredValves" class="deacOnRefresh">
    <button data-target="variable_base" data-valuename="numConfiguredValves" class="deacOnRefresh">Set Value</button><br>

    <h4>Vehicle Transciever</h4>

    <h4>Vehicle</h4>
</BODY>
</HTML>
)=====";