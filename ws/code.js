 (() => {
  const ws = new WebSocket('ws://192.168.1.177:80/ws')
  

  var button = document.getElementById("button");
  var sliderA = document.getElementById("myRangeA");
  var sliderB = document.getElementById("myRangeB");
  var distA = document.getElementById("distA");
  var output = document.getElementById("demo");
  output.innerHTML = sliderA.value; // Display the default slider value
  
  ws.onopen = () => {
    console.log('ws opened on browser')
    ws.send('hello world')
  }


  ws.onmessage = (message) => {
    console.log(`message received ` + message.data)
    const s = message.data.split("=");
    //console.log(s);
    if (s[0] == "distance_A") {
      d = parseInt(s[1]);
      distA.value = d;

      if (sliderA.value > 0) {
        ws.send('speed_A?' + (255 - d*5));
      }
    } else {
    }
  }

  sliderA.oninput = function() {
    output.innerHTML = this.value;
    ws.send('speed_A?' + this.value);
    //console.log(this.value);

} 

     button.onclick = function() {
         output.innerHTML = this.value;
         ws.send('button');
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
})()
