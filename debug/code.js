(() => {
    const ws = new WebSocket('ws://192.168.1.177:80/ws')
    var txt = document.getElementById("debug");
    var button = document.getElementById("button");
    ws.onopen = () => {
        console.log('ws opened on browser')
        ws.send('hello world')
    }
    ws.onmessage = (message) => {
        //console.log(`message received ` + message.data)
        const s = message.data.split("=");
        //console.log(s);
        if (s[0] == "eko") {            
            txt.innerHTML += "<br>" + s[1];
        }
        if (s[0] == "count") {
            ws.send("message=yyy");
        }
    }
    button.onclick = function() {
        ws.send('message=aaa');
    } 
    
    console.log("ok");
})()
