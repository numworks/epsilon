/* To use this file you should adhere to the following conventions:
 *  - Main canvas has id #canvas
 *  - Secondary canvas (fullscreen on the side) has id #secondary-canvas
 *  - Calculator keyboard has id #keyboard */

var Module;

(function () {
  var mainCanvas = document.getElementById('canvas');
  var secondaryCanvasContext = document.getElementById('secondary-canvas').getContext('2d');
  var epsilonLanguage = document.documentElement.lang || window.navigator.language.split('-')[0];
  Module = {
    canvas: mainCanvas,
    arguments: ['--language', epsilonLanguage],
    onDisplayRefresh: function() {
      secondaryCanvasContext.drawImage(mainCanvas, 0, 0);
    }
  }

  Epsilon(Module);


  document.querySelectorAll('#keyboard span').forEach(function(span){
    function eventHandler(keyHandler) {
      return function(ev) {
        var key = this.getAttribute('data-key');
        keyHandler(key);
        /* Always prevent default action of event.
         * First, this will prevent the browser from delaying that event. Indeed
         * the browser would otherwise try to see if that event could have any
         * other meaning (e.g. a click) and might delay it as a result.
         * Second, this prevents touch events to be handled twice. Indeed, for
         * backward compatibility reasons, mobile browsers usually create a fake
         * mouse event after each real touch event. This allows desktop websites
         * to work unmodified on mobile devices. But here we are explicitly
         * handling both touch and mouse events. We therefore need to disable
         * the default action of touch events, otherwise the handler would get
         * called twice. */
        ev.preventDefault();
      };
    }
    /* We decide to hook both to touch and mouse events
     * On most mobile browsers, mouse events are generated if addition to touch
     * events, so this could seem pointless. But those mouse events are not
     * generated in real time: instead, they are buffered and eventually fired
     * in a very rapid sequence. This prevents Epsilon from generating an event
     * since this quick sequence will trigger the debouncer. */
    ['touchstart', 'mousedown'].forEach(function(type){
      span.addEventListener(type, eventHandler(Module._IonSimulatorKeyboardKeyDown));
    });
    ['touchend', 'mouseup'].forEach(function(type){
      span.addEventListener(type, eventHandler(Module._IonSimulatorKeyboardKeyUp));
    });
  });
}());

function screenshot() {
  // toDataURL needs the canvas to be refreshed
  Module._IonDisplayForceRefresh();

  var canvas = document.getElementById('canvas');
  var link = document.createElement('a');
  link.download = 'screenshot.png';
  link.href = canvas.toDataURL('image/png').replace('image/png', 'image/octet-stream');
  link.click();
}

const notif = document.getElementById("notif");

function createNotif(message, isError){
  let child = document.createElement("div")
  child.classList.add(isError ? "error" : "success");
  child.classList.add("fadein")
  child.innerHTML = message;
  notif.appendChild(child);
  setTimeout(() => {
    child.classList.remove("fadein");
    child.classList.add("fadeout");
    setTimeout(() => {
      notif.removeChild(child);
    }, 500)
  }, 5000)
}

function sendScript(evt) {
  console.log(evt)
  for (var i = 0; i < evt.target.files.length; i++) {
    const file = evt.target.files[i]
    //FIXME Scenario : test.py.py
    const name = file.name.split(".py")[0];
    const nameptr  = Module.allocate(Module.intArrayFromString(name), 'i8', 0);
    let reader = new FileReader();
    reader.readAsText(file);
    reader.onload = function() {
      var content  = Module.allocate(Module.intArrayFromString("\001" + reader.result), 'i8', 0);
      const ErrorStauts = Module._IonStorageAddScript(nameptr, content);
      switch(ErrorStauts){
        case 1 :{
          createNotif(`${name} couldn't be added : this name is already taken`, true);
          break;
        }
        case 2 :{
          createNotif(`${name} couldn't be added : this name is not compliant`, true);
          break;
        }
        case 3 :{
          createNotif(`${name} couldn't be added : there isn't enough space`, true);
          break;
        }
        default:{
          createNotif(name + " has succesfully been added !", false)
        }
      }
      Module._free(nameptr);
      Module._free(content) 
    };
  }
}

function downloadScriptAtIndex(index){
  var a = document.createElement("a");
  a.style.display = "none";
  const name = Module.UTF8ToString(Module._IonStorageScriptAtIndexName(index));
  const content = Module.UTF8ToString(Module._IonStorageScriptAtIndexCotent(index)).substr(1);
  var file = new Blob([content], {type: "text/plain;charset=utf-8"});
  a.href = URL.createObjectURL(file);
  a.download = name;
  a.click()
}

const download = document.getElementById("download");
const downloadList = document.getElementById("downloadList");

function openDownload(){
  download.classList.add("show");
  const length = Module._IonStorageNumberOfScripts();
  for(var i = 0; i<length; i++){
    const name = Module.UTF8ToString(Module._IonStorageScriptAtIndexName(i));
    let tr = document.createElement("tr")
    let nameth = document.createElement("th")
    nameth.classList.add("name")
    nameth.innerHTML = name;
    tr.appendChild(nameth);
    let downloadth = document.createElement("th");
    let a = document.createElement("a");
    a.id = "download-"+i;
    a.innerHTML = "Download file"
    a.onclick = function(e){
      downloadScriptAtIndex(a.id.split("download-")[1])
    }
    downloadth.appendChild(a);
    tr.appendChild(downloadth);
    downloadList.appendChild(tr);
  }
}

function closeDownload(){
  download.classList.remove("show");
  downloadList.innerHTML = "";
}
