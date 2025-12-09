(() => {
    const ws = new WebSocket('ws://192.168.1.177:80/ws')
    var button = document.getElementById("button");
    var sliderA = document.getElementById("myRangeA");
    var sliderB = document.getElementById("myRangeB");
    var distA = document.getElementById("distA");
    var seuil = document.getElementById("seuil");
    var output = document.getElementById("demo");
    var d0 = document.getElementById("d0");
    var d1 = document.getElementById("d1");
    var d2 = document.getElementById("d2");
    const dd = {"d_0" : d0, "d_1" : d1, "d_2" : d2};
    output.innerHTML = sliderA.value; // Display the default slider value







    
    ws.onopen = () => {
        console.log('ws opened on browser')
        ws.send('hello world')
    }

    seuil.oninput = function() {
        ws.send('seuil?sensor=1,value=' + this.value)
    }
    
    ws.onmessage = (message) => {
        console.log(`message received ` + message.data)
    }
    
    sliderA.oninput = function() {
        output.innerHTML = this.value;
    } 

    button.onclick = function() {
        output.innerHTML = this.value;
        ws.send('sample');
    } 

    
})()
