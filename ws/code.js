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
        //console.log(`message received ` + message.data)
        const s = message.data.split("=");
        //console.log(s);
        if (s[0] == "distance_0") {
            d = parseInt(s[1]);
            distA.value = d;
            
            if (sliderA.value > 0) {
                ws.send('speed_A?' + (255 - d*5));
            }
        } else {
            dd[s[0]].innerHTML = s;
        }
    }
    
    sliderA.oninput = function() {
        output.innerHTML = this.value;
        ws.send('speed_A?' + this.value);
        //console.log(this.value);

    } 

    button.onclick = function() {
        output.innerHTML = this.value;
        ws.send('distance');
        //console.log(this.value);

    } 
    sliderB.oninput = function() {
        output.innerHTML = this.value;
        ws.send('speed_B?' + this.value);
        //console.log(this.value);

    } 

    function handleOrientation(event) {
        //updateFieldIfNotNull('Orientation_a', event.alpha);
        //updateFieldIfNotNull('Orientation_b', event.beta);
        //updateFieldIfNotNull('Orientation_g', event.gamma);
        //incrementEventCount();
        v = event.alpha;
        console.log(event.data);
    }

    
    // Request permission for iOS 13+ devices
    if (
        DeviceMotionEvent &&
            typeof DeviceMotionEvent.requestPermission === "function"
    ) {
        DeviceMotionEvent.requestPermission();
    }
    
    //window.addEventListener("devicemotion", handleMotion);
    window.addEventListener("deviceorientation", handleOrientation);
    console.log("ok");

  $( function() {
    $( "#slider-range" ).slider({
      range: true,
      min: 0,
      max: 100,
      values: [ 10, 50 ],
      slide: function( event, ui ) {
        $( "#amount" ).val( "$" + ui.values[ 0 ] + " - $" + ui.values[ 1 ] );
      }
    });
    $( "#amount" ).val( "$" + $( "#slider-range" ).slider( "values", 0 ) +
      " - $" + $( "#slider-range" ).slider( "values", 1 ) );
  } );

    
})()
