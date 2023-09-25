#include <avr/pgmspace.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>NutHub</TITLE>
            <script>

                var auto_refresh = 1;
                var leftIsDown, rightIsDown, forwardIsDown, backwardIsDown, stopIsDown, hardLeftIsDown, hardRightIsDown;
                var v1IsDown, v2IsDown, v3IsDown, v4IsDown, radioIsDown;

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
                    } else if (button.dataset.target == 'variable_vehicle_i2c' && isPressDown > 0) {
                        var value = document.getElementById(button.dataset.valuename).value;
                        url = 'http://'+location.hostname+'/value_i2c?valueName='+button.dataset.valuename+'&value='+value;
                    } else if (button.dataset.target == 'internal_refresh' && isPressDown > 0) {
                        if (auto_refresh > 0) {
                            auto_refresh = 0;
                            button.innerHTML = "Auto Refresh: OFF"
                            button.style = "background-color: grey"
                            document.querySelectorAll('.deacOnRefresh').forEach(element => element.disabled = false);
                        } else {
                            auto_refresh = 1;
                            button.innerHTML = "Auto Refresh: ON"
                            button.style = "background-color: #4CAF50"
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

                    window.addEventListener("keydown", function(event){
                        let mevent;
                        if (window.MouseEvent && typeof window.MouseEvent === 'function') {
                            mevent = new MouseEvent("mousedown");
                        } else {
                            mevent = document.createEvent('MouseEvent');
                            mevent.initMouseEvent("mousedown");
                        }

                        switch (event.keyCode) {
                            case 65: //A
                                if (!leftIsDown) {
                                    document.getElementById("left").dispatchEvent(mevent);
                                    document.getElementById("left").classList.add("active");
                                    leftIsDown = true;
                                }
                            break;

                            case 68: //D
                                if (!rightIsDown) {
                                    document.getElementById("right").dispatchEvent(mevent);
                                    document.getElementById("right").classList.add("active");
                                    rightIsDown = true;
                                }
                            break;

                            case 87: //W
                                if (!forwardIsDown) {
                                    document.getElementById("forward").dispatchEvent(mevent);
                                    document.getElementById("forward").classList.add("active");
                                    forwardIsDown = true;
                                }
                            break;

                            case 83: //S
                                if (!backwardIsDown) {
                                    document.getElementById("backward").dispatchEvent(mevent);
                                    document.getElementById("backward").classList.add("active");
                                    backwardIsDown = true;
                                }
                            break;

                            case 86: //V
                                if (!stopIsDown) {
                                    document.getElementById("stop").dispatchEvent(mevent);
                                    document.getElementById("stop").classList.add("active");
                                    stopIsDown = true;
                                }
                            break;

                            case 81: //Q
                                if (!hardLeftIsDown) {
                                    document.getElementById("hard_left").dispatchEvent(mevent);
                                    document.getElementById("hard_left").classList.add("active");
                                    hardLeftIsDown = true;
                                }
                            break;

                            case 69: //E
                                if (!hardRightIsDown) {
                                    document.getElementById("hard_right").dispatchEvent(mevent);
                                    document.getElementById("hard_right").classList.add("active");
                                    hardRightIsDown = true;
                                }
                            break;

                            case 49: //1
                                if (!v1IsDown) {
                                    document.getElementById("v1").dispatchEvent(mevent);
                                    document.getElementById("v1").classList.add("active");
                                    v1IsDown = true;
                                }
                            break;

                            case 50: //2
                                if (!v2IsDown) {
                                    document.getElementById("v2").dispatchEvent(mevent);
                                    document.getElementById("v2").classList.add("active");
                                    v2IsDown = true;
                                }
                            break;

                            case 51: //3
                                if (!v3IsDown) {
                                    document.getElementById("v3").dispatchEvent(mevent);
                                    document.getElementById("v3").classList.add("active");
                                    v3IsDown = true;
                                }
                            break;

                            case 52: //4
                                if (!v4IsDown) {
                                    document.getElementById("v4").dispatchEvent(mevent);
                                    document.getElementById("v4").classList.add("active");
                                    v4IsDown = true;
                                }
                            break;

                            case 82: //R
                                if (!radioIsDown) {
                                    document.getElementById("radio").dispatchEvent(mevent);
                                    document.getElementById("radio").classList.add("active");
                                    radioIsDown = true;
                                }
                            break;
                        }
                    });

                    window.addEventListener("keyup", function(event){
                        let mevent;
                        if (window.MouseEvent && typeof window.MouseEvent === 'function') {
                            mevent = new MouseEvent("mouseup");
                        } else {
                            mevent = document.createEvent('MouseEvent');
                            mevent.initMouseEvent("mouseup");
                        }

                        switch (event.keyCode) {
                            case 65: //A
                                document.getElementById("left").dispatchEvent(mevent);
                                document.getElementById("left").classList.remove("active");
                                leftIsDown = false;
                            break;

                            case 68: //D
                                document.getElementById("right").dispatchEvent(mevent);
                                document.getElementById("right").classList.remove("active");
                                rightIsDown = false;
                            break;

                            case 87: //W
                                document.getElementById("forward").dispatchEvent(mevent);
                                document.getElementById("forward").classList.remove("active");
                                forwardIsDown = false;
                            break;

                            case 83: //S
                                document.getElementById("backward").dispatchEvent(mevent);
                                document.getElementById("backward").classList.remove("active");
                                backwardIsDown = false;
                            break;

                            case 86: //V
                                document.getElementById("stop").dispatchEvent(mevent);
                                document.getElementById("stop").classList.remove("active");
                                stopIsDown = false;
                            break;

                            case 81: //Q
                                document.getElementById("hard_left").dispatchEvent(mevent);
                                document.getElementById("hard_left").classList.remove("active");
                                hardLeftIsDown = false;
                            break;

                            case 69: //E
                                document.getElementById("hard_right").dispatchEvent(mevent);
                                document.getElementById("hard_right").classList.remove("active");
                                hardRightIsDown = false;
                            break;

                            case 49: //1
                                document.getElementById("v1").dispatchEvent(mevent);
                                document.getElementById("v1").classList.remove("active");
                                v1IsDown = false;
                            break;

                            case 50: //2
                                document.getElementById("v2").dispatchEvent(mevent);
                                document.getElementById("v2").classList.remove("active");
                                v2IsDown = false;
                            break;

                            case 51: //3
                                document.getElementById("v3").dispatchEvent(mevent);
                                document.getElementById("v3").classList.remove("active");
                                v3IsDown = false;
                            break;

                            case 52: //4
                                document.getElementById("v4").dispatchEvent(mevent);
                                document.getElementById("v4").classList.remove("active");
                                v4IsDown = false;
                            break;

                            case 82: //R
                                document.getElementById("radio").dispatchEvent(mevent);
                                document.getElementById("radio").classList.remove("active");
                                radioIsDown = false;
                            break;

                            
                        }
                    });

                    window.setInterval(function(){
                        if (auto_refresh > 0) {
                            var variables_base = ["enableAutomaticMode", "automaticState", "numConfiguredValves", "foundVehicle"];
                            var variables_vehicle_i2c = [];//"operatingmode", "irstatus1", "irstatus2"];

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
                                Http.setRequestHeader('Access-Control-Allow-Headers', '*');
                                Http.setRequestHeader('Access-Control-Allow-Origin', '*');
                                Http.setRequestHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
                                Http.send();
                            });

                            variables_vehicle_i2c.forEach(function(variable){
                                const Http = new XMLHttpRequest();
                                Http.onreadystatechange = function() {
                                    if (Http.readyState == XMLHttpRequest.DONE) {
                                        document.getElementById(variable).value = Http.responseText;
                                    }
                                }
                                var url = 'http://'+location.hostname+'/value_i2c?valueName='+variable+'&value=request';
                                console.log('Sending request: ' + url);
                                Http.open("GET", url);
                                Http.setRequestHeader('Access-Control-Allow-Headers', '*');
                                Http.setRequestHeader('Access-Control-Allow-Origin', '*');
                                Http.setRequestHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
                                Http.send();
                            });
                        }
                        
                    }, 1000);
                });
                
            </script>
    <style>
        body {
            font-size: 16px;
            font-family: Consolas, Menlo, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace, serif;
        }

        button {
            border: 5px solid green;
            background-color: #4CAF50;
            color: white;
            padding: 15px 32px;      
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            /* box-shadow: 5px 10px 8px #888888; */
        }

        button.arrow {
            border: 3px solid black;
            background-color: transparent;
            color: black;
            padding: 0px; 
            width: 100px;
            height: 100px;     
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 80px;
            margin: 4px 2px;
            cursor: pointer;
            /* box-shadow: 5px 10px 8px #888888; */
        }

        input {
            height: 80px;
            
            font-size: 30px;
            margin-right: 30px;
            font-family: Consolas, Menlo, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace, serif;
            padding-left: 20px;
        }

        label {
            width: 350px;
            font-size: 30px;
            text-align: right;
            margin-right: 10px;
        }

        .textbox {
            background-color: transparent;
            color: black;
            padding: 0px; 
            width: 100px;
            height: 70px;     
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 60px;
            margin: 4px 2px;
            /* box-shadow: 5px 10px 8px #888888; */
            cursor: pointer;
            padding-top: 10px;
        }

        .center {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .category {
            font-size: 50px;
            margin-bottom: 0px;
        }

        .subcategory {
            font-size: 40px;
            margin-top: 20px;
            margin-bottom: 10px;
        }
        
        button:hover:enabled {
            background-color: lightgreen;
            color: white;
        }

        

      

        .glow-on-hover {
            /* width: 220px; */
            /* height: 50px; */
            /* border: none; */
            /* outline: none; */
            /* color: #fff; */
            /* background: #111; */
            /* cursor: pointer; */
            position: relative;
            z-index: 0;
            /* border-radius: 10px; */
        }

        .glow-on-hover:before{
            content: '';
            background: linear-gradient(90deg,#276221, #46923c, #5bb450, #8bca84, #cce7c9, #8bca84, #5bb450, #46923c, #276221);
            position: absolute;
            top: 0px;
            left:0px;
            background-size: 400%;
            z-index: -1;
            filter: blur(5px);
            width: 100%;
            height: 100%;
            animation: glowing 20s linear infinite;
            opacity: 0;
            transition: opacity .3s ease-in-out;
            /* border-radius: 10px; */
            
            border: none;
            outline: none;
            border-color: transparent;
        }

        .glow-on-hover:active:before,
        .glow-on-hover.active:before{
            color: #000;
            opacity: 1;
        }

        .glow-on-hover:active:after,
        .glow-on-hover.active:after{
            background: transparent;
        }

        .glow-on-hover:hover:before{
            
            border: none;
            outline: none;
        }

        .glow-on-hover:after{
            z-index: -1;
            content: '';
            position: absolute;
            width: 100%;
            height: 100%;
            /* background: #111; */
            left: 0;
            top: 0;
            border: none;
            outline: none;
            /* border-radius: 10px; */
        }

        @keyframes glowing {
            0% { background-position: 0 0; }
            50% { background-position: 400% 0; }
            100% { background-position: 0 0; }
        }

        button:disabled,
        button[disabled]{
            border: 5px solid black;
            background-color: grey;
            color: white;
            padding: 15px 32px;      
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: not-allowed;
            /* box-shadow: 5px 10px 8px red; */
        }

        .arrow:disabled,
        .arrow[disabled]{
            border: 3px solid black;
            color: grey;
            padding: 0px; 
            width: 100px;
            height: 100px;     
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 80px;
            margin: 4px 2px;
            cursor: pointer;
            /* box-shadow: 5px 10px 8px #888888; */


            background-color: lightgray;
            cursor: not-allowed;
        }
        
    </style>
	</HEAD>
<BODY>
    <div style="display: flex;">
        <div style="flex: 50%; padding-left: 50px; padding-top: 60px; display: block; margin-left: auto; margin-right: auto;">
            <img style="width: 90%;" src="data:image/jpeg;base64,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viRo/wC3V+1p4ff4QeKvhDoXwK8FawyQ+JtUk8U22u6jd2QIMkFqtsoRN/Q78ZAI3YJDfbNFAHlvieLxb8C/hR4d8Lfs+/BkeOG0X7Folnop1630lLaxSMqbiS4nVgwUIuQqs7M+cdTXqVFFAHzH8APg38StH/av+Pnx2+JHh/8As+18Uz6ZpHhSRryCYzaZaxlHkCxOxiV/Lgba4Vs7iRnNey2viz4mT/Fy88HTfCX7N4Gt9JF3D4yk163P2m9LIDZpYKDMMKzHzXKr8jADlc9vRQB8nfEz4R/tDfCv9o7X/wBpb9nHw94d8aW/jjSbPT/FfhPVdQ/s64nns08u3uLW5YGNT5YVSH4GG+9uBTkvip4H/bJ/bS8Ow/CD4lfB3Qfgl8Pru/trjxHcSeKLfXtUv4IJFlWG1Fsojjy6qSXxyoOSAVf7eooA8/vtb+IHhjx54P8Ah/4O+D41HwPNZSR6p4obXre2TRRFGwghWzIaa5Z9irldqrvUk8Nj0CiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKhvLOz1C1kstQtYbm3mXbJDNGHRx6FTwR9amopNKS5ZbAnY8I8e/so+F9caTUPBd82h3Tc/ZnBltWPt/FHn2LD0UV4N4o+BXxQ8JtI174XuLu3j/5eLD/AEhCP72F+ZR/vKK+8KK+Bzjw3ybNJOpSi6M3/Jt/4C9Puse5hM/xeGXLJ8y89/v/AM7n5qSRSQyNDNG0bocMrLgg+4NMr9H9S0HQ9aAXWNFsb8AbR9pt0lwPT5ga5y4+DnwrumZpPAOiqW6+XarGPwC4A/Cvia/hFi4v9xiYtecWvycj14cU038dNr0d/wDI+A6XrwK++7f4OfCy3/1fgHRT/wBdLVX/APQs10Gl+GvDmiY/sbQNOsMDH+jWscX/AKCBU0fCLGSf77ExS8ot/nyjnxTSXwU2/nb/ADPg/wAO/Cn4jeKtjaL4P1GWOT7s0kXkxH33yYX9a9i8I/sg6hN5d1428RxW65DNa2C73I9DI2Ap+itX1BRX1uWeF2T4JqeKcqz89I/ctfvbPLxHEeKraU7RX3v8f8jnPB3w88HeArX7L4X0OC0Zl2yT43TS/wC9IfmP0zgdgK6Oiiv0TD4ejhKao4eKjFbJKyXyR4U6kqsuabu/MKKKK2ICiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooA//2Q==">
        </div>
        <div style="flex: 50%; padding-left: 10px; display: block; margin-left: auto; margin-right: auto;">
            <img style="width: 80%;" src="data:image/png;base64,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">
        </div>
    </div>
    
    <p class="center category">Manual Control</p>
    <div class="center">
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="hard_left" id="hard_left"><b>&#8666;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="left" id="left"><b>&lArr;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="forward" id="forward"><b>&uArr;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="stop" id="stop"><b>&#9634;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="backward" id="backward"><b>&dArr;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="right" id="right"><b>&rArr;</b></button>
        <button class="arrow glow-on-hover" data-target="vehicle" data-command="hard_right" id="hard_right"><b>&#8667;</b></button>
    </div>

    <p class="center category">Valve Control</p>
    <div class="center">        
        <p class="textbox">1</p>
        <p class="textbox">2</p>
        <p class="textbox">3</p>
        <p class="textbox">4</p>
    </div>
    <div class="center">        
        <button id="v1" class="arrow glow-on-hover" data-target="valve" data-value="2" data-valvenum="1"><b>&#9732;</b></button>
        <button id="v2" class="arrow glow-on-hover" data-target="valve" data-value="2" data-valvenum="2"><b>&#9732;</b></button>
        <button id="v3" class="arrow glow-on-hover" data-target="valve" data-value="2" data-valvenum="3"><b>&#9732;</b></button>
        <button id="v4" class="arrow glow-on-hover" data-target="valve" data-value="2" data-valvenum="4"><b>&#9732;</b></button>
    </div>
    <div class="center">        
        <button class="arrow glow-on-hover" data-target="valve" data-value="1" data-valvenum="1"><b>&#9711;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="1" data-valvenum="2"><b>&#9711;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="1" data-valvenum="3"><b>&#9711;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="1" data-valvenum="4"><b>&#9711;</b></button>
    </div>
    <div class="center">        
        <button class="arrow glow-on-hover" data-target="valve" data-value="0" data-valvenum="1"><b>&#9152;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="0" data-valvenum="2"><b>&#9152;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="0" data-valvenum="3"><b>&#9152;</b></button>
        <button class="arrow glow-on-hover" data-target="valve" data-value="0" data-valvenum="4"><b>&#9152;</b></button>
    </div>

    <p class="center category">Variables</p>
    <div class="center">
        <button data-target="internal_refresh">Auto Refresh: ON</button>
    </div>
    <div class="center">
        <p class="subcategory">Base Transceiver</p>
    </div>
    <div class="center">
        <label for="enableAutomaticMode">enableAutomaticMode : </label>
        <input type="text" id="enableAutomaticMode" class="deacOnRefresh">
        <button class="arrow deacOnRefresh" data-target="variable_base" data-valuename="enableAutomaticMode">&#9654;</button>
    </div>
    <div class="center">
        <label for="automaticState">automaticState : </label>
        <input type="text" id="automaticState" disabled>
        <div style="width: 100px; height: 100px;"></div>
    </div>
    <div class="center">
        <label for="numConfiguredValves">numConfiguredValves : </label>
        <input type="text" id="numConfiguredValves" class="deacOnRefresh">
        <button class="arrow deacOnRefresh" data-target="variable_base" data-valuename="numConfiguredValves">&#9654;</button>
    </div>
    <div class="center">
        <label for="foundVehicle">foundVehicle : </label>
        <input type="text" id="foundVehicle" disabled><br>
        <div style="width: 100px; height: 100px;"></div>
    </div>
    

    <!-- <h4>Vehicle Transceiver</h4> -->
    
    <p class="center subcategory">Vehicle</p>
    <div class="center">
        <label for="operatingmode">operatingmode :</label>
        <input type="text" id="operatingmode" disabled>
        <div style="width: 100px; height: 100px;"></div>
    </div>
    <div class="center">
        <label for="irstatus1">irstatus1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:</label>
        <input type="text" id="irstatus1" disabled>
        <div style="width: 100px; height: 100px;"></div>
    </div>
    <div class="center">
        <label for="irstatus2">irstatus2&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:</label>
        <input type="text" id="irstatus2" disabled>
        <div style="width: 100px; height: 100px;"></div>
    </div>
    <p class="center category">Radio</p>
    <div class="center" style="padding-bottom: 40px;">
        <button class="arrow " data-target="vehicle" data-command="radio" id="radio">&reg;</button>
    </div>

    <div class="center" style="background-color: #bf9000; height: 100px;">
        <p style="font-size: 40px;">GORP</p>
    </div>
</BODY>
</HTML>
)=====";