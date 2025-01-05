#ifndef WEBPAGE_H
#define WEBPAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Stepper Motor Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <meta name="theme-color" content="#0f172a">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-primary: #0f172a;
            --bg-secondary: #1e293b;
            --text-primary: #e2e8f0;
            --text-secondary: #94a3b8;
            --accent-primary: #6366f1;
            --accent-hover: #818cf8;
            --success: #10b981;
            --success-hover: #34d399;
            --danger: #ef4444;
            --danger-hover: #f87171;
            --border-color: #334155;
            --section-gap: 16px;
            --control-padding: 20px;
            --max-content-width: 1200px;
        }

        * {
            box-sizing: border-box;
            font-family: 'Inter', system-ui, -apple-system, sans-serif;
            margin: 0;
            padding: 0;
        }
        
        body {
            margin: 0;
            background-color: var(--bg-primary);
            color: var(--text-primary);
            min-height: 100vh;
            max-width: var(--max-content-width);
            margin: 0 auto;
            display: flex;
            flex-direction: column;
            gap: var(--section-gap);
            padding: 32px;
        }
        
        .control-button {
            margin: 5px;
            padding: 12px 24px;
            font-size: 16px;
            border-radius: 12px;
            border: 1px solid var(--border-color);
            background-color: var(--bg-secondary);
            color: var(--text-primary);
            touch-action: manipulation;
            cursor: pointer;
            transition: all 0.2s ease;
            backdrop-filter: blur(10px);
        }
        
        .control-button:hover {
            background-color: var(--accent-primary);
            transform: translateY(-1px);
            border-color: var(--accent-primary);
        }
        
        .motor-section {
            padding: 24px;
            border-radius: 20px;
            background-color: var(--bg-secondary);
            border: 1px solid var(--border-color);
            backdrop-filter: blur(10px);
            display: flex;
            flex-direction: column;
            gap: var(--section-gap);
        }
        
        .motor-section h2 {
            margin-top: 0;
            margin-bottom: 12px;
            color: var(--text-primary);
            font-size: 20px;
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        
        .movement-buttons {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 12px;
        }
        
        .control-section {
            padding: 24px;
            border-radius: 20px;
            background-color: var(--bg-secondary);
            border: 1px solid var(--border-color);
            backdrop-filter: blur(10px);
            display: flex;
            flex-direction: column;
            gap: var(--section-gap);
        }

        .control-group {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-bottom: 16px;
        }

        .control-group label {
            display: flex;
            flex-direction: column;
            gap: 8px;
            font-size: 14px;
            color: var(--text-secondary);
        }

        .control-group input {
            padding: 12px 16px;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            font-size: 14px;
            background-color: var(--bg-primary);
            color: var(--text-primary);
            transition: all 0.2s ease;
        }

        .control-group input:focus {
            outline: none;
            border-color: var(--accent-primary);
            box-shadow: 0 0 0 2px rgba(99, 102, 241, 0.2);
        }

        .pattern-controls {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 16px;
            margin-top: 24px;
        }

        .pattern-controls input {
            padding: 12px;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            font-size: 14px;
            background-color: var(--bg-primary);
            color: var(--text-primary);
            transition: all 0.2s ease;
            width: 100%;
        }

        .pattern-controls input:focus {
            outline: none;
            border-color: var(--accent-primary);
            box-shadow: 0 0 0 2px rgba(99, 102, 241, 0.2);
        }

        .pattern-controls button {
            padding: 16px 32px;
            border-radius: 12px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.2s ease;
            border: 1px solid transparent;
            width: 100%;
        }

        .goto-controls {
            display: grid;
            grid-template-columns: auto auto repeat(2, 1fr) auto;
            gap: 10px;
        }

        .goto-controls input {
            padding: 12px;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            font-size: 14px;
            background-color: var(--bg-primary);
            color: var(--text-primary);
            transition: all 0.2s ease;
        }

        .goto-controls input:focus {
            outline: none;
            border-color: var(--accent-primary);
            box-shadow: 0 0 0 2px rgba(99, 102, 241, 0.2);
        }

        .goto-button {
            background-color: var(--accent-primary);
            color: var(--text-primary);
            opacity: 0;
            pointer-events: none;
            transition: all 0.2s ease;
        }

        .goto-button.visible {
            opacity: 1;
            pointer-events: auto;
        }

        .goto-button:hover {
            background-color: var(--accent-hover);
            transform: translateY(-1px);
        }

        .start-pattern {
            background-color: var(--accent-primary);
            color: var(--text-primary);
        }

        .start-pattern:hover {
            background-color: var(--accent-hover);
            transform: translateY(-1px);
        }

        .stop-pattern {
            background-color: #dc2626;
            color: white;
        }

        .stop-pattern:hover {
            background-color: var(--danger-hover);
            transform: translateY(-1px);
        }

        /* Add subtle glass morphism effect */
        .glass-effect {
            background: rgba(30, 41, 59, 0.7);
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
        
        @media (max-width: 768px) {
            body {
                padding: 8px;
                gap: 8px;
            }
            
            .page-layout {
                grid-template-columns: 1fr;
            }
            
            .axis-controls {
                grid-template-columns: 1fr;
                gap: 8px;
            }

            .control-button {
                padding: 8px 16px;
                font-size: 14px;
                margin: 2px;
            }

            .pick-place-button {
                padding: 12px 24px;
                font-size: 16px;
            }

            .motor-section h2 {
                font-size: 16px;
                margin-bottom: 8px;
            }

            .control-group {
                grid-template-columns: 1fr;
                gap: 8px;
            }

            .pattern-controls, .goto-controls {
                grid-template-columns: auto repeat(2, 1fr) auto;
                gap: 8px;
            }

            .control-group input, 
            .pattern-controls input,
            .goto-controls input {
                padding: 8px;
                font-size: 14px;
            }

            .relay-controls {
                grid-template-columns: 1fr;
            }
        }

        /* Enhance small screen breakpoint */
        @media (max-width: 360px) {
            :root {
                --section-gap: 4px;
                --control-padding: 8px;
            }

            body {
                padding: 4px;
            }

            .motor-section, 
            .control-section,
            .pick-place-section {
                padding: 12px 8px;
                border-radius: 12px;
            }

            .movement-buttons {
                gap: 4px;
            }

            .control-button {
                padding: 8px;
                font-size: 12px;
                margin: 0;
                border-radius: 8px;
                min-height: 36px;
                display: flex;
                align-items: center;
                justify-content: center;
            }

            .pick-place-button {
                padding: 12px;
                font-size: 14px;
                border-radius: 8px;
                min-height: 44px;
            }

            .control-group {
                gap: 4px;
            }

            .control-group input,
            .pattern-controls input,
            .goto-controls input {
                padding: 8px;
                height: 36px;
                font-size: 13px;
                border-radius: 6px;
            }

            .pattern-controls {
                gap: 8px;
            }

            .pattern-controls button {
                padding: 8px;
                font-size: 13px;
                border-radius: 8px;
                min-height: 36px;
            }

            .motor-section h2,
            .control-section h2 {
                font-size: 14px;
                margin-bottom: 8px;
            }

            .control-group label {
                font-size: 12px;
                margin-bottom: 2px;
            }

            .relay-controls {
                gap: 6px;
            }

            .relay-button {
                padding: 8px;
                font-size: 12px;
                border-radius: 6px;
                min-height: 36px;
            }

            /* Stack goto controls vertically on very small screens */
            .goto-controls {
                grid-template-columns: 1fr;
                gap: 6px;
            }

            .zero-button {
                padding: 8px;
                height: 36px;
                font-size: 13px;
                border-radius: 6px;
            }
        }

        /* Add an extra breakpoint for medium-small screens */
        @media (min-width: 361px) and (max-width: 480px) {
            body {
                padding: 8px;
            }

            .motor-section, 
            .control-section,
            .pick-place-section {
                padding: 16px;
            }

            .control-button {
                padding: 10px;
                font-size: 13px;
            }

            .goto-controls {
                grid-template-columns: 1fr 1fr auto;
            }
        }

        .relay-controls {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
        }

        .relay-button {
            padding: 12px;
            border-radius: 8px;
            font-size: 14px;
            cursor: pointer;
            transition: all 0.2s ease;
            border: 1px solid var(--border-color);
            background-color: var(--bg-primary);
            color: var(--text-primary);
        }

        .relay-button.active {
            background-color: var(--success);
            border-color: var(--success);
        }

        .relay-button:hover {
            transform: translateY(-1px);
        }

        .main-controls, .pattern-and-manual {
            display: flex;
            flex-direction: column;
            gap: var(--section-gap);
        }

        .axis-controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 24px;
        }

        .control-section, .motor-section, .pick-place-section {
            height: fit-content;
        }

        /* Add section headers styling */
        .control-section h2,
        .motor-section h2 {
            font-size: 22px;
            font-weight: 600;
            margin-bottom: 24px;
            color: var(--text-primary);
            display: flex;
            align-items: center;
            gap: 12px;
        }

        /* Improve input styling */
        .control-group input,
        .pattern-controls input,
        .goto-controls input {
            padding: 12px 16px;
            height: 48px;
            font-size: 16px;
            width: 100%;
        }

        .control-group label {
            font-size: 15px;
            font-weight: 500;
            color: var(--text-secondary);
            margin-bottom: 8px;
        }

        .pick-place-controls {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
            margin-bottom: 16px;
        }

        .pick-place-controls button {
            padding: 12px;
            border-radius: 8px;
            font-size: 14px;
            cursor: pointer;
            transition: all 0.2s ease;
            border: 1px solid var(--border-color);
            background-color: var(--bg-primary);
            color: var(--text-primary);
        }

        .pick-place-controls button:hover {
            background-color: var(--accent-primary);
            border-color: var(--accent-primary);
            transform: translateY(-1px);
        }

        .page-layout {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
            gap: var(--section-gap);
            width: 100%;
        }

        .main-controls {
            display: grid;
            grid-template-columns: 1fr;
            gap: var(--section-gap);
        }

        .zero-button {
            background-color: var(--bg-secondary);
            color: var(--text-primary);
            border: 1px solid var(--border-color);
            border-radius: 8px;
            padding: 12px;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .zero-button:hover {
            background-color: var(--accent-primary);
            border-color: var(--accent-primary);
            transform: translateY(-1px);
        }
    </style>
</head>
<body>
    <div class="page-layout">
        <div class="main-controls">
            <div class="axis-controls">
                <div class="motor-section">
                    <h2>Y Axis</h2>
                    <div class="movement-buttons">
                        <button class="control-button" onclick="sendCommand('homeY')">Home</button>
                        <button class="control-button" onclick="sendCommand('moveY -1000')">-1000</button>
                        <button class="control-button" onclick="sendCommand('moveY 1000')">+1000</button>
                    </div>
                </div>

                <div class="motor-section">
                    <h2>X Axis</h2>
                    <div class="movement-buttons">
                        <button class="control-button" onclick="sendCommand('homeX')">Home</button>
                        <button class="control-button" onclick="sendCommand('moveX -1000')">-1000</button>
                        <button class="control-button" onclick="sendCommand('moveX 1000')">+1000</button>
                    </div>
                </div>
            </div>

            <div class="control-section">
                <h2>Pattern Control</h2>
                <div class="control-group">
                    <label>
                        Rows
                        <input type="number" id="patternRows" value="5">
                    </label>
                    <label>
                        Columns
                        <input type="number" id="patternCols" value="7">
                    </label>
                </div>
                <div class="control-group">
                    <label>
                        Start X
                        <input type="number" id="startX" value="3" step="0.5">
                    </label>
                    <label>
                        Start Y
                        <input type="number" id="startY" value="9" step="0.5">
                    </label>
                </div>
                <div class="control-group">
                    <label>
                        End X
                        <input type="number" id="endX" value="30.5" step="0.5">
                    </label>
                    <label>
                        End Y
                        <input type="number" id="endY" value="28.5" step="0.5">
                    </label>
                </div>
                <div class="control-group">
                    <label>
                        Pickup X
                        <input type="number" id="pickupX" value="15.5" step="0.5">
                    </label>
                    <label>
                        Pickup Y
                        <input type="number" id="pickupY" value="3.5" step="0.5">
                    </label>
                </div>
                <div class="control-group">
                    <label>
                        Speed (steps/sec)
                        <input type="number" id="patternSpeed" value="7500">
                    </label>
                    <label>
                        Acceleration (steps/sec²)
                        <input type="number" id="patternAccel" value="50000">
                    </label>
                </div>
                <div class="pattern-controls">
                    <button class="start-pattern" onclick="startPattern()">Start Pattern</button>
                    <button class="stop-pattern" onclick="stopPattern()">Stop Pattern</button>
                </div>
            </div>

            <div class="control-section">
                <h2>Manual Control</h2>
                <div class="relay-controls">
                    <button id="suctionButton" class="relay-button" onclick="toggleSuction()">
                        Suction: OFF
                    </button>
                    <button id="extensionButton" class="relay-button" onclick="toggleExtension()">
                        Extension: OFF
                    </button>
                </div>
                <div class="pick-place-controls">
                    <button onclick="sendCommand('pick')">Pick</button>
                    <button onclick="sendCommand('place')">Place</button>
                </div>
                <div class="goto-controls">
                    <button class="zero-button" onclick="gotoZero()">Zero</button>
                    <button class="zero-button" onclick="gotoPickup()">Pickup</button>
                    <input type="number" id="gotoX" placeholder="X Position" step="1">
                    <input type="number" id="gotoY" placeholder="Y Position" step="1">
                    <button class="goto-button" id="gotoButton" onclick="executeGoto()">Go</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        
        window.addEventListener('load', onLoad);
        
        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen    = onOpen;
            websocket.onclose   = onClose;
            websocket.onmessage = onMessage;
        }
        
        function onOpen(event) {
            console.log('Connection opened');
        }
        
        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }
        
        function onMessage(event) {
            console.log(event.data);
            if (event.data === "Suction ON") {
                suctionState = true;
                document.getElementById('suctionButton').classList.add('active');
                document.getElementById('suctionButton').textContent = 'Suction: ON';
            } else if (event.data === "Suction OFF") {
                suctionState = false;
                document.getElementById('suctionButton').classList.remove('active');
                document.getElementById('suctionButton').textContent = 'Suction: OFF';
            } else if (event.data === "Extension ON") {
                extensionState = true;
                document.getElementById('extensionButton').classList.add('active');
                document.getElementById('extensionButton').textContent = 'Extension: ON';
            } else if (event.data === "Extension OFF") {
                extensionState = false;
                document.getElementById('extensionButton').classList.remove('active');
                document.getElementById('extensionButton').textContent = 'Extension: OFF';
            }
        }
        
        function onLoad(event) {
            initWebSocket();
        }
        
        function sendCommand(command) {
            websocket.send(command);
        }

        function startPattern() {
            // Get values from inputs
            const rows = parseInt(document.getElementById('patternRows').value);
            const cols = parseInt(document.getElementById('patternCols').value);
            const startX = parseFloat(document.getElementById('startX').value);
            const startY = parseFloat(document.getElementById('startY').value);
            const endX = parseFloat(document.getElementById('endX').value);
            const endY = parseFloat(document.getElementById('endY').value);
            const pickupX = parseFloat(document.getElementById('pickupX').value);
            const pickupY = parseFloat(document.getElementById('pickupY').value);
            const speed = document.getElementById('patternSpeed').value || 1000;
            const accel = document.getElementById('patternAccel').value || 500;
            
            // Validate values
            if (rows <= 0 || cols <= 0) {
                console.error("Rows and columns must be positive");
                return;
            }

            // Format command with validated parameters
            const command = `pattern start ${rows} ${cols} ${startX} ${startY} ${endX} ${endY} ${pickupX} ${pickupY} ${speed} ${accel}`;
            console.log("Sending command:", command);
            websocket.send(command);
        }

        function stopPattern() {
            websocket.send('pattern stop');
        }

        // Add event listeners for goto inputs
        document.getElementById('gotoX').addEventListener('input', showGotoButton);
        document.getElementById('gotoY').addEventListener('input', showGotoButton);
        
        // Add keydown event listeners for tab and enter
        document.getElementById('gotoX').addEventListener('keydown', handleGotoKeydown);
        document.getElementById('gotoY').addEventListener('keydown', handleGotoKeydown);

        function showGotoButton() {
            const button = document.getElementById('gotoButton');
            button.classList.add('visible');
        }

        function hideGotoButton() {
            const button = document.getElementById('gotoButton');
            button.classList.remove('visible');
        }

        function handleGotoKeydown(event) {
            if (event.key === 'Enter') {
                executeGoto();
            } else if (event.key === 'Tab') {
                event.preventDefault();
                const gotoX = document.getElementById('gotoX');
                const gotoY = document.getElementById('gotoY');
                if (document.activeElement === gotoX) {
                    gotoY.focus();
                } else {
                    gotoX.focus();
                }
            }
        }

        function executeGoto() {
            const x = parseFloat(document.getElementById('gotoX').value);
            const y = parseFloat(document.getElementById('gotoY').value);
            const speed = document.getElementById('patternSpeed').value || 1000;
            const accel = document.getElementById('patternAccel').value || 500;

            if (!isNaN(x) && !isNaN(y)) {
                const command = `goto ${x} ${y} ${speed} ${accel}`;
                console.log("Sending command:", command);
                websocket.send(command);
                hideGotoButton();
            }
        }

        let suctionState = false;
        let extensionState = false;

        function toggleSuction() {
            suctionState = !suctionState;
            const button = document.getElementById('suctionButton');
            button.textContent = `Suction: ${suctionState ? 'ON' : 'OFF'}`;
            button.classList.toggle('active', suctionState);
            websocket.send(suctionState ? 'suction on' : 'suction off');
        }

        function toggleExtension() {
            extensionState = !extensionState;
            const button = document.getElementById('extensionButton');
            button.textContent = `Extension: ${extensionState ? 'ON' : 'OFF'}`;
            button.classList.toggle('active', extensionState);
            websocket.send(extensionState ? 'extension on' : 'extension off');
        }

        function gotoZero() {
            const speed = document.getElementById('patternSpeed').value || 1000;
            const accel = document.getElementById('patternAccel').value || 500;
            const command = `goto 0 0 ${speed} ${accel}`;
            console.log("Sending command:", command);
            websocket.send(command);
        }

        function gotoPickup() {
            const speed = document.getElementById('patternSpeed').value || 1000;
            const accel = document.getElementById('patternAccel').value || 500;
            const pickupX = document.getElementById('pickupX').value;
            const pickupY = document.getElementById('pickupY').value;
            const command = `goto ${pickupX} ${pickupY} ${speed} ${accel}`;
            console.log("Sending command:", command);
            websocket.send(command);
        }
    </script>
</body>
</html>
)rawliteral";

#endif